#include "../ScreenPlaySDK/screenplaysdk.h"
#include "src/SPWmainwindow.h"
#include <QApplication>
#include <QObject>
#include <QStringList>
#include <QtWebEngine>

int main(int argc, char* argv[])
{

    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseDesktopOpenGL);
    QApplication a(argc, argv);
    ScreenPlaySDK sdk;
    QtWebEngine::initialize();

    // 6 parameter + 1 OS working directory default paramter
    QStringList argumentList = a.arguments();
    if (argumentList.length() != 7) {
        return -3;
    }

    bool canParseMonitorNumber = false;
    int monitor = argumentList.at(1).toInt(&canParseMonitorNumber);

    if (!canParseMonitorNumber) {
        return -4;
    }

    // Args: which monitor, (2) path to project, (3)wallpaper secret to identify the connected socket, (4) decoder, (5) volume, (6) fillmode
    // See screenplay.h @ScreenPlayWallpaper constructor how the args get created
    // MainWindow w(0,"D:/672870/827148653","","","","");
    sdk.setAppID(argumentList.at(3));
    qDebug() << "Starting MainWindow: " << argumentList.at(2) << argumentList.at(3) << argumentList.at(4) << argumentList.at(5) << argumentList.at(6);
    MainWindow w(monitor, argumentList.at(2), argumentList.at(3), argumentList.at(4), argumentList.at(5), argumentList.at(6));

    QObject::connect(&sdk, &ScreenPlaySDK::sdkDisconnected, &w, &MainWindow::destroyThis);
    QObject::connect(&sdk, &ScreenPlaySDK::incommingMessage, &w, &MainWindow::messageReceived);

    return a.exec();
}
