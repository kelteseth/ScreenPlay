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

namespace ScreenPlay {

class ScreenPlayWallpaper : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")

    Q_PROPERTY(bool isConnected READ isConnected WRITE setIsConnected NOTIFY isConnectedChanged)
    Q_PROPERTY(QVector<int> screenNumber READ screenNumber WRITE setScreenNumber NOTIFY screenNumberChanged)
    Q_PROPERTY(float volume READ volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERTY(float playbackRate READ playbackRate WRITE setPlaybackRate NOTIFY playbackRateChanged)
    Q_PROPERTY(bool isLooping READ isLooping WRITE setIsLooping NOTIFY isLoopingChanged)
    Q_PROPERTY(QString file READ file WRITE setFile NOTIFY fileChanged)
    Q_PROPERTY(QString absolutePath READ absolutePath WRITE setAbsolutePath NOTIFY absolutePathChanged)
    Q_PROPERTY(QString previewImage READ previewImage WRITE setPreviewImage NOTIFY previewImageChanged)
    Q_PROPERTY(QString appID READ appID WRITE setAppID NOTIFY appIDChanged)
    Q_PROPERTY(Video::FillMode fillMode READ fillMode WRITE setFillMode NOTIFY fillModeChanged)
    Q_PROPERTY(ContentTypes::InstalledType type READ type WRITE setType NOTIFY typeChanged)

public:
    explicit ScreenPlayWallpaper(
        const QVector<int>& screenNumber,
        const std::shared_ptr<GlobalVariables>& globalVariables,
        const QString& appID,
        const QString& absolutePath,
        const QString& previewImage,
        const QString& file,
        const float volume,
        const float playbackRate,
        const Video::FillMode fillMode,
        const ContentTypes::InstalledType type,
        const QJsonObject& properties,
        const std::shared_ptr<Settings>& settings,
        QObject* parent = nullptr);

    bool start();

    bool replace(
        const QString& absolutePath,
        const QString& previewImage,
        const QString& file,
        const float volume,
        const Video::FillMode fillMode,
        const ContentTypes::InstalledType type,
        const bool checkWallpaperVisible);

    void setSDKConnection(std::unique_ptr<SDKConnection> connection);

    QJsonObject getActiveSettingsJson();

    QVector<int> screenNumber() const { return m_screenNumber; }
    QString previewImage() const { return m_previewImage; }
    QString appID() const { return m_appID; }
    ContentTypes::InstalledType type() const { return m_type; }
    QString file() const { return m_file; }
    Video::FillMode fillMode() const { return m_fillMode; }
    QString absolutePath() const { return m_absolutePath; }
    float volume() const { return m_volume; }
    bool isLooping() const { return m_isLooping; }
    ProjectSettingsListModel* getProjectSettingsListModel() { return &m_projectSettingsListModel; }
    float playbackRate() const { return m_playbackRate; }
    bool isConnected() const { return m_isConnected; }

signals:
    void screenNumberChanged(QVector<int> screenNumber);
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

    void requestSave();
    void requestClose(const QString& appID);
    void error(const QString& msg);

    void isConnectedChanged(bool isConnected);

public slots:
    void close();
    void processExit(int exitCode, QProcess::ExitStatus exitStatus);
    void processError(QProcess::ProcessError error);
    bool setWallpaperValue(const QString& key, const QString& value, const bool save = false);

    void setScreenNumber(QVector<int> screenNumber)
    {
        if (m_screenNumber == screenNumber)
            return;

        m_screenNumber = screenNumber;
        emit screenNumberChanged(m_screenNumber);
    }

    void setPreviewImage(QString previewImage)
    {
        if (m_previewImage == previewImage)
            return;

        m_previewImage = previewImage;
        emit previewImageChanged(m_previewImage);
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
        if (m_type == type)
            return;

        m_type = type;
        emit typeChanged(m_type);
    }

    void setFile(QString file)
    {
        if (m_file == file)
            return;

        m_file = file;
        emit fileChanged(m_file);
    }

    void setFillMode(Video::FillMode fillMode)
    {
        if (m_fillMode == fillMode)
            return;

        m_fillMode = fillMode;
        emit fillModeChanged(m_fillMode);
    }

    void setAbsolutePath(QString absolutePath)
    {
        if (m_absolutePath == absolutePath)
            return;

        m_absolutePath = absolutePath;
        emit absolutePathChanged(m_absolutePath);
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

    void setIsLooping(bool isLooping)
    {
        if (m_isLooping == isLooping)
            return;

        m_isLooping = isLooping;
        emit isLoopingChanged(m_isLooping);
    }

    void setPlaybackRate(float playbackRate)
    {
        if (playbackRate < 0.0f || playbackRate > 1.0f)
            return;

        m_playbackRate = playbackRate;
        emit playbackRateChanged(m_playbackRate);
    }

    void setIsConnected(bool isConnected)
    {
        if (m_isConnected == isConnected)
            return;
        m_isConnected = isConnected;
        emit isConnectedChanged(m_isConnected);
    }

private:
    const std::shared_ptr<GlobalVariables> m_globalVariables;
    std::unique_ptr<SDKConnection> m_connection;
    const std::shared_ptr<Settings> m_settings;

    ProjectSettingsListModel m_projectSettingsListModel;
    QJsonObject m_projectJson;
    QVector<int> m_screenNumber;
    QProcess m_process;
    QString m_previewImage;
    ContentTypes::InstalledType m_type { ContentTypes::InstalledType::Unknown };
    Video::FillMode m_fillMode;
    QString m_appID;
    QString m_absolutePath;
    QString m_file;
    float m_volume { 1.0f };
    bool m_isLooping { true };
    float m_playbackRate { 1.0f };
    QTimer m_pingAliveTimer;
    QStringList m_appArgumentsList;
    bool m_isConnected { false };
    // There are still cases where we can access the current item
    // while exiting. This flag is to ignore all setWallpaperValue calls
    bool m_isExiting { false };
};
}
