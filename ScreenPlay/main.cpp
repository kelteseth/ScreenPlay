#include <QDebug>
#include <QDir>
#include <QFontDatabase>
#include <QGuiApplication>
#include <QIcon>
#include <QLibrary>
#include <QModelIndex>
#include <QObject>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickStyle>
#include <QQuickView>
#include <QScreen>
#include <QStringList>
#include <QTimer>
#include <QTransform>
#include <QUrl>
#include <QVariant>
#include <QWindow>
#include <QtQuick/QQuickItem>

#include <QtGlobal>
#ifdef Q_OS_WIN
    #include <qt_windows.h>
#endif
#include "qmlutilities.h"
#include "installedlistfilter.h"
#include "installedlistmodel.h"
#include "monitorlistmodel.h"
#include "profilelistmodel.h"
#include "sdkconnector.h"
#include "settings.h"
#include "startuperror.h"
#include "steam/steam_api.h"
#include "steamworkshop.h"
#include "steamworkshoplistmodel.h"
#include "screenplay.h"
#include "create.h"


int main(int argc, char* argv[])
{

    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication::setAttribute(Qt::AA_UseOpenGLES);

    QGuiApplication app(argc, argv);
    app.setQuitOnLastWindowClosed(false);

    QTranslator trsl;
    trsl.load(":/translations/ScreenPlay_de.qm");
    app.installTranslator(&trsl);

    QFontDatabase::addApplicationFont(":/assets/fonts/LibreBaskerville-Italic.ttf");
    QFontDatabase::addApplicationFont(":/assets/fonts/Roboto-Light.ttf");
    QFontDatabase::addApplicationFont(":/assets/fonts/Roboto-Regular.ttf");
    QFontDatabase::addApplicationFont(":/assets/fonts/Roboto-Thin.ttf");
    QFontDatabase::addApplicationFont(":/assets/fonts/RobotoMono-Light.ttf");
    QFontDatabase::addApplicationFont(":/assets/fonts/RobotoMono-Thin.ttf");

    AppId_t steamID = 672870;
    QCoreApplication::setOrganizationName("Aimber");
    QCoreApplication::setOrganizationDomain("aimber.net");
    QCoreApplication::setApplicationName("ScreenPlay");
    QCoreApplication::setApplicationVersion("0.1.0");

    app.setWindowIcon(QIcon(":/assets/icons/favicon.ico"));

    QMLUtilities qmlUtil;
    InstalledListModel installedListModel;
    MonitorListModel monitorListModel(&app);

    ProfileListModel profileListModel;
    SteamWorkshopListModel steamWorkshopListModel;
    SDKConnector sdkConnector;

    InstalledListFilter installedListFilter(&installedListModel);


    // Create settings in the end because for now it depends on
    // such things as the profile list model to complete
    // It will also set the m_absoluteStoragePath in  profileListModel and installedListModel
    Settings settings(&profileListModel, &monitorListModel, &installedListModel, &sdkConnector, steamID, &app);
    ScreenPlay screenPlay(&installedListModel,&settings,&monitorListModel,&app, &sdkConnector);
    QDir SPWorkingDir(QDir::currentPath());
    QDir SPBaseDir(QDir::currentPath());

#ifdef QT_DEBUG
    if (SPWorkingDir.cdUp()) {
        settings.setScreenPlayWindowPath(QUrl(SPWorkingDir.path() + "/ScreenPlayWindow/debug/ScreenPlayWindow.exe"));
        settings.setScreenPlayWidgetPath(QUrl(SPWorkingDir.path() + "/ScreenPlayWidget/debug/ScreenPlayWidget.exe"));
    }

    // We need to detect the right base path so we can copy later the example projects
    SPBaseDir.cdUp();
    SPBaseDir.cdUp();
    SPBaseDir.cd("ScreenPlay");
    SPBaseDir.cd("ScreenPlay");
    settings.setScreenPlayBasePath(QUrl(SPBaseDir.path()));

#elif
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
        settings.setScreenPlayWindowPath(QUrl("ScreenPlayWidget.exe"));
    }

#endif
    SteamWorkshop steamWorkshop(steamID, &steamWorkshopListModel, &settings);
    Create create(&settings,&qmlUtil);

    // All the list need the default path from the settings
    // to know where to look for the files
    profileListModel.loadProfiles();
    settings.loadActiveProfiles();

    QQmlApplicationEngine mainWindowEngine;
    mainWindowEngine.rootContext()->setContextProperty("screenPlay", &screenPlay);
    mainWindowEngine.rootContext()->setContextProperty("screenPlayCreate", &create);
    mainWindowEngine.rootContext()->setContextProperty("utility", &qmlUtil);
    mainWindowEngine.rootContext()->setContextProperty("installedListFilter", &installedListFilter);
    mainWindowEngine.rootContext()->setContextProperty("workshopListModel", &steamWorkshopListModel);
    mainWindowEngine.rootContext()->setContextProperty("monitorListModel", &monitorListModel);
    mainWindowEngine.rootContext()->setContextProperty("installedListModel", &installedListModel);
    mainWindowEngine.rootContext()->setContextProperty("profileListModel", &profileListModel);
    mainWindowEngine.rootContext()->setContextProperty("screenPlaySettings", &settings);
    mainWindowEngine.rootContext()->setContextProperty("steamWorkshop", &steamWorkshop);
    mainWindowEngine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    // Set visible if the -silent parameter was not set
    QStringList argumentList = app.arguments();
    if (!argumentList.contains("-silent")) {
        settings.setMainWindowVisible(true);
    }

    QObject::connect(&steamWorkshop, &SteamWorkshop::workshopSearchResult,
        &steamWorkshopListModel, &SteamWorkshopListModel::append, Qt::QueuedConnection);

    int status = app.exec();

    SteamAPI_Shutdown();

    return status;
}
