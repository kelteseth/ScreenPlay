#include "src/mainwindow.h"
#include <QApplication>
#include <QStringList>

int main(int argc, char* argv[])
{
    //QCoreApplication::addLibraryPath("C:/msys64/mingw64/bin");

    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseOpenGLES);
    QApplication a(argc, argv);

    QStringList argumentList  = a.arguments();

    if(argumentList.length() != 3) {
        return -3;
    }

    bool ok = false;
    int monitor = argumentList.at(1).toInt(&ok);

    if (!ok) {
        return -4;
    }

    MainWindow w(monitor,argumentList.at(2));

    return a.exec();
}
