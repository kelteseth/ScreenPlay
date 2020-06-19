#include "app.h"

namespace ScreenPlay {

/*!
    \namespace ScreenPlay
    \inmodule ScreenPlay
    \brief Namespace for ScreenPlay.
*/

/*!
    \class ScreenPlay::App
    \inmodule ScreenPlay
    \brief The App class contains all members for ScreenPlay.


    \raw HTML
  <div class="mermaid">
        graph TD

        Main.cpp --> App
        App --> QQmlApplicationEngine
        App --> GlobalVariables
        App --> ScreenPlayManager
        ScreenPlayManager --> ScreenPlayWallpaper
        ScreenPlayManager --> ScreenPlayWidget
        App --> Create
        Create--> CreateVideoImport
        App --> Util
        App --> Settings
        App --> SDKConnector
        App --> InstalledListModel
        InstalledListModel --> ProjectFile
        App --> InstalledListFilter
        App --> MonitorListModel
        MonitorListModel --> Monitor
        App --> ProfileListModel
        ProfileListModel --> Profile

  </div>
  \endraw
*/

App::App()
    : QObject(nullptr)
{
    QGuiApplication::setWindowIcon(QIcon(":/assets/icons/favicon.ico"));
    QGuiApplication::setOrganizationName("ScreenPlay");
    QGuiApplication::setOrganizationDomain("screen-play.app");
    QGuiApplication::setApplicationName("ScreenPlay");
    QGuiApplication::setApplicationVersion("0.11.0");
    QGuiApplication::setQuitOnLastWindowClosed(false);

#ifdef Q_OS_WINDOWS
    QtBreakpad::init(QDir::current().absolutePath());
#endif

    QFontDatabase::addApplicationFont(":/assets/fonts/LibreBaskerville-Italic.ttf");

    QFontDatabase::addApplicationFont(":/assets/fonts/Roboto-Light.ttf");
    QFontDatabase::addApplicationFont(":/assets/fonts/Roboto-Regular.ttf");
    QFontDatabase::addApplicationFont(":/assets/fonts/Roboto-Thin.ttf");
    QFontDatabase::addApplicationFont(":/assets/fonts/RobotoMono-Light.ttf");
    QFontDatabase::addApplicationFont(":/assets/fonts/RobotoMono-Thin.ttf");

    QFontDatabase::addApplicationFont(":/assets/fonts/NotoSans-Thin.ttf");
    QFontDatabase::addApplicationFont(":/assets/fonts/NotoSans-Regular.ttf");
    QFontDatabase::addApplicationFont(":/assets/fonts/NotoSans-Medium.ttf");
    QFontDatabase::addApplicationFont(":/assets/fonts/NotoSans-Light.ttf");

    if (-1 == QFontDatabase::addApplicationFont(QDir::current().absolutePath() + "/assets/fonts/NotoSansCJKkr-Regular.otf")) {
        qWarning() << "Could not load korean font from: " << QDir::current().absolutePath() + "/assets/fonts/NotoSansCJKkr-Regular.otf";
    }

    QQuickWindow::setTextRenderType(QQuickWindow::TextRenderType::NativeTextRendering);

    qRegisterMetaType<QQmlApplicationEngine*>();
    qRegisterMetaType<GlobalVariables*>();
    qRegisterMetaType<ScreenPlayManager*>();
    qRegisterMetaType<Util*>();
    qRegisterMetaType<Create*>();
    qRegisterMetaType<SDKConnector*>();
    qRegisterMetaType<Settings*>();

    qRegisterMetaType<InstalledListModel*>();
    qRegisterMetaType<InstalledListFilter*>();
    qRegisterMetaType<MonitorListModel*>();
    qRegisterMetaType<ProfileListModel*>();

    // Registers the enums from globalvariables.
    // Apparently this is the only way for qml to work
    // https://www.kdab.com/new-qt-5-8-meta-object-support-namespaces/
    qRegisterMetaType<Enums::FillMode>();
    qmlRegisterUncreatableMetaObject(ScreenPlay::Enums::staticMetaObject,
        "ScreenPlayEnums",
        1, 0,
        "ScreenPlayEnums",
        "Error: only enums");

    qRegisterMetaType<InstalledType::InstalledType>();
    qmlRegisterUncreatableMetaObject(ScreenPlay::InstalledType::staticMetaObject,
        "ScreenPlay.Enums.InstalledType",
        1, 0,
        "InstalledType",
        "Error: only enums");


    qmlRegisterAnonymousType<GlobalVariables>("ScreenPlay", 1);
    qmlRegisterAnonymousType<ScreenPlayManager>("ScreenPlay", 1);
    qmlRegisterAnonymousType<Util>("ScreenPlay", 1);
    qmlRegisterAnonymousType<Create>("ScreenPlay", 1);
    qmlRegisterAnonymousType<Settings>("ScreenPlay", 1);
    qmlRegisterAnonymousType<SDKConnector>("ScreenPlay", 1);

    // SDKConnect first to check if another ScreenPlay Instace is running
    m_sdkConnector = std::make_shared<SDKConnector>();
    m_isAnotherScreenPlayInstanceRunning = m_sdkConnector->m_isAnotherScreenPlayInstanceRunning;
}

/*!
    \brief Used for initialization after the constructor. The sole purpose is to check if
    another ScreenPlay instance is running and then quit early. This is also because we cannot
    call QApplication::quit(); in the SDKConnector before the app.exec(); ( the Qt main event
    loop ) has started.
*/
void App::init()
{
    using std::make_shared, std::make_unique;

    // Util should be created as first so we redirect qDebugs etc. into the log
    m_globalVariables = make_shared<GlobalVariables>();
    auto* nam = new QNetworkAccessManager(this);
    m_util = make_unique<Util>(nam);
    m_installedListModel = make_shared<InstalledListModel>(m_globalVariables);
    m_installedListFilter = make_shared<InstalledListFilter>(m_installedListModel);
    m_monitorListModel = make_shared<MonitorListModel>();
    m_profileListModel = make_shared<ProfileListModel>(m_globalVariables);
    m_settings = make_shared<Settings>(m_globalVariables);
    m_mainWindowEngine = make_unique<QQmlApplicationEngine>();

    // Only create tracker if user did not disallow!
    if (m_settings->anonymousTelemetry()) {
        m_telemetry = make_shared<GAnalytics>("UA-152830367-3");
        m_telemetry->setNetworkAccessManager(nam);
        m_telemetry->setSendInterval(1000);
        m_telemetry->startSession();
        m_telemetry->sendEvent("version", QApplication::applicationVersion());
    }

    m_create = make_unique<Create>(m_globalVariables);
    m_screenPlayManager = make_unique<ScreenPlayManager>(m_globalVariables, m_monitorListModel, m_sdkConnector, m_telemetry, m_settings);
    QObject::connect(m_sdkConnector.get(), &SDKConnector::requestDecreaseWidgetCount, m_screenPlayManager.get(), &ScreenPlayManager::decreaseActiveWidgetsCounter);

    // When the installed storage path changed
    QObject::connect(m_settings.get(), &Settings::resetInstalledListmodel, m_installedListModel.get(), &InstalledListModel::reset);
    QObject::connect(m_settings.get(), &Settings::requestRetranslation, m_mainWindowEngine.get(), &QQmlEngine::retranslate);
    m_settings->setupLanguage();

    QObject::connect(m_globalVariables.get(), &GlobalVariables::localStoragePathChanged, this, [this](QUrl localStoragePath) {
        m_settings->resetInstalledListmodel();
        m_settings->setqSetting("ScreenPlayContentPath", localStoragePath.toString());
    });
    QObject::connect(m_monitorListModel.get(), &MonitorListModel::monitorConfigurationChanged, m_sdkConnector.get(), &SDKConnector::closeAllWallpapers);

    // Init after we have the paths from settings
    m_installedListModel->init();

    // Set visible if the -silent parameter was not set
    if (QApplication::instance()->arguments().contains("-silent")) {
        settings()->setSilentStart(true);
    }

    qmlRegisterSingletonInstance("ScreenPlay", 1, 0, "ScreenPlay", this);
    m_mainWindowEngine->load(QUrl(QStringLiteral("qrc:/main.qml")));
}

/*!
    \brief Tries to send the telemetry quit event before we call quit ourself.
*/
void App::exit()
{
    if (!m_telemetry) {
        QApplication::instance()->quit();
        return;
    } else {
        // Workaround because we cannot force to send exit event
        m_telemetry->setSendInterval(5);
        m_telemetry->endSession();
        QTimer::singleShot(150, []() { QApplication::instance()->quit(); });
    }
}
}
