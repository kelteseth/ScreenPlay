#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <qt_windows.h>
#include <QWindow>
#include <QQuickView>
#include "src/screenplay.h"



int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine mainWindow(QUrl(QStringLiteral("qrc:/qml/mainWindow.qml")));

    ScreenPlay sp(GetSystemMetrics(SM_CXSCREEN),GetSystemMetrics(SM_CYSCREEN));

    return app.exec();

}
