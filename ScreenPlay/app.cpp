#include "app.h"

App::App()
    : QObject(nullptr)
{

    QGuiApplication::setWindowIcon(QIcon(":/assets/icons/favicon.ico"));
    QGuiApplication::setOrganizationName("ScreenPlay");
    QGuiApplication::setOrganizationDomain("screen-play.app");
    QGuiApplication::setApplicationName("ScreenPlay");
    QGuiApplication::setApplicationVersion("0.5.0");
    QGuiApplication::setQuitOnLastWindowClosed(false);

    QFontDatabase::addApplicationFont(":/assets/fonts/LibreBaskerville-Italic.ttf");
    QFontDatabase::addApplicationFont(":/assets/fonts/Roboto-Light.ttf");
    QFontDatabase::addApplicationFont(":/assets/fonts/Roboto-Regular.ttf");
    QFontDatabase::addApplicationFont(":/assets/fonts/Roboto-Thin.ttf");
    QFontDatabase::addApplicationFont(":/assets/fonts/RobotoMono-Light.ttf");
    QFontDatabase::addApplicationFont(":/assets/fonts/RobotoMono-Thin.ttf");

    QQuickWindow::setTextRenderType(QQuickWindow::TextRenderType::NativeTextRendering);

    // Qt < 6.0 needs this init QtWebEngine
    QtWebEngine::initialize();

    qRegisterMetaType<GlobalVariables*>();
    qRegisterMetaType<ScreenPlayManager*>();
    qRegisterMetaType<Create*>();
    qRegisterMetaType<Util*>();
    qRegisterMetaType<SDKConnector*>();
    qRegisterMetaType<Settings*>();

    qRegisterMetaType<InstalledListModel*>();
    qRegisterMetaType<InstalledListFilter*>();
    qRegisterMetaType<MonitorListModel*>();
    qRegisterMetaType<ProfileListModel*>();

    // Util should be created as first so we redirect qDebugs etc. into the log
    m_util = make_unique<Util>(new QNetworkAccessManager(this));
    m_globalVariables = make_shared<GlobalVariables>();
    m_installedListModel = make_shared<InstalledListModel>(m_globalVariables);
    m_installedListFilter = make_shared<InstalledListFilter>(m_installedListModel);
    m_monitorListModel = make_shared<MonitorListModel>();
    m_profileListModel = make_shared<ProfileListModel>(m_globalVariables);
    m_sdkConnector = make_shared<SDKConnector>();
    m_settings = make_shared<Settings>(m_globalVariables);

    m_create = make_unique<Create>(m_globalVariables);
    m_screenPlayManager = make_unique<ScreenPlayManager>(m_globalVariables, m_monitorListModel, m_sdkConnector);

    // When the installed storage path changed
    QObject::connect(m_settings.get(), &Settings::resetInstalledListmodel, m_installedListModel.get(), &InstalledListModel::reset);
    QObject::connect(m_globalVariables.get(), &GlobalVariables::localStoragePathChanged, this, [this](QUrl localStoragePath) {
        m_settings->resetInstalledListmodel();
        m_settings->writeSingleSettingConfig("absoluteStoragePath", localStoragePath.toString());
    });
    QObject::connect(m_monitorListModel.get(), &MonitorListModel::monitorConfigurationChanged, m_sdkConnector.get(), &SDKConnector::closeAllWallpapers);

    // Init after we have the paths from settings
    m_installedListModel->init();

    // Set visible if the -silent parameter was not set
    if (!QGuiApplication::instance()->arguments().contains("-silent")) {
        settings()->setSilentStart(true);
    }

    qmlRegisterSingletonInstance("ScreenPlay", 1, 0, "ScreenPlay", this);
    m_mainWindowEngine = make_unique<QQmlApplicationEngine>();
    m_mainWindowEngine->load(QUrl(QStringLiteral("qrc:/main.qml")));

}
