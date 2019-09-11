#include <QDir>
#include <QGuiApplication>
#include <QIcon>
#include <QObject>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlEngine>
#include <QStringList>
#include <QUrl>
#include <QtWebEngine>

#include <memory>

#include "src/create.h"
#include "src/globalvariables.h"
#include "src/installedlistfilter.h"
#include "src/installedlistmodel.h"
#include "src/monitorlistmodel.h"
#include "src/profilelistmodel.h"
#include "src/screenplaymanager.h"
#include "src/sdkconnector.h"
#include "src/settings.h"
#include "src/util.h"

using std::make_shared,
    ScreenPlay::Util,
    ScreenPlay::InstalledListModel,
    ScreenPlay::ScreenPlayManager,
    ScreenPlay::InstalledListFilter,
    ScreenPlay::MonitorListModel,
    ScreenPlay::ProfileListModel,
    ScreenPlay::SDKConnector,
    ScreenPlay::Settings,
    ScreenPlay::Create;

int main(int argc, char* argv[])
{

    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication::setOrganizationName("ScreenPlay");
    QGuiApplication::setOrganizationDomain("screen-play.app");
    QGuiApplication::setApplicationName("ScreenPlay");
    QGuiApplication::setApplicationVersion("0.3.0");

    QGuiApplication app(argc, argv);
    QGuiApplication::setQuitOnLastWindowClosed(false);
    QGuiApplication::setWindowIcon(QIcon(":/assets/icons/favicon.ico"));

    // Qt < 6.0 needs this init QtWebEngine
    QtWebEngine::initialize();

    auto globalVariables = make_shared<GlobalVariables>();
    auto installedListModel = make_shared<InstalledListModel>(globalVariables);
    auto installedListFilter = make_shared<InstalledListFilter>(installedListModel);
    auto monitorListModel = make_shared<MonitorListModel>();
    auto profileListModel = make_shared<ProfileListModel>(globalVariables);
    auto sdkConnector = make_shared<SDKConnector>();

    // Create settings in the end because for now it depends on
    // such things as the profile list model to complete
    // It will also set the m_absoluteStoragePath in  profileListModel and installedListModel
    auto settings = make_shared<Settings>(
        installedListModel,
        globalVariables);

    ScreenPlayManager screenPlay(
        globalVariables,
        monitorListModel,
        sdkConnector);

    Create create(globalVariables);

    QQmlApplicationEngine mainWindowEngine;
    Util util { mainWindowEngine.networkAccessManager() };

    // This needs to change in the future because setContextProperty gets depricated in Qt 6
    mainWindowEngine.rootContext()->setContextProperty(QStringLiteral("screenPlay"), &screenPlay);
    mainWindowEngine.rootContext()->setContextProperty(QStringLiteral("screenPlayCreate"), &create);
    mainWindowEngine.rootContext()->setContextProperty(QStringLiteral("utility"), &util);
    mainWindowEngine.rootContext()->setContextProperty(QStringLiteral("installedListFilter"), installedListFilter.get());
    mainWindowEngine.rootContext()->setContextProperty(QStringLiteral("monitorListModel"), monitorListModel.get());
    mainWindowEngine.rootContext()->setContextProperty(QStringLiteral("installedListModel"), installedListModel.get());
    mainWindowEngine.rootContext()->setContextProperty(QStringLiteral("profileListModel"), profileListModel.get());
    mainWindowEngine.rootContext()->setContextProperty(QStringLiteral("screenPlaySettings"), settings.get());
    mainWindowEngine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    // Instead of setting "renderType: Text.NativeRendering" every time
    // we can set it here once :)
    QQuickWindow::setTextRenderType(QQuickWindow::TextRenderType::NativeTextRendering);

    // Set visible if the -silent parameter was not set
    QStringList argumentList = app.arguments();
    if (!argumentList.contains("-silent")) {
        settings->setMainWindowVisible(true);
    }
    installedListModel->init();

    return app.exec();
}
