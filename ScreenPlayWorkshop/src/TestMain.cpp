#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQml/qqmlextensionplugin.h>
#include "steam/steam_qt_enums_generated.h"

Q_IMPORT_QML_PLUGIN(ScreenPlayWorkshopPlugin)

int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);

    // TODO: This is a workaround because I don't know how to
    //       init this in the ScreenPlayWorkshop plugin.
    //       Move to workshop plugin.
    qmlRegisterUncreatableMetaObject(ScreenPlayWorkshopSteamEnums::staticMetaObject,
        "WorkshopEnums",
        1, 0,
        "SteamEnums",
        "Error: only enums");


    QQmlApplicationEngine engine;
    // The first subfolder is the libraryName followed by the regular
    // folder strucutre:     LibararyName/Subfolder
    const QUrl url(u"qrc:/ScreenPlayWorkshop/src/TestMain.qml"_qs);
    QObject::connect(
        &engine, &QQmlApplicationEngine::objectCreated,
        &app, [url](QObject* obj, const QUrl& objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
