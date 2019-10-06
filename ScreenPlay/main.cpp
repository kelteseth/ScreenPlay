#include <QGuiApplication>
#include <QQuickWindow>
#include <qqml.h>

#include "app.h"

int main(int argc, char* argv[])
{

    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication qtGuiApp(argc, argv);

    QGuiApplication::setWindowIcon(QIcon(":/assets/icons/favicon.ico"));

    qmlRegisterSingletonType<App>("ScreenPlay", 1, 0, "ScreenPlay", [](QQmlEngine* engine, QJSEngine*) -> QObject* {
        engine->setObjectOwnership(App::instance(), QQmlEngine::ObjectOwnership::CppOwnership);
        return App::instance();
    });

    QQmlApplicationEngine m_mainWindowEngine;
    m_mainWindowEngine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    // Set visible if the -silent parameter was not set
    if (!QGuiApplication::instance()->arguments().contains("-silent")) {
        App::instance()->settings()->setMainWindowVisible(true);
    }

    return qtGuiApp.exec();
}
