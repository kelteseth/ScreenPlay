#include "app.h"

App::App(int& argc, char** argv)
    : QObject(nullptr)
    , app { std::make_unique<QGuiApplication>(argc, argv) }
    , mainWindowEngine{std::make_unique<QQmlApplicationEngine>()}
{
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication::setOrganizationName("ScreenPlay");
    QGuiApplication::setOrganizationDomain("screen-play.app");
    QGuiApplication::setApplicationName("ScreenPlay");
    QGuiApplication::setApplicationVersion("0.3.0");
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
    auto settings = make_shared<Settings>(globalVariables);
    QObject::connect(settings.get(), &Settings::resetInstalledListmodel, installedListModel.get(), &InstalledListModel::reset);

    Create create(globalVariables);
    Util util {
        mainWindowEngine->networkAccessManager()
    };
    ScreenPlayManager screenPlay {
        globalVariables,
        monitorListModel,
        sdkConnector
    };

    // This needs to change in the future because setContextProperty gets depricated in Qt 6
    mainWindowEngine->rootContext()->setContextProperty(QStringLiteral("globalVariables"), globalVariables.get());
    mainWindowEngine->rootContext()->setContextProperty(QStringLiteral("screenPlay"), &screenPlay);
    mainWindowEngine->rootContext()->setContextProperty(QStringLiteral("screenPlayCreate"), &create);
    mainWindowEngine->rootContext()->setContextProperty(QStringLiteral("utility"), &util);
    mainWindowEngine->rootContext()->setContextProperty(QStringLiteral("installedListFilter"), installedListFilter.get());
    mainWindowEngine->rootContext()->setContextProperty(QStringLiteral("monitorListModel"), monitorListModel.get());
    mainWindowEngine->rootContext()->setContextProperty(QStringLiteral("installedListModel"), installedListModel.get());
    mainWindowEngine->rootContext()->setContextProperty(QStringLiteral("profileListModel"), profileListModel.get());
    mainWindowEngine->rootContext()->setContextProperty(QStringLiteral("screenPlaySettings"), settings.get());
    mainWindowEngine->load(QUrl(QStringLiteral("qrc:/main.qml")));

    // Instead of setting "renderType: Text.NativeRendering" every time
    // we can set it here once :)
    QQuickWindow::setTextRenderType(QQuickWindow::TextRenderType::NativeTextRendering);

    // Set visible if the -silent parameter was not set
    QStringList argumentList = app->arguments();
    if (!argumentList.contains("-silent")) {
        settings->setMainWindowVisible(true);
    }
    installedListModel->init();
}

int App::run()
{
    return app->exec();
}
