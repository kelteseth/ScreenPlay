// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#include <QDebug>
#include <QDir>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickItem>
#include <QQuickStyle>
#include <QQuickWindow>
#include <QtTest>
#include <memory>

#include "ScreenPlayCore/testhelper.h"
#include "screenplayworkshop.h"
#include "steamworkshop.h"
#include "uploadlistmodel.h"

#include "CMakeVariables.h"

using ScreenPlay::clickItem;
using ScreenPlay::findChildRecursive;
using ScreenPlay::findItemDelegate;
using ScreenPlay::waitForCondition;

/*!
 * \class WorkshopUploadTest
 * \brief Test case for uploading content to Steam Workshop.
 *
 * This test uses the wallpaper_godot content from the Content folder to test
 * the complete upload workflow including:
 * - Navigating to the upload page
 * - Selecting content to upload
 * - Completing the upload process
 * - Navigating to the profile page
 * - Deleting the uploaded item
 */
class WorkshopUploadTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase()
    {

        qSetMessagePattern("[%{time yy.MM.dd HH:mm:ss.zzz}] %{if-category}%{category}.%{endif}%{type} | %{function} | %{message}\n   Loc: [%{file}:%{line}]");
        // https://steamcommunity.com/profiles/76561198744817917/myworkshopfiles?appid=672870
        qInfo() << "Initializing Workshop Upload test case...";

        QGuiApplication::setOrganizationName("ScreenPlay");
        QGuiApplication::setOrganizationDomain("screen-play.app");
        QGuiApplication::setApplicationName("ScreenPlay");

        m_testContentPath = QUrl::fromLocalFile(QString(SCREENPLAY_SOURCE_DIR) + "/Content");
        qInfo() << "Test content path:" << m_testContentPath;

        QQuickStyle::setStyle("Material");
        m_engine = std::make_shared<QQmlApplicationEngine>();
        m_engine->loadFromModule("ScreenPlayWorkshop", "TestMain");

        QTest::qWait(2000);

        if (!m_engine->rootObjects().isEmpty()) {
            m_window = qobject_cast<QQuickWindow*>(m_engine->rootObjects().first());
        }
        QVERIFY(m_window);

        // Get the ScreenPlayWorkshop instance from QML
        m_screenPlayWorkshop = m_window->findChild<ScreenPlayWorkshop::ScreenPlayWorkshop*>("screenPlayWorkshop");
        QVERIFY2(m_screenPlayWorkshop, "ScreenPlayWorkshop not found");

        // Set test content path and reload the installed list model
        m_screenPlayWorkshop->setContentPath(m_testContentPath);
        m_screenPlayWorkshop->installedListModel()->reset();

        // Wait for content to reload
        QTest::qWait(1000);

        m_steamWorkshop = m_screenPlayWorkshop->steamWorkshop();

        // Verify Steam is online
        if (m_steamWorkshop) {
            QVERIFY2(m_steamWorkshop->online(), "Steam must be online for this test");
        }

        qInfo() << "Workshop Upload test case initialized successfully";
    }

    void cleanupTestCase()
    {
        m_window = nullptr;
        m_engine.reset();
        qInfo() << "Workshop Upload test case cleaned up";
    }

    void upload_and_delete_workshop_item();

private:
    std::shared_ptr<QQmlApplicationEngine> m_engine;
    QQuickWindow* m_window = nullptr;
    ScreenPlayWorkshop::ScreenPlayWorkshop* m_screenPlayWorkshop = nullptr;
    ScreenPlayWorkshop::SteamWorkshop* m_steamWorkshop = nullptr;
    QUrl m_testContentPath;
};

void WorkshopUploadTest::upload_and_delete_workshop_item()
{
    // Wait for the start page to load
    QTest::qWait(3000);

    // Step 1: Click the Upload button to navigate to upload page
    auto* btnSteamUpload = m_window->findChild<QQuickItem*>("btnSteamUpload");
    QVERIFY2(btnSteamUpload, "Upload button not found");
    clickItem(btnSteamUpload);
    QTest::qWait(1000);

    // Step 2: Find the upload page
    auto* uploadProjectPage = m_window->findChild<QQuickItem*>("uploadProjectPage");
    QVERIFY2(uploadProjectPage, "Upload project page not found");

    // Step 3: Find the grid view with installable content
    auto* uploadGridView = m_window->findChild<QQuickItem*>("uploadGridView");
    QVERIFY2(uploadGridView, "Upload grid view not found");

    // Step 4: Wait for delegates to be created
    // Use findItemDelegate since delegates need special handling
    QQuickItem* firstUploadItem = nullptr;
    bool foundItem = QTest::qWaitFor(
        [&]() {
            firstUploadItem = findItemDelegate(uploadGridView, "uploadProjectBigItem0");
            return firstUploadItem != nullptr;
        },
        5000);
    QVERIFY2(foundItem && firstUploadItem, "No upload items found - make sure Content folder has installable wallpapers");

    // Step 5: Select items by clicking their checkboxes
    // Note: GridView recycles delegates, so we select items one at a time
    // and verify the checkbox state is properly set before moving on
    const int itemsToSelect = 3;
    int selectedCount = 0;
    QList<QString> selectedPaths;

    for (int i = 0; i < itemsToSelect; ++i) {
        const auto itemName = QString("uploadProjectBigItem%1").arg(i);
        const auto checkBoxName = QString("itemCheckBox%1").arg(i);

        auto* uploadItem = findItemDelegate(uploadGridView, itemName);
        if (!uploadItem) {
            qWarning() << "Upload item not found:" << itemName;
            continue;
        }

        // Scroll the item into view to ensure the checkbox is not overlapping with buttons
        QMetaObject::invokeMethod(uploadGridView, "positionViewAtIndex",
            Q_ARG(int, i), Q_ARG(int, 0)); // 0 = GridView.Beginning
        QTest::qWait(200); // Wait for scroll animation

        QQuickItem* checkBox = nullptr;
        bool foundCheckBox = QTest::qWaitFor(
            [&]() {
                checkBox = findChildRecursive(uploadItem, checkBoxName);
                return checkBox != nullptr && checkBox->isVisible();
            },
            2000);

        if (foundCheckBox && checkBox) {
            // Click the checkbox
            clickItem(checkBox);
            QTest::qWait(100);

            // Verify the item is now selected by checking isSelected property
            const bool isSelected = uploadItem->property("isSelected").toBool();
            if (isSelected) {
                ++selectedCount;
                const QString path = uploadItem->property("absoluteStoragePath").toString();
                selectedPaths.append(path);
                qInfo() << "Selected item" << i << ":" << itemName << "path:" << path;
            } else {
                qWarning() << "Checkbox clicked but item not selected:" << itemName;
            }
        } else {
            qWarning() << "Checkbox not found for item:" << itemName;
        }
    }

    QVERIFY2(selectedCount > 0, "Failed to select any items for upload");
    qInfo() << "Selected" << selectedCount << "items for upload:" << selectedPaths;

    // Step 6: Connect to signals BEFORE clicking upload to ensure we don't miss them
    int uploadsCompleted = 0;
    QList<QVariant> uploadedFileIDs;
    const int expectedUploads = selectedCount;

    if (m_steamWorkshop && m_steamWorkshop->uploadListModel()) {
        QObject::connect(m_steamWorkshop->uploadListModel(), &ScreenPlayWorkshop::UploadListModel::itemUploadCompleted,
            this, [&uploadsCompleted, &uploadedFileIDs, expectedUploads](QVariant publishedFileId, bool successful) {
                if (successful) {
                    uploadedFileIDs.append(publishedFileId);
                    ++uploadsCompleted;
                    qInfo() << "Upload" << uploadsCompleted << "/" << expectedUploads << "completed with publishedFileID:" << publishedFileId;
                } else {
                    qWarning() << "Upload failed for item:" << publishedFileId;
                }
            });
    }

    // Step 7: Click the Upload button
    auto* btnUploadProjects = m_window->findChild<QQuickItem*>("btnUploadProjects");
    QVERIFY2(btnUploadProjects, "Upload projects button not found");
    QVERIFY2(btnUploadProjects->isEnabled(), "Upload button should be enabled after selecting items");
    clickItem(btnUploadProjects);
    QTest::qWait(1000);

    // Step 8: Wait for all uploads to complete
    const int uploadTimeoutMs = 180000; // 3 minutes for multiple uploads
    const bool uploadSuccess = waitForCondition([&uploadsCompleted, expectedUploads]() {
        return uploadsCompleted >= expectedUploads;
    },
        uploadTimeoutMs);
    QVERIFY2(uploadSuccess, "Uploads did not complete within timeout");
    QVERIFY2(!uploadedFileIDs.isEmpty(), "No valid publishedFileIDs received from uploads");

    qInfo() << "All" << uploadsCompleted << "uploads completed. IDs:" << uploadedFileIDs;

    // Step 9: Click Finish button to return to start page
    auto* btnFinish = m_window->findChild<QQuickItem*>("btnFinish");
    QVERIFY2(btnFinish, "Finish button not found");
    bool finishEnabled = waitForCondition([btnFinish]() { return btnFinish->isEnabled(); }, 5000);
    QVERIFY2(finishEnabled, "Finish button did not become enabled");
    clickItem(btnFinish);
    QTest::qWait(1000);

    // Step 10: Navigate to Profile page to delete uploaded items
    auto* btnSteamProfile = m_window->findChild<QQuickItem*>("btnSteamProfile");
    QVERIFY2(btnSteamProfile, "Profile button not found");
    clickItem(btnSteamProfile);
    QTest::qWait(2000);

    // Step 11: Verify we're on the profile page
    auto* profilePage = m_window->findChild<QQuickItem*>("WorkshopProfilePage");
    QVERIFY2(profilePage, "Profile page not found");

    // Step 12: Wait for user items to load
    QTest::qWait(3000);

    // Step 13: Delete all uploaded workshop items
    int deleteCount = 0;
    int deleteSuccessCount = 0;

    if (m_steamWorkshop) {
        for (const auto& fileID : uploadedFileIDs) {
            bool deleteCompleted = false;
            bool deleteSuccess = false;

            auto connection = QObject::connect(m_steamWorkshop, &ScreenPlayWorkshop::SteamWorkshop::workshopItemDeleted,
                this, [&deleteCompleted, &deleteSuccess, fileID](bool success, QVariant publishedFileID) {
                    if (publishedFileID == fileID) {
                        deleteCompleted = true;
                        deleteSuccess = success;
                        qInfo() << "Delete completed for" << publishedFileID << ", success:" << success;
                    }
                });

            const quint64 itemId = fileID.toULongLong();
            qInfo() << "Requesting deletion of workshop item:" << itemId;
            m_steamWorkshop->deleteItem(fileID);

            const bool deleteFinished = waitForCondition([&deleteCompleted]() { return deleteCompleted; }, 30000);
            QObject::disconnect(connection);

            ++deleteCount;
            if (deleteFinished && deleteSuccess) {
                ++deleteSuccessCount;
            } else {
                qWarning() << "Failed to delete item:" << itemId;
            }

            QTest::qWait(500); // Small delay between deletions
        }
    }

    qInfo() << "Deleted" << deleteSuccessCount << "/" << deleteCount << "items";
    QVERIFY2(deleteSuccessCount == deleteCount, "Not all workshop items were deleted successfully");

    qInfo() << "✓ Workshop multi-upload and delete test completed successfully";
}

QTEST_MAIN(WorkshopUploadTest)

#include "tst_workshop_upload.moc"
