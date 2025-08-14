// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#include "ScreenPlay/settings.h"
#include "CMakeVariables.h"
#include "ScreenPlayCore/util.h"

#include <QDebug>
#include <QDir>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNodeList>
#include <QFile>
#include <QFileInfo>
#include <QFontDatabase>
#include <QGuiApplication>
#include <QIODevice>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QPair>
#include <QProcess>
#include <QProcessEnvironment>
#include <QQmlPropertyMap>
#include <QStandardPaths>
#include <QSysInfo>
#include <QTextStream>
#include <QThread>
#include <QUrl>
#include <QVector>
#include <QtConcurrent/QtConcurrent>
#include <QtGlobal>

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

    const QString kernel = QSysInfo::kernelType();
    if (kernel == "winnt") {
        setDesktopEnvironment(DesktopEnvironment::Windows);
    } else if (kernel == "darwin") {
        setDesktopEnvironment(DesktopEnvironment::OSX);
    } else if (kernel == "linux") {
        // We only support Wayland wl_roots for now
        setDesktopEnvironment(DesktopEnvironment::Wayland);
    }

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
        setVideoFillMode(QStringToEnum<Video::FillMode>(value, Video::FillMode::Cover));
    } else {
        setVideoFillMode(Video::FillMode::Cover);
    }

    if (m_qSettings.contains("Theme")) {
        auto value = m_qSettings.value("Theme").toString();
        setTheme(QStringToEnum<Theme>(value, Theme::Dark));
    } else {
        setTheme(Theme::Dark);
    }

    if (m_qSettings.contains("GodotFps")) {
        auto value = m_qSettings.value("GodotFps").toString();
        setGodotFps(QStringToEnum<GodotFps>(value, GodotFps::Fps60));
    } else {
        setGodotFps(GodotFps::Fps60);
    }

    if (m_qSettings.contains("GraphicsApi")) {
        auto value = m_qSettings.value("GraphicsApi").toString();
        setGraphicsApi(QStringToEnum<ScreenPlayEnums::GraphicsApi>(value, ScreenPlayEnums::GraphicsApi::Auto));
    } else {
        setGraphicsApi(ScreenPlayEnums::GraphicsApi::Auto);
    }

    setAnonymousTelemetry(m_qSettings.value("AnonymousTelemetry", true).toBool());
    setAlwaysMinimize(m_qSettings.value("alwaysMinimize", false).toBool());

    setupProfilesSettings();
    setupInstalledPath();
    setupWidgetAndWindowPaths();
}

/*!
  \brief Writes the default profiles json if we do not have any or we do have a major
         version bump. We delete the old config for now.
*/
void Settings::setupProfilesSettings()
{
    const QString defaultProfilesFilePath = m_globalVariables->localSettingsPath().toString() + "/profiles.json";
    // Wallpaper and Widgets config
    QFile profilesFile(defaultProfilesFilePath);
    if (!profilesFile.exists()) {
        qInfo("No profiles.json found, creating default profiles.json");
        return writeDefaultProfiles();
    }
    profilesFile.close();

    // TODO version check
    const auto jsonObjOpt = Util().openJsonFileToObject(defaultProfilesFilePath);
    if (jsonObjOpt.has_value()) {
        const QJsonObject profilesSettings = jsonObjOpt.value();
        if (!profilesSettings.contains("version"))
            return writeDefaultProfiles();

        const auto profilesSettingsVersion = QVersionNumber::fromString(profilesSettings.value("version").toString());

        if (profilesSettingsVersion.isNull())
            return writeDefaultProfiles();

        if (profilesSettingsVersion < m_profilesVersion)
            return writeDefaultProfiles();
    }
}

void Settings::writeDefaultProfiles()
{
    QFileInfo fileInfo(m_globalVariables->localSettingsPath().toString() + "/profiles.json");
    QFile profilesFile(fileInfo.absoluteFilePath());

    qInfo() << "Writing default profiles config to: " << fileInfo.absoluteFilePath();
    QDir directory(m_globalVariables->localSettingsPath().toString());
    if (!directory.exists()) {
        directory.mkpath(directory.path());
    }
    QFile defaultProfileFile(":/qt/qml/ScreenPlay/profiles.json");

    profilesFile.open(QIODevice::WriteOnly | QIODevice::Text);
    defaultProfileFile.open(QIODevice::ReadOnly | QIODevice::Text);

    QTextStream out(&profilesFile);
    QTextStream defaultOut(&defaultProfileFile);

    out << defaultOut.readAll();

    profilesFile.close();
    defaultProfileFile.close();
}

QVersionNumber Settings::getProfilesVersion() const
{
    return m_profilesVersion;
}

/*!
  \brief To have a better developer experience we check if we use a debug version. Then we assume
  That the paths are the default QtCreator paths and set the widgets and wallpaper executable
  paths accordingly.
*/
void Settings::setupWidgetAndWindowPaths()
{
    QDir appDir(QGuiApplication::applicationDirPath());
    const QString osType = QSysInfo::productType();
    Util util;

    const QString godotVersion = QString(SCREENPLAY_GODOT_VERSION);
    const QString godotReleaseType = QString(SCREENPLAY_GODOT_RELEASE_TYPE);
    const bool isDeployVersion = SCREENPLAY_DEPLOY_VERSION;
    if (osType == "windows") {
        if (isDeployVersion) {
            m_globalVariables->setWidgetExecutablePath(QUrl(appDir.path() + "/SPWidget.exe"));
            m_globalVariables->setWallpaperExecutablePath(QUrl(appDir.path() + "/SPWallpaper.exe"));
        } else {
            m_globalVariables->setWidgetExecutablePath(QUrl(appDir.path() + "/../ScreenPlayWidget/SPWidget.exe"));
            m_globalVariables->setWallpaperExecutablePath(QUrl(appDir.path() + "/../ScreenPlayWallpaper/SPWallpaper.exe"));
        }
        m_globalVariables->setGodotWallpaperExecutablePath(QUrl(appDir.path() + "/ScreenPlayWallpaperGodot.exe"));
        const auto godotEditorName = "Godot_" + godotVersion + "_win64.exe";
        m_globalVariables->setGodotEditorExecutablePath(QUrl(appDir.path() + "/" + godotEditorName));
        if (!QFileInfo::exists(m_globalVariables->godotWallpaperExecutablePath().toString())) {
            qCritical() << "Invalid godotWallpaperExecutablePath:" << m_globalVariables->godotWallpaperExecutablePath();
        }
        if (!QFileInfo::exists(m_globalVariables->godotEditorExecutablePath().toString())) {
            qInfo() << "Invalid godotEditorExecutablePath :" << m_globalVariables->godotEditorExecutablePath();
        }
    } else if (osType == "macos") {
        m_globalVariables->setWidgetExecutablePath(QUrl::fromUserInput(appDir.path() + "/SPWidget").toLocalFile());
        m_globalVariables->setWallpaperExecutablePath(QUrl::fromUserInput(appDir.path() + "/SPWallpaper").toLocalFile());
        m_globalVariables->setGodotWallpaperExecutablePath(QUrl(appDir.path() + "/ScreenPlayWallpaperGodot").toLocalFile());
        const auto godotEditorName = "Godot_" + godotVersion + "-stable_osx.universal";
        m_globalVariables->setGodotEditorExecutablePath(QUrl(appDir.path() + "/" + godotEditorName));
    } else if (QSysInfo::kernelType() == "linux") {
        m_globalVariables->setWidgetExecutablePath(QUrl(appDir.path() + "/../ScreenPlayWidget/SPWidget"));
        m_globalVariables->setWallpaperExecutablePath(QUrl(appDir.path() + "/../ScreenPlayWallpaper/SPWallpaper"));
        m_globalVariables->setGodotWallpaperExecutablePath(QUrl(appDir.path() + "/../ScreenPlayWallpaperGodot"));
        const auto godotEditorName = "Godot_" + godotVersion + "-stable_x11.64";
        m_globalVariables->setGodotEditorExecutablePath(QUrl(appDir.path() + "/" + godotEditorName));
    } else {
        qFatal("OS not supported.");
    }

    if (!QFileInfo::exists(m_globalVariables->widgetExecutablePath().toString())) {
        qCritical() << "Invalid widgetExecutablePath:" << m_globalVariables->widgetExecutablePath();
    }
    if (!QFileInfo::exists(m_globalVariables->wallpaperExecutablePath().toString())) {
        qCritical() << "Invalid wallpaperExecutablePath:" << m_globalVariables->wallpaperExecutablePath();
    }
}

void Settings::setupInstalledPath()
{
    const QString contentPath = m_qSettings.value("ScreenPlayContentPath", "").toString();
    if (!contentPath.isEmpty()) {
        return m_globalVariables->setLocalStoragePath(QUrl::fromUserInput(contentPath));
    }

    if (SCREENPLAY_DEPLOY_VERSION == 0) {
        QString exampleContentPath = QString(SCREENPLAY_SOURCE_DIR) + "/Content";
        qWarning() << "Development version without set contentPath defaults to:" << exampleContentPath;
        return m_globalVariables->setLocalStoragePath(QUrl::fromUserInput(exampleContentPath));
    }

    if (m_globalVariables->isSteamVersion()) {
        initSteamInstalledPath();
    } else {
        const QString path = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
        m_qSettings.setValue("ScreenPlayContentPath", QUrl::fromUserInput(path));
        m_qSettings.sync();
        m_globalVariables->setLocalStoragePath(path);
    }
}

/*!
  \brief Path initialization for Steam workshop content without using Steam API to maintain FOSS status.

    Starting from the ScreenPlay executable location, we navigate up the folder structure
    to locate the Steam library folder. There are two possible configurations:

    1. Main Steam installation:
    From -> C:\Program Files (x86)\Steam\steamapps\common\ScreenPlay
    To   -> C:\Program Files (x86)\Steam\steamapps\
    VDF  -> C:\Program Files (x86)\Steam\steamapps\libraryfolders.vdf

    2. Secondary Steam library:
    From -> F:\SteamLibrary\steamapps\common\ScreenPlay
    To   -> F:\SteamLibrary\steamapps\
    VDF  -> F:\SteamLibrary\libraryfolder.vdf

    Workshop path (Dest.) -> [Steam Library Path]\steamapps\workshop\content\672870

    The workshop folder may or may not exist depending on whether the user has
    previously installed workshop content. If it doesn't exist, we create it
    to prevent QFileSystemWatcher warnings in InstalledListModel.
*/
void Settings::initSteamInstalledPath()
{
    // QString appBasePath = "C:/Program Files (x86)/Steam/steamapps/common/ScreenPlay";
    QString appBasePath = QGuiApplication::instance()->applicationDirPath();
    QString workshopPath;

    // Handle OSX specific path adjustment
    if (desktopEnvironment() == DesktopEnvironment::OSX) {
        appBasePath += "/../../..";
    }

    // From ScreenPlay.exe path, go up exactly 2 directories to reach steamapps
    QDir currentDir(appBasePath);
    currentDir.cdUp(); // up from ScreenPlay
    currentDir.cdUp(); // up from common

    // Now we should be in steamapps directory
    // First check if this is a library folder (libraryfolder.vdf in parent)
    QFileInfo libraryFolder(currentDir.absolutePath() + "/../libraryfolder.vdf");
    QFileInfo libraryFolders(currentDir.absolutePath() + "/libraryfolders.vdf");

    if (!libraryFolder.exists() && !libraryFolders.exists()) {
        qWarning() << "Could not find Steam library VDF files at:" << currentDir.absolutePath();
        return;
    }

    // Construct workshop path relative to steamapps
    workshopPath = currentDir.absolutePath() + "/workshop/content/672870";

    // Create workshop directory structure if it doesn't exist
    QDir workshopDir(workshopPath);
    if (!workshopDir.exists()) {
        if (!workshopDir.mkpath(".")) {
            qWarning() << "Could not create Steam workshop path:" << workshopPath;
            return;
        }
    }

    // Update settings with the new path
    workshopPath = QDir::cleanPath(workshopPath);
    m_globalVariables->setLocalStoragePath(QUrl::fromUserInput(workshopPath));
    m_qSettings.setValue("ScreenPlayContentPath", workshopPath);
    m_qSettings.sync();

    qInfo() << "Steam workshop path initialized:" << workshopPath;
}

/*!
    \brief Checks if there is already a saved language. If not we try to use the system langauge.
     If we do not support the system language we use english.
*/
void Settings::setupLanguage()
{
    QString langCode;

    // Note: isNull is true of no "Language" entry _at all_ is set
    //       isEmpty is true if we have an "Language" entry that is empty
    if (m_qSettings.value("Language").isNull() || m_qSettings.value("Language").toString().isEmpty()) {
        langCode = QLocale::system().name();
        // QML enum begin with uppercase: de_DE -> De_DE
        langCode = langCode.replace(0, 1, langCode.at(0).toUpper());
    } else {
        langCode = m_qSettings.value("Language").toString();
    }
    QStringList parts = langCode.split('_');

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
    const QString qmPath = QGuiApplication::applicationDirPath() + "/assets/translations/ScreenPlay_" + langCode + ".qm";
    if (tsFile.exists(qmPath)) {
        if (!m_translator.load(qmPath)) {
            qWarning() << "Unable to load translation file: " << qmPath;
            return false;
        }
        auto* guiAppInst = dynamic_cast<QGuiApplication*>(QGuiApplication::instance());
        guiAppInst->installTranslator(&m_translator);
        emit requestRetranslation();

        const Language lang = language();
        if (lang == Settings::Language::Ko_KR) {
            setFont("Noto Sans CJK KR Regular");
        } else {
            setFont("Roboto");
        }
        return true;
    }
    qWarning() << tsFile.fileName() << ", cannot be loaded width langCode " << langCode;
    return false;
}

void Settings::setShowDefaultContent(bool showDefaultContent)
{
    if (m_showDefaultContent == showDefaultContent)
        return;
    m_showDefaultContent = showDefaultContent;
    emit showDefaultContentChanged(showDefaultContent);
}

void Settings::setqSetting(const QString& key, const QVariant& value)
{
    m_qSettings.setValue(key, value);
    m_qSettings.sync();
}

void Settings::setAutostart(bool autostart)
{
    if (desktopEnvironment() == DesktopEnvironment::Windows) {

        QSettings settings("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
        if (autostart) {
            settings.setValue("ScreenPlay", QDir::toNativeSeparators(QCoreApplication::applicationFilePath()) + " -silent");
            settings.sync();
        } else {
            settings.remove("ScreenPlay");
        }
    }
    if (desktopEnvironment() == DesktopEnvironment::OSX) {
        const QString plistFileName = "app.screenplay.plist";
        QFile defaultPListFile(":/qt/qml/ScreenPlay/assets/macos/" + plistFileName);
        defaultPListFile.open(QIODevice::ReadOnly);
        QString settingsPlistContent = defaultPListFile.readAll();
        if (!settingsPlistContent.contains("{{SCREENPLAY_PATH}}")) {
            qCritical() << "Unable to load plist settings template from qrc to set autostart!";
            return;
        }

        QDir workingDir(QGuiApplication::applicationDirPath());
        workingDir.cdUp();
        workingDir.cdUp();
        workingDir.cdUp();
        const QString screenPlayPath = QUrl::fromUserInput(workingDir.path() + "/ScreenPlay.app/Contents/MacOS/ScreenPlay").toLocalFile();
        settingsPlistContent.replace("{{SCREENPLAY_PATH}}", screenPlayPath);
        settingsPlistContent.replace("{{SCREENPLAY_AUTOSTART}}", autostart ? "true" : "false");

        const QString homePath = QDir::homePath();
        QFile settingsPlist(homePath + "/Library/LaunchAgents/" + plistFileName);
        if (settingsPlist.exists()) {
            QDomDocument doc;
            if (!doc.setContent(&settingsPlist)) {
                settingsPlist.close();
                return;
            }
            settingsPlist.close();

            QDomElement root = doc.firstChildElement();
            QDomNodeList dictList = root.elementsByTagName("dict");
            if (dictList.size() > 1 && dictList.size() < 1) {
                return;
            }

            // Check if autostart and corresponding path is set and abort if so. This is needed since osx 13.0 Ventura
            // because it displays an annoying message every time we change the file.
            bool isCorrectPath = false;
            bool isAutostartEnabled = false;
            QDomNode dictNode = dictList.at(0);
            if (dictNode.isElement()) {
                QDomElement dictElement = dictNode.toElement();
                QDomNodeList keyList = dictElement.elementsByTagName("key");
                for (int j = 0; j < keyList.size(); j++) {
                    QDomNode keyNode = keyList.at(j);
                    if (keyNode.isElement()) {
                        QDomElement keyElement = keyNode.toElement();
                        if (keyElement.text() == "ProgramArguments") {
                            QDomNode valueNode = keyNode.nextSibling();
                            if (valueNode.isElement()) {
                                QDomElement valueElement = valueNode.toElement();
                                QDomNodeList stringList = valueElement.elementsByTagName("string");
                                if (!stringList.isEmpty()) {
                                    QDomNode stringNode = stringList.at(0);
                                    if (stringNode.isElement()) {
                                        QDomElement stringElement = stringNode.toElement();
                                        const QString path = stringElement.text();
                                        if (path == screenPlayPath) {
                                            isCorrectPath = true;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }

            if (dictNode.isElement()) {
                QDomElement dictElement = dictNode.toElement();
                QDomNodeList keyList = dictElement.elementsByTagName("key");
                for (int j = 0; j < keyList.size(); j++) {
                    QDomNode keyNode = keyList.at(j);
                    if (keyNode.isElement()) {
                        QDomElement keyElement = keyNode.toElement();
                        if (keyElement.text() == "RunAtLoad") {
                            QDomNode valueNode = keyNode.nextSibling();
                            if (valueNode.isElement()) {
                                QDomElement valueElement = valueNode.toElement();
                                if (valueElement.tagName() == "true") {
                                    isAutostartEnabled = true;
                                }
                            }
                        }
                    }
                }
            }

            // Nothing to do. Autostart has the same value and the path is also correct.
            if (isAutostartEnabled == autostart && isCorrectPath)
                return;

            if (!settingsPlist.remove()) {
                qCritical() << "Unable to remove: " << settingsPlist.fileName();
            }
        }

        settingsPlist.open(QIODevice::WriteOnly | QIODevice::Truncate);
        QTextStream out(&settingsPlist);
        out.setEncoding(QStringConverter::Utf8);
        out << settingsPlistContent;
        settingsPlist.flush();
        settingsPlist.close();

        qInfo() << "Set autostart enabled: " << autostart;
    }
    setqSetting("Autostart", autostart);

    m_autostart = autostart;
    emit autostartChanged(m_autostart);
}

void Settings::setHighPriorityStart(bool highPriorityStart)
{
    if (m_highPriorityStart == highPriorityStart)
        return;

    setqSetting("HighPriorityStart", highPriorityStart);

    const QString app = "'" + QGuiApplication::applicationDirPath() + "/WindowsServiceHelper.exe" + "'";
    QStringList args { "-Command", QString("Start-Process %1 -Verb runAs").arg(app), "-ArgumentList" };

    // Because we must use powershell, we need to add an extra 'var' and ,
    auto appendAsString = [&](const QString& string, const bool isLast = false) {
        QString arg = "'" + string + "'";
        if (!isLast)
            arg += ",";
        args.append(arg);
    };

    appendAsString("--t");
    appendAsString("create");
    appendAsString("--sn");
    appendAsString("ScreenPlayService");
    appendAsString("--dn");
    appendAsString("ScreenPlayService");
    appendAsString("--a");
    appendAsString(QVariant(highPriorityStart).toString(), true);

    QProcess process;
    process.start(QStringLiteral("powershell"), args);
    process.waitForFinished(5000);

    m_highPriorityStart = highPriorityStart;
    emit highPriorityStartChanged(m_highPriorityStart);
}

void Settings::setLocalStoragePath(QUrl localStoragePath)
{
    // Remove: "file:///"
    QJsonValue cleanedPath = QJsonValue(localStoragePath.toString());

    setqSetting("ScreenPlayContentPath", cleanedPath);

    m_globalVariables->setLocalStoragePath(cleanedPath.toString());
    emit resetInstalledListmodel();
}

void Settings::setDecoder(QString decoder)
{
    if (m_decoder == decoder)
        return;

    m_decoder = decoder;

    emit decoderChanged(m_decoder);
}

void Settings::setSilentStart(bool silentStart)
{
    if (m_silentStart == silentStart)
        return;

    m_silentStart = silentStart;
    emit silentStartChanged(m_silentStart);
}

void Settings::setAnonymousTelemetry(bool anonymousTelemetry)
{
    if (m_anonymousTelemetry == anonymousTelemetry)
        return;

    setqSetting("AnonymousTelemetry", anonymousTelemetry);

    m_anonymousTelemetry = anonymousTelemetry;
    emit anonymousTelemetryChanged(m_anonymousTelemetry);
}

void Settings::setCheckWallpaperVisible(bool checkWallpaperVisible)
{
    if (m_checkWallpaperVisible == checkWallpaperVisible)
        return;

    setqSetting("CheckWallpaperVisible", checkWallpaperVisible);

    m_checkWallpaperVisible = checkWallpaperVisible;
    emit checkWallpaperVisibleChanged(m_checkWallpaperVisible);
}

void Settings::setVideoFillMode(ScreenPlay::Video::FillMode videoFillMode)
{
    if (m_videoFillMode == videoFillMode)
        return;

    setqSetting("VideoFillMode", QVariant::fromValue(videoFillMode).toString());

    m_videoFillMode = videoFillMode;
    emit videoFillModeChanged(m_videoFillMode);
}

void Settings::setLanguage(ScreenPlay::Settings::Language language)
{
    if (m_language == language)
        return;

    setqSetting("Language", QVariant::fromValue(language).toString());

    m_language = language;
    emit languageChanged(m_language);
}

void Settings::setFont(QString font)
{
    if (m_font == font)
        return;

    m_font = font;
    emit fontChanged(m_font);
}

void Settings::setTheme(ScreenPlay::Settings::Theme theme)
{
    if (m_theme == theme)
        return;

    setqSetting("Theme", QVariant::fromValue(theme).toString());

    m_theme = theme;
    emit themeChanged(m_theme);
}

void Settings::setDesktopEnvironment(DesktopEnvironment desktopEnvironment)
{
    if (m_desktopEnvironment == desktopEnvironment)
        return;

    m_desktopEnvironment = desktopEnvironment;
    emit desktopEnvironmentChanged(m_desktopEnvironment);
}

void Settings::setBuildInfos(const QString& buildInfos)
{
    if (m_buildInfos == buildInfos)
        return;
    m_buildInfos = buildInfos;
    emit buildInfosChanged(m_buildInfos);
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

void Settings::setStartWallpaperMuted(bool startWallpaperMuted)
{
    if (m_startWallpaperMuted == startWallpaperMuted)
        return;
    m_startWallpaperMuted = startWallpaperMuted;
    setqSetting("startWallpaperMuted", m_startWallpaperMuted);
    emit startWallpaperMutedChanged(m_startWallpaperMuted);
}

void Settings::setAlwaysMinimize(bool alwaysMinimize)
{
    if (m_alwaysMinimize == alwaysMinimize)
        return;
    m_alwaysMinimize = alwaysMinimize;
    setqSetting("alwaysMinimize", m_alwaysMinimize);
    emit alwaysMinimizeChanged(m_alwaysMinimize);
}

void Settings::setGodotFps(ScreenPlay::Settings::GodotFps godotFps)
{
    if (m_godotFps == godotFps)
        return;

    setqSetting("GodotFps", QVariant::fromValue(godotFps).toString());

    m_godotFps = godotFps;
    emit godotFpsChanged(m_godotFps);
}

void Settings::setGraphicsApi(ScreenPlay::ScreenPlayEnums::GraphicsApi graphicsApi)
{
    if (m_graphicsApi == graphicsApi)
        return;

    setqSetting("GraphicsApi", QVariant::fromValue(graphicsApi).toString());

    m_graphicsApi = graphicsApi;
    emit graphicsApiChanged(m_graphicsApi);
}
}

#include "moc_settings.cpp"
