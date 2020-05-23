#include <QApplication>
#include <QObject>
#include <QStringList>
#include <QVector>
#include <QtGlobal>
#include <QtWebEngine>

#if defined(Q_OS_WIN)

#include "src/winwindow.h"
#endif

#if defined(Q_OS_LINUX)
#include "src/linuxwindow.h"
#endif

#if defined(Q_OS_OSX)
#include "src/macwindow.h"
#endif

#include "../ScreenPlaySDK/screenplaysdk.h"

int main(int argc, char* argv[])
{

    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_ShareOpenGLContexts);

    QApplication app(argc, argv);

    // This gives us nice clickable output in QtCreator
    qSetMessagePattern("%{if-category}%{category}: %{endif}%{message}\n   Loc: [%{file}:%{line}]");

    // If we start with only one argument (app path)
    // It means we want to test a single wallpaper
    QStringList argumentList = app.arguments();
    if (argumentList.length() == 1) {

        //Set the monitor number to test

#if defined(Q_OS_WIN)

        //WinWindow window1({ 0 }, "test", "appid", "1", "fill");
        //WinWindow window2({ 1 }, "test", "appid", "1", "fill");
        //WinWindow window3({ 2 }, "test", "appid", "1", "fill");

        WinWindow window({ 0 }, "C:/Program Files (x86)/Steam/steamapps/workshop/content/672870/1958068745", "appid", "1", "fill", true);
#endif
#if defined(Q_OS_LINUX)
        LinuxWindow window({ 0 }, "/home/graphicscore/Desktop/wallpapers/MechaGirl", "appid", "1", "fill", false);
#endif
#if defined(Q_OS_OSX)
        MacWindow window({ 0 }, "test", "appid", "1", "fill");
#endif

        return app.exec();
    }

    // 6 parameter + 1 OS working directory default paramter
    if (argumentList.length() != 9) {
        return -3;
    }

    // AppID, Type
    ScreenPlaySDK sdk(argumentList.at(3), argumentList.at(6));

    QString monitorNumbers = argumentList.at(1);
    QStringList activeScreensList = monitorNumbers.split(",");

    activeScreensList.removeAll(",");
    QVector<int> list;

    if (monitorNumbers.length() == 1) {
        bool canParseMonitorNumber = false;
        int monitor = monitorNumbers.toInt(&canParseMonitorNumber);
        if (!canParseMonitorNumber) {
            qFatal("Could not parse monitor id to diplay wallpaper");
        } else {
            list.append(monitor);
        }

    } else {

        for (const QString& s : activeScreensList) {
            bool ok = false;
            int val = s.toInt(&ok);
            if (!ok) {
                qFatal("Could not parse monitor id to diplay wallpaper");
            } else {
                list.append(val);
            }
        }
    }

    if (list.empty()) {
        return -4;
    }

    // Args: which monitor, (2) path to project, (3)wallpaper secret to identify the connected socket, (4) volume, (5) fillmode, (6) type, (7)
    // See screenplay.h @ScreenPlayWallpaper constructor how the args get created
    qDebug() << argumentList;

    bool okParseCheckWallpaperVisible = false;
    bool checkWallpaperVisible = argumentList.at(7).toInt(&okParseCheckWallpaperVisible);
    if (!okParseCheckWallpaperVisible) {
        qFatal("Could not parse checkWallpaperVisible");
        return -5;
    }

#if defined(Q_OS_WIN)
    WinWindow window(list, argumentList.at(2), argumentList.at(3), argumentList.at(4), argumentList.at(5), checkWallpaperVisible);
    QObject::connect(&sdk, &ScreenPlaySDK::sdkDisconnected, &window, &WinWindow::destroyThis);
    QObject::connect(&sdk, &ScreenPlaySDK::incommingMessage, &window, &WinWindow::messageReceived);
#endif

#if defined(Q_OS_LINUX)
    LinuxWindow window(list, argumentList.at(2), argumentList.at(3), argumentList.at(4), argumentList.at(5), checkWallpaperVisible);
    QObject::connect(&sdk, &ScreenPlaySDK::sdkDisconnected, &window, &LinuxWindow::destroyThis);
    QObject::connect(&sdk, &ScreenPlaySDK::incommingMessage, &window, &LinuxWindow::messageReceived);
#endif

#if defined(Q_OS_OSX)
    MacWindow window(list, argumentList.at(2), argumentList.at(3), argumentList.at(4), argumentList.at(5));
    QObject::connect(&sdk, &ScreenPlaySDK::sdkDisconnected, &window, &MacWindow::destroyThis);
    QObject::connect(&sdk, &ScreenPlaySDK::incommingMessage, &window, &MacWindow::messageReceived);
#endif

    return app.exec();
}
