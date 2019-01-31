#include <QDebug>
#include <QDir>
#include <QFontDatabase>
#include <QGuiApplication>
#include <QIcon>
#include <QLibrary>
#include <QObject>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlEngine>
#include <QThread>

#include <QQuickView>
#include <QScreen>
#include <QStringList>
#include <QUrl>
#include <QVariant>
#include <QWindow>
#include <QtGlobal>
#include <QtQuick/QQuickItem>
#include <QtWebEngine>

#ifdef Q_OS_WIN
#include <qt_windows.h>
#endif

#include "src/create.h"
#include "src/installedlistfilter.h"
#include "src/installedlistmodel.h"
#include "src/monitorlistmodel.h"
#include "src/profilelistmodel.h"
#include "src/qmlutilities.h"
#include "src/screenplay.h"
#include "src/sdkconnector.h"
#include "src/settings.h"

int main(int argc, char* argv[])
{

    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication::setOrganizationName("Aimber");
    QGuiApplication::setOrganizationDomain("screen-play.app");
    QGuiApplication::setApplicationName("ScreenPlay");
    QGuiApplication::setApplicationVersion("0.2.0");

    QGuiApplication app(argc, argv);
    app.setQuitOnLastWindowClosed(false);
    app.setWindowIcon(QIcon(":/assets/icons/favicon.ico"));

    // This gives us nice clickable output in QtCreator
    qSetMessagePattern("%{if-category}%{category}: %{endif}%{message}\n   Loc: [%{file}:%{line}]");

    QtWebEngine::initialize();

    QFontDatabase::addApplicationFont(":/assets/fonts/LibreBaskerville-Italic.ttf");
    QFontDatabase::addApplicationFont(":/assets/fonts/Roboto-Light.ttf");
    QFontDatabase::addApplicationFont(":/assets/fonts/Roboto-Regular.ttf");
    QFontDatabase::addApplicationFont(":/assets/fonts/Roboto-Thin.ttf");
    QFontDatabase::addApplicationFont(":/assets/fonts/RobotoMono-Light.ttf");
    QFontDatabase::addApplicationFont(":/assets/fonts/RobotoMono-Thin.ttf");

    QMLUtilities qmlUtil;
    InstalledListModel installedListModel;
    InstalledListFilter installedListFilter(&installedListModel);
    MonitorListModel monitorListModel(&app);
    ProfileListModel profileListModel;
    SDKConnector sdkConnector;

    // Create settings in the end because for now it depends on
    // such things as the profile list model to complete
    // It will also set the m_absoluteStoragePath in  profileListModel and installedListModel
    Settings settings(&profileListModel, &monitorListModel, &installedListModel, &sdkConnector, &app);
    ScreenPlay screenPlay(&installedListModel, &settings, &monitorListModel, &app, &sdkConnector);
    Create create(&settings, &qmlUtil);

    QDir SPWorkingDir(QDir::currentPath());
    QDir SPBaseDir(QDir::currentPath());

#ifdef QT_DEBUG
    qDebug() << "Starting in Debug mode!";

    if (SPWorkingDir.cdUp()) {
#ifdef Q_OS_OSX
        settings.setScreenPlayWindowPath(QUrl::fromUserInput(SPWorkingDir.path() + "/../../../ScreenPlayWindow/ScreenPlayWindow.app/Contents/MacOS/ScreenPlayWindow").toLocalFile());
        settings.setScreenPlayWidgetPath(QUrl::fromUserInput(SPWorkingDir.path() + "/../../../ScreenPlayWidget/ScreenPlayWidget.app/Contents/MacOS/ScreenPlayWidget").toLocalFile());
        qDebug() << "Setting ScreenPlayWindow Path to " << settings.getScreenPlayWindowPath();
        qDebug() << "Setting ScreenPlayWdiget Path to " << settings.getScreenPlayWidgetPath();
#endif

#ifdef Q_OS_WIN
        settings.setScreenPlayWindowPath(QUrl(SPWorkingDir.path() + "/ScreenPlayWindow/debug/ScreenPlayWindow.exe"));
        settings.setScreenPlayWidgetPath(QUrl(SPWorkingDir.path() + "/ScreenPlayWidget/debug/ScreenPlayWidget.exe"));
#endif
    }

    // We need to detect the right base path so we can copy later the example projects
    SPBaseDir.cdUp();
    SPBaseDir.cdUp();
    SPBaseDir.cd("ScreenPlay");
    SPBaseDir.cd("ScreenPlay");
    settings.setScreenPlayBasePath(QUrl(SPBaseDir.path()));
#endif
#ifdef QT_NO_DEBUG
    qDebug() << "Starting in Release mode!";
    settings.setScreenPlayBasePath(QUrl(SPWorkingDir.path()));

    // If we build in the release version we must be cautious!
    // The working dir in steam is the ScreenPlay.exe location
    // In QtCreator is the dir above ScreenPlay.exe (!)

    SPWorkingDir.cdUp();
    SPWorkingDir.cd("ScreenPlayWindow");

    if (QDir(SPWorkingDir.path() + "/release").exists()) {
        // If started by QtCreator
        SPWorkingDir.cd("release");
        settings.setScreenPlayWindowPath(QUrl(SPWorkingDir.path() + "/ScreenPlayWindow.exe"));
        SPWorkingDir.cdUp();
        SPWorkingDir.cdUp();
        SPWorkingDir.cd("ScreenPlayWidget");
        SPWorkingDir.cd("release");
        settings.setScreenPlayWidgetPath(QUrl(SPWorkingDir.path() + "/ScreenPlayWidget.exe"));
    } else {
        // If started by Steam
        settings.setScreenPlayWindowPath(QUrl("ScreenPlayWindow.exe"));
        settings.setScreenPlayWidgetPath(QUrl("ScreenPlayWidget.exe"));
    }
#endif

    // All the list need the default path from the settings
    // to know where to look for the files
    profileListModel.loadProfiles();
    settings.loadActiveProfiles();

    QQmlApplicationEngine mainWindowEngine;

    mainWindowEngine.rootContext()->setContextProperty("screenPlay", &screenPlay);
    mainWindowEngine.rootContext()->setContextProperty("screenPlayCreate", &create);
    mainWindowEngine.rootContext()->setContextProperty("utility", &qmlUtil);
    mainWindowEngine.rootContext()->setContextProperty("installedListFilter", &installedListFilter);
    mainWindowEngine.rootContext()->setContextProperty("monitorListModel", &monitorListModel);
    mainWindowEngine.rootContext()->setContextProperty("installedListModel", &installedListModel);
    mainWindowEngine.rootContext()->setContextProperty("profileListModel", &profileListModel);
    mainWindowEngine.rootContext()->setContextProperty("screenPlaySettings", &settings);

    mainWindowEngine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    installedListFilter.sortByRoleType("All");
    installedListModel.loadScreens();

    // Instead of setting "renderType: Text.NativeRendering" every time
    // we can set it here once :)
    auto* window = static_cast<QQuickWindow*>(mainWindowEngine.rootObjects().first());
    window->setTextRenderType(QQuickWindow::TextRenderType::NativeTextRendering);

    // Set visible if the -silent parameter was not set
    QStringList argumentList = app.arguments();
    if (!argumentList.contains("-silent")) {
        settings.setMainWindowVisible(true);
    }

    return app.exec();
}
