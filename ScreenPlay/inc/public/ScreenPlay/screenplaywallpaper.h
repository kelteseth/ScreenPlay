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
    Q_PROPERTY(int fpsLimit READ fpsLimit WRITE setFpsLimit NOTIFY fpsLimitChanged FINAL)
    Q_PROPERTY(Godot::Fps godotFps READ godotFps WRITE setGodotFps NOTIFY godotFpsChanged FINAL)
    Q_PROPERTY(Godot::ScaleMode3D godot3DScaleMode READ godot3DScaleMode WRITE setGodot3DScaleMode NOTIFY godot3DScaleModeChanged FINAL)
    Q_PROPERTY(float godot3DScale READ godot3DScale WRITE setGodot3DScale NOTIFY godot3DScaleChanged FINAL)

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
    int fpsLimit() const { return m_wallpaperData.fpsLimit(); }
    Godot::Fps godotFps() const { return m_wallpaperData.godotFps(); }
    Godot::ScaleMode3D godot3DScaleMode() const { return m_wallpaperData.godot3DScaleMode(); }
    float godot3DScale() const { return m_wallpaperData.godot3DScale(); }

signals:
    // Signals for WallpaperData properties
    void monitorsChanged(QVector<int> monitors);
    void volumeChanged(float volume);
    void isLoopingChanged(bool isLooping);
    void fileChanged(QString file);
    void fillModeChanged(Video::FillMode fillMode);
    void fpsLimitChanged(int fpsLimit);
    void godotFpsChanged(Godot::Fps godotFps);
    void godot3DScaleModeChanged(Godot::ScaleMode3D godot3DScaleMode);
    void godot3DScaleChanged(float godot3DScale);

public slots:
    bool setWallpaperValue(const QString& key, const QVariant& value, const QString& category, const bool save = false);

    void updateVolume(const float volume);
    void updateFillMode(const Video::FillMode fillMode);
    void updateFpsLimit(const int fpsLimit);
    void updateGodotFps(const Godot::Fps godotFps);
    void updateGodot3DScaleMode(const Godot::ScaleMode3D godot3DScaleMode);
    void updateGodot3DScale(const float godot3DScale);
    void updateProperty(const QString& category, const QString& key, const QVariant& value);

    // Setters for WallpaperData properties
    void setMonitors(QVector<int> monitors);
    void setVolume(float volume);
    void setIsLooping(bool isLooping);
    void setFile(QString file);
    void setFillMode(Video::FillMode fillMode);
    void setFpsLimit(int fpsLimit);
    void setGodotFps(Godot::Fps godotFps);
    void setGodot3DScaleMode(Godot::ScaleMode3D godot3DScaleMode);
    void setGodot3DScale(float godot3DScale);

protected:
    void setupSDKConnection() override;

private:
    void syncAllProperties();
    void updateDynamicArguments();

private:
    const std::shared_ptr<Settings> m_settings;
    WallpaperData m_wallpaperData;
    QJsonObject m_projectJson;
};
}
