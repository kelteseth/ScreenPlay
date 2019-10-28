#include "app.h"
#include <QGuiApplication>

int main(int argc, char* argv[])
{
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication::setAttribute(Qt::AA_ShareOpenGLContexts);

    QGuiApplication qtGuiApp(argc, argv);
    App app;

    return qtGuiApp.exec();
}
