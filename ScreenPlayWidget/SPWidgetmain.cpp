#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QStringList>

#include "src/spwidgetmainwindow.h"

int main(int argc, char* argv[])
{
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);


    QStringList argumentList = app.arguments();

    if (argumentList.length() != 3) {
        return -3;
    }

    SPWidgetmainwindow spwmw(argumentList.at(1), argumentList.at(2));
    //SPWidgetmainwindow spwmw("D:/672870/xkcd","asasasasd" );

    return app.exec();
}
