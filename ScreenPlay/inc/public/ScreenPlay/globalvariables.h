// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#pragma once

#include <QMetaType>
#include <QObject>
#include <QQmlEngine>
#include <QStandardPaths>
#include <QUrl>
#include <QVersionNumber>

#include <QQmlEngine>

namespace ScreenPlay {

class GlobalVariables : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")

    Q_PROPERTY(QVersionNumber version READ version CONSTANT)
    Q_PROPERTY(QUrl localStoragePath READ localStoragePath WRITE setLocalStoragePath NOTIFY localStoragePathChanged FINAL)
    Q_PROPERTY(QUrl localSettingsPath READ localSettingsPath WRITE setLocalSettingsPath NOTIFY localSettingsPathChanged FINAL)
    Q_PROPERTY(QUrl wallpaperExecutablePath READ wallpaperExecutablePath WRITE setWallpaperExecutablePath NOTIFY wallpaperExecutablePathChanged FINAL)
    Q_PROPERTY(QUrl widgetExecutablePath READ widgetExecutablePath WRITE setWidgetExecutablePath NOTIFY widgetExecutablePathChanged FINAL)
    Q_PROPERTY(QUrl godotWallpaperExecutablePath READ godotWallpaperExecutablePath WRITE setGodotWallpaperExecutablePath NOTIFY godotWallpaperExecutablePathChanged FINAL)
    Q_PROPERTY(QUrl godotEditorExecutablePath READ godotEditorExecutablePath WRITE setGodotEditorExecutablePath NOTIFY godotEditorExecutablePathChanged FINAL)

public:
    explicit GlobalVariables(QObject* parent = nullptr);

    /*!
        \property GlobalVariables::m_version
        \brief Returns the current app version. Not yet used.
    */
    QVersionNumber version() const { return m_version; }
    /*!
        \property GlobalVariables::localStoragePath
        \brief Returns the localStoragePath.
    */
    QUrl localStoragePath() const { return m_localStoragePath; }
    /*!
        \property GlobalVariables::localSettingsPath
        \brief Returns the localSettingsPath.
    */
    QUrl localSettingsPath() const { return m_localSettingsPath; }
    /*!
        \property GlobalVariables::wallpaperExecutablePath
        \brief  Returns the wallpaperExecutablePath. This only differes in development builds.
    */
    QUrl wallpaperExecutablePath() const { return m_wallpaperExecutablePath; }
    /*!
        \property GlobalVariables::widgetExecutablePath
        \brief Returns the widgetExecutablePath. This only differes in development builds.
    */
    QUrl widgetExecutablePath() const { return m_widgetExecutablePath; }
    /*!
        \property GlobalVariables::m_version
        \brief Returns the current app version. Not yet used.
    */
    QUrl godotWallpaperExecutablePath() const { return m_godotWallpaperExecutablePath; }
    /*!
        \property GlobalVariables::m_version
        \brief Returns the current app version. Not yet used.
    */
    QUrl godotEditorExecutablePath() const { return m_godotEditorExecutablePath; }

signals:
    void localStoragePathChanged(QUrl localStoragePath);
    void localSettingsPathChanged(QUrl localSettingsPath);
    void wallpaperExecutablePathChanged(QUrl wallpaperExecutablePath);
    void widgetExecutablePathChanged(QUrl widgetExecutablePath);
    void godotWallpaperExecutablePathChanged(QUrl godotWallpaperExecutablePath);
    void godotEditorExecutablePathChanged(QUrl godotEditorExecutablePath);

public slots:
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
    QVersionNumber m_version { 1, 0, 0 };
    QUrl m_godotWallpaperExecutablePath;
    QUrl m_godotEditorExecutablePath;
};
}
