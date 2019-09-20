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

    QGuiApplication::setWindowIcon(QIcon(":/assets/icons/favicon.ico"));

    // Qt < 6.0 needs this init QtWebEngine
    QtWebEngine::initialize();

    m_globalVariables = make_shared<GlobalVariables>();
    m_installedListModel = make_shared<InstalledListModel>(m_globalVariables);
    m_installedListFilter = make_shared<InstalledListFilter>(m_installedListModel);
    m_monitorListModel = make_shared<MonitorListModel>();
    m_profileListModel = make_shared<ProfileListModel>(m_globalVariables);
    m_sdkConnector = make_shared<SDKConnector>();
    m_settings = make_shared<Settings>(m_globalVariables);
    m_create = make_shared<Create>(m_globalVariables);
    m_util = make_shared<Util>(new QNetworkAccessManager(this));
    m_screenPlayManager = make_shared<ScreenPlayManager>(m_globalVariables, m_monitorListModel, m_sdkConnector);

    QObject::connect(m_settings.get(), &Settings::resetInstalledListmodel, m_installedListModel.get(), &InstalledListModel::reset);
    // Instead of setting "renderType: Text.NativeRendering" every time
    // we can set it here once :)
    QQuickWindow::setTextRenderType(QQuickWindow::TextRenderType::NativeTextRendering);




    m_installedListModel->init();

}

