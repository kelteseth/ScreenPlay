#include <QDir>
#include <QFontDatabase>
#include <QGuiApplication>
#include <QIcon>
#include <QObject>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlEngine>
#include <QStringList>
#include <QUrl>
#include <QtWebEngine>

#include "src/create.h"
#include "src/installedlistfilter.h"
#include "src/installedlistmodel.h"
#include "src/monitorlistmodel.h"
#include "src/profilelistmodel.h"
#include "src/qmlutilities.h"
#include "src/screenplay.h"
#include "src/sdkconnector.h"
#include "src/settings.h"

int main(int argc, char* argv[])
{

    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication::setOrganizationName("Aimber");
    QGuiApplication::setOrganizationDomain("screen-play.app");
    QGuiApplication::setApplicationName("ScreenPlay");
    QGuiApplication::setApplicationVersion("0.2.0");

    QGuiApplication app(argc, argv);

    app.setQuitOnLastWindowClosed(false);
    app.setWindowIcon(QIcon(":/assets/icons/favicon.ico"));

    // This gives us nice clickable output in QtCreator
    qSetMessagePattern("%{if-category}%{category}: %{endif}%{message}\n   Loc: [%{file}:%{line}]");

    // Qt < 6.0 needs this init QtWebEngine
    QtWebEngine::initialize();

    QFontDatabase::addApplicationFont(":/assets/fonts/LibreBaskerville-Italic.ttf");
    QFontDatabase::addApplicationFont(":/assets/fonts/Roboto-Light.ttf");
    QFontDatabase::addApplicationFont(":/assets/fonts/Roboto-Regular.ttf");
    QFontDatabase::addApplicationFont(":/assets/fonts/Roboto-Thin.ttf");
    QFontDatabase::addApplicationFont(":/assets/fonts/RobotoMono-Light.ttf");
    QFontDatabase::addApplicationFont(":/assets/fonts/RobotoMono-Thin.ttf");

    QMLUtilities qmlUtil;
    InstalledListModel installedListModel;
    InstalledListFilter installedListFilter(&installedListModel);
    MonitorListModel monitorListModel;
    ProfileListModel profileListModel;
    SDKConnector sdkConnector;

    // Create settings in the end because for now it depends on
    // such things as the profile list model to complete
    // It will also set the m_absoluteStoragePath in  profileListModel and installedListModel
    Settings settings(&profileListModel, &monitorListModel, &installedListModel, &sdkConnector);
    ScreenPlay screenPlay(&installedListModel, &settings, &monitorListModel, &sdkConnector);
    Create create(&settings, &qmlUtil);

    // All the list need the default path from the settings
    // to know where to look for the files
    profileListModel.loadProfiles();
    settings.loadActiveProfiles();

    QQmlApplicationEngine mainWindowEngine;
    mainWindowEngine.rootContext()->setContextProperty("screenPlay", &screenPlay);
    mainWindowEngine.rootContext()->setContextProperty("screenPlayCreate", &create);
    mainWindowEngine.rootContext()->setContextProperty("utility", &qmlUtil);
    mainWindowEngine.rootContext()->setContextProperty("installedListFilter", &installedListFilter);
    mainWindowEngine.rootContext()->setContextProperty("monitorListModel", &monitorListModel);
    mainWindowEngine.rootContext()->setContextProperty("installedListModel", &installedListModel);
    mainWindowEngine.rootContext()->setContextProperty("profileListModel", &profileListModel);
    mainWindowEngine.rootContext()->setContextProperty("screenPlaySettings", &settings);
    mainWindowEngine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    installedListModel.loadInstalledContent();

    // Instead of setting "renderType: Text.NativeRendering" every time
    // we can set it here once :)
    auto* window = static_cast<QQuickWindow*>(mainWindowEngine.rootObjects().first());
    window->setTextRenderType(QQuickWindow::TextRenderType::NativeTextRendering);

    // Set visible if the -silent parameter was not set
    QStringList argumentList = app.arguments();
    if (!argumentList.contains("-silent")) {
        settings.setMainWindowVisible(true);
    }

    return app.exec();
}
