#include <QGuiApplication>

#include "app.h"

int main(int argc, char* argv[])
{
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication::setAttribute(Qt::AA_ShareOpenGLContexts);

    QGuiApplication qtGuiApp(argc, argv);

    ScreenPlay::App app;

    return qtGuiApp.exec();
}
