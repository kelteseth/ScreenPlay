
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


int main(int argc, char* argv[])
{

    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseOpenGLES);

    QGuiApplication app(argc, argv);
    app.setQuitOnLastWindowClosed(false);

    QTranslator trsl;
    trsl.load(":/languages/ScreenPlay_de.qm");
    app.installTranslator(&trsl);

    QObject::connect(&app, &QGuiApplication::screenRemoved, [&]() { qDebug() << "removed"; });

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

    bool steamErrorRestart = false;
    bool steamErrorAPIInit = false;

    if (SteamAPI_RestartAppIfNecessary(steamID)) {
        qWarning() << "SteamAPI_RestartAppIfNecessary";
        steamErrorRestart = true;
    }

    if (!SteamAPI_Init()) {
        qWarning() << "Could not init steam sdk!";
        steamErrorAPIInit = true;
    }

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

#ifdef QT_DEBUG
    if (SPWorkingDir.cdUp()) {
        settings.setScreenPlayWindowPath(QUrl(SPWorkingDir.path() + "/ScreenPlayWindow/debug/ScreenPlayWindow.exe"));
    }
#elif QT_NO_DEBUG

    // If we build in the release version we must be cautious!
    // The working dir in steam is the ScreenPlay.exe location
    // In QtCreator is the dir above ScreenPlay.exe (!)

    SPWorkingDir.cdUp();
    SPWorkingDir.cd("ScreenPlayWindow");

    if (QDir(SPWorkingDir.path() + "/release").exists()) {
        // If started by QtCreator
        SPWorkingDir.cd("release");
        settings.setScreenPlayWindowPath(QUrl(SPWorkingDir.path() + "/ScreenPlayWindow.exe"));
    } else {
        // If started by Steam
        settings.setScreenPlayWindowPath(QUrl("ScreenPlayWindow.exe"));
    }

#endif
    SteamWorkshop steamWorkshop(steamID, &steamWorkshopListModel, &settings);

    // All the list need the default path from the settings
    // to know where to look for the files
    profileListModel.loadProfiles();
    settings.loadActiveProfiles();

    QQmlApplicationEngine errorWindowEngine,mainWindowEngine;
    StartupError suError(&mainWindowEngine, &errorWindowEngine);
    mainWindowEngine.rootContext()->setContextProperty("screenPlay", &screenPlay);
    mainWindowEngine.rootContext()->setContextProperty("utility", &qmlUtil);
    mainWindowEngine.rootContext()->setContextProperty("installedListFilter", &installedListFilter);
    mainWindowEngine.rootContext()->setContextProperty("workshopListModel", &steamWorkshopListModel);
    mainWindowEngine.rootContext()->setContextProperty("monitorListModel", &monitorListModel);
    mainWindowEngine.rootContext()->setContextProperty("installedListModel", &installedListModel);
    mainWindowEngine.rootContext()->setContextProperty("profileListModel", &profileListModel);
    mainWindowEngine.rootContext()->setContextProperty("screenPlaySettings", &settings);
    mainWindowEngine.rootContext()->setContextProperty("steamWorkshop", &steamWorkshop);

    if (steamErrorRestart || steamErrorAPIInit) {
        errorWindowEngine.load(QUrl(QStringLiteral("qrc:/qml/StartupErrorWindow.qml")));
        errorWindowEngine.rootContext()->setContextProperty("suError", &suError);
        settings.setOfflineMode(true);

    } else {
        mainWindowEngine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    }

    // Set visible if the -silent parameter was not set
    QStringList argumentList = app.arguments();
    if (!argumentList.contains("-silent")) {
        settings.setMainWindowVisible(true);
    }

    // Timer for steam polls. WTF?
    QTimer timer;
    QObject::connect(&timer, &QTimer::timeout, [&]() { SteamAPI_RunCallbacks(); });
    timer.setInterval(100);
    timer.start();

    QObject::connect(&steamWorkshop, &SteamWorkshop::workshopSearchResult,
        &steamWorkshopListModel, &SteamWorkshopListModel::append, Qt::QueuedConnection);

    int status = app.exec();

    SteamAPI_Shutdown();

    return status;
}
