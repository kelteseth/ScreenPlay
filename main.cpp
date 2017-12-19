#include <QDebug>
#include <QDir>
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
#include <QTimer>
#include <QUrl>
#include <QVariant>
#include <QWindow>
#include <QtQuick/QQuickItem>
#include <qt_windows.h>

#include "installedlistmodel.h"
#include "monitorlistmodel.h"
#include "packagefilehandler.h"
#include "profilelistmodel.h"
#include "quazip5/quazip.h"
#include "settings.h"
#include "steam/steam_api.h"
#include "steamworkshop.h"
#include "steamworkshoplistmodel.h"
#include "widget.h"

int main(int argc, char* argv[])
{
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseOpenGLES);

    QGuiApplication app(argc, argv);

    AppId_t steamID = 672870;
    QCoreApplication::setOrganizationName("Aimber");
    QCoreApplication::setOrganizationDomain("aimber.net");
    QCoreApplication::setApplicationName("ScreenPlay");
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

    InstalledListModel installedListModel;
    MonitorListModel monitorListModel;
    PackageFileHandler packageFileHandler;
    ProfileListModel profileListModel;
    SteamWorkshopListModel steamWorkshopListModel;

    // Create settings in the end because for now it depends on
    // such things as the profile list model to complete
    // It will also set the m_absoluteStoragePath in  profileListModel and installedListModel
    Settings settings(&profileListModel, &monitorListModel, &installedListModel, steamID);
    SteamWorkshop steamWorkshop(steamID, &steamWorkshopListModel, &settings);

    // All the list need the default path from the settings
    // to know where to look for the files
    profileListModel.loadProfiles();
    settings.loadActiveProfiles();

    QQmlApplicationEngine mainWindowEngine;
    mainWindowEngine.rootContext()->setContextProperty("workshopListModel", &steamWorkshopListModel);
    mainWindowEngine.rootContext()->setContextProperty("monitorListModel", &monitorListModel);
    mainWindowEngine.rootContext()->setContextProperty("installedListModel", &installedListModel);
    mainWindowEngine.rootContext()->setContextProperty("profileListModel", &profileListModel);

    mainWindowEngine.rootContext()->setContextProperty("screenPlaySettings", &settings);
    mainWindowEngine.rootContext()->setContextProperty("packageFileHandler", &packageFileHandler);
    mainWindowEngine.rootContext()->setContextProperty("steamWorkshop", &steamWorkshop);

    QQmlApplicationEngine errorWindowEngine;
    if (steamErrorRestart || steamErrorAPIInit) {
        errorWindowEngine.load(QUrl(QStringLiteral("qrc:/qml/StartupErrorWindow.qml")));
    } else {
        mainWindowEngine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    }

    installedListModel.loadScreens();
    // FIXME: Needed workaround to close the app because
    // apparently some thread still runs in the background
    QObject::connect(&app, &QGuiApplication::lastWindowClosed, [&]() { exit(app.exec()); });

    // Timer for steam polls. WTF?
    QTimer timer;
    QObject::connect(&timer, &QTimer::timeout, [&]() { SteamAPI_RunCallbacks(); });
    timer.setInterval(100);
    timer.start();

    int status = app.exec();

    SteamAPI_Shutdown();
    //Shutdown
    return status;
}
