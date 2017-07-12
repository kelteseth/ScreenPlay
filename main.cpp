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
#include "quazip/quazip.h"
#include "settings.h"
#include "steam/steam_api.h"
#include "steamworkshop.h"

int main(int argc, char* argv[])
{

    Q_INIT_RESOURCE(qml);

    QGuiApplication app(argc, argv);

    AppId_t steamID = 672870;

    if (SteamAPI_RestartAppIfNecessary(steamID)) {
        qWarning() << "SteamAPI_RestartAppIfNecessary";
        return 1;
    }

    if (!SteamAPI_Init()) {
        qWarning() << "Could not init steam sdk!";
        return 1;
    }

    SteamWorkshop steamWorkshop(steamID);

    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setOrganizationName("Aimber");
    QCoreApplication::setOrganizationDomain("aimber.net");
    QCoreApplication::setApplicationName("ScreenPlay");
    app.setWindowIcon(QIcon(":/assets/icons/favicon.ico"));
    QQuickStyle::setStyle("Material");

    InstalledListModel installedListModel;
    MonitorListModel monitorListModel;
    PackageFileHandler packageFileHandler;
    ProfileListModel profileListModel;

    // Create settings at the end because for now it depends on
    // such things as the profile list model to complete
    // It will also set the m_absoluteStoragePath in  profileListModel and installedListModel
    Settings settings(&profileListModel, &monitorListModel, &installedListModel);
    // All the list need the default path from the settings
    // to know where to look for the files
    installedListModel.loadScreens();
    profileListModel.loadProfiles();
    // The settings depend on the list of available profiles
    settings.loadActiveProfiles();

    QQmlApplicationEngine mainWindowEngine;
    mainWindowEngine.rootContext()->setContextProperty("monitorListModel", &monitorListModel);
    mainWindowEngine.rootContext()->setContextProperty("installedListModel", &installedListModel);
    mainWindowEngine.rootContext()->setContextProperty("settings", &settings);
    mainWindowEngine.rootContext()->setContextProperty("packageFileHandler", &packageFileHandler);
    mainWindowEngine.rootContext()->setContextProperty("profileListModel", &profileListModel);
    mainWindowEngine.rootContext()->setContextProperty("steamWorkshop", &steamWorkshop);
    mainWindowEngine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));

    // FIXME: Needed workaround to close the app because
    // apparently some thread still runs in the background
    QObject::connect(&app, &QGuiApplication::lastWindowClosed,
        [&]() { exit(app.exec()); });

    // Timer for steam polls. WTF?
    QTimer timer;
    QObject::connect(&timer, &QTimer::timeout, [&]() { SteamAPI_RunCallbacks(); });
    timer.setInterval(500);
    timer.start();

    int status = app.exec();

    //Shutdown
    return status;
    SteamAPI_Shutdown();
}
