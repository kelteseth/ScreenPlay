#include <QApplication>
#include <QObject>
#include <QStringList>
#include <QtGlobal>
#include <QtWebEngine>

#if defined(Q_OS_WIN)
#include "src/winwindow.h"
#endif

#if defined(Q_OS_OSX)

// TODO MAC OSX PORT HERE

#endif

#include "../ScreenPlaySDK/screenplaysdk.h"

int main(int argc, char* argv[])
{

    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication app(argc, argv);

    // This gives us nice clickable output in QtCreator
    qSetMessagePattern("%{if-category}%{category}: %{endif}%{message}\n   Loc: [%{file}:%{line}]");

    // Qt < 6.0 needs this init QtWebEngine
    QtWebEngine::initialize();

    // If we start with only one argument (app path)
    // It means we want to test a single wallpaper
    QStringList argumentList = app.arguments();
    if (argumentList.length() == 1) {
        QVector<int> list;
        list.append(0);
#if defined(Q_OS_WIN)
        WinWindow window(list, "test", "appid", "1");
#endif
        return app.exec();
    }

    // 6 parameter + 1 OS working directory default paramter
    if (argumentList.length() != 7) {
        return -3;
    }

    ScreenPlaySDK sdk;
    sdk.setAppID(argumentList.at(3));

    QString monitorNumbers = argumentList.at(1);
    QVector<int> list;

    if (monitorNumbers.length() == 1) {
        bool canParseMonitorNumber = false;
        int monitor = monitorNumbers.toInt(&canParseMonitorNumber);
        if (!canParseMonitorNumber) {
            qFatal("Could not parse monitor id to diplay wallpaper");
        }
        list.append(monitor);
    } else {
        QStringList activeScreensList = monitorNumbers.split(",");
        for (QString s : activeScreensList) {
            bool ok = false;
            int val = s.toInt(&ok);
            if (!ok) {
                qFatal("Could not parse monitor id to diplay wallpaper");
            }
            list.append(val);
        }
    }

    // Args: which monitor, (2) path to project, (3)wallpaper secret to identify the connected socket, (5) volume
    // See screenplay.h @ScreenPlayWallpaper constructor how the args get created

#if defined(Q_OS_WIN)
    WinWindow window(list, argumentList.at(2), argumentList.at(3), argumentList.at(5));
    QObject::connect(&sdk, &ScreenPlaySDK::sdkDisconnected, &window, &WinWindow::destroyThis);
    QObject::connect(&sdk, &ScreenPlaySDK::incommingMessage, &window, &WinWindow::messageReceived);
#endif

#if defined(Q_OS_OSX)

    // TODO MAC OSX PORT HERE
    // QObject::connect(&sdk, &ScreenPlaySDK::sdkDisconnected, &TODO, &TODO::destroyThis);
    // QObject::connect(&sdk, &ScreenPlaySDK::incommingMessage, &TODO, &TODO::messageReceived);

#endif

    return app.exec();
}
