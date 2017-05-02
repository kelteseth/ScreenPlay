﻿#include <QDebug>
#include <QDir>
#include <QGuiApplication>
#include <QLibrary>
#include <QModelIndex>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickView>
#include <QQmlContext>
#include <QScreen>
#include <QUrl>
#include <QVariant>
#include <QWindow>
#include <qt_windows.h>
#include <QIcon>

#include "backend.h"
#include "installedlistmodel.h"
#include "monitorlistmodel.h"
#include "screenplay.h"
#include "settings.h"

int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);

    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QCoreApplication::setOrganizationName("Aimber");
    QCoreApplication::setOrganizationDomain("aimber.net");
    QCoreApplication::setApplicationName("ScreenPlay");
    app.setWindowIcon(QIcon(":/assets/icons/favicon.ico"));

    InstalledListModel installedListModel;
    MonitorListModel monitorListModel;
    Backend backend;

    QQmlApplicationEngine mainWindow;
    mainWindow.rootContext()->setContextProperty("monitorListModel", &monitorListModel);
    mainWindow.rootContext()->setContextProperty("installedListModel", &installedListModel);
    mainWindow.rootContext()->setContextProperty("backend", &backend);

    mainWindow.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));
    QObject::connect(&mainWindow, SIGNAL(exitScreenPlay()), &app, SLOT(app.exit()));

    ScreenPlay sp(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
    sp.context()->setContextProperty("installedListModel", &installedListModel);
    sp.context()->setContextProperty("backend", &backend);

    sp.loadQQuickView(QUrl(QStringLiteral("qrc:/qml/Components/ScreenPlay.qml")));
    sp.showQQuickView(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));

    QObject::connect(&installedListModel, &InstalledListModel::setScreenVisible,
        &sp, &ScreenPlay::setVisible);

    int status = app.exec();

    //Shutdown
    return app.exec();
}
