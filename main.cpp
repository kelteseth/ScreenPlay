#include <QDebug>
#include <QDir>
#include <QGuiApplication>
#include <QIcon>
#include <QLibrary>
#include <QModelIndex>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickStyle>
#include <QQuickView>
#include <QScreen>
#include <QUrl>
#include <QVariant>
#include <QWindow>
#include <QtQuick/QQuickItem>
#include <qt_windows.h>

#include "backend.h"
#include "installedlistmodel.h"
#include "monitorlistmodel.h"
#include "screenplay.h"
#include "settings.h"

int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);

    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setOrganizationName("Aimber");
    QCoreApplication::setOrganizationDomain("aimber.net");
    QCoreApplication::setApplicationName("ScreenPlay");
    app.setWindowIcon(QIcon(":/assets/icons/favicon.ico"));
    QQuickStyle::setStyle("Material");

    InstalledListModel installedListModel;
    MonitorListModel monitorListModel;
    Settings settings;

    QQmlApplicationEngine mainWindow;
    mainWindow.rootContext()->setContextProperty("monitorListModel", &monitorListModel);
    mainWindow.rootContext()->setContextProperty("installedListModel", &installedListModel);
    mainWindow.rootContext()->setContextProperty("settings", &settings);

    mainWindow.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));
    ScreenPlay sp(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
    sp.context()->setContextProperty("installedListModel", &installedListModel);
    sp.context()->setContextProperty("settings", &settings);

    sp.loadQQuickView(QUrl(QStringLiteral("qrc:/qml/Components/ScreenPlay.qml")));
    sp.showQQuickView(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));

    int status = app.exec();

    //Shutdown
    return app.exec();
}
