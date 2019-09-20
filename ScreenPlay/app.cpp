#include "app.h"

App::App()
    : QObject(nullptr)
{
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication::setOrganizationName("ScreenPlay");
    QGuiApplication::setOrganizationDomain("screen-play.app");
    QGuiApplication::setApplicationName("ScreenPlay");
    QGuiApplication::setApplicationVersion("0.3.0");
    QGuiApplication::setQuitOnLastWindowClosed(false);
    QGuiApplication::setWindowIcon(QIcon(":/assets/icons/favicon.ico"));

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

    m_globalVariables = make_shared<GlobalVariables>();
    m_installedListModel = make_shared<InstalledListModel>(m_globalVariables);
    m_installedListFilter = make_shared<InstalledListFilter>(m_installedListModel);
    m_monitorListModel = make_shared<MonitorListModel>();
    m_profileListModel = make_shared<ProfileListModel>(m_globalVariables);
    m_sdkConnector = make_shared<SDKConnector>();
    m_settings = make_shared<Settings>(m_globalVariables);

    m_create = make_unique<Create>(m_globalVariables);
    m_util = make_unique<Util>(new QNetworkAccessManager(this));
    m_screenPlayManager = make_unique<ScreenPlayManager>(m_globalVariables, m_monitorListModel, m_sdkConnector);

    // When the installed storage path changed
    QObject::connect(m_settings.get(), &Settings::resetInstalledListmodel, m_installedListModel.get(), &InstalledListModel::reset);

    // Init after we have the paths from settings
    m_installedListModel->init();
}
