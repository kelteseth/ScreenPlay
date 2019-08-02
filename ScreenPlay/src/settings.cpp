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

Settings::Settings(const shared_ptr<InstalledListModel>& ilm,
    const shared_ptr<ProfileListModel>& plm,
    const shared_ptr<MonitorListModel>& mlm,
    const shared_ptr<SDKConnector>& sdkc,
    QObject* parent)
    : QObject(parent)
    , m_version { QVersionNumber(0, 0, 1) }
    , m_profileListModel { plm }
    , m_installedListModel { ilm }
    , m_monitorListModel { mlm }
    , m_sdkconnector { sdkc }
{
    auto* app = static_cast<QGuiApplication*>(QGuiApplication::instance());

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

    QFile configTmp;
    QString appConfigLocation = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    m_localSettingsPath = QUrl::fromUserInput(appConfigLocation);
    if (!QDir(appConfigLocation).exists()) {
        if (!QDir().mkpath(appConfigLocation)) {
            qWarning("ERROR: Cloud not create appConfigLocation dir");
            return;
        }
    }

    configTmp.setFileName(appConfigLocation + "/settings.json");

    // App Settings
    if (!configTmp.exists()) {
        qWarning("No Settings found, creating default settings");
        createDefaultConfig();
    }

    QFile profiles;
    profiles.setFileName(appConfigLocation + "/profiles.json");
    if (!profiles.exists()) {
        qWarning("No profiles.json found, creating default settings");
        createDefaultProfiles();
    }

    QJsonDocument configJsonDocument;
    QJsonParseError parseError {};
    QJsonObject configObj;

    configTmp.open(QIODevice::ReadOnly | QIODevice::Text);
    QString config = configTmp.readAll();
    configJsonDocument = QJsonDocument::fromJson(config.toUtf8(), &parseError);

    if (!(parseError.error == QJsonParseError::NoError)) {
        qWarning("Settings Json Parse Error ");
        return;
    }

    configObj = configJsonDocument.object();

    QString tmp(configObj.value("version").toVariant().toString());

    int major = QString(tmp.at(0)).toInt();
    int minor = QString(tmp.at(2)).toInt();
    int patch = QString(tmp.at(4)).toInt();
    QVersionNumber fileVersion(major, minor, patch);

    //Checks if the settings file has the same version as ScreeenPlay
    if (fileVersion != m_version) {
        // TODO(Kelteseth): Display error message
        qWarning() << "Version missmatch fileVersion: " << fileVersion.toString() << "m_version: " << m_version.toString();
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

        m_localStoragePath = steamTmpUrl.path();
    } else {
        m_localStoragePath = QUrl::fromUserInput(configObj.value("absoluteStoragePath").toString());
    }

    if (m_qSettings.value("ScreenPlayContentPath").toUrl() != m_localStoragePath) {
        m_qSettings.setValue("ScreenPlayContentPath", QDir::toNativeSeparators(m_localStoragePath.toString().remove("file:///")));
        m_qSettings.sync();
    }

    m_installedListModel->setabsoluteStoragePath(m_localStoragePath);
    m_profileListModel->m_localStoragePath = m_localStoragePath;

    m_autostart = configObj.value("autostart").toBool();
    m_highPriorityStart = configObj.value("highPriorityStart").toBool();
    m_sendStatistics = configObj.value("sendStatistics").toBool();

    setupWidgetAndWindowPaths();
}


void Settings::writeSingleSettingConfig(QString name, QVariant value)
{

    QJsonDocument configJsonDocument;
    QJsonParseError parseError {};
    QJsonObject configObj;
    QFile configTmp;

    configTmp.setFileName(m_localSettingsPath.toLocalFile() + "/settings.json");
    configTmp.open(QIODevice::ReadOnly | QIODevice::Text);
    QString config = configTmp.readAll();
    configJsonDocument = QJsonDocument::fromJson(config.toUtf8(), &parseError);

    if (!(parseError.error == QJsonParseError::NoError)) {
        qWarning("Settings Json Parse Error ");
        return;
    }
    configObj = configJsonDocument.object();
    configObj.insert(name, value.toJsonValue());

    configTmp.close();
    // Reopen to empty the file via Truncate
    configTmp.open(QIODevice::ReadWrite | QIODevice::Truncate);
    QTextStream out(&configTmp);
    out << QJsonDocument(configObj).toJson();

    configTmp.close();
}

void Settings::saveWallpaperToConfig(const int monitorIndex, const QUrl& absoluteStoragePath, const QString& type)
{
}

void Settings::removeWallpaperFromConfig(const int monitorIndex)
{
}

void Settings::setqSetting(const QString& key, const QString& value)
{
    m_qSettings.setValue(key, value);
    m_qSettings.sync();
}

void Settings::createDefaultConfig()
{

    QFile file(m_localSettingsPath.toLocalFile() + "/settings.json");
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

    QFile file(m_localSettingsPath.toLocalFile() + "/profiles.json");
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
    QDir SPWorkingDir(QDir::currentPath());
    QDir SPBaseDir(QDir::currentPath());

#ifdef QT_DEBUG

    if (SPWorkingDir.cdUp()) {
#ifdef Q_OS_OSX
        setScreenPlayWallpaperPath(QUrl::fromUserInput(SPWorkingDir.path() + "/../../../ScreenPlayWallpaper/ScreenPlayWallpaper.app/Contents/MacOS/ScreenPlayWallpaper").toLocalFile());
        setScreenPlayWidgetPath(QUrl::fromUserInput(SPWorkingDir.path() + "/../../../ScreenPlayWidget/ScreenPlayWidget.app/Contents/MacOS/ScreenPlayWidget").toLocalFile());
        qDebug() << "Setting ScreenPlayWallpaper Path to " << settings.getScreenPlayWallpaperPath();
        qDebug() << "Setting ScreenPlayWdiget Path to " << settings.getScreenPlayWidgetPath();
#endif

#ifdef Q_OS_WIN
        setScreenPlayWallpaperPath(QUrl(SPWorkingDir.path() + "/ScreenPlayWallpaper/debug/ScreenPlayWallpaper.exe"));
        setScreenPlayWidgetPath(QUrl(SPWorkingDir.path() + "/ScreenPlayWidget/debug/ScreenPlayWidget.exe"));
#endif
    }

    // We need to detect the right base path so we can copy later the example projects
    SPBaseDir.cdUp();
    SPBaseDir.cdUp();
    SPBaseDir.cd("ScreenPlay");
    SPBaseDir.cd("ScreenPlay");
    setScreenPlayBasePath(QUrl(SPBaseDir.path()));
#endif
#ifdef QT_NO_DEBUG
    qDebug() << "Starting in Release mode!";
    setScreenPlayBasePath(QUrl(SPWorkingDir.path()));

    // If we build in the release version we must be cautious!
    // The working dir in steam is the ScreenPlay.exe location
    // In QtCreator is the dir above ScreenPlay.exe (!)

    SPWorkingDir.cdUp();
    SPWorkingDir.cd("ScreenPlayWallpaper");

    if (QDir(SPWorkingDir.path() + "/release").exists()) {
        // If started by QtCreator
        SPWorkingDir.cd("release");
        setScreenPlayWallpaperPath(QUrl(SPWorkingDir.path() + "/ScreenPlayWallpaper.exe"));
        SPWorkingDir.cdUp();
        SPWorkingDir.cdUp();
        SPWorkingDir.cd("ScreenPlayWidget");
        SPWorkingDir.cd("release");
        setScreenPlayWidgetPath(QUrl(SPWorkingDir.path() + "/ScreenPlayWidget.exe"));
    } else {
        // If started by Steam
        setScreenPlayWallpaperPath(QUrl("ScreenPlayWallpaper.exe"));
        setScreenPlayWidgetPath(QUrl("ScreenPlayWidget.exe"));
    }
#endif
}


}
