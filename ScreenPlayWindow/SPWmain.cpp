#include "src/SPWmainwindow.h"
#include <QApplication>
#include <QStringList>

int main(int argc, char* argv[])
{
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication a(argc, argv);

    QStringList argumentList = a.arguments();

    if (argumentList.length() != 7) {
        return -3;
    }

    bool ok = false;
    int monitor = argumentList.at(1).toInt(&ok);

    if (!ok) {
        return -4;
    }

    // Args: which monitor, path to project, wallpaper secret to identify the connected socket
    //MainWindow w(monitor, argumentList.at(2), argumentList.at(3), argumentList.at(4), argumentList.at(5), argumentList.at(6));
    //MainWindow w(0,"D:/672870/827148653","","","","");
    MainWindow w(monitor, argumentList.at(2), argumentList.at(3), argumentList.at(4), argumentList.at(5), argumentList.at(6));

    return a.exec();
}
