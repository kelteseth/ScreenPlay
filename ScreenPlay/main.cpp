#include <QGuiApplication>

#include "app.h"

int main(int argc, char* argv[])
{
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication::setAttribute(Qt::AA_ShareOpenGLContexts);

    QGuiApplication qtGuiApp(argc, argv);

    using namespace ScreenPlay;

#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
    qmlRegisterSingletonType<App>("ScreenPlay", 1, 0, "ScreenPlay", [](QQmlEngine* engine, QJSEngine*) -> QObject* {
        engine->setObjectOwnership(App::instance(), QQmlEngine::ObjectOwnership::CppOwnership);
        return App::instance();
    });
    QQmlApplicationEngine m_mainWindowEngine;
    m_mainWindowEngine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    // Set visible if the -silent parameter was not set
    if (QGuiApplication::instance()->arguments().contains("-silent")) {
        App::instance()->settings()->setSilentStart(true);
    }

#else
    App app;
#endif

    return qtGuiApp.exec();
}
