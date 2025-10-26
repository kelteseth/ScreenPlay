// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QElapsedTimer>
#include <QFile>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickItem>
#include <QQuickStyle>
#include <QQuickWindow>
#include <QtTest>
#include <memory>

#include "ScreenPlay/app.h"
#include "ScreenPlay/create.h"

#include "CMakeVariables.h"

class ScreenPlayGuiTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase()
    {
        qInfo() << "Initializing GUI test case...";
        
        // Setup QGuiApplication similar to main.cpp
        QGuiApplication::setOrganizationName("ScreenPlay");
        QGuiApplication::setOrganizationDomain("screen-play.app");
        QGuiApplication::setApplicationName("ScreenPlay");
        QGuiApplication::setApplicationVersion(QString(SCREENPLAY_VERSION));
        
        // Initialize QML engine and get the App singleton instance (like main.cpp does)
        QQuickStyle::setStyle("Material");
        m_engine = std::make_shared<QQmlApplicationEngine>();
        m_app = m_engine->singletonInstance<ScreenPlay::App*>("ScreenPlay", "App");
        QVERIFY(m_app);
        m_app->setEngine(m_engine);
        m_engine->loadFromModule("ScreenPlay", "ScreenPlayMain");
        
        // Wait for QML to load
        QTest::qWait(2000);
        
        // Get window and UI elements
        if (!m_engine->rootObjects().isEmpty()) {
            m_window = qobject_cast<QQuickWindow*>(m_engine->rootObjects().first());
        }
        
        qInfo() << "GUI test case initialized successfully";
    }

    void cleanupTestCase()
    {
        m_window = nullptr;
        m_app = nullptr;
        m_engine.reset();
        qInfo() << "GUI test case cleaned up";
    }

    void start_shutdown_wallpaper();
    void import_convert_video();
    void import_video_no_conversion();

private:
    std::shared_ptr<QQmlApplicationEngine> m_engine;
    ScreenPlay::App* m_app = nullptr;
    QQuickWindow* m_window = nullptr;
};

/*!
 *  For some reason a direct findChild does not work for item
 *  delegates.
 *  https://stackoverflow.com/questions/36767512/how-to-access-qml-listview-delegate-items-from-c
 *
 */
QQuickItem* findItemDelegate(QQuickItem* listView, const QString objectName)
{
    if (!listView->property("contentItem").isValid())
        return {};

    auto contentItem = listView->property("contentItem").value<QQuickItem*>();
    auto contentItemChildren = contentItem->childItems();
    QQuickItem* videoImportConvertButton {};
    for (auto childItem : contentItemChildren) {
        if (childItem->objectName() == objectName)
            return childItem;
    }
    return {};
}

void clickItem(QQuickItem* item, Qt::MouseButton button = Qt::LeftButton)
{
    QQuickWindow* itemWindow = item->window();
    QVERIFY(itemWindow);
    auto centre = item->mapToScene(QPoint(item->width() / 2, item->height() / 2)).toPoint();
    qInfo() << "click_:" << centre;
    QTest::mouseClick(itemWindow, button, Qt::NoModifier, centre);
}

void ScreenPlayGuiTest::import_convert_video()
{
    using namespace ScreenPlay;
    
    auto* createTab = m_window->findChild<QQuickItem*>("createTab");
    QVERIFY(createTab);
    clickItem(createTab);
    QTest::qWait(300);
    
    auto* stackView = m_window->findChild<QQuickItem*>("stackView");
    QVERIFY(stackView);
    QVERIFY(stackView->property("currentItem").isValid());
    auto* createView = qvariant_cast<QQuickItem*>(stackView->property("currentItem"));
    QVERIFY(createView);
    QTest::qWait(300);
    
    auto* wizardsListView = m_window->findChild<QQuickItem*>("wizardsListView");
    QVERIFY(wizardsListView);
    QQuickItem* videoImportConvertButton = findItemDelegate(wizardsListView, "videoImportConvert");
    QVERIFY(videoImportConvertButton);
    clickItem(videoImportConvertButton);
    QTest::qWait(300);

    auto* createWallpaperInit = m_window->findChild<QQuickItem*>("createWallpaperInit");
    QVERIFY(createWallpaperInit);

    const QString originalVideoPath = QString(SCREENPLAY_SOURCE_DIR) + "/Content/wallpaper_video_nebula_h264/mantissa.xyz_loop_072.mp4";
    qInfo() << originalVideoPath;

    QVERIFY(QMetaObject::invokeMethod(createWallpaperInit,
        QString("startConvert").toLatin1().constData(),
        Qt::ConnectionType::AutoConnection,
        Q_ARG(QVariant, originalVideoPath),
        Q_ARG(QVariant, 1))); // VideoCodec::VP9

    QTest::qWait(1000);
    // Wait for Create::createWallpaperStart
    {
        Import::State status = Import::State::Idle;
        QObject::connect(m_app->create(), &Create::createWallpaperStateChanged, this, [&status](Import::State state) {
            status = state;
        });

        while (true) {
            QSignalSpy videoConvertFinishSpy(m_app->create(), &Create::createWallpaperStateChanged);
            if (status == Import::State::Finished || status == Import::State::Failed) {
                QVERIFY(status == Import::State::Finished);
                QTest::qWait(1000); // Wait for the ui to process the event
                break;
            }
            videoConvertFinishSpy.wait();
        }
    }

    QTest::qWait(1000);
    auto* btnSave = m_window->findChild<QQuickItem*>("btnSave");
    QVERIFY(btnSave);
    clickItem(btnSave);

    // Wait for Create::saveWallpaper
    {
        Import::State status = Import::State::Idle;
        QObject::connect(m_app->create(), &Create::createWallpaperStateChanged, this, [&status](Import::State state) {
            status = state;
        });

        while (true) {
            QSignalSpy videoConvertFinishSpy(m_app->create(), &Create::createWallpaperStateChanged);
            if (status == Import::State::CreateProjectFileFinished || status == Import::State::CreateProjectFileError || status == Import::State::CopyFilesError) {
                QVERIFY(status == Import::State::CreateProjectFileFinished);
                QTest::qWait(1000); // Wait for the ui to process the event
                break;
            }
            videoConvertFinishSpy.wait();
        }
    }

    QTest::qWait(1000);
    auto* installedTab = m_window->findChild<QQuickItem*>("installedTab");
    QVERIFY(installedTab);
    clickItem(installedTab);
    QTest::qWait(1000);

    auto* gridView = m_window->findChild<QQuickItem*>("gridView");
    QVERIFY(gridView);
    const QString installedListItemIndex = "0";
    auto* firstInstalledItem = findItemDelegate(gridView, "installedItem" + installedListItemIndex);
    QVERIFY(firstInstalledItem);
    clickItem(firstInstalledItem, Qt::RightButton);
    QTest::qWait(1000);
    auto* installedItemContextMenu = m_window->findChild<QQuickItem*>("installedItemContextMenu");
    QVERIFY(installedItemContextMenu);

    auto* removeItem = findItemDelegate(installedItemContextMenu, "removeItem");
    QVERIFY(removeItem);
    clickItem(removeItem);
}

void ScreenPlayGuiTest::start_shutdown_wallpaper()
{
    auto* installedSidebar = m_window->findChild<QQuickItem*>("installedSidebar");
    QVERIFY(installedSidebar);
    
    QTest::qWait(1000);
    auto* stackView = m_window->findChild<QQuickItem*>("stackView");
    QVERIFY(stackView);
    QVERIFY(stackView->property("currentItem").isValid());
    auto* installedView = qvariant_cast<QQuickItem*>(stackView->property("currentItem"));
    QVERIFY(installedView);
    QTest::qWait(1000);
    
    auto* gridView = m_window->findChild<QQuickItem*>("gridView");
    QVERIFY(gridView);
    const QString installedListItemIndex = "0";
    auto* firstInstalledItem = findItemDelegate(gridView, "installedItem" + installedListItemIndex);
    QVERIFY(firstInstalledItem);
    clickItem(firstInstalledItem);
    
    auto* monitorSelection = installedSidebar->findChild<QQuickItem*>("monitorSelection");
    QVERIFY(monitorSelection);
    QTest::qWait(1000);
    QVERIFY(QMetaObject::invokeMethod(monitorSelection,
        QString("selectOnly").toLatin1().constData(),
        Qt::ConnectionType::AutoConnection,
        Q_ARG(QVariant, 0))); // First monitor

    QTest::qWait(1000);
    auto* btnSetWallpaper = installedSidebar->findChild<QQuickItem*>("btnSetWallpaper");
    QVERIFY(btnSetWallpaper);
    clickItem(btnSetWallpaper);
    QTest::qWait(10000);
}

void ScreenPlayGuiTest::import_video_no_conversion()
{
    using namespace ScreenPlay;
    
    auto* createTab = m_window->findChild<QQuickItem*>("createTab");
    QVERIFY(createTab);
    clickItem(createTab);
    QTest::qWait(300);
    
    auto* stackView = m_window->findChild<QQuickItem*>("stackView");
    QVERIFY(stackView);
    QVERIFY(stackView->property("currentItem").isValid());
    auto* createView = qvariant_cast<QQuickItem*>(stackView->property("currentItem"));
    QVERIFY(createView);
    QTest::qWait(300);
    
    auto* wizardsListView = m_window->findChild<QQuickItem*>("wizardsListView");
    QVERIFY(wizardsListView);
    QQuickItem* videoImportConvertButton = findItemDelegate(wizardsListView, "videoImportConvert");
    QVERIFY(videoImportConvertButton);
    clickItem(videoImportConvertButton);
    QTest::qWait(300);

    auto* createWallpaperInit = m_window->findChild<QQuickItem*>("createWallpaperInit");
    QVERIFY(createWallpaperInit);

    // Use H.264 video with NoConversion to test that it skips encoding
    const QString h264VideoPath = QString(SCREENPLAY_SOURCE_DIR) + "/Content/wallpaper_video_nebula_h264/mantissa.xyz_loop_072.mp4";
    QVERIFY(QFile::exists(h264VideoPath));
    qInfo() << "Testing no-conversion import with:" << h264VideoPath;

    // Use VideoCodec::NoConversion (value 1 in the enum)
    QVERIFY(QMetaObject::invokeMethod(createWallpaperInit,
        QString("startConvert").toLatin1().constData(),
        Qt::ConnectionType::AutoConnection,
        Q_ARG(QVariant, h264VideoPath),
        Q_ARG(QVariant, static_cast<int>(Video::VideoCodec::NoConversion))));

    QTest::qWait(1000);
    
    // Wait for Create::createWallpaperStart - should be much faster since no conversion
    {
        Import::State status = Import::State::Idle;
        QObject::connect(m_app->create(), &Create::createWallpaperStateChanged, this, [&status](Import::State state) {
            qInfo() << "Import state changed to:" << static_cast<int>(state);
            status = state;
        });

        QElapsedTimer timer;
        timer.start();
        
        while (true) {
            QSignalSpy videoConvertFinishSpy(m_app->create(), &Create::createWallpaperStateChanged);
            if (status == Import::State::Finished || status == Import::State::Failed) {
                QVERIFY(status == Import::State::Finished);
                qint64 elapsed = timer.elapsed();
                qInfo() << "✓ No-conversion import finished in" << elapsed << "ms (should be fast, no encoding)";
                QTest::qWait(1000); // Wait for the ui to process the event
                break;
            }
            // Timeout after 30 seconds (should be much faster without conversion)
            if (timer.elapsed() > 30000) {
                QFAIL("Import timed out");
            }
            videoConvertFinishSpy.wait(1000);
        }
    }

    QTest::qWait(1000);
    auto* btnSave = m_window->findChild<QQuickItem*>("btnSave");
    QVERIFY(btnSave);
    clickItem(btnSave);

    // Wait for Create::saveWallpaper
    {
        Import::State status = Import::State::Idle;
        QObject::connect(m_app->create(), &Create::createWallpaperStateChanged, this, [&status](Import::State state) {
            status = state;
        });

        while (true) {
            QSignalSpy videoConvertFinishSpy(m_app->create(), &Create::createWallpaperStateChanged);
            if (status == Import::State::CreateProjectFileFinished || status == Import::State::CreateProjectFileError || status == Import::State::CopyFilesError) {
                QVERIFY(status == Import::State::CreateProjectFileFinished);
                qInfo() << "✓ Project file created successfully";
                QTest::qWait(1000);
                break;
            }
            videoConvertFinishSpy.wait();
        }
    }

    // Cleanup - remove the imported wallpaper
    QTest::qWait(1000);
    auto* installedTab = m_window->findChild<QQuickItem*>("installedTab");
    QVERIFY(installedTab);
    clickItem(installedTab);
    QTest::qWait(1000);

    auto* gridView = m_window->findChild<QQuickItem*>("gridView");
    QVERIFY(gridView);
    const QString installedListItemIndex = "0";
    auto* firstInstalledItem = findItemDelegate(gridView, "installedItem" + installedListItemIndex);
    QVERIFY(firstInstalledItem);
    clickItem(firstInstalledItem, Qt::RightButton);
    QTest::qWait(1000);
    auto* installedItemContextMenu = m_window->findChild<QQuickItem*>("installedItemContextMenu");
    QVERIFY(installedItemContextMenu);

    auto* removeItem = findItemDelegate(installedItemContextMenu, "removeItem");
    QVERIFY(removeItem);
    clickItem(removeItem);
    
    qInfo() << "✓ No-conversion import test completed successfully";
}

QTEST_MAIN(ScreenPlayGuiTest)

#include "tst_ScreenPlayGui.moc"
