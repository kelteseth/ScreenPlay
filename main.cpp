#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <qt_windows.h>
#include <QWindow>
#include <QQuickView>
#include <QLibrary>
#include <QScreen>
#include <QDebug>
#include <QDir>
#include <QUrl>
#include "screenplay.h"
//#include "steam_api.h"
//#include "steamworkshop.h"


int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);


    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

//    if(!SteamAPI_Init()){
//        qFatal("Could not init Steam sdk!");
//    }




    QQmlApplicationEngine mainWindow(QUrl(QStringLiteral("qrc:/qml/main.qml")));

    //ScreenPlay sp(GetSystemMetrics(SM_CXSCREEN),GetSystemMetrics(SM_CYSCREEN));



    return app.exec();

}
