// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#pragma once

#include <QDebug>
#include <QDir>
#include <QFileInfoList>
#include <QJsonObject>
#include <QJsonArray>
#include <QObject>
#include <QProcess>
#include <QString>
#include <QStringList>

#include "ScreenPlayUtil/contenttypes.h"

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
    QJsonObject serialize() const;

    static std::optional<WallpaperData> loadWallpaperConfig(const QJsonObject& wallpaperObj);

};
}
