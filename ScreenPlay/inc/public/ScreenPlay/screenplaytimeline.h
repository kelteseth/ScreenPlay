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

#include "ScreenPlay/wallpapertimelinesection.h"

namespace ScreenPlay {


struct ScreenPlayTimeline{


    float calculateRelativePosition(const QTime& endTime);

    std::optional<std::shared_ptr<WallpaperTimelineSection>> loadTimelineWallpaperConfig(const QJsonObject& timelineObj);
    std::shared_ptr<WallpaperTimelineSection> findActiveWallpaperTimelineSection();
    std::optional<std::shared_ptr<WallpaperTimelineSection>> activeWallpaperSectionByAppID(const QString& appID);
    std::shared_ptr<WallpaperTimelineSection> getCurrentTimeline();


    QVector<std::shared_ptr<WallpaperTimelineSection>> m_wallpaperTimelineSectionsList;

    // We use a24 hour system
    const QString m_timelineTimeFormat = "hh:mm:ss";
};

}
