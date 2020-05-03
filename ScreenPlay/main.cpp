#include <QApplication>

#include "app.h"

int main(int argc, char* argv[])
{
    // Buggy with every Qt version except 5.14.0!
    // Displays wrong DPI scaled monitor resolution
    // 4k with 150% scaling to FULL HD on Windows
    // https://bugreports.qt.io/browse/QTBUG-81694
    #if defined(Q_OS_LINUX) || defined(Q_OS_OSX)
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    #endif
    QApplication::setAttribute(Qt::AA_ShareOpenGLContexts);

    QApplication qtGuiApp(argc, argv);

    ScreenPlay::App app;

    if (app.m_isAnotherScreenPlayInstanceRunning) {
        return 0;
    } else {
        app.init();
        return qtGuiApp.exec();
    }
}
