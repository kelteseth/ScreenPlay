#include <QDebug>
#include <QDir>
#include <QGuiApplication>
#include <QLibrary>
#include <QModelIndex>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickView>
#include <QScreen>
#include <QUrl>
#include <QVariant>
#include <QWindow>
#include <qt_windows.h>

#include "installedlistmodel.h"
#include "screenplay.h"
#include "mainwindow.h"

int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);

    QCoreApplication::setOrganizationName("Aimber");
    QCoreApplication::setOrganizationDomain("aimber.net");
    QCoreApplication::setApplicationName("ScreenPlay");
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    InstalledListModel ilm;

    QQmlApplicationEngine mainWindow;
    mainWindow.rootContext()->setContextProperty("installedListModel", &ilm);
    mainWindow.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));


    ScreenPlay sp(GetSystemMetrics(SM_CXSCREEN),GetSystemMetrics(SM_CYSCREEN));
    sp.context()->setContextProperty("installedListModel",&ilm);
    sp.loadQQuickView(QUrl(QStringLiteral("qrc:/qml/Components/ScreenPlay.qml")));
    sp.showQQuickView(GetSystemMetrics(SM_CXSCREEN),GetSystemMetrics(SM_CYSCREEN));

    QObject::connect(&ilm, &InstalledListModel::setScreenVisible,
                     &sp,&ScreenPlay::setVisible);


    int status = app.exec();

    //Shutdown
    return status;
}
