#include <QApplication>
#include <QQmlApplicationEngine>
#include <QStringList>

#include "src/widgetwindow.h"

int main(int argc, char* argv[])
{
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_ShareOpenGLContexts);

    QApplication app(argc, argv);

    const QStringList argumentList = app.arguments();

    // If we start with only one argument (path, appID, type),
    // it means we want to test a single widget
    if (argumentList.length() == 1) {
        //WidgetWindow spwmw("test", "appid", "qmlWidget", { 0, 0 }, true);
        WidgetWindow spwmw("C:/Program Files (x86)/Steam/steamapps/workshop/content/672870/2136442401", "appid", "qmlWidget", { 0, 0 }, true);
        return app.exec();
    }

    if (argumentList.length() != 6) {
        return -3;
    }

    bool okPosX = false;
    int positionX = QVariant(argumentList.at(4)).toInt(&okPosX);
    if (!okPosX) {
        qWarning() << "Could not parse PositionX value to int: " << argumentList.at(4);
        positionX = 0;
    }
    bool okPosY = false;
    int positionY = QVariant(argumentList.at(5)).toInt(&okPosY);
    if (!okPosY) {
        qWarning() << "Could not parse PositionY value to int: " << argumentList.at(5);
        positionY = 0;
    }

    WidgetWindow spwmw(
        argumentList.at(1), // Project path,
        argumentList.at(2), // AppID
        argumentList.at(3), // Type
        QPoint { positionX, positionY });

    return app.exec();
}
