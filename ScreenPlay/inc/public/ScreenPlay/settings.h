// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#pragma once

#include <QDebug>
#include <QDir>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNodeList>
#include <QFile>
#include <QFontDatabase>
#include <QGuiApplication>
#include <QIODevice>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>
#include <QPair>
#include <QProcess>
#include <QProcessEnvironment>
#include <QQmlPropertyMap>
#include <QSettings>
#include <QStandardPaths>
#include <QString>
#include <QSysInfo>
#include <QTextStream>
#include <QThread>
#include <QUrl>
#include <QVariant>
#include <QVector>
#include <QVersionNumber>
#include <QtConcurrent/QtConcurrent>
#include <QtGlobal>

#include "ScreenPlay/globalvariables.h"
#include "ScreenPlay/util.h"

#include <memory>
#include <optional>

namespace ScreenPlay {
class ActiveProfile;

class Settings : public QObject {
    Q_OBJECT
    QML_UNCREATABLE("")

    Q_PROPERTY(bool showDefaultContent READ showDefaultContent WRITE setShowDefaultContent NOTIFY showDefaultContentChanged)
    Q_PROPERTY(bool anonymousTelemetry READ anonymousTelemetry WRITE setAnonymousTelemetry NOTIFY anonymousTelemetryChanged)
    Q_PROPERTY(bool silentStart READ silentStart WRITE setSilentStart NOTIFY silentStartChanged)
    Q_PROPERTY(bool autostart READ autostart WRITE setAutostart NOTIFY autostartChanged)
    Q_PROPERTY(bool highPriorityStart READ highPriorityStart WRITE setHighPriorityStart NOTIFY highPriorityStartChanged)
    Q_PROPERTY(bool checkWallpaperVisible READ checkWallpaperVisible WRITE setCheckWallpaperVisible NOTIFY checkWallpaperVisibleChanged)
    Q_PROPERTY(bool offlineMode READ offlineMode WRITE setOfflineMode NOTIFY offlineModeChanged)
    Q_PROPERTY(bool steamVersion READ steamVersion WRITE setSteamVersion NOTIFY steamVersionChanged)

    Q_PROPERTY(ScreenPlay::FillMode::FillMode videoFillMode READ videoFillMode WRITE setVideoFillMode NOTIFY videoFillModeChanged)
    Q_PROPERTY(DesktopEnvironment desktopEnvironment READ desktopEnvironment WRITE setDesktopEnvironment NOTIFY desktopEnvironmentChanged)
    Q_PROPERTY(Language language READ language WRITE setLanguage NOTIFY languageChanged)
    Q_PROPERTY(Theme theme READ theme WRITE setTheme NOTIFY themeChanged)

    Q_PROPERTY(QString decoder READ decoder WRITE setDecoder NOTIFY decoderChanged)
    Q_PROPERTY(QString buildInfos READ buildInfos WRITE setBuildInfos NOTIFY buildInfosChanged)
    Q_PROPERTY(QString font READ font WRITE setFont NOTIFY fontChanged)

public:
    explicit Settings(
        const std::shared_ptr<GlobalVariables>& globalVariables,
        QObject* parent = nullptr);

    enum class DesktopEnvironment {
        Unknown,
        OSX,
        Windows,
        Cinnamon,
        Enlightenment,
        Gnome,
        KDE,
        Lxde,
        Lxqt,
        Mate,
        Unity,
        XFCE,
    };

    enum class Language {
        En_US,
        De_DE,
        Pl_PL,
        It_IT,
        Ru_RU,
        Fr_FR,
        Es_ES,
        Ko_KR,
        Vi_VN,
        Pt_BR,
        Zh_CN,
        Tr_TR,
        Nl_NL,
    };
    Q_ENUM(Language)

    enum class Theme {
        System,
        Dark,
        Light
    };
    Q_ENUM(Theme)

    bool offlineMode() const { return m_offlineMode; }
    bool getOfflineMode() const { return m_offlineMode; }
    bool autostart() const { return m_autostart; }
    bool highPriorityStart() const { return m_highPriorityStart; }
    QString decoder() const { return m_decoder; }
    bool silentStart() const { return m_silentStart; }
    bool anonymousTelemetry() const { return m_anonymousTelemetry; }
    bool checkWallpaperVisible() const { return m_checkWallpaperVisible; }
    ScreenPlay::FillMode::FillMode videoFillMode() const { return m_videoFillMode; }
    Language language() const { return m_language; }
    QString font() const { return m_font; }
    Theme theme() const { return m_theme; }
    bool steamVersion() const { return m_steamVersion; }
    DesktopEnvironment desktopEnvironment() const { return m_desktopEnvironment; }
    const QString& buildInfos() const { return m_buildInfos; }
    bool showDefaultContent() const { return m_showDefaultContent; }

signals:
    void requestRetranslation();
    void resetInstalledListmodel();
    void autostartChanged(bool autostart);
    void highPriorityStartChanged(bool highPriorityStart);
    void hasWorkshopBannerSeenChanged(bool hasWorkshopBannerSeen);
    void decoderChanged(QString decoder);
    void setMainWindowVisible(bool visible);
    void offlineModeChanged(bool offlineMode);
    void silentStartChanged(bool silentStart);
    void anonymousTelemetryChanged(bool anonymousTelemetry);
    void checkWallpaperVisibleChanged(bool checkWallpaperVisible);
    void videoFillModeChanged(ScreenPlay::FillMode::FillMode videoFillMode);
    void languageChanged(ScreenPlay::Settings::Language language);
    void fontChanged(QString font);
    void themeChanged(ScreenPlay::Settings::Theme theme);
    void steamVersionChanged(bool steamVersion);
    void desktopEnvironmentChanged(DesktopEnvironment desktopEnvironment);
    void buildInfosChanged(const QString& buildInfos);
    void showDefaultContentChanged(bool showDefaultContent);

public slots:
    void setupLanguage();
    void writeJsonFileFromResource(const QString& filename);
    void setupWidgetAndWindowPaths();
    bool retranslateUI();

    void setShowDefaultContent(bool showDefaultContent)
    {
        if (m_showDefaultContent == showDefaultContent)
            return;
        m_showDefaultContent = showDefaultContent;
        emit showDefaultContentChanged(showDefaultContent);
    }

    void setqSetting(const QString& key, const QVariant& value)
    {
        m_qSettings.setValue(key, value);
        m_qSettings.sync();
    }

    void setAutostart(bool autostart)
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
            QFile defaultPListFile(":/qml/ScreenPlayApp/assets/macos/" + plistFileName);
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

    void setHighPriorityStart(bool highPriorityStart)
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

    void setLocalStoragePath(QUrl localStoragePath)
    {
        // Remove: "file:///"
        QJsonValue cleanedPath = QJsonValue(localStoragePath.toString());

        setqSetting("ScreenPlayContentPath", cleanedPath);

        m_globalVariables->setLocalStoragePath(cleanedPath.toString());
        emit resetInstalledListmodel();
    }

    void setDecoder(QString decoder)
    {
        if (m_decoder == decoder)
            return;

        m_decoder = decoder;

        emit decoderChanged(m_decoder);
    }

    void setOfflineMode(bool offlineMode)
    {
        if (m_offlineMode == offlineMode)
            return;

        m_offlineMode = offlineMode;
        emit offlineModeChanged(m_offlineMode);
    }

    void setSilentStart(bool silentStart)
    {
        if (m_silentStart == silentStart)
            return;

        m_silentStart = silentStart;
        emit silentStartChanged(m_silentStart);
    }

    void setAnonymousTelemetry(bool anonymousTelemetry)
    {
        if (m_anonymousTelemetry == anonymousTelemetry)
            return;

        setqSetting("AnonymousTelemetry", anonymousTelemetry);

        m_anonymousTelemetry = anonymousTelemetry;
        emit anonymousTelemetryChanged(m_anonymousTelemetry);
    }

    void setCheckWallpaperVisible(bool checkWallpaperVisible)
    {
        if (m_checkWallpaperVisible == checkWallpaperVisible)
            return;

        setqSetting("CheckWallpaperVisible", checkWallpaperVisible);

        m_checkWallpaperVisible = checkWallpaperVisible;
        emit checkWallpaperVisibleChanged(m_checkWallpaperVisible);
    }

    void setVideoFillMode(ScreenPlay::FillMode::FillMode videoFillMode)
    {
        if (m_videoFillMode == videoFillMode)
            return;

        setqSetting("VideoFillMode", QVariant::fromValue(videoFillMode).toString());

        m_videoFillMode = videoFillMode;
        emit videoFillModeChanged(m_videoFillMode);
    }

    void setLanguage(ScreenPlay::Settings::Language language)
    {
        if (m_language == language)
            return;

        setqSetting("Language", QVariant::fromValue(language).toString());

        m_language = language;
        emit languageChanged(m_language);
    }

    void setFont(QString font)
    {
        if (m_font == font)
            return;

        m_font = font;
        emit fontChanged(m_font);
    }

    void setTheme(ScreenPlay::Settings::Theme theme)
    {
        if (m_theme == theme)
            return;

        setqSetting("Theme", QVariant::fromValue(theme).toString());

        m_theme = theme;
        emit themeChanged(m_theme);
    }

    void setSteamVersion(bool steamVersion)
    {
        if (m_steamVersion == steamVersion)
            return;

        m_steamVersion = steamVersion;
        emit steamVersionChanged(m_steamVersion);
    }

    void setDesktopEnvironment(DesktopEnvironment desktopEnvironment)
    {
        if (m_desktopEnvironment == desktopEnvironment)
            return;

        m_desktopEnvironment = desktopEnvironment;
        emit desktopEnvironmentChanged(m_desktopEnvironment);
    }

    void setBuildInfos(const QString& buildInfos)
    {
        if (m_buildInfos == buildInfos)
            return;
        m_buildInfos = buildInfos;
        emit buildInfosChanged(m_buildInfos);
    }

private:
    void restoreDefault(const QString& appConfigLocation, const QString& settingsFileType);
    void initInstalledPath();
    void initSteamInstalledPath();
    QString fixLanguageCode(const QString& languageCode);

private:
    QSettings m_qSettings;
    QTranslator m_translator;

    const std::shared_ptr<GlobalVariables>& m_globalVariables;

    bool m_autostart { true };
    bool m_highPriorityStart { false };
    bool m_offlineMode { true };
    bool m_checkWallpaperVisible { false };
    bool m_silentStart { false };
    bool m_anonymousTelemetry { true };
    bool m_showDefaultContent { true };

    QString m_decoder;
    ScreenPlay::FillMode::FillMode m_videoFillMode { ScreenPlay::FillMode::FillMode::Cover };
    Language m_language { Language::En_US };
    Theme m_theme { Theme::System };
    QString m_font { "Roboto" };
    bool m_steamVersion { false };
    DesktopEnvironment m_desktopEnvironment = DesktopEnvironment::Unknown;
    QString m_buildInfos;
};
}
