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
#include "ScreenPlayUtil/util.h"

#include <memory>
#include <optional>

namespace ScreenPlay {
class ActiveProfile;

class Settings : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")
    Q_CLASSINFO("RegisterEnumClassesUnscoped", "false")

    Q_PROPERTY(bool showDefaultContent READ showDefaultContent WRITE setShowDefaultContent NOTIFY showDefaultContentChanged)
    Q_PROPERTY(bool anonymousTelemetry READ anonymousTelemetry WRITE setAnonymousTelemetry NOTIFY anonymousTelemetryChanged)
    Q_PROPERTY(bool silentStart READ silentStart WRITE setSilentStart NOTIFY silentStartChanged)
    Q_PROPERTY(bool autostart READ autostart WRITE setAutostart NOTIFY autostartChanged)
    Q_PROPERTY(bool highPriorityStart READ highPriorityStart WRITE setHighPriorityStart NOTIFY highPriorityStartChanged)
    Q_PROPERTY(bool checkWallpaperVisible READ checkWallpaperVisible WRITE setCheckWallpaperVisible NOTIFY checkWallpaperVisibleChanged)
    Q_PROPERTY(bool offlineMode READ offlineMode WRITE setOfflineMode NOTIFY offlineModeChanged)
    Q_PROPERTY(bool steamVersion READ steamVersion WRITE setSteamVersion NOTIFY steamVersionChanged)

    Q_PROPERTY(ScreenPlay::Video::FillMode videoFillMode READ videoFillMode WRITE setVideoFillMode NOTIFY videoFillModeChanged)
    Q_PROPERTY(ScreenPlay::Settings::DesktopEnvironment desktopEnvironment READ desktopEnvironment WRITE setDesktopEnvironment NOTIFY desktopEnvironmentChanged)
    Q_PROPERTY(ScreenPlay::Settings::Language language READ language WRITE setLanguage NOTIFY languageChanged)
    Q_PROPERTY(ScreenPlay::Settings::Theme theme READ theme WRITE setTheme NOTIFY themeChanged)

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
        Wayland,
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
    Q_ENUM(DesktopEnvironment)

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
    ScreenPlay::Video::FillMode videoFillMode() const { return m_videoFillMode; }
    ScreenPlay::Settings::Language language() const { return m_language; }
    QString font() const { return m_font; }
    ScreenPlay::Settings::Theme theme() const { return m_theme; }
    bool steamVersion() const { return m_steamVersion; }
    ScreenPlay::Settings::DesktopEnvironment desktopEnvironment() const { return m_desktopEnvironment; }
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
    void videoFillModeChanged(ScreenPlay::Video::FillMode videoFillMode);
    void languageChanged(ScreenPlay::Settings::Language language);
    void fontChanged(QString font);
    void themeChanged(ScreenPlay::Settings::Theme theme);
    void steamVersionChanged(bool steamVersion);
    void desktopEnvironmentChanged(ScreenPlay::Settings::DesktopEnvironment desktopEnvironment);
    void buildInfosChanged(const QString& buildInfos);
    void showDefaultContentChanged(bool showDefaultContent);

public slots:
    void setupLanguage();
    void writeJsonFileFromResource(const QString& filename);
    void setupWidgetAndWindowPaths();
    bool retranslateUI();

    void setShowDefaultContent(bool showDefaultContent);
    void setqSetting(const QString& key, const QVariant& value);
    void setAutostart(bool autostart);
    void setHighPriorityStart(bool highPriorityStart);
    void setLocalStoragePath(QUrl localStoragePath);
    void setDecoder(QString decoder);
    void setOfflineMode(bool offlineMode);
    void setSilentStart(bool silentStart);
    void setAnonymousTelemetry(bool anonymousTelemetry);
    void setCheckWallpaperVisible(bool checkWallpaperVisible);
    void setVideoFillMode(ScreenPlay::Video::FillMode videoFillMode);
    void setLanguage(ScreenPlay::Settings::Language language);
    void setFont(QString font);
    void setTheme(ScreenPlay::Settings::Theme theme);
    void setSteamVersion(bool steamVersion);
    void setDesktopEnvironment(DesktopEnvironment desktopEnvironment);
    void setBuildInfos(const QString& buildInfos);

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

    ScreenPlay::Video::FillMode m_videoFillMode { ScreenPlay::Video::FillMode::Cover };
    ScreenPlay::Settings::Language m_language { Language::En_US };
    ScreenPlay::Settings::Theme m_theme { Theme::System };
    ScreenPlay::Settings::DesktopEnvironment m_desktopEnvironment { DesktopEnvironment::Unknown };
    QString m_font { "Roboto" };
    QString m_decoder;
    bool m_steamVersion { false };
    QString m_buildInfos;
};
}
