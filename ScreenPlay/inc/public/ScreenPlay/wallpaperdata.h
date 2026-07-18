// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#pragma once

#include "ScreenPlay/installedcontentdata.h"
#include "ScreenPlayCore/contenttypes.h"
#include <QDebug>
#include <QDir>
#include <QFileInfoList>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>
#include <QProcess>
#include <QString>
#include <QStringList>
#include <QUuid>
#include <expected>

namespace ScreenPlay {

class WallpaperData : public InstalledContentData {
    Q_GADGET
    QML_ANONYMOUS
    Q_PROPERTY(bool isLooping READ isLooping WRITE setIsLooping)
    Q_PROPERTY(float volume READ volume WRITE setVolume)
    Q_PROPERTY(ScreenPlay::Video::FillMode fillMode READ fillMode WRITE setFillMode)
    Q_PROPERTY(int fpsLimit READ fpsLimit WRITE setFpsLimit)
    Q_PROPERTY(QVector<int> monitors READ monitors WRITE setMonitors)
    Q_PROPERTY(ScreenPlay::Godot::Fps godotFps READ godotFps WRITE setGodotFps)
    Q_PROPERTY(ScreenPlay::Godot::ScaleMode3D godot3DScaleMode READ godot3DScaleMode WRITE setGodot3DScaleMode)
    Q_PROPERTY(float godot3DScale READ godot3DScale WRITE setGodot3DScale)
    Q_PROPERTY(ScreenPlay::Godot::RenderingDriver godotRenderingDriver READ godotRenderingDriver WRITE setGodotRenderingDriver)

public:
    enum class LoadError {
        None,
        EmptyConfiguration,
        InvalidMonitorNumber,
        DuplicateMonitor,
        MonitorDoesNotExist,
        WallpaperFileDoesNotExist
    };
    Q_ENUM(LoadError)

    static QString loadErrorToString(LoadError error);
    bool isLooping() const { return m_isLooping; }
    float volume() const { return m_volume; }
    Video::FillMode fillMode() const { return m_fillMode; }
    // Per-wallpaper render fps cap for non-Godot wallpapers. -1 means "inherit
    // the global Settings::wallpaperFpsLimit"; 0 means unlimited; >0 is an
    // explicit cap. Godot wallpapers use godotFps instead.
    int fpsLimit() const { return m_fpsLimit; }
    QVector<int> monitors() const { return m_monitors; }
    Godot::Fps godotFps() const { return m_godotFps; }
    float godot3DScale() const { return m_godot3DScale; }
    ScreenPlay::Godot::ScaleMode3D godot3DScaleMode() const { return m_godot3DScaleMode; }
    ScreenPlay::Godot::RenderingDriver godotRenderingDriver() const { return m_godotRenderingDriver; }

    void setIsLooping(bool value) { m_isLooping = value; }
    void setVolume(float value) { m_volume = value; }
    void setFillMode(Video::FillMode value) { m_fillMode = value; }
    void setFpsLimit(int value) { m_fpsLimit = value; }
    void setMonitors(const QVector<int>& value) { m_monitors = value; }
    void setGodotFps(Godot::Fps value) { m_godotFps = value; }
    void setGodot3DScaleMode(Godot::ScaleMode3D value) { m_godot3DScaleMode = value; }
    void setGodotRenderingDriver(ScreenPlay::Godot::RenderingDriver godotRenderingDriver) { m_godotRenderingDriver = godotRenderingDriver; };
    void setGodot3DScale(float value) { m_godot3DScale = value; }

    Q_INVOKABLE QString toString() const;
    Q_INVOKABLE bool hasContent();

    QJsonObject serialize() const;
    static std::expected<WallpaperData, LoadError> loadTimelineWallpaperConfig(const QJsonObject& wallpaperObj);

private:
    bool m_isLooping = true;
    float m_volume = 1.0f;
    Video::FillMode m_fillMode = Video::FillMode::Fill;
    int m_fpsLimit = -1; // -1 = inherit global setting
    QVector<int> m_monitors {};
    Godot::Fps m_godotFps = Godot::Fps::Fps60;
    float m_godot3DScale { 1.0f };
    Godot::ScaleMode3D m_godot3DScaleMode = Godot::ScaleMode3D::Bilinear;
    Godot::RenderingDriver m_godotRenderingDriver { Godot::RenderingDriver::Vulkan };
};
}
