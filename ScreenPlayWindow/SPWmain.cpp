#include "../ScreenPlaySDK/screenplaysdk.h"

#if defined(Q_OS_WIN)
#include "src/winwindow.h"
#endif

#include <QApplication>
#include <QObject>
#include <QStringList>
#include <QtWebEngine>

int main(int argc, char* argv[])
{

    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication app(argc, argv);

    QtWebEngine::initialize();

    bool debug = false;
    if (debug) {
        QVector<int> list;
        list.append(0);
        WinWindow window(list, "D:/672870/_tmp_135011", "argumentList.at(3)", "1");
        return app.exec();
    }

    // 6 parameter + 1 OS working directory default paramter
    QStringList argumentList = app.arguments();
    qDebug() << argumentList;
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

    return app.exec();
}
