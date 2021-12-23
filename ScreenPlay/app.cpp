#include "app.h"

#include "steam/steam_qt_enums_generated.h"

namespace ScreenPlay {
/*!
    \module ScreenPlay
    \title ScreenPlay
    \brief Module for ScreenPlay.
*/
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

/*!
    \brief Constructor creates and holds all classes used by ScreenPlay via unique_ptr or
           shared_ptr.
*/
App::App()
    : QObject(nullptr)
{

    m_continuousIntegrationMetricsTimer.start();

    QGuiApplication::setWindowIcon(QIcon(":/assets/icons/app.ico"));
    QGuiApplication::setOrganizationName("ScreenPlay");
    QGuiApplication::setOrganizationDomain("screen-play.app");
    QGuiApplication::setApplicationName("ScreenPlay");
    QGuiApplication::setApplicationVersion("0.15.0");
    QGuiApplication::setQuitOnLastWindowClosed(false);

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

    QFontDatabase::addApplicationFont(":/assets/fonts/NotoSansCJKkr-Regular.otf");

    QQuickWindow::setTextRenderType(QQuickWindow::TextRenderType::NativeTextRendering);

    qRegisterMetaType<QQmlApplicationEngine*>();
    qRegisterMetaType<GlobalVariables*>();
    qRegisterMetaType<ScreenPlayManager*>();
    qRegisterMetaType<Util*>();
    qRegisterMetaType<Create*>();
    qRegisterMetaType<Settings*>();

    qmlRegisterType<ScreenPlayWallpaper>("ScreenPlay", 1, 0, "ScreenPlayWallpaper");
    qmlRegisterType<ScreenPlayWidget>("ScreenPlay", 1, 0, "ScreenPlayWidget");

    qRegisterMetaType<ScreenPlayWallpaper*>("ScreenPlayWallpaper*");
    qRegisterMetaType<ScreenPlayWidget*>("ScreenPlayWidget*");

    qRegisterMetaType<InstalledListModel*>();
    qRegisterMetaType<InstalledListFilter*>();
    qRegisterMetaType<MonitorListModel*>();
    qRegisterMetaType<ProfileListModel*>();


    // TODO: This is a workaround because I don't know how to
    //       init this in the ScreenPlayWorkshop plugin.
    //       Move to workshop plugin.
    qmlRegisterUncreatableMetaObject(ScreenPlayWorkshopSteamEnums::staticMetaObject,
        "WorkshopEnums",
        1, 0,
        "SteamEnums",
        "Error: only enums");

    // Registers the enums from globalvariables.
    // Apparently this is the only way for qml to work
    // https://www.kdab.com/new-qt-5-8-meta-object-support-namespaces/
    qRegisterMetaType<FillMode::FillMode>();
    qmlRegisterUncreatableMetaObject(ScreenPlay::FillMode::staticMetaObject,
        "ScreenPlay.Enums.FillMode",
        1, 0,
        "FillMode",
        "Error: only enums");

    qRegisterMetaType<InstalledType::InstalledType>();
    qmlRegisterUncreatableMetaObject(ScreenPlay::InstalledType::staticMetaObject,
        "ScreenPlay.Enums.InstalledType",
        1, 0,
        "InstalledType",
        "Error: only enums");

    qRegisterMetaType<SearchType::SearchType>();
    qmlRegisterUncreatableMetaObject(ScreenPlay::SearchType::staticMetaObject,
        "ScreenPlay.Enums.SearchType",
        1, 0,
        "SearchType",
        "Error: only enums");

    qmlRegisterAnonymousType<GlobalVariables>("ScreenPlay", 1);
    qmlRegisterAnonymousType<ScreenPlayManager>("ScreenPlay", 1);
    qmlRegisterAnonymousType<Util>("ScreenPlay", 1);
    qmlRegisterAnonymousType<Create>("ScreenPlay", 1);
    qmlRegisterAnonymousType<Wizards>("ScreenPlay", 1);
    qmlRegisterAnonymousType<Settings>("ScreenPlay", 1);

    // ScreenPlayManager first to check if another ScreenPlay Instace is running
    m_screenPlayManager = std::make_unique<ScreenPlayManager>();
    m_isAnotherScreenPlayInstanceRunning = m_screenPlayManager->isAnotherScreenPlayInstanceRunning();
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
    m_util = std::make_unique<Util>(&m_networkAccessManager);
    m_globalVariables = make_shared<GlobalVariables>();
    m_installedListModel = make_shared<InstalledListModel>(m_globalVariables);
    m_installedListFilter = make_shared<InstalledListFilter>(m_installedListModel);
    m_monitorListModel = make_shared<MonitorListModel>();
    m_profileListModel = make_shared<ProfileListModel>(m_globalVariables);
    m_settings = make_shared<Settings>(m_globalVariables);
    m_mainWindowEngine = make_unique<QQmlApplicationEngine>();

    // Only create anonymousTelemetry if user did not disallow!
    if (m_settings->anonymousTelemetry()) {

        sentry_options_t* options = sentry_options_new();
        sentry_options_set_dsn(options, "https://425ea0b77def4f91a5a9decc01b36ff4@o428218.ingest.sentry.io/5373419");
        QString environment = QGuiApplication::applicationVersion() + "";
        sentry_options_set_environment(options, QString(environment).toStdString().c_str());

        const QString appPath = QGuiApplication::applicationDirPath();
        sentry_options_set_handler_path(options, QString(appPath + "/crashpad_handler.exe").toStdString().c_str());
        sentry_options_set_database_path(options, appPath.toStdString().c_str());
        sentry_options_set_handler_path(options, QString(QGuiApplication::applicationDirPath() + "/crashpad_handler" + ScreenPlayUtil::executableBinEnding()).toStdString().c_str());
        sentry_options_set_database_path(options, QGuiApplication::applicationDirPath().toStdString().c_str());
        const int sentryInitStatus = sentry_init(options);
        if (sentryInitStatus != 0) {
            qWarning() << "Unable to inti sentry crashhandler with statuscode: " << sentryInitStatus;
        }
    }

    m_create = make_unique<Create>(m_globalVariables);
    QObject::connect(m_create.get(), &Create::finished, m_installedListModel.get(), &InstalledListModel::reset);

    m_wizards = make_unique<Wizards>(m_globalVariables);

    // When the installed storage path changed
    QObject::connect(m_settings.get(), &Settings::resetInstalledListmodel, m_installedListModel.get(), &InstalledListModel::reset);
    QObject::connect(m_settings.get(), &Settings::requestRetranslation, m_mainWindowEngine.get(), &QQmlEngine::retranslate);
    m_settings->setupLanguage();

    QObject::connect(m_globalVariables.get(), &GlobalVariables::localStoragePathChanged, this, [this](QUrl localStoragePath) {
        emit m_settings->resetInstalledListmodel();
        m_settings->setqSetting("ScreenPlayContentPath", localStoragePath.toString());
    });

    // Init after we have the paths from settings
    m_installedListModel->init();

    // Set visible if the -silent parameter was not set
    if (QApplication::instance()->arguments().contains("-silent")) {
        qInfo() << "Starting in silent mode.";
        settings()->setSilentStart(true);
    }

    qmlRegisterSingletonInstance("ScreenPlay", 1, 0, "ScreenPlay", this);
    QGuiApplication::instance()->addLibraryPath(QGuiApplication::instance()->applicationDirPath());

    m_mainWindowEngine->load(QUrl(QStringLiteral("qrc:/ScreenPlay/main.qml")));

    // Must be called last to display a error message on startup by the qml engine
    m_screenPlayManager->init(m_globalVariables, m_monitorListModel, m_settings);
    QObject::connect(m_monitorListModel.get(), &MonitorListModel::monitorConfigurationChanged, m_screenPlayManager.get(), &ScreenPlayManager::removeAllWallpapers);
}

QString App::version() const
{
    return QGuiApplication::applicationVersion();
}

/*!
    \brief Calls QApplication quit() and can be used to do additional
           tasks before exiting.
*/
void App::exit()
{
    QApplication::instance()->quit();
}

}
