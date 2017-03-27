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

#include "screenplay.h"

int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);

    QCoreApplication::setOrganizationName("Aimber");
    QCoreApplication::setOrganizationDomain("aimber.net");
    QCoreApplication::setApplicationName("ScreenPlay");
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QQmlApplicationEngine mainWindow(QUrl(QStringLiteral("qrc:/qml/main.qml")));

    //ScreenPlay sp(GetSystemMetrics(SM_CXSCREEN),GetSystemMetrics(SM_CYSCREEN));

    return app.exec();
}
