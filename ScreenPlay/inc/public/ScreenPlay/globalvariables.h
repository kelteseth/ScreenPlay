// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#pragma once

#include <QObject>
#include <QQmlEngine>
#include <QUrl>

namespace ScreenPlay {

class GlobalVariables : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")
    Q_CLASSINFO("RegisterEnumClassesUnscoped", "false")

    Q_PROPERTY(ScreenPlay::GlobalVariables::Version version READ version WRITE setVersion NOTIFY versionChanged FINAL)
    Q_PROPERTY(QUrl localStoragePath READ localStoragePath WRITE setLocalStoragePath NOTIFY localStoragePathChanged FINAL)
    Q_PROPERTY(QUrl localSettingsPath READ localSettingsPath WRITE setLocalSettingsPath NOTIFY localSettingsPathChanged FINAL)
    Q_PROPERTY(QUrl wallpaperExecutablePath READ wallpaperExecutablePath WRITE setWallpaperExecutablePath NOTIFY wallpaperExecutablePathChanged FINAL)
    Q_PROPERTY(QUrl widgetExecutablePath READ widgetExecutablePath WRITE setWidgetExecutablePath NOTIFY widgetExecutablePathChanged FINAL)
    Q_PROPERTY(QUrl godotWallpaperExecutablePath READ godotWallpaperExecutablePath WRITE setGodotWallpaperExecutablePath NOTIFY godotWallpaperExecutablePathChanged FINAL)
    Q_PROPERTY(QUrl godotEditorExecutablePath READ godotEditorExecutablePath WRITE setGodotEditorExecutablePath NOTIFY godotEditorExecutablePathChanged FINAL)

public:
    explicit GlobalVariables(QObject* parent = nullptr);

    enum class Version {
        OpenSourceStandalone,
        OpenSourceSteam,
        OpenSourceProStandalone,
        OpenSourceProSteam,
        OpenSourceUltraStandalone,
        OpenSourceUltraSteam
    };
    Q_ENUM(Version)
    Q_INVOKABLE bool isBasicVersion() const;
    Q_INVOKABLE bool isStandaloneVersion() const;
    Q_INVOKABLE bool isSteamVersion() const;
    Q_INVOKABLE bool isProVersion() const;
    Q_INVOKABLE bool isUltraVersion() const;

    ScreenPlay::GlobalVariables::Version version() const { return m_version; }
    QUrl localStoragePath() const { return m_localStoragePath; }
    QUrl localSettingsPath() const { return m_localSettingsPath; }
    QUrl wallpaperExecutablePath() const { return m_wallpaperExecutablePath; }
    QUrl widgetExecutablePath() const { return m_widgetExecutablePath; }
    QUrl godotWallpaperExecutablePath() const { return m_godotWallpaperExecutablePath; }
    QUrl godotEditorExecutablePath() const { return m_godotEditorExecutablePath; }

signals:
    void versionChanged(Version version);
    void localStoragePathChanged(QUrl localStoragePath);
    void localSettingsPathChanged(QUrl localSettingsPath);
    void wallpaperExecutablePathChanged(QUrl wallpaperExecutablePath);
    void widgetExecutablePathChanged(QUrl widgetExecutablePath);
    void godotWallpaperExecutablePathChanged(QUrl godotWallpaperExecutablePath);
    void godotEditorExecutablePathChanged(QUrl godotEditorExecutablePath);

public slots:
    void setVersion(Version version);
    void setLocalStoragePath(QUrl localStoragePath);
    void setLocalSettingsPath(QUrl localSettingsPath);
    void setWallpaperExecutablePath(QUrl wallpaperExecutablePath);
    void setWidgetExecutablePath(QUrl widgetExecutablePath);
    void setGodotWallpaperExecutablePath(QUrl godotWallpaperExecutablePath);
    void setGodotEditorExecutablePath(QUrl godotEditorExecutablePath);

public:
    /*!
      \brief We need to check if there was an error in the Wallpaper/Widgets.
             For this we ping it every 3s.
     */
    static const int contentPingAliveIntervalMS = 3000;

private:
    QUrl m_localStoragePath;
    QUrl m_localSettingsPath;
    QUrl m_wallpaperExecutablePath;
    QUrl m_widgetExecutablePath;
    QUrl m_godotWallpaperExecutablePath;
    QUrl m_godotEditorExecutablePath;
    Version m_version = Version::OpenSourceStandalone;
};
}
