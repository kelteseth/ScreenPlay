#include "ScreenPlay/app.h"

#include "steam/steam_qt_enums_generated.h"
#include <QProcessEnvironment>
#include <QVersionNumber>
#include <QQuickStyle>

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

    QGuiApplication::setWindowIcon(QIcon(":/qml/ScreenPlayApp/assets/icons/app.ico"));
    QGuiApplication::setOrganizationName("ScreenPlay");
    QGuiApplication::setOrganizationDomain("screen-play.app");
    QGuiApplication::setApplicationName("ScreenPlay");
    QGuiApplication::setApplicationVersion(QVersionNumber(0, 15, 0).toString());
    QGuiApplication::setQuitOnLastWindowClosed(false);

    QString fontsPath = QGuiApplication::instance()->applicationDirPath() + "/assets/fonts/";
#if defined(Q_OS_OSX)
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

    QQuickWindow::setTextRenderType(QQuickWindow::TextRenderType::NativeTextRendering);

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

    auto* guiApplication = QGuiApplication::instance();

    // Set visible if the -silent parameter was not set
    if (guiApplication->arguments().contains("-silent")) {
        qInfo() << "Starting in silent mode.";
        settings()->setSilentStart(true);
    }

    qmlRegisterSingletonInstance("ScreenPlay", 1, 0, "App", this);
    m_mainWindowEngine->addImportPath(guiApplication->applicationDirPath() + "/qml");
#if defined(Q_OS_OSX)
    QDir workingDir(guiApplication->applicationDirPath());
    workingDir.cdUp();
    workingDir.cdUp();
    workingDir.cdUp();
    // OSX Development workaround:
    m_mainWindowEngine->addImportPath(workingDir.path()+"/qml");
#endif
    guiApplication->addLibraryPath(guiApplication->applicationDirPath()+ "/qml");

    if (m_settings->desktopEnvironment() == Settings::DesktopEnvironment::KDE) {
        setupKDE();
    }

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
    \brief Calls QApplication quit() and can be used to do additional
           tasks before exiting.
*/
void App::exit()
{
    QApplication::instance()->quit();
}

bool App::isKDEInstalled()
{
    QProcess plasmaShellVersionProcess;
    plasmaShellVersionProcess.start("plasmashell", { "--version" });
    plasmaShellVersionProcess.waitForFinished();
    QString versionOut = plasmaShellVersionProcess.readAll();
    if (!versionOut.contains("plasmashell ")) {
        qWarning() << "Unable to read plasma shell version. ScreenPlay only works on KDE";
        return false;
    }
    return true;
}

void App::installKDEWallpaper()
{

    qInfo() << "Install ScreenPlay KDE Wallpaper";
    QProcess process;
    process.setWorkingDirectory(m_appKdeWallapperPath);
    process.start("plasmapkg2", { "--install", "ScreenPlay" });
    process.waitForFinished();
    process.terminate();
}

void App::upgradeKDEWallpaper()
{

    qInfo() << "Upgrade ScreenPlay KDE Wallpaper";
    QProcess process;
    process.setWorkingDirectory(m_appKdeWallapperPath);
    process.start("plasmapkg2", { "--upgrade", "ScreenPlay" });
    process.waitForFinished();
    process.terminate();
    qInfo() << process.readAllStandardError() << process.readAllStandardOutput();
}

void App::restartKDE()
{
    qInfo() << "Restart KDE ";
    QProcess process;
    process.start("kquitapp5", { "plasmashell" });
    process.waitForFinished();
    process.terminate();
    qInfo() << process.readAllStandardError() << process.readAllStandardOutput();
    process.startDetached("kstart5", { "plasmashell" });
    qInfo() << process.readAllStandardError() << process.readAllStandardOutput();
}

std::optional<bool> App::isNewestKDEWallpaperInstalled()
{

    QFileInfo installedWallpaperMetadata(m_kdeWallpaperPath + "/metadata.desktop");
    QSettings installedWallpaperSettings(installedWallpaperMetadata.absoluteFilePath(), QSettings::Format::IniFormat);
    installedWallpaperSettings.beginGroup("Desktop Entry");
    const QString installedWallpaperVersion = installedWallpaperSettings.value("Version").toString();
    installedWallpaperSettings.endGroup();
    const QVersionNumber installedVersionNumber = QVersionNumber::fromString(installedWallpaperVersion);

    QFileInfo currentWallpaperMetadata(m_appKdeWallapperPath + "/ScreenPlay/metadata.desktop");
    QSettings currentWallpaperSettings(currentWallpaperMetadata.absoluteFilePath(), QSettings::Format::IniFormat);
    currentWallpaperSettings.beginGroup("Desktop Entry");
    const QString currentWallpaperVersion = currentWallpaperSettings.value("Version").toString();
    currentWallpaperSettings.endGroup();
    const QVersionNumber currentVersionNumber = QVersionNumber::fromString(currentWallpaperVersion);

    qInfo() << "installedVersionNumber" << installedVersionNumber << "currentVersionNumber " << currentVersionNumber;
    if (installedVersionNumber.isNull() || currentVersionNumber.isNull()) {
        qInfo() << "Unable to parse version number from:" << currentWallpaperVersion << installedWallpaperVersion;
        qInfo() << "Reinstall ScreenPlay Wallpaper";
        return std::nullopt;
    } else {
        return { installedVersionNumber >= currentVersionNumber };
    }
}

/*!
   \brief
*/
bool App::setupKDE()
{

    m_kdeWallpaperPath = QDir(QDir::homePath() + "/.local/share/plasma/wallpapers/ScreenPlay/").canonicalPath();
    m_appKdeWallapperPath = QGuiApplication::instance()->applicationDirPath() + "/kde";

    if (!isKDEInstalled())
        return false;

    QFileInfo installedWallpaperMetadata(m_kdeWallpaperPath + "/metadata.desktop");
    if (!installedWallpaperMetadata.exists()) {
        installKDEWallpaper();
        restartKDE();
        return true;
    }
    if (auto isNewer = isNewestKDEWallpaperInstalled()) {
        if (!isNewer.value()) {
            upgradeKDEWallpaper();
            return true;
        }

        qInfo() << "All up to date!" << isNewer.value();
        return true;
    } else {
        return false;
    }
}

}
