// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#pragma once

#include <QDebug>
#include <QFile>
#include <QFileSystemWatcher>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QObject>
#include <QString>
#include <QSysInfo>
#include <QtQml>

#include "ScreenPlaySDK/screenplaysdk.h"
#include "ScreenPlayUtil/exitcodes.h"
#include "ScreenPlayUtil/util.h"

#include <memory>

class BaseWindow : public QObject {
    Q_OBJECT

public:
    BaseWindow();

    virtual ScreenPlay::WallpaperExitCode setup() final;

    virtual ScreenPlay::WallpaperExitCode start() = 0;

    Q_PROPERTY(int width READ width WRITE setWidth NOTIFY widthChanged)
    Q_PROPERTY(int height READ height WRITE setHeight NOTIFY heightChanged)
    Q_PROPERTY(QVector<int> activeScreensList READ activeScreensList WRITE setActiveScreensList NOTIFY activeScreensListChanged)

    Q_PROPERTY(QString appID READ appID WRITE setAppID NOTIFY appIDChanged)
    Q_PROPERTY(QString fillMode READ fillMode WRITE setFillMode NOTIFY fillModeChanged)

    Q_PROPERTY(QString projectPath READ projectPath WRITE setProjectPath NOTIFY projectPathChanged)
    Q_PROPERTY(QString projectSourceFile READ projectSourceFile WRITE setProjectSourceFile NOTIFY projectSourceFileChanged)
    Q_PROPERTY(QUrl projectSourceFileAbsolute READ projectSourceFileAbsolute WRITE setProjectSourceFileAbsolute NOTIFY projectSourceFileAbsoluteChanged)

    Q_PROPERTY(bool loops READ loops WRITE setLoops NOTIFY loopsChanged)
    Q_PROPERTY(bool isPlaying READ isPlaying WRITE setIsPlaying NOTIFY isPlayingChanged)
    Q_PROPERTY(bool muted READ muted WRITE setMuted NOTIFY mutedChanged)
    Q_PROPERTY(bool canFade READ canFade WRITE setCanFade NOTIFY canFadeChanged)
    Q_PROPERTY(bool debugMode READ debugMode WRITE setDebugMode NOTIFY debugModeChanged)

    // Save performance by checking if the wallpaper is visible (video wallpaper only for now)
    Q_PROPERTY(bool checkWallpaperVisible READ checkWallpaperVisible WRITE setCheckWallpaperVisible NOTIFY checkWallpaperVisibleChanged)
    Q_PROPERTY(bool visualsPaused READ visualsPaused WRITE setVisualsPaused NOTIFY visualsPausedChanged)

    Q_PROPERTY(float volume READ volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERTY(float playbackRate READ playbackRate WRITE setPlaybackRate NOTIFY playbackRateChanged)
    Q_PROPERTY(float currentTime READ currentTime WRITE setCurrentTime NOTIFY currentTimeChanged)

    Q_PROPERTY(ScreenPlay::InstalledType::InstalledType type READ type WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(ScreenPlay::VideoCodec::VideoCodec videoCodec READ videoCodec WRITE setVideoCodec NOTIFY videoCodecChanged)
    Q_PROPERTY(QString OSVersion READ OSVersion WRITE setOSVersion NOTIFY OSVersionChanged)

    Q_PROPERTY(ScreenPlaySDK* sdk READ sdk WRITE setSdk NOTIFY sdkChanged)

    bool loops() const { return m_loops; }
    float volume() const { return m_volume; }
    bool isPlaying() const { return m_isPlaying; }
    float playbackRate() const { return m_playbackRate; }
    ScreenPlay::InstalledType::InstalledType type() const { return m_type; }
    QString appID() const { return m_appID; }
    QString OSVersion() const { return m_OSVersion; }
    bool muted() const { return m_muted; }
    float currentTime() const { return m_currentTime; }
    bool canFade() const { return m_canFade; }
    QString fillMode() const { return m_fillMode; }
    int width() const { return m_width; }
    int height() const { return m_height; }
    QVector<int> activeScreensList() const { return m_activeScreensList; }
    bool checkWallpaperVisible() const { return m_checkWallpaperVisible; }
    bool visualsPaused() const { return m_visualsPaused; }
    bool debugMode() const { return m_debugMode; }
    ScreenPlaySDK* sdk() const { return m_sdk.get(); }
    const QString& projectPath() const { return m_projectPath; }
    const QString& projectSourceFile() const { return m_projectSourceFile; }
    const QUrl& projectSourceFileAbsolute() const { return m_projectSourceFileAbsolute; }

    ScreenPlay::VideoCodec::VideoCodec videoCodec() const;
    void setVideoCodec(ScreenPlay::VideoCodec::VideoCodec newVideoCodec);

signals:
    void qmlStart();
    void qmlExit();
    void fadeIn();
    void reloadQML(const ScreenPlay::InstalledType::InstalledType oldType);
    void reloadVideo(const ScreenPlay::InstalledType::InstalledType oldType);
    void reloadGIF(const ScreenPlay::InstalledType::InstalledType oldType);

    void loopsChanged(bool loops);
    void volumeChanged(float volume);
    void isPlayingChanged(bool isPlaying);
    void playbackRateChanged(float playbackRate);
    void typeChanged(ScreenPlay::InstalledType::InstalledType type);
    void appIDChanged(QString appID);
    void qmlSceneValueReceived(QString key, QString value);
    void OSVersionChanged(QString OSVersion);
    void mutedChanged(bool muted);
    void currentTimeChanged(float currentTime);
    void canFadeChanged(bool canFade);
    void fillModeChanged(QString fillMode);
    void widthChanged(int width);
    void heightChanged(int height);
    void activeScreensListChanged(QVector<int> activeScreensList);
    void checkWallpaperVisibleChanged(bool checkWallpaperVisible);
    void visualsPausedChanged(bool visualsPaused);
    void debugModeChanged(bool debugMode);
    void sdkChanged(ScreenPlaySDK* sdk);
    void projectPathChanged(const QString& rojectPath);
    void projectSourceFileChanged(const QString& projectSourceFile);
    void projectSourceFileAbsoluteChanged(const QUrl& rojectSourceFileAbsolute);
    void videoCodecChanged(ScreenPlay::VideoCodec::VideoCodec codec);

public slots:
    void requestFadeIn();
    virtual void destroyThis() { }
    virtual void terminate() { }
    virtual void setVisible(bool show) { Q_UNUSED(show) }
    virtual void messageReceived(QString key, QString value) final;
    virtual void clearComponentCache() { }
    virtual void replaceWallpaper(
        const QString absolutePath,
        const QString file,
        const float volume,
        const QString fillMode,
        const QString type,
        const bool checkWallpaperVisible) final;

    QString loadFromFile(const QString& filename);
    QString getApplicationPath();

    void setLoops(bool loops)
    {
        if (m_loops == loops)
            return;

        m_loops = loops;
        emit loopsChanged(m_loops);
    }
    void setVolume(float volume)
    {
        if (volume < 0.0f || volume > 1.0f)
            return;

        if (qFuzzyCompare(m_volume, volume))
            return;

        m_volume = volume;
        emit volumeChanged(m_volume);
    }
    void setIsPlaying(bool isPlaying)
    {
        if (m_isPlaying == isPlaying)
            return;

        m_isPlaying = isPlaying;
        emit isPlayingChanged(m_isPlaying);
    }
    void setPlaybackRate(float playbackRate)
    {
        if (playbackRate < 0.0f || playbackRate > 1.0f)
            return;

        if (qFuzzyCompare(m_playbackRate, playbackRate))
            return;

        m_playbackRate = playbackRate;
        emit playbackRateChanged(m_playbackRate);
    }
    void setType(ScreenPlay::InstalledType::InstalledType type)
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
    void setMuted(bool muted)
    {
        if (m_muted == muted)
            return;

        m_muted = muted;
        emit mutedChanged(m_muted);
    }
    void setCurrentTime(float currentTime)
    {
        if (currentTime < 0.0f || currentTime > 100000000000.0f)
            return;

        if (qFuzzyCompare(m_currentTime, currentTime))
            return;

        m_currentTime = currentTime;
        emit currentTimeChanged(m_currentTime);
    }
    void setCanFade(bool canFade)
    {
        if (m_canFade == canFade)
            return;

        m_canFade = canFade;
        emit canFadeChanged(m_canFade);
    }
    void setFillMode(QString fillMode)
    {
        if (m_fillMode == fillMode)
            return;

        fillMode = fillMode.toLower();

        if (!ScreenPlayUtil::getAvailableFillModes().contains(fillMode)) {
            qWarning() << "Unable to set fillmode, the provided value did not match the available values"
                       << "Provided: " << fillMode
                       << "Available: " << ScreenPlayUtil::getAvailableFillModes();
            return;
        }

        // Actual web fillmodes are lowercase
        // https://developer.mozilla.org/en-US/docs/Web/CSS/object-fit
        m_fillMode = fillMode.toLower();
        emit fillModeChanged(m_fillMode);
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
    void setCheckWallpaperVisible(bool checkWallpaperVisible)
    {
        if (m_checkWallpaperVisible == checkWallpaperVisible)
            return;

        m_checkWallpaperVisible = checkWallpaperVisible;
        emit checkWallpaperVisibleChanged(m_checkWallpaperVisible);
    }
    void setVisualsPaused(bool visualsPaused)
    {
        if (m_visualsPaused == visualsPaused)
            return;

        m_visualsPaused = visualsPaused;
        emit visualsPausedChanged(m_visualsPaused);
    }
    void setDebugMode(bool debugMode)
    {
        if (m_debugMode == debugMode)
            return;
        m_debugMode = debugMode;
        emit debugModeChanged(debugMode);
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

private:
    void setupLiveReloading();

protected:
    bool m_checkWallpaperVisible { false };
    bool m_visualsPaused { false };
    bool m_loops { true };
    bool m_isPlaying { true };
    bool m_muted { false };
    bool m_canFade { false };
    bool m_debugMode { false };

    float m_volume { 1.0f };
    float m_playbackRate { 1.0f };
    float m_currentTime { 0.0f };

    QString m_projectPath;
    QString m_projectSourceFile;
    QString m_appID;
    QString m_OSVersion;
    QString m_fillMode;

    int m_width { 0 };
    int m_height { 0 };

    ScreenPlay::InstalledType::InstalledType m_type = ScreenPlay::InstalledType::InstalledType::Unknown;
    QVector<int> m_activeScreensList;
    QFileSystemWatcher m_fileSystemWatcher;
    QTimer m_liveReloadLimiter;
    QSysInfo m_sysinfo;
    std::unique_ptr<ScreenPlaySDK> m_sdk;
    QUrl m_projectSourceFileAbsolute;
    ScreenPlay::VideoCodec::VideoCodec m_videoCodec = ScreenPlay::VideoCodec::VideoCodec::Unknown;
};
