// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#include "ScreenPlay/settings.h"
#include "ScreenPlayUtil/util.h"
#include <QFileInfo>
#include <QSysInfo>

#include "ScreenPlay/CMakeVariables.h"

#ifdef Q_OS_WIN
#include <qt_windows.h>
#endif

namespace ScreenPlay {

/*!
    \class ScreenPlay::Settings
    \inmodule ScreenPlay
    \brief Global settings class for reading and writing settings.

    Used for:
    \list
        \li User configuration via AppData\Local\ScreenPlay\ScreenPlay
        \list
            \li profiles.json - saved wallpaper and widgets config
            \li Computer\\HKEY_CURRENT_USER\\Software\\ScreenPlay\\ScreenPlay - ScreenPlayContentPath for steam plugin
        \endlist
        \li Communication via the SDK Connector
    \endlist
    Currently we save the regular settings (not the setup of the wallpaper and widgets) in two different locations.
    This can change in the future!
*/

/*!
    \brief Constructor and sets up:
    \list 1
        \li Sets the git build hash via ScreenPlay.pro c++ define
        \li Checks the langauge via settings or system and available ones and installes a translator.
        \li Checks the AbsoluteStoragePath.
        \li Checks regisitry for steam plugin settings
        \li Parses autostart, anonymousTelemetry, highPriorityStart
        \li Checks ScreenPlayWallpaper and ScreenPlayWidgets executable paths.
    \endlist
    More errorchecking is needed here. For example when the proile or settings json cannot to parsed, use the default settings.
 */
Settings::Settings(const std::shared_ptr<GlobalVariables>& globalVariables,
    QObject* parent)
    : QObject(parent)
    , m_globalVariables { globalVariables }
{
    const QString qtVersion = QString("Qt Version: %1.%2.%3\n").arg(QT_VERSION_MAJOR).arg(QT_VERSION_MINOR).arg(QT_VERSION_PATCH);
    const QString buildType = QString("Build type: %1\n").arg(QString(SCREENPLAY_BUILD_TYPE));
    const QString buildDate = QString("Build date: %1\n").arg(QString(SCREENPLAY_BUILD_DATE));
    const QString commitHash = QString("Git commit hash: %1\n").arg(QString(SCREENPLAY_GIT_COMMIT_HASH));
    const QString isDeployVersion = QString("Is deploy version: %1\n").arg((SCREENPLAY_DEPLOY_VERSION ? QString("✅ Yes") : QString("❌ No")));
    const QString isSteamVersion = QString("Is steam version: %1").arg((SCREENPLAY_STEAM_VERSION ? QString("✅ Yes") : QString("❌ No")));
    setBuildInfos(qtVersion + buildType + buildDate + commitHash + isDeployVersion + isSteamVersion);

    setSteamVersion(SCREENPLAY_STEAM_VERSION);

#ifdef Q_OS_WIN
    setDesktopEnvironment(DesktopEnvironment::Windows);
#endif
#ifdef Q_OS_MACOS
    setDesktopEnvironment(DesktopEnvironment::OSX);
#endif
#ifdef Q_OS_LINUX
    // We only support Wayland wl_roots for now
    setDesktopEnvironment(DesktopEnvironment::Wayland);
#endif

    qRegisterMetaType<Settings::Language>("Settings::Language");
    qRegisterMetaType<Settings::Theme>("Settings::Theme");
    qRegisterMetaType<Settings::DesktopEnvironment>("Settings::DesktopEnvironment");

    qmlRegisterUncreatableType<Settings>("Settings", 1, 0, "Settings", "Error only for enums");

    // Lets not set the dev version as startup.
    if (SCREENPLAY_DEPLOY_VERSION)
        if (desktopEnvironment() == DesktopEnvironment::Windows) {
            QSettings settings("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
            if (!settings.childGroups().contains("ScreenPlay", Qt::CaseSensitive)) {
                settings.setValue("ScreenPlay", QDir::toNativeSeparators(QCoreApplication::applicationFilePath()) + " -silent");
                settings.sync();
                if (!m_qSettings.contains("Autostart")) {
                    m_qSettings.setValue("Autostart", true);
                    m_qSettings.sync();
                }
            }
        }

    setCheckWallpaperVisible(m_qSettings.value("CheckWallpaperVisible", false).toBool());
    setHighPriorityStart(m_qSettings.value("ScreenPlayExecutable", false).toBool());
    if (m_qSettings.contains("VideoFillMode")) {
        auto value = m_qSettings.value("VideoFillMode").toString();
        setVideoFillMode(QStringToEnum<FillMode::FillMode>(value, FillMode::FillMode::Cover));
    } else {
        setVideoFillMode(FillMode::FillMode::Cover);
    }

    if (m_qSettings.contains("Theme")) {
        auto value = m_qSettings.value("Theme").toString();
        setTheme(QStringToEnum<Theme>(value, Theme::Dark));
    } else {
        setTheme(Theme::Dark);
    }

    setAnonymousTelemetry(m_qSettings.value("AnonymousTelemetry", true).toBool());

    // Wallpaper and Widgets config
    QFile profilesFile(m_globalVariables->localSettingsPath().toString() + "/profiles.json");
    if (!profilesFile.exists()) {
        qInfo("No profiles.json found, creating default profiles.json");
        writeJsonFileFromResource("profiles");
    }

    initInstalledPath();
    setupWidgetAndWindowPaths();
}

/*!
  \brief Writes the default JsonFile from the resources and the given \a filename. Currently we have two default json files:
   \list
    \li profiles.json
    \li settings.json
   \endlist
*/
void Settings::writeJsonFileFromResource(const QString& filename)
{
    QFile file(m_globalVariables->localSettingsPath().toString() + "/" + filename + ".json");
    QDir directory(m_globalVariables->localSettingsPath().toString());
    if (!directory.exists()) {
        directory.mkpath(directory.path());
    }
    QFile defaultSettings(":/qml/ScreenPlayApp/" + filename + ".json");

    file.open(QIODevice::WriteOnly | QIODevice::Text);
    defaultSettings.open(QIODevice::ReadOnly | QIODevice::Text);

    QTextStream out(&file);
    QTextStream defaultOut(&defaultSettings);

    out << defaultOut.readAll();

    file.close();
    defaultSettings.close();
}

/*!
  \brief To have a better developer experience we check if we use a debug version. Then we assume
  That the paths are the default QtCreator paths and set the widgets and wallpaper executable
  paths accordingly.
*/
void Settings::setupWidgetAndWindowPaths()
{
    QDir workingDir(QGuiApplication::applicationDirPath());
    const QString osType = QSysInfo::productType();

    QString godotVersion = QString(SCREENPLAY_GODOT_VERSION);
    QString godotReleaseType = QString(SCREENPLAY_GODOT_RELEASE_TYPE);
    if (osType == "windows") {
        m_globalVariables->setWidgetExecutablePath(QUrl(workingDir.path() + "/ScreenPlayWidget" + ScreenPlayUtil::executableBinEnding()));
        m_globalVariables->setWallpaperExecutablePath(QUrl(workingDir.path() + "/ScreenPlayWallpaper" + ScreenPlayUtil::executableBinEnding()));
        m_globalVariables->setGodotWallpaperExecutablePath(QUrl(workingDir.path() + "/ScreenPlayWallpaperGodot" + ScreenPlayUtil::executableBinEnding()));
        const auto godotEditorName = "Godot_" + godotVersion + "_win64.exe";
        m_globalVariables->setGodotEditorExecutablePath(QUrl(workingDir.path() + "/" + godotEditorName));
        if (!QFileInfo::exists(m_globalVariables->godotWallpaperExecutablePath().toString())) {
            qInfo() << "godotWallpaperExecutablePath:" << m_globalVariables->godotWallpaperExecutablePath();
            qCritical("Godot Wallpaper not found");
        }
        if (!QFileInfo::exists(m_globalVariables->godotEditorExecutablePath().toString())) {
            qInfo() << "godotEditorExecutablePath :" << m_globalVariables->godotEditorExecutablePath();
            qCritical("Godot Editor not found");
        }
    } else if (osType == "macos") {
        m_globalVariables->setWidgetExecutablePath(QUrl::fromUserInput(workingDir.path() + "/ScreenPlayWidget").toLocalFile());
        m_globalVariables->setWallpaperExecutablePath(QUrl::fromUserInput(workingDir.path()  + "/ScreenPlayWallpaper").toLocalFile());
        m_globalVariables->setGodotWallpaperExecutablePath(QUrl(workingDir.path() + "/ScreenPlayWallpaperGodot").toLocalFile());
        const auto godotEditorName = "Godot_" + godotVersion + "-stable_osx.universal";
        m_globalVariables->setGodotEditorExecutablePath(QUrl(workingDir.path() + "/" + godotEditorName));
    } else if (osType == "linux" || osType == "ubuntu") {
        m_globalVariables->setWidgetExecutablePath(QUrl(workingDir.path() + "/ScreenPlayWidget"));
        m_globalVariables->setWallpaperExecutablePath(QUrl(workingDir.path() + "/ScreenPlayWallpaper"));
        m_globalVariables->setGodotWallpaperExecutablePath(QUrl(workingDir.path() + "/ScreenPlayWallpaperGodot"));
        const auto godotEditorName = "Godot_" + godotVersion + "-stable_x11.64";
        m_globalVariables->setGodotEditorExecutablePath(QUrl(workingDir.path() + "/" + godotEditorName));
    } else {
        qFatal("OS not supported.");
    }

    if (!QFileInfo::exists(m_globalVariables->widgetExecutablePath().toString())) {
        qInfo() << "widgetExecutablePath:" << m_globalVariables->widgetExecutablePath();
        qCritical("widget executable not found!");
    }
    if (!QFileInfo::exists(m_globalVariables->wallpaperExecutablePath().toString())) {
        qInfo() << "wallpaperExecutablePath:" << m_globalVariables->wallpaperExecutablePath();
        qCritical("wallpaper executable not found!");
    }
}

/*!
  \brief When no default language is set in the registry we check the system set language. If there is no
  matching translation is available we set it to english. This function gets called from the UI when
  the user manually changes the language.
*/
void Settings::restoreDefault(const QString& appConfigLocation, const QString& settingsFileType)
{
    QString fullSettingsPath { appConfigLocation + "/" + settingsFileType + ".json" };
    qWarning() << "Unable to load config from " << fullSettingsPath
               << ". Restoring default!";
    QFile file { fullSettingsPath };
    file.remove();
    writeJsonFileFromResource(settingsFileType);
}

void Settings::initInstalledPath()
{
    const QString contentPath = m_qSettings.value("ScreenPlayContentPath", "").toString();

    // Steamless
    if (!steamVersion() && contentPath.isEmpty()) {
        const QString path = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
        m_qSettings.setValue("ScreenPlayContentPath", QUrl::fromUserInput(path));
        m_qSettings.sync();
        m_globalVariables->setLocalStoragePath(path);
        return;
    }

    // Steam
    if (contentPath.isEmpty()) {
        return initSteamInstalledPath();
    }

    m_globalVariables->setLocalStoragePath(QUrl::fromUserInput(contentPath));
}

/*!
  \brief We must use this (ugly) method, because to stay FOSS we cannot call the steamAPI here !

  We start with the assumption that when we go up 2 folder.
  So that there must be at least a common folder:
  Windows example:
  From -> C:\Program Files (x86)\Steam\steamapps\common\ScreenPlay
  To   -> C:\Program Files (x86)\Steam\steamapps\
  Dest.-> C:\Program Files (x86)\Steam\steamapps\workshop\content\672870

  When we reach the folder it _can_ contain a workshop folder when the user
  previously installed any workshop content. If the folder does not exsist we
  need to create it by hand. Normally Steam will create this folder but we need to
  set it here at this point so that the QFileSystemWatcher in InstalledListModel does
  not generate warnings.
*/
void Settings::initSteamInstalledPath()
{
    QString appBasePath = QGuiApplication::instance()->applicationDirPath();
    if (desktopEnvironment() == DesktopEnvironment::OSX) {
        appBasePath += "/../../..";
    }
    QString path = appBasePath + "/../../workshop/content/672870";
    qInfo() << "InitSteamInstalledPath:" << path;

    QDir dir;
    if (!dir.exists()) {
        if (!dir.mkpath(path)) {
            qWarning() << "Could not create steam workshop path for path: " << path;
        }
    }

    if (QDir(path).exists()) {
        m_globalVariables->setLocalStoragePath(QUrl::fromUserInput(path));
        m_qSettings.setValue("ScreenPlayContentPath", dir.cleanPath(path));
        m_qSettings.sync();
    } else {
        qWarning() << "The following path could not be resolved to search for workshop content: " << path;
    }
}

/*!
    \brief Checks if there is already a saved language. If not we try to use the system langauge.
     If we do not support the system language we use english.
*/
void Settings::setupLanguage()
{
    QString langCode;
    if (m_qSettings.value("Language").isNull()) {
        langCode = QLocale::system().name();
        // QML enum begin with uppercase: de_DE -> De_DE
        langCode = langCode.replace(0, 1, langCode.at(0).toUpper());
    } else {
        langCode = m_qSettings.value("Language").toString();
    }

    setLanguage(QStringToEnum<Language>(langCode, Language::En_US));
    retranslateUI();
}

/*!
    \brief Check for supported langauges. If we use a langauge that not uses
    latin characters, we change the font. For example this happens for korean user. We ship google
    Noto Sans CJK KR Regular for this..
*/
bool Settings::retranslateUI()
{
    QString langCode = fixLanguageCode(QVariant::fromValue(language()).toString());

    QFile tsFile;
    const QString qmPath = QGuiApplication::applicationDirPath() + "/translations/ScreenPlay_" + langCode + ".qm";
    if (tsFile.exists(qmPath)) {
        if (!m_translator.load(qmPath)) {
            qWarning() << "Unable to load translation file: " << qmPath;
            return false;
        }
        auto* guiAppInst = dynamic_cast<QGuiApplication*>(QGuiApplication::instance());
        guiAppInst->installTranslator(&m_translator);
        emit requestRetranslation();

        if (language() == Settings::Language::Ko_KR) {
            setFont("Noto Sans CJK KR Regular");
        } else {
            setFont("Roboto");
        }
        return true;
    }
    qWarning() << tsFile.fileName() << ", cannot be loaded width langCode " << langCode;
    return false;
}

/*!
   \brief We must translate between qml langauge code and real ones.
*/
QString Settings::fixLanguageCode(const QString& languageCode)
{
    QString langCode = languageCode;
    // QML enums must begin with uppercase, but our code begin with lowercase
    langCode = langCode.replace(0, 1, langCode.at(0).toLower());
    // For US we use the default .ts file without langauge code
    if (langCode == "en_US")
        langCode = "";
    return langCode;
}

}

#include "moc_settings.cpp"
