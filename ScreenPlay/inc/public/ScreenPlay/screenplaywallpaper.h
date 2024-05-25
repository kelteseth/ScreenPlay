// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#pragma once

#include <QDebug>
#include <QDir>
#include <QFileInfoList>
#include <QJsonObject>
#include <QObject>
#include <QProcess>
#include <QString>
#include <QStringList>
#include <memory>

#include "ScreenPlay/globalvariables.h"
#include "ScreenPlay/projectsettingslistmodel.h"
#include "ScreenPlay/sdkconnection.h"
#include "ScreenPlay/settings.h"
#include "ScreenPlayUtil/processmanager.h"

namespace ScreenPlay {

struct WallpaperData {
    bool isLooping = false;
    QString absolutePath;
    QString previewImage;
    float playbackRate = {};
    float volume = {};
    QString file;
    QJsonObject properties;
    ContentTypes::InstalledType type = ContentTypes::InstalledType::Unknown;
    Video::FillMode fillMode = Video::FillMode::Fill;
    QVector<int> monitors;
    QJsonObject serialize() const
    {
        QJsonObject data;
        data.insert("isLooping", isLooping);
        data.insert("absolutePath", absolutePath);
        data.insert("previewImage", previewImage);
        data.insert("playbackRate", playbackRate);
        data.insert("volume", volume);
        data.insert("file", file);
        data.insert("properties", properties);
        data.insert("type", QVariant::fromValue(type).toString());
        data.insert("fillMode", QVariant::fromValue(fillMode).toString());

        // Serialize QVector<int> monitors
        QJsonArray monitorArray;
        for (int monitor : monitors) {
            monitorArray.append(monitor);
        }
        data.insert("monitors", monitorArray);

        return data;
    }
};

class ScreenPlayWallpaper;
// Represents one line in the UI. ScreenPlayManager has a list of
// WallpaperTimeline. Only the active timeline section has
// a filled vector of ScreenPlayWallpaper
struct WallpaperTimelineSection {
    bool isActive = false;

    QString identifier;
    int index = 0; // Needed to check
    float relativePosition = 0.0f;
    QTime startTime;
    QTime endTime;
    // Data from the profiles.json that we need when we
    // enable this section of the pipeline. We keep a copy
    // here when this timeline needs to become active
    std::vector<WallpaperData> wallpaperData;
    // All active wallpaper.
    std::vector<std::shared_ptr<ScreenPlayWallpaper>> activeWallpaperList;

    // Check if currentTime falls within the timeline section
    bool containsTime(const QTime& time) const
    {
        if (endTime < startTime) { // Timeline spans midnight
            return (time >= startTime || time < endTime);
        } else {
            return (time >= startTime && time < endTime);
        }
    }
};

class ScreenPlayWallpaper : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")

    Q_PROPERTY(bool isConnected READ isConnected WRITE setIsConnected NOTIFY isConnectedChanged)
    Q_PROPERTY(QVector<int> monitors READ monitors WRITE setMonitors NOTIFY monitorsChanged)
    Q_PROPERTY(float volume READ volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERTY(float playbackRate READ playbackRate WRITE setPlaybackRate NOTIFY playbackRateChanged)
    Q_PROPERTY(bool isLooping READ isLooping WRITE setIsLooping NOTIFY isLoopingChanged)
    Q_PROPERTY(QString file READ file WRITE setFile NOTIFY fileChanged)
    Q_PROPERTY(QString absolutePath READ absolutePath WRITE setAbsolutePath NOTIFY absolutePathChanged)
    Q_PROPERTY(QString previewImage READ previewImage WRITE setPreviewImage NOTIFY previewImageChanged)
    Q_PROPERTY(QString appID READ appID WRITE setAppID NOTIFY appIDChanged)
    Q_PROPERTY(Video::FillMode fillMode READ fillMode WRITE setFillMode NOTIFY fillModeChanged)
    Q_PROPERTY(ContentTypes::InstalledType type READ type WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(qint64 processID READ processID WRITE setProcessID NOTIFY processIDChanged FINAL)

public:
    explicit ScreenPlayWallpaper(
        const std::shared_ptr<GlobalVariables>& globalVariables,
        const QString& appID,
        const WallpaperData wallpaperData,
        const std::shared_ptr<Settings>& settings,
        QObject* parent = nullptr);

    bool start();

    bool replace(
        const WallpaperData wallpaperData,
        const bool checkWallpaperVisible);

    void setSDKConnection(std::unique_ptr<SDKConnection> connection);

    QJsonObject getActiveSettingsJson();

    QVector<int> monitors() const { return m_wallpaperData.monitors; }
    QString previewImage() const { return m_wallpaperData.previewImage; }
    QString appID() const { return m_appID; }
    ContentTypes::InstalledType type() const { return m_wallpaperData.type; }
    QString file() const { return m_wallpaperData.file; }
    Video::FillMode fillMode() const { return m_wallpaperData.fillMode; }
    QString absolutePath() const { return m_wallpaperData.absolutePath; }
    float volume() const { return m_wallpaperData.volume; }
    bool isLooping() const { return m_wallpaperData.isLooping; }
    ProjectSettingsListModel* getProjectSettingsListModel() { return &m_projectSettingsListModel; }
    float playbackRate() const { return m_wallpaperData.playbackRate; }
    bool isConnected() const { return m_isConnected; }
    qint64 processID() const { return m_processID; }

    const WallpaperData& wallpaperData();

signals:
    void monitorsChanged(QVector<int> monitors);
    void previewImageChanged(QString previewImage);
    void appIDChanged(QString appID);
    void typeChanged(ContentTypes::InstalledType type);
    void fileChanged(QString file);
    void fillModeChanged(Video::FillMode fillMode);
    void absolutePathChanged(QString absolutePath);
    void profileJsonObjectChanged(QJsonObject profileJsonObject);
    void volumeChanged(float volume);
    void isLoopingChanged(bool isLooping);
    void playbackRateChanged(float playbackRate);
    void isConnectedChanged(bool isConnected);
    void processIDChanged(qint64 processID);

    void requestSave();
    void requestClose(const QString& appID);
    void error(const QString& msg);

public slots:
    void close();
    void processExit(int exitCode, QProcess::ExitStatus exitStatus);
    void processError(QProcess::ProcessError error);
    bool setWallpaperValue(const QString& key, const QString& value, const bool save = false);

    void setMonitors(QVector<int> monitors)
    {
        if (m_wallpaperData.monitors == monitors)
            return;

        m_wallpaperData.monitors = monitors;
        emit monitorsChanged(m_wallpaperData.monitors);
    }

    void setPreviewImage(QString previewImage)
    {
        if (m_wallpaperData.previewImage == previewImage)
            return;

        m_wallpaperData.previewImage = previewImage;
        emit previewImageChanged(m_wallpaperData.previewImage);
    }

    void setAppID(QString appID)
    {
        if (m_appID == appID)
            return;

        m_appID = appID;
        emit appIDChanged(m_appID);
    }

    void setType(ContentTypes::InstalledType type)
    {
        if (m_wallpaperData.type == type)
            return;

        m_wallpaperData.type = type;
        emit typeChanged(m_wallpaperData.type);
    }

    void setFile(QString file)
    {
        if (m_wallpaperData.file == file)
            return;

        m_wallpaperData.file = file;
        emit fileChanged(m_wallpaperData.file);
    }

    void setFillMode(Video::FillMode fillMode)
    {
        if (m_wallpaperData.fillMode == fillMode)
            return;

        m_wallpaperData.fillMode = fillMode;
        emit fillModeChanged(m_wallpaperData.fillMode);
    }

    void setAbsolutePath(QString absolutePath)
    {
        if (m_wallpaperData.absolutePath == absolutePath)
            return;

        m_wallpaperData.absolutePath = absolutePath;
        emit absolutePathChanged(m_wallpaperData.absolutePath);
    }

    void setVolume(float volume)
    {
        if (volume < 0.0f || volume > 1.0f)
            return;

        if (qFuzzyCompare(m_wallpaperData.volume, volume))
            return;

        m_wallpaperData.volume = volume;
        emit volumeChanged(m_wallpaperData.volume);
    }

    void setIsLooping(bool isLooping)
    {
        if (m_wallpaperData.isLooping == isLooping)
            return;

        m_wallpaperData.isLooping = isLooping;
        emit isLoopingChanged(m_wallpaperData.isLooping);
    }

    void setPlaybackRate(float playbackRate)
    {
        if (playbackRate < 0.0f || playbackRate > 1.0f)
            return;

        m_wallpaperData.playbackRate = playbackRate;
        emit playbackRateChanged(m_wallpaperData.playbackRate);
    }

    void setIsConnected(bool isConnected)
    {
        if (m_isConnected == isConnected)
            return;
        m_isConnected = isConnected;
        emit isConnectedChanged(m_isConnected);
    }

    void setProcessID(qint64 processID)
    {
        if (m_processID == processID)
            return;
        m_processID = processID;
        emit processIDChanged(m_processID);
    }

private:
    const std::shared_ptr<GlobalVariables> m_globalVariables;
    std::unique_ptr<SDKConnection> m_connection;
    const std::shared_ptr<Settings> m_settings;

    ProcessManager m_processManager;
    ProjectSettingsListModel m_projectSettingsListModel;
    QJsonObject m_projectJson;
    QProcess m_process;
    QString m_appID;

    WallpaperData m_wallpaperData;

    QTimer m_pingAliveTimer;
    QStringList m_appArgumentsList;
    bool m_isConnected { false };
    // There are still cases where we can access the current item
    // while exiting. This flag is to ignore all setWallpaperValue calls
    bool m_isExiting { false };
    qint64 m_processID { 0 };
};
}
