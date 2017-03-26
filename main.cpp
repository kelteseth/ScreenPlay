#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <qt_windows.h>
#include <QWindow>
#include <QQuickView>
#include <QLibrary>
#include "screenplay.h"
#include "steam_api.h"



int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    if(!SteamAPI_Init()){
        qFatal("Could not init Steam sdk!");
    }

    QQmlApplicationEngine mainWindow(QUrl(QStringLiteral("qrc:/qml/mainWindow.qml")));

    ScreenPlay sp(GetSystemMetrics(SM_CXSCREEN),GetSystemMetrics(SM_CYSCREEN));

    return app.exec();

}
