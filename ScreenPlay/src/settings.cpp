#include "settings.h"

/*!
    \class Settings
    \brief Used for:
    \list
        \i User configuration
        \i Communication via the SDK Connector
    \endlist

*/

namespace ScreenPlay {

Settings::Settings(
    const shared_ptr<InstalledListModel>& ilm,
    const shared_ptr<GlobalVariables>& globalVariables,
    QObject* parent)
    : QObject(parent)
    , m_version { QVersionNumber(1, 0, 0) }
    , m_installedListModel { ilm }
    , m_globalVariables { globalVariables }
{
    auto* app = static_cast<QGuiApplication*>(QGuiApplication::instance());

    setGitBuildHash(GIT_VERSION);

#ifdef QT_NO_DEBUG
    qInfo() << "ScreenPlay git hash: " << gitBuildHash();
#endif

    QFontDatabase::addApplicationFont(":/assets/fonts/LibreBaskerville-Italic.ttf");
    QFontDatabase::addApplicationFont(":/assets/fonts/Roboto-Light.ttf");
    QFontDatabase::addApplicationFont(":/assets/fonts/Roboto-Regular.ttf");
    QFontDatabase::addApplicationFont(":/assets/fonts/Roboto-Thin.ttf");
    QFontDatabase::addApplicationFont(":/assets/fonts/RobotoMono-Light.ttf");
    QFontDatabase::addApplicationFont(":/assets/fonts/RobotoMono-Thin.ttf");

    if (m_qSettings.value("language").isNull()) {
        auto locale = QLocale::system().uiLanguages();
        auto localeSplits = locale.at(0).split("-");

        // Only install a translator if one exsists
        QFile tsFile;
        qDebug() << ":/translations/ScreenPlay_" + localeSplits.at(0) + ".qm";
        if (tsFile.exists(":/translations/ScreenPlay_" + localeSplits.at(0) + ".qm")) {
            m_translator.load(":/translations/ScreenPlay_" + localeSplits.at(0) + ".qm");
            m_qSettings.setValue("language", QVariant(localeSplits.at(0)));
            m_qSettings.sync();
            app->installTranslator(&m_translator);
        }
    } else {
        QFile tsFile;
        if (tsFile.exists(":/translations/ScreenPlay_" + m_qSettings.value("language").toString() + ".qm")) {
            m_translator.load(":/translations/ScreenPlay_" + m_qSettings.value("language").toString() + ".qm");
            app->installTranslator(&m_translator);
        }
    }

    if (m_qSettings.value("ScreenPlayExecutable").isNull()) {
        m_qSettings.setValue("ScreenPlayExecutable", QDir::toNativeSeparators(QCoreApplication::applicationFilePath()));
        m_qSettings.sync();
    }

    QString appConfigLocation = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
     m_globalVariables->setLocalSettingsPath(QUrl::fromUserInput(appConfigLocation));

    if (!QDir(appConfigLocation).exists()) {
        if (!QDir().mkpath(appConfigLocation)) {
            qWarning("ERROR: Cloud not create appConfigLocation dir");
            return;
        }
    }

    // App settings
    QFile settingsFile;
    settingsFile.setFileName(appConfigLocation + "/settings.json");
    if (!settingsFile.exists()) {
        qWarning("No Settings found, creating default settings");
        createDefaultConfig();
    }

    // Wallpaper and Widgets config
    QFile profilesFile;
    profilesFile.setFileName(appConfigLocation + "/profiles.json");
    if (!profilesFile.exists()) {
        qWarning("No profiles.json found, creating default settings");
        createDefaultProfiles();
    }

    QJsonDocument configJsonDocument;
    QJsonParseError parseError {};
    QJsonObject configObj;

    settingsFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QString config = settingsFile.readAll();
    configJsonDocument = QJsonDocument::fromJson(config.toUtf8(), &parseError);

    if (!(parseError.error == QJsonParseError::NoError)) {
        qWarning("Settings Json Parse Error ");
        return;
    }

    configObj = configJsonDocument.object();

    std::optional<QVersionNumber> version = Util::getVersionNumberFromString(configObj.value("version").toString());

    //Checks if the settings file has the same version as ScreeenPlay
    if (version.has_value() && version.value() != m_version) {
        // TODO(Kelteseth): Display error message
        qWarning() << "Version missmatch fileVersion: " << version.value().toString() << "m_version: " << m_version.toString();
        return;
    }

    //If empty use steam workshop location
    if (QString(configObj.value("absoluteStoragePath").toString()).isEmpty()) {
        QDir steamTmpUrl = QDir::toNativeSeparators(QCoreApplication::applicationFilePath());
        steamTmpUrl.cdUp();
        steamTmpUrl.cdUp();
        steamTmpUrl.cdUp();
        steamTmpUrl.cd("workshop");
        steamTmpUrl.cd("content");
        steamTmpUrl.cd("672870");

        m_globalVariables->setLocalStoragePath(steamTmpUrl.path());
    } else {
        m_globalVariables->setLocalStoragePath(QUrl::fromUserInput(configObj.value("absoluteStoragePath").toString()));
     }

    if (m_qSettings.value("ScreenPlayContentPath").toUrl() != m_globalVariables->localStoragePath()) {
        m_qSettings.setValue("ScreenPlayContentPath", QDir::toNativeSeparators( m_globalVariables->localStoragePath().toString().remove("file:///")));
        m_qSettings.sync();
    }

    m_autostart = configObj.value("autostart").toBool();
    m_highPriorityStart = configObj.value("highPriorityStart").toBool();
    m_sendStatistics = configObj.value("sendStatistics").toBool();

    setupWidgetAndWindowPaths();
}

void Settings::writeSingleSettingConfig(QString name, QVariant value)
{
    QString filename = m_globalVariables->localSettingsPath().toLocalFile() + "/settings.json";
    auto obj = Util::openJsonFileToObject(filename);

    if (!obj.has_value()) {
        qWarning("Settings Json Parse Error ");
        return;
    }

    obj.value().insert(name, value.toJsonValue());

    QFile configTmp;
    configTmp.setFileName(filename);
    configTmp.open(QIODevice::ReadWrite | QIODevice::Truncate);
    QTextStream out(&configTmp);
    out << QJsonDocument(obj.value()).toJson();

    configTmp.close();
}

void Settings::setqSetting(const QString& key, const QString& value)
{
    m_qSettings.setValue(key, value);
    m_qSettings.sync();
}

void Settings::createDefaultConfig()
{

    QFile file(m_globalVariables->localSettingsPath().toLocalFile() + "/settings.json");
    QFile defaultSettings(":/settings.json");

    file.open(QIODevice::WriteOnly | QIODevice::Text);
    defaultSettings.open(QIODevice::ReadOnly | QIODevice::Text);

    QTextStream out(&file);
    QTextStream defaultOut(&defaultSettings);

    out << defaultOut.readAll();

    file.close();
    defaultSettings.close();
}

void Settings::createDefaultProfiles()
{

    QFile file(m_globalVariables->localSettingsPath().toLocalFile() + "/profiles.json");
    QFile defaultSettings(":/profiles.json");

    file.open(QIODevice::WriteOnly | QIODevice::Text);
    defaultSettings.open(QIODevice::ReadOnly | QIODevice::Text);

    QTextStream out(&file);
    QTextStream defaultOut(&defaultSettings);

    out << defaultOut.readAll();

    file.close();
    defaultSettings.close();
}

void Settings::setupWidgetAndWindowPaths()
{
    QDir workingDir(QDir::currentPath());
    QDir baseDir(QDir::currentPath());

#ifdef QT_DEBUG

    if (workingDir.cdUp()) {

#ifdef Q_OS_OSX
        m_globalVariables->setWidgetExecutablePath(QUrl::fromUserInput(workingDir.path() + "/../../../ScreenPlayWidget/ScreenPlayWidget.app/Contents/MacOS/ScreenPlayWidget").toLocalFile());
        m_globalVariables->setWallpaperExecutablePath(QUrl::fromUserInput(workingDir.path() + "/../../../ScreenPlayWallpaper/ScreenPlayWallpaper.app/Contents/MacOS/ScreenPlayWallpaper").toLocalFile());
#endif

#ifdef Q_OS_WIN
        m_globalVariables->setWidgetExecutablePath(QUrl(workingDir.path() + "/ScreenPlayWidget/debug/ScreenPlayWidget.exe"));
        m_globalVariables->setWallpaperExecutablePath(QUrl(workingDir.path() + "/ScreenPlayWallpaper/debug/ScreenPlayWallpaper.exe"));
#endif

    }

    // We need to detect the right base path so we can copy later the example projects
    baseDir.cdUp();
    baseDir.cdUp();
    baseDir.cd("ScreenPlay");
    baseDir.cd("ScreenPlay");
#endif
#ifdef QT_NO_DEBUG
    qDebug() << "Starting in Release mode!";

    // If we build in the release version we must be cautious!
    // The working dir in steam is the ScreenPlay.exe location
    // In QtCreator is the dir above ScreenPlay.exe (!)

    workingDir.cdUp();
    workingDir.cd("ScreenPlayWallpaper");

    if (QDir(workingDir.path() + "/release").exists()) {
        // If started by QtCreator
        workingDir.cd("release");
        m_globalVariables->setWallpaperExecutablePath(QUrl(workingDir.path() + "/ScreenPlayWallpaper.exe"));
        workingDir.cdUp();
        workingDir.cdUp();
        workingDir.cd("ScreenPlayWidget");
        workingDir.cd("release");
        m_globalVariables->setWidgetExecutablePath(QUrl(workingDir.path() + "/ScreenPlayWidget.exe"));
    } else {
        // If started by Steam
        m_globalVariables->setWallpaperExecutablePath(QUrl("ScreenPlayWallpaper.exe"));
        m_globalVariables->setWidgetExecutablePath(QUrl("ScreenPlayWidget.exe"));
    }
#endif
}

}
