#include "app.h"
#include <QGuiApplication>
#include <qqml.h>

int main(int argc, char* argv[])
{
    // Needs to be created before
    App* app = App::instance();

    QGuiApplication qtGuiApp(argc, argv);

    qmlRegisterSingletonType<App>("ScreenPlay", 1, 0, "ScreenPlay", [](QQmlEngine* engine, QJSEngine*) -> QObject* {
        engine->setObjectOwnership(App::instance(), QQmlEngine::ObjectOwnership::CppOwnership);
        return App::instance();
    });

    app->init();

    return qtGuiApp.exec();
}
