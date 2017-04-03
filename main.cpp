#include <QDebug>
#include <QDir>
#include <QGuiApplication>
#include <QLibrary>
#include <QQmlApplicationEngine>
#include <QQuickView>
#include <QScreen>
#include <QUrl>
#include <QWindow>
#include <qt_windows.h>
#include <QQmlContext>
#include <QModelIndex>
#include <QVariant>

#include "screenplay.h"
#include "installedlistmodel.h"

int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);

    QCoreApplication::setOrganizationName("Aimber");
    QCoreApplication::setOrganizationDomain("aimber.net");
    QCoreApplication::setApplicationName("ScreenPlay");
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    InstalledListModel ilm;
    ilm.setData(QModelIndex(),QVariant("ascb"),0);

    QQmlApplicationEngine mainWindow;
    mainWindow.rootContext()->setContextProperty("installedListModel", &ilm);

    mainWindow.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));


    //ScreenPlay sp(GetSystemMetrics(SM_CXSCREEN),GetSystemMetrics(SM_CYSCREEN));

    int status = app.exec();

    //Shutdown

    return  status;
}
