#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QVersionNumber>
#include <QtQml/qqmlextensionplugin.h>

Q_IMPORT_QML_PLUGIN(ScreenPlaySysInfoPlugin)

int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    // The first subfolder is the libraryName followed by the regular
    // folder strucutre:     LibararyName/Subfolder
    const QUrl url(u"qrc:/ScreenPlaySysInfo/src/TestMain.qml"_qs);
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
