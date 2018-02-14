#include "src/mainwindow.h"
#include <QApplication>


int main(int argc, char *argv[])
{
    //QCoreApplication::addLibraryPath("C:/msys64/mingw64/bin");
    QApplication a(argc, argv);

    QStringList argumentList = a.arguments();

    //MainWindow w(argumentList.at(0));
    //MainWindow w(0,"D:/672870/860170953");
    //MainWindow w1(1,"D:/672870/818696361");
    MainWindow w2(2,"D:/672870/860170953");

    return a.exec();
}
