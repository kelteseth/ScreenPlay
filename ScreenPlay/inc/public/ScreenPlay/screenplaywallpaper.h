// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#pragma once

#include "ScreenPlay/screenplayexternalprocess.h"
#include "ScreenPlay/settings.h"
#include "ScreenPlay/wallpaperdata.h"
#include "ScreenPlay/wallpapertimelinesection.h"

namespace ScreenPlay {

class ScreenPlayWallpaper : public ScreenPlayExternalProcess {
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")

    // Q_PROPERTY declarations for wallpaper-specific properties
    Q_PROPERTY(QVector<int> monitors READ monitors WRITE setMonitors NOTIFY monitorsChanged FINAL)
    Q_PROPERTY(float volume READ volume WRITE setVolume NOTIFY volumeChanged FINAL)
    Q_PROPERTY(bool isLooping READ isLooping WRITE setIsLooping NOTIFY isLoopingChanged FINAL)
    Q_PROPERTY(QString file READ file WRITE setFile NOTIFY fileChanged FINAL)
    Q_PROPERTY(Video::FillMode fillMode READ fillMode WRITE setFillMode NOTIFY fillModeChanged FINAL)

public:
    explicit ScreenPlayWallpaper(
        const std::shared_ptr<GlobalVariables>& globalVariables,
        const QString& appID,
        const WallpaperData wallpaperData,
        const std::shared_ptr<Settings>& settings,
        QObject* parent = nullptr);

    bool start() override;
    QCoro::Task<Result> close() override;

    bool setWallpaperData(const WallpaperData wallpaperData);
    bool replaceLive(const WallpaperData wallpaperData);
    const WallpaperData wallpaperData() const { return m_wallpaperData; }

    // Getters for WallpaperData properties
    QVector<int> monitors() const { return m_wallpaperData.monitors(); }
    float volume() const { return m_wallpaperData.volume(); }
    bool isLooping() const { return m_wallpaperData.isLooping(); }
    QString file() const { return m_wallpaperData.file(); }
    Video::FillMode fillMode() const { return m_wallpaperData.fillMode(); }

signals:
    // Signals for WallpaperData properties
    void monitorsChanged(QVector<int> monitors);
    void volumeChanged(float volume);
    void isLoopingChanged(bool isLooping);
    void fileChanged(QString file);
    void fillModeChanged(Video::FillMode fillMode);

public slots:
    bool setWallpaperValue(const QString& key, const QVariant& value, const QString& category, const bool save = false);

    // Setters for WallpaperData properties
    void setMonitors(QVector<int> monitors);
    void setVolume(float volume);
    void setIsLooping(bool isLooping);
    void setFile(QString file);
    void setFillMode(Video::FillMode fillMode);

protected:
    void setupSDKConnection() override;

private:
    void syncAllProperties();

private:
    const std::shared_ptr<Settings> m_settings;
    WallpaperData m_wallpaperData;
    QJsonObject m_projectJson;
};
}
