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
    Q_PROPERTY(QVector<int> monitors READ monitors WRITE setMonitors)
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
    QVector<int> monitors() const { return m_monitors; }

    void setIsLooping(bool value) { m_isLooping = value; }
    void setVolume(float value) { m_volume = value; }
    void setFillMode(Video::FillMode value) { m_fillMode = value; }
    void setMonitors(const QVector<int>& value) { m_monitors = value; }

    Q_INVOKABLE QString toString() const;
    Q_INVOKABLE bool hasContent();

    QJsonObject serialize() const;
    static std::expected<WallpaperData, LoadError> loadTimelineWallpaperConfig(const QJsonObject& wallpaperObj);

private:
    bool m_isLooping = true;
    float m_volume = 1.0f;
    Video::FillMode m_fillMode = Video::FillMode::Fill;
    QVector<int> m_monitors {};
};
}
