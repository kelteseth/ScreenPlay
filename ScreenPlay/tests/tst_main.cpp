/****************************************************************************
**
** Copyright (C) 2020 Elias Steurer (Kelteseth)
** Contact: https://screen-play.app
**
** This file is part of ScreenPlay. ScreenPlay is licensed under a dual license in
** order to ensure its sustainability. When you contribute to ScreenPlay
** you accept that your work will be available under the two following licenses:
**
** $SCREENPLAY_BEGIN_LICENSE$
**
** #### Affero General Public License Usage (AGPLv3)
** Alternatively, this file may be used under the terms of the GNU Affero
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file "ScreenPlay License.md" included in the
** packaging of this App. Please review the following information to
** ensure the GNU Affero Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/agpl-3.0.en.html.
**
** #### Commercial License
** This code is owned by Elias Steurer. By changing/adding to the code you agree to the
** terms written in:
**  * Legal/corporate_contributor_license_agreement.md - For corporate contributors
**  * Legal/individual_contributor_license_agreement.md - For individual contributors
**
** #### Additional Limitations to the AGPLv3 and Commercial Lincese
** This License does not grant any rights in the trademarks,
** service marks, or logos.
**
**
** $SCREENPLAY_END_LICENSE$
**
****************************************************************************/

#include "ScreenPlay/app.h"
#include "ScreenPlay/create.h"
#include <QApplication>
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QGuiApplication>
#include <QQuickItem>
#include <QtTest>

Q_IMPORT_QML_PLUGIN(ScreenPlayQmlPlugin)
Q_IMPORT_QML_PLUGIN(ScreenPlayUtilPlugin)
#ifdef SCREENPLAY_STEAM
Q_IMPORT_QML_PLUGIN(ScreenPlayWorkshopPlugin)
#endif
class ScreenPlayTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase()
    {
        Q_INIT_RESOURCE(Resources);

        app.init();
        m_window = qobject_cast<QQuickWindow*>(app.mainWindowEngine()->rootObjects().first());
        QVERIFY(m_window);
        QVERIFY(QTest::qWaitForWindowExposed(m_window));

        QTest::qWait(1000);
        m_installedSidebar = m_window->findChild<QQuickItem*>("installedSidebar");
        QVERIFY(m_installedSidebar);

        QTest::qWait(1000);
    }

    void start_shutdown_wallpaper();
    void import_convert_video();

private:
    QQuickWindow* m_window = nullptr;
    QQuickItem* m_installedSidebar = nullptr;
    ScreenPlay::App app;
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

void ScreenPlayTest::import_convert_video()
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

    const QString originalVideoPath = QString(SOURCE_DIR) + "/ScreenPlay/assets/tests/video_import.mp4";
    qInfo() << originalVideoPath;

    QVERIFY(QMetaObject::invokeMethod(createWallpaperInit,
        QString("startConvert").toLatin1().constData(),
        Qt::ConnectionType::AutoConnection,
        Q_ARG(QVariant, originalVideoPath),
        Q_ARG(QVariant, 1))); // VideoCodec::VP9

    QTest::qWait(1000);
    // Wait for Create::createWallpaperStart
    {
        ImportVideoState::ImportVideoState status = ImportVideoState::ImportVideoState::Idle;
        QObject::connect(app.create(), &Create::createWallpaperStateChanged, this, [&status](ImportVideoState::ImportVideoState state) {
            status = state;
        });

        while (true) {
            QSignalSpy videoConvertFinishSpy(app.create(), &Create::createWallpaperStateChanged);
            if (status == ImportVideoState::ImportVideoState::Finished || status == ImportVideoState::ImportVideoState::Failed) {
                QVERIFY(status == ImportVideoState::ImportVideoState::Finished);
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
        ImportVideoState::ImportVideoState status = ImportVideoState::ImportVideoState::Idle;
        QObject::connect(app.create(), &Create::createWallpaperStateChanged, this, [&status](ImportVideoState::ImportVideoState state) {
            status = state;
        });

        while (true) {
            QSignalSpy videoConvertFinishSpy(app.create(), &Create::createWallpaperStateChanged);
            if (status == ImportVideoState::ImportVideoState::CreateProjectFileFinished || status == ImportVideoState::ImportVideoState::CreateProjectFileError || status == ImportVideoState::ImportVideoState::CopyFilesError) {
                QVERIFY(status == ImportVideoState::ImportVideoState::CreateProjectFileFinished);
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

void ScreenPlayTest::start_shutdown_wallpaper()
{
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
    auto* monitorSelection = m_installedSidebar->findChild<QQuickItem*>("monitorSelection");
    QVERIFY(monitorSelection);
    QTest::qWait(1000);
    QVERIFY(QMetaObject::invokeMethod(monitorSelection,
        QString("selectOnly").toLatin1().constData(),
        Qt::ConnectionType::AutoConnection,
        Q_ARG(QVariant, 0))); // First monitor

    QTest::qWait(1000);
    auto* btnSetWallpaper = m_installedSidebar->findChild<QQuickItem*>("btnSetWallpaper");
    QVERIFY(btnSetWallpaper);
    clickItem(btnSetWallpaper);
    QTest::qWait(10000);
}

QTEST_MAIN(ScreenPlayTest)

#include "tst_main.moc"
