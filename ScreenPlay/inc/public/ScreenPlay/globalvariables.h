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

    Q_PROPERTY(QVersionNumber version READ version CONSTANT)
    Q_PROPERTY(QUrl localStoragePath READ localStoragePath WRITE setLocalStoragePath NOTIFY localStoragePathChanged)
    Q_PROPERTY(QUrl localSettingsPath READ localSettingsPath WRITE setLocalSettingsPath NOTIFY localSettingsPathChanged)
    Q_PROPERTY(QUrl wallpaperExecutablePath READ wallpaperExecutablePath WRITE setWallpaperExecutablePath NOTIFY wallpaperExecutablePathChanged)
    Q_PROPERTY(QUrl widgetExecutablePath READ widgetExecutablePath WRITE setWidgetExecutablePath NOTIFY widgetExecutablePathChanged)

public:
    explicit GlobalVariables(QObject* parent = nullptr);

    /*!
      \brief We need to check if there was an error in the Wallpaper/Widgets.
             For this we ping it every 3s.
     */
    static const int contentPingAliveIntervalMS = 3000;

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
    QVersionNumber version() const { return m_version; }

signals:
    void localStoragePathChanged(QUrl localStoragePath);
    void localSettingsPathChanged(QUrl localSettingsPath);
    void wallpaperExecutablePathChanged(QUrl wallpaperExecutablePath);
    void widgetExecutablePathChanged(QUrl widgetExecutablePath);

public slots:

    void setLocalStoragePath(QUrl localStoragePath)
    {
        if (m_localStoragePath == localStoragePath)
            return;

        m_localStoragePath = localStoragePath;
        emit localStoragePathChanged(m_localStoragePath);
    }

    void setLocalSettingsPath(QUrl localSettingsPath)
    {
        if (m_localSettingsPath == localSettingsPath)
            return;

        m_localSettingsPath = localSettingsPath;
        emit localSettingsPathChanged(m_localSettingsPath);
    }

    void setWallpaperExecutablePath(QUrl wallpaperExecutablePath)
    {
        if (m_wallpaperExecutablePath == wallpaperExecutablePath)
            return;

        m_wallpaperExecutablePath = wallpaperExecutablePath;
        emit wallpaperExecutablePathChanged(m_wallpaperExecutablePath);
    }

    void setWidgetExecutablePath(QUrl widgetExecutablePath)
    {
        if (m_widgetExecutablePath == widgetExecutablePath)
            return;

        m_widgetExecutablePath = widgetExecutablePath;
        emit widgetExecutablePathChanged(m_widgetExecutablePath);
    }

private:
    QUrl m_localStoragePath;
    QUrl m_localSettingsPath;
    QUrl m_wallpaperExecutablePath;
    QUrl m_widgetExecutablePath;
    QVersionNumber m_version { 1, 0, 0 };
};
}
