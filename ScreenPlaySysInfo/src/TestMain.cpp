#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QVersionNumber>
#include <QtQml/qqmlextensionplugin.h>

#if defined(Q_OS_WIN)
Q_IMPORT_QML_PLUGIN(ScreenPlaySysInfoPlugin)
#endif

int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    engine.addImportPath(app.applicationDirPath() + "/qml");

    // The first subfolder is the libraryName followed by the regular
    // folder strucutre:     LibararyName/Subfolder
    const QUrl url(u"qrc:/qml/ScreenPlaySysInfo/qml/TestMain.qml"_qs);
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
