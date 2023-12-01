// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#include "ScreenPlayUtil/steamenumsgenerated.h"
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <QtQml/qqmlextensionplugin.h>

Q_IMPORT_QML_PLUGIN(ScreenPlayWorkshopPlugin)

int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);

    // Must be set so we can access the global ScreenPlay settings like install path.
    QGuiApplication::setOrganizationName("ScreenPlay");
    QGuiApplication::setOrganizationDomain("screen-play.app");
    QGuiApplication::setApplicationName("ScreenPlay");

    QQuickStyle::setStyle("Material");
    QQmlApplicationEngine engine;
    engine.addImportPath(app.applicationDirPath() + "/qml");
    // The first subfolder is the libraryName followed by the regular
    // folder strucutre:     LibararyName/Subfolder
    const QUrl url(u"qrc:/qml/ScreenPlayWorkshop/qml/TestMain.qml"_qs);
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
