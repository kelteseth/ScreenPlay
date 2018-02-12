#include "src/mainwindow.h"
#include <QApplication>


int main(int argc, char *argv[])
{
    //QCoreApplication::addLibraryPath("C:/msys64/mingw64/bin");
    QApplication a(argc, argv);

    MainWindow w;
    return a.exec();
}
