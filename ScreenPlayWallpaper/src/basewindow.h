// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#pragma once

#include <QDebug>
#include <QFile>
#include <QFileSystemWatcher>
#include <QGuiApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QObject>
#include <QQmlEngine>
#include <QQuickView>
#include <QString>
#include <QSysInfo>
#include <QtQml>

#include "ScreenPlayCore/exitcodes.h"
#include "ScreenPlayCore/processmanager.h"
#include "ScreenPlayCore/projectfile.h"
#include "ScreenPlayCore/util.h"
#include "ScreenPlaySDK/screenplaysdk.h"
#include "wallpaperstate.h"

#include <memory>

namespace ScreenPlay {
class BaseWindow : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("cpp singleton")

public:
    BaseWindow();

    virtual WallpaperExit::Code setup() final;
    virtual WallpaperExit::Code start() = 0;
    void connectToMainApp();

    // Identity/configuration properties (stay in BaseWindow)
    Q_PROPERTY(qint64 mainAppPID READ mainAppPID WRITE setMainAppPID NOTIFY mainAppPIDChanged FINAL)
    Q_PROPERTY(int width READ width WRITE setWidth NOTIFY widthChanged)
    Q_PROPERTY(int height READ height WRITE setHeight NOTIFY heightChanged)
    Q_PROPERTY(QVector<int> activeScreensList READ activeScreensList WRITE setActiveScreensList NOTIFY activeScreensListChanged)
    Q_PROPERTY(QString appID READ appID WRITE setAppID NOTIFY appIDChanged)
    Q_PROPERTY(QString projectPath READ projectPath WRITE setProjectPath NOTIFY projectPathChanged)
    Q_PROPERTY(QString projectSourceFile READ projectSourceFile WRITE setProjectSourceFile NOTIFY projectSourceFileChanged)
    Q_PROPERTY(QUrl projectSourceFileAbsolute READ projectSourceFileAbsolute WRITE setProjectSourceFileAbsolute NOTIFY projectSourceFileAbsoluteChanged)
    Q_PROPERTY(bool canFade READ canFade WRITE setCanFade NOTIFY canFadeChanged)
    Q_PROPERTY(bool debugMode READ debugMode WRITE setDebugMode NOTIFY debugModeChanged)
    Q_PROPERTY(bool frameStatsVisible READ frameStatsVisible WRITE setFrameStatsVisible NOTIFY frameStatsVisibleChanged)
    Q_PROPERTY(ScreenPlay::ContentTypes::InstalledType type READ type WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(ScreenPlay::Video::VideoCodec videoCodec READ videoCodec WRITE setVideoCodec NOTIFY videoCodecChanged)
    Q_PROPERTY(ScreenPlaySDK* sdk READ sdk WRITE setSdk NOTIFY sdkChanged)
    Q_PROPERTY(QString OSVersion READ OSVersion WRITE setOSVersion NOTIFY OSVersionChanged)

    // Playback state objects (WallpaperState holds volume, fillMode, muted, loops, isPlaying, visualsPaused, checkWallpaperVisible)
    Q_PROPERTY(WallpaperState* currentState READ currentState CONSTANT)
    Q_PROPERTY(WallpaperState* targetState READ targetState CONSTANT)

    // Getters for identity/configuration
    QString appID() const { return m_appID; }
    QString OSVersion() const { return m_OSVersion; }
    bool canFade() const { return m_canFade; }
    int width() const { return m_width; }
    int height() const { return m_height; }
    QVector<int> activeScreensList() const { return m_activeScreensList; }
    bool debugMode() const { return m_debugMode; }
    bool frameStatsVisible() const { return m_frameStatsVisible; }
    ScreenPlaySDK* sdk() const { return m_sdk.get(); }
    const QString& projectPath() const { return m_projectPath; }
    const QString& projectSourceFile() const { return m_projectSourceFile; }
    const QUrl& projectSourceFileAbsolute() const { return m_projectSourceFileAbsolute; }
    ScreenPlay::ContentTypes::InstalledType type() const { return m_type; }

    // State getters
    WallpaperState* currentState() const { return m_currentState.get(); }
    WallpaperState* targetState() const { return m_targetState.get(); }

    ScreenPlay::Video::VideoCodec videoCodec() const;
    void setVideoCodec(ScreenPlay::Video::VideoCodec newVideoCodec);

    qint64 mainAppPID() const;
    void setMainAppPID(qint64 mainAppPID);
    virtual void setReapplySpacesEnabled(bool /*enabled*/) { }

signals:
    void qmlStart();
    void qmlExit();
    void fadeIn();
    void reloadQML(const ScreenPlay::ContentTypes::InstalledType oldType);
    void reloadVideo(const ScreenPlay::ContentTypes::InstalledType oldType);
    void reloadGIF(const ScreenPlay::ContentTypes::InstalledType oldType);

    void typeChanged(ScreenPlay::ContentTypes::InstalledType type);
    void appIDChanged(QString appID);
    void qmlSceneValueReceived(QString key, QString value);
    void OSVersionChanged(QString OSVersion);
    void canFadeChanged(bool canFade);
    void widthChanged(int width);
    void heightChanged(int height);
    void activeScreensListChanged(QVector<int> activeScreensList);
    void debugModeChanged(bool debugMode);
    void frameStatsVisibleChanged(bool frameStatsVisible);
    void sdkChanged(ScreenPlaySDK* sdk);
    void projectPathChanged(const QString& projectPath);
    void projectSourceFileChanged(const QString& projectSourceFile);
    void projectSourceFileAbsoluteChanged(const QUrl& projectSourceFileAbsolute);
    void videoCodecChanged(ScreenPlay::Video::VideoCodec codec);
    void mainAppPIDChanged(qint64 mainAppPID);

public slots:
    void requestFadeIn();
    Q_INVOKABLE void applyTargetSettings();
    virtual void destroyThis() { }
    virtual void terminate() { }
    virtual void setVisible(bool show) { Q_UNUSED(show) }
    virtual void messageReceived(const QString& key, const QString& value) final;
    virtual void clearComponentCache() { }
    virtual void replaceWallpaper(
        const QString absolutePath,
        const QString file,
        const float volume,
        const QString fillMode,
        const QString type,
        const bool checkWallpaperVisible,
        const QJsonObject wallpaperProperties) final;

    QString loadFromFile(const QString& filename);
    QString getApplicationPath();

    void setType(ScreenPlay::ContentTypes::InstalledType type)
    {
        if (m_type == type)
            return;
        m_type = type;
        emit typeChanged(m_type);
    }

    void setAppID(QString appID)
    {
        if (m_appID == appID)
            return;
        m_appID = appID;
        emit appIDChanged(m_appID);
    }

    void setOSVersion(QString OSVersion)
    {
        if (m_OSVersion == OSVersion)
            return;
        m_OSVersion = OSVersion;
        emit OSVersionChanged(m_OSVersion);
    }

    void setCanFade(bool canFade)
    {
        if (m_canFade == canFade)
            return;
        m_canFade = canFade;
        emit canFadeChanged(m_canFade);
    }

    void setWidth(int width)
    {
        if (m_width == width)
            return;
        m_width = width;
        emit widthChanged(m_width);
    }

    void setHeight(int height)
    {
        if (m_height == height)
            return;
        m_height = height;
        emit heightChanged(m_height);
    }

    void setActiveScreensList(QVector<int> activeScreensList)
    {
        if (m_activeScreensList == activeScreensList)
            return;
        m_activeScreensList = activeScreensList;
        emit activeScreensListChanged(m_activeScreensList);
    }

    void setDebugMode(bool debugMode)
    {
        if (m_debugMode == debugMode)
            return;
        m_debugMode = debugMode;
        emit debugModeChanged(debugMode);
    }

    void setFrameStatsVisible(bool frameStatsVisible)
    {
        if (m_frameStatsVisible == frameStatsVisible)
            return;
        m_frameStatsVisible = frameStatsVisible;
        emit frameStatsVisibleChanged(frameStatsVisible);
    }

    void setSdk(ScreenPlaySDK* sdk)
    {
        if (m_sdk.get() == sdk)
            return;
        m_sdk.reset(sdk);
        emit sdkChanged(sdk);
    }

    void setProjectPath(const QString& projectPath)
    {
        if (m_projectPath == projectPath)
            return;
        m_projectPath = projectPath;
        emit projectPathChanged(m_projectPath);
    }

    void setProjectSourceFile(const QString& projectSourceFile)
    {
        if (m_projectSourceFile == projectSourceFile)
            return;
        m_projectSourceFile = projectSourceFile;
        emit projectSourceFileChanged(m_projectSourceFile);
    }

    void setProjectSourceFileAbsolute(const QUrl& projectSourceFileAbsolute)
    {
        if (m_projectSourceFileAbsolute == projectSourceFileAbsolute)
            return;
        m_projectSourceFileAbsolute = projectSourceFileAbsolute;
        emit projectSourceFileAbsoluteChanged(m_projectSourceFileAbsolute);
    }

    void setQuickView(std::shared_ptr<QQuickView> quickView)
    {
        m_quickView = quickView;
    }

private:
    void setupLiveReloading();

protected:
    bool m_canFade { false };
    bool m_debugMode { false };
    bool m_frameStatsVisible { false };

    QString m_projectPath;
    QString m_projectSourceFile;
    QString m_appID;
    QString m_OSVersion;

    int m_width { 0 };
    int m_height { 0 };
    qint64 m_mainAppPID { 0 };

    std::shared_ptr<QQuickView> m_quickView;

    // Playback state objects
    std::unique_ptr<WallpaperState> m_currentState;
    std::unique_ptr<WallpaperState> m_targetState;

    ProcessManager m_processManager;
    ScreenPlay::ContentTypes::InstalledType m_type = ScreenPlay::ContentTypes::InstalledType::Unknown;
    QVector<int> m_activeScreensList;
    QFileSystemWatcher m_fileSystemWatcher;
    QTimer m_liveReloadLimiter;
    std::unique_ptr<ScreenPlaySDK> m_sdk;
    QUrl m_projectSourceFileAbsolute;
    ScreenPlay::Video::VideoCodec m_videoCodec = ScreenPlay::Video::VideoCodec::Unknown;
};
}
