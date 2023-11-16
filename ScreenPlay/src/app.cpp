// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#include "ScreenPlay/app.h"
#if defined(Q_OS_MACOS)
#include "ScreenPlayUtil/macutils.h"
#endif

#include "ScreenPlay/CMakeVariables.h"
#include "app.h"
#include "steam/steam_qt_enums_generated.h"
#include <QGuiApplication>
#include <QProcessEnvironment>
#include <QQuickStyle>
#include <QVersionNumber>

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

    QGuiApplication::setWindowIcon(QIcon(":/qml/ScreenPlayApp/assets/icons/app.ico"));
    QGuiApplication::setOrganizationName("ScreenPlay");
    QGuiApplication::setOrganizationDomain("screen-play.app");
    QGuiApplication::setApplicationName("ScreenPlay");
    QGuiApplication::setApplicationVersion(QString(SCREENPLAY_VERSION));
    QGuiApplication::setQuitOnLastWindowClosed(false);

    QString fontsPath = QGuiApplication::instance()->applicationDirPath() + "/assets/fonts/";
#if defined(Q_OS_MACOS)
    fontsPath = QGuiApplication::instance()->applicationDirPath() + "/../Resources/fonts/";
#endif
    const QDir fontsDir(fontsPath);
    if (!fontsDir.isEmpty() && fontsDir.exists()) {
        QDirIterator it(fontsPath, { "*.ttf", "*.otf" }, QDir::Files);
        while (it.hasNext()) {
            QFontDatabase::addApplicationFont(it.next());
        }
    } else {
        qWarning() << "Unable to load font from: " << fontsPath;
    }

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

    // ScreenPlayManager first to check if another ScreenPlay Instace is running
    m_screenPlayManager = std::make_unique<ScreenPlayManager>();
    m_isAnotherScreenPlayInstanceRunning = m_screenPlayManager->isAnotherScreenPlayInstanceRunning();
}

/*!
    \brief Used for initialization after the constructor. The sole purpose is to check if
    another ScreenPlay instance is running and then quit early. This is also because we cannot
    call QGuiApplication::quit(); in the SDKConnector before the app.exec(); ( the Qt main event
    loop ) has started.
*/
void App::init()
{

    using std::make_shared, std::make_unique;

    // Util should be created as first so we redirect qDebugs etc. into the log
    m_util = make_unique<Util>();
    m_globalVariables = make_shared<GlobalVariables>();
    m_monitorListModel = make_shared<MonitorListModel>();
    m_profileListModel = make_shared<ProfileListModel>(m_globalVariables);
    m_settings = make_shared<Settings>(m_globalVariables);
    m_installedListModel = make_shared<InstalledListModel>(m_globalVariables, m_settings);
    m_installedListFilter = make_shared<InstalledListFilter>(m_installedListModel);
    m_mainWindowEngine = make_unique<QQmlApplicationEngine>();

    // Only create anonymousTelemetry if user did not disallow!
    if (m_settings->anonymousTelemetry()) {
#if defined(Q_OS_WIN)

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
#endif
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

    auto* guiAppInst = dynamic_cast<QGuiApplication*>(QGuiApplication::instance());

    // Set visible if the -silent parameter was not set
    if (guiAppInst->arguments().contains("-silent")) {
        qInfo() << "Starting in silent mode.";
        settings()->setSilentStart(true);
    }

    qmlRegisterSingletonInstance("ScreenPlay", 1, 0, "App", this);
    m_mainWindowEngine->addImportPath(guiAppInst->applicationDirPath() + "/qml");
    guiAppInst->addLibraryPath(guiAppInst->applicationDirPath() + "/qml");

    QQuickStyle::setStyle("Material");
    m_mainWindowEngine->load(QUrl(QStringLiteral("qrc:/qml/ScreenPlayApp/main.qml")));

    // Must be called last to display a error message on startup by the qml engine
    m_screenPlayManager->init(m_globalVariables, m_monitorListModel, m_settings);
    QObject::connect(m_monitorListModel.get(), &MonitorListModel::monitorConfigurationChanged, m_screenPlayManager.get(), &ScreenPlayManager::removeAllWallpapers);
}

QString App::version() const
{
    return QGuiApplication::applicationVersion();
}

/*!
    \brief Calls QGuiApplication quit() and can be used to do additional
           tasks before exiting.
*/
void App::exit()
{
    m_screenPlayManager->removeAllWallpapers();
    m_screenPlayManager->removeAllWidgets();
    // Must be called inside a separate event loop otherwise we
    // would kill the qml engine while it is calling this function.
    // A single shot timer is a handy woraround for this.
    QTimer::singleShot(0, this, [this]() {
        auto* appInst = QGuiApplication::instance();
        // We must ensure that we kill the qml engine first
        // before we destory the rest of the app
        m_mainWindowEngine->clearSingletons();
        m_mainWindowEngine.reset();
        appInst->quit();
    });
}

void App::showDockIcon(const bool show)
{
#if defined(Q_OS_MACOS)
    MacUtils::instance()->showDockIcon(show);
#endif
}

/*!
    \property App::globalVariables
    \brief .

   .
*/
void App::setGlobalVariables(GlobalVariables* globalVariables)
{
    if (m_globalVariables.get() == globalVariables)
        return;

    m_globalVariables.reset(globalVariables);
    emit globalVariablesChanged(m_globalVariables.get());
}

/*!
    \property App::screenPlayManager
    \brief Sets the screen play manager.
*/
void App::setScreenPlayManager(ScreenPlayManager* screenPlayManager)
{
    if (m_screenPlayManager.get() == screenPlayManager)
        return;

    m_screenPlayManager.reset(screenPlayManager);
    emit screenPlayManagerChanged(m_screenPlayManager.get());
}
/*!
    \property App::create
    \brief .

   .
*/
void App::setCreate(Create* create)
{
    if (m_create.get() == create)
        return;

    m_create.reset(create);
    emit createChanged(m_create.get());
}
/*!
    \property App::util
    \brief .

   .
*/
void App::setUtil(Util* util)
{
    if (m_util.get() == util)
        return;

    m_util.reset(util);
    emit utilChanged(m_util.get());
}
/*!
    \property App::settings
    \brief .

   .
*/
void App::setSettings(Settings* settings)
{
    if (m_settings.get() == settings)
        return;

    m_settings.reset(settings);
    emit settingsChanged(m_settings.get());
}
/*!
    \property App::installedListModel
    \brief .

   .
*/
void App::setInstalledListModel(InstalledListModel* installedListModel)
{
    if (m_installedListModel.get() == installedListModel)
        return;

    m_installedListModel.reset(installedListModel);
    emit installedListModelChanged(m_installedListModel.get());
}
/*!
    \property App::monitorListModel
    \brief .

   .
*/
void App::setMonitorListModel(MonitorListModel* monitorListModel)
{
    if (m_monitorListModel.get() == monitorListModel)
        return;

    m_monitorListModel.reset(monitorListModel);
    emit monitorListModelChanged(m_monitorListModel.get());
}
/*!
    \property App::profileListModel
    \brief .

   .
*/
void App::setProfileListModel(ProfileListModel* profileListModel)
{
    if (m_profileListModel.get() == profileListModel)
        return;

    m_profileListModel.reset(profileListModel);
    emit profileListModelChanged(m_profileListModel.get());
}
/*!
    \property App::installedListFilter
    \brief .

   .
*/
void App::setInstalledListFilter(InstalledListFilter* installedListFilter)
{
    if (m_installedListFilter.get() == installedListFilter)
        return;

    m_installedListFilter.reset(installedListFilter);
    emit installedListFilterChanged(m_installedListFilter.get());
}
/*!
    \property App::mainWindowEngine
    \brief .

   .
*/
void App::setMainWindowEngine(QQmlApplicationEngine* mainWindowEngine)
{
    if (m_mainWindowEngine.get() == mainWindowEngine)
        return;

    m_mainWindowEngine.reset(mainWindowEngine);
    emit mainWindowEngineChanged(m_mainWindowEngine.get());
}
/*!
    \property App::wizards
    \brief .

   .
*/
void App::setWizards(Wizards* wizards)
{
    if (m_wizards.get() == wizards)
        return;

    m_wizards.reset(wizards);
    emit wizardsChanged(m_wizards.get());
}
}

#include "moc_app.cpp"
