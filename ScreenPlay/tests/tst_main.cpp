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

#include "app.h"
#include "create.h"
#include <QApplication>
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QGuiApplication>
#include <QtTest>
#include <QtWebEngine>
#define DOCTEST_CONFIG_IMPLEMENT
#define DOCTEST_CONFIG_SUPER_FAST_ASSERTS
#include <doctest/doctest.h>

class ScreenPlayTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase()
    {
        Q_INIT_RESOURCE(ScreenPlayQML);
        Q_INIT_RESOURCE(ScreenPlayAssets);

        app.init();
        m_window = qobject_cast<QQuickWindow*>(app.mainWindowEngine()->rootObjects().first());
        QVERIFY(m_window);
        QVERIFY(QTest::qWaitForWindowExposed(m_window));

        QTest::qWait(1000);
    }
    void import_convert_video();

private:
    QQuickWindow* m_window = nullptr;
    ScreenPlay::App app;
};

/*!
 *  For some reason a direct findChild does not work for item
 *  delegates.
 *  https://stackoverflow.com/questions/36767512/how-to-access-qml-listview-delegate-items-from-c
 *
 */
QQuickItem* findItemDelegate(QQuickItem* listView)
{
    if (!listView->property("contentItem").isValid())
        return {};

    auto contentItem = listView->property("contentItem").value<QQuickItem*>();
    auto contentItemChildren = contentItem->childItems();
    QQuickItem* videoImportConvertButton {};
    for (auto childItem : contentItemChildren) {
        if (childItem->objectName() == "videoImportConvert")
            return childItem;
    }
    return {};
}

void clickItem(QQuickItem* item)
{
    QQuickWindow* itemWindow = item->window();
    QVERIFY(itemWindow);
    auto centre = item->mapToScene(QPoint(item->width() / 2, item->height() / 2)).toPoint();
    qInfo() << "click_:" << centre;
    QTest::mouseClick(itemWindow, Qt::LeftButton, Qt::NoModifier, centre);
}

void ScreenPlayTest::import_convert_video()
{

    using namespace ScreenPlay;
    auto* createTab = m_window->findChild<QQuickItem*>("Create");
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
    QQuickItem* videoImportConvertButton = findItemDelegate(wizardsListView);
    QVERIFY(videoImportConvertButton);
    clickItem(videoImportConvertButton);
    QTest::qWait(300);

    auto* createWallpaperInit = m_window->findChild<QQuickItem*>("createWallpaperInit");
    QVERIFY(createWallpaperInit);

    QVERIFY(QMetaObject::invokeMethod(createWallpaperInit,
        QString("startConvert").toLatin1().constData(),
        Qt::ConnectionType::AutoConnection,
        Q_ARG(QVariant, "file:///D:/Video Loop/bbb.mp4"),
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
}

QTEST_MAIN(ScreenPlayTest)

#include "tst_main.moc"
