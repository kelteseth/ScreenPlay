#include <QApplication>

#include "app.h"

int main(int argc, char* argv[])
{
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_ShareOpenGLContexts);

    QApplication qtGuiApp(argc, argv);

    ScreenPlay::App app;

    return qtGuiApp.exec();
}
