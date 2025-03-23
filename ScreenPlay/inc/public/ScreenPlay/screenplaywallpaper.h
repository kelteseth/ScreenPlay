// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#pragma once

#include <QJsonObject>
#include <QObject>
#include <QProcess>
#include <QString>
#include <QStringList>
#include <QTimer>
#include <memory>

#include "ScreenPlay/globalvariables.h"
#include "ScreenPlay/projectsettingslistmodel.h"
#include "ScreenPlay/sdkconnection.h"
#include "ScreenPlay/settings.h"
#include "ScreenPlay/wallpaperdata.h"
#include "ScreenPlay/wallpapertimelinesection.h"
#include "ScreenPlayCore/globalenums.h"
#include "ScreenPlayCore/processmanager.h"

namespace ScreenPlay {

class ScreenPlayWallpaper : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")
    Q_CLASSINFO("RegisterEnumClassesUnscoped", "false")

    // Properties from WallpaperData - delegated
    Q_PROPERTY(QVector<int> monitors READ monitors WRITE setMonitors NOTIFY monitorsChanged)
    Q_PROPERTY(float volume READ volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERTY(bool isLooping READ isLooping WRITE setIsLooping NOTIFY isLoopingChanged)
    Q_PROPERTY(QString file READ file WRITE setFile NOTIFY fileChanged)
    Q_PROPERTY(QString absolutePath READ absolutePath WRITE setAbsolutePath NOTIFY absolutePathChanged)
    Q_PROPERTY(QString previewImage READ previewImage WRITE setPreviewImage NOTIFY previewImageChanged)
    Q_PROPERTY(ScreenPlay::Video::FillMode fillMode READ fillMode WRITE setFillMode NOTIFY fillModeChanged)
    Q_PROPERTY(ScreenPlay::ContentTypes::InstalledType type READ type WRITE setType NOTIFY typeChanged)

    // Original properties
    Q_PROPERTY(bool isConnected READ isConnected WRITE setIsConnected NOTIFY isConnectedChanged)
    Q_PROPERTY(QString appID READ appID WRITE setAppID NOTIFY appIDChanged)
    Q_PROPERTY(qint64 processID READ processID WRITE setProcessID NOTIFY processIDChanged FINAL)
    Q_PROPERTY(ScreenPlay::ScreenPlayEnums::AppState state READ state WRITE setState NOTIFY stateChanged FINAL)

public:
    explicit ScreenPlayWallpaper(
        const std::shared_ptr<GlobalVariables>& globalVariables,
        const QString& appID,
        const WallpaperData wallpaperData,
        const std::shared_ptr<Settings>& settings,
        QObject* parent = nullptr);

    bool start();
    bool setWallpaperData(const WallpaperData wallpaperData)
    {
        if (isConnected()) {
            qCritical() << "setWallpaperData was called on a live wallpaper. This will not work";
            return false;
        }
        m_wallpaperData = wallpaperData;
        return true;
    }
    bool replaceLive(const WallpaperData wallpaperData);
    void setSDKConnection(std::unique_ptr<SDKConnection> connection);
    std::shared_ptr<ProjectSettingsListModel> getProjectSettingsListModel() { return m_projectSettingsListModel; }
    const WallpaperData wallpaperData() const { return m_wallpaperData; }

    // Getters for WallpaperData properties
    QVector<int> monitors() const { return m_wallpaperData.monitors(); }
    float volume() const { return m_wallpaperData.volume(); }
    bool isLooping() const { return m_wallpaperData.isLooping(); }
    QString file() const { return m_wallpaperData.file(); }
    QString absolutePath() const { return m_wallpaperData.absolutePath(); }
    QString previewImage() const { return m_wallpaperData.previewImage(); }
    Video::FillMode fillMode() const { return m_wallpaperData.fillMode(); }
    ContentTypes::InstalledType type() const { return m_wallpaperData.type(); }

    // Original getters
    bool isConnected() const { return m_isConnected; }
    QString appID() const { return m_appID; }
    qint64 processID() const { return m_processID; }
    ScreenPlay::ScreenPlayEnums::AppState state() const;

signals:
    // Signals for WallpaperData properties
    void monitorsChanged(QVector<int> monitors);
    void volumeChanged(float volume);
    void isLoopingChanged(bool isLooping);
    void fileChanged(QString file);
    void absolutePathChanged(QString absolutePath);
    void previewImageChanged(QString previewImage);
    void fillModeChanged(Video::FillMode fillMode);
    void typeChanged(ContentTypes::InstalledType type);

    // Original signals
    void isConnectedChanged(bool isConnected);
    void appIDChanged(QString appID);
    void processIDChanged(qint64 processID);
    void requestClose(const QString& appID);
    void stateChanged(ScreenPlay::ScreenPlayEnums::AppState state);

public slots:
    QCoro::Task<Result> close();
    void processExit(int exitCode, QProcess::ExitStatus exitStatus);
    bool setWallpaperValue(const QString& key, const QVariant& value, const QString& category, const bool save = false);

    // Setters for WallpaperData properties
    void setMonitors(QVector<int> monitors)
    {
        if (m_wallpaperData.monitors() == monitors)
            return;
        m_wallpaperData.setMonitors(monitors);
        emit monitorsChanged(monitors);
    }

    void setVolume(float volume)
    {
        if (volume < 0.0f || volume > 1.0f)
            return;
        if (qFuzzyCompare(m_wallpaperData.volume(), volume))
            return;
        m_wallpaperData.setVolume(volume);
        emit volumeChanged(volume);
    }

    void setIsLooping(bool isLooping)
    {
        if (m_wallpaperData.isLooping() == isLooping)
            return;
        m_wallpaperData.setIsLooping(isLooping);
        emit isLoopingChanged(isLooping);
    }

    void setFile(QString file)
    {
        if (m_wallpaperData.file() == file)
            return;
        m_wallpaperData.setFile(file);
        emit fileChanged(file);
    }

    void setAbsolutePath(QString absolutePath)
    {
        if (m_wallpaperData.absolutePath() == absolutePath)
            return;
        m_wallpaperData.setAbsolutePath(absolutePath);
        emit absolutePathChanged(absolutePath);
    }

    void setPreviewImage(QString previewImage)
    {
        if (m_wallpaperData.previewImage() == previewImage)
            return;
        m_wallpaperData.setPreviewImage(previewImage);
        emit previewImageChanged(previewImage);
    }

    void setFillMode(Video::FillMode fillMode)
    {
        if (m_wallpaperData.fillMode() == fillMode)
            return;
        m_wallpaperData.setFillMode(fillMode);
        emit fillModeChanged(fillMode);
    }

    void setType(ContentTypes::InstalledType type)
    {
        if (m_wallpaperData.type() == type)
            return;
        m_wallpaperData.setType(type);
        emit typeChanged(type);
    }

    // Original setters
    void setIsConnected(bool isConnected)
    {
        if (m_isConnected == isConnected)
            return;
        m_isConnected = isConnected;
        emit isConnectedChanged(isConnected);
    }

    void setAppID(QString appID)
    {
        if (m_appID == appID)
            return;
        m_appID = appID;
        emit appIDChanged(appID);
    }

    void setProcessID(qint64 processID)
    {
        if (m_processID == processID)
            return;
        m_processID = processID;
        emit processIDChanged(processID);
    }

    void setState(ScreenPlay::ScreenPlayEnums::AppState state);

private:
    void syncAllProperties();

private:
    const std::shared_ptr<GlobalVariables> m_globalVariables;
    std::unique_ptr<SDKConnection> m_connection;
    const std::shared_ptr<Settings> m_settings;

    ProcessManager m_processManager;
    std::shared_ptr<ProjectSettingsListModel> m_projectSettingsListModel;
    QProcess m_process;
    QString m_appID;
    ScreenPlay::ScreenPlayEnums::AppState m_state = ScreenPlay::ScreenPlayEnums::AppState::Inactive;

    WallpaperData m_wallpaperData;

    QTimer m_pingAliveTimer;
    QStringList m_appArgumentsList;
    bool m_isConnected { false };
    bool m_isExiting { false };
    qint64 m_processID { 0 };
    qint64 m_pingAliveTimerMissedPings = 0;
    const qint64 m_pingAliveTimerMaxAllowedMissedPings = 3;
    QJsonObject m_projectJson;
};
}
