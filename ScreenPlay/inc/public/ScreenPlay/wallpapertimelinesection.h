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

#include "ScreenPlay/wallpaperdata.h"

namespace ScreenPlay {
class ScreenPlayWallpaper;
// Represents one line in the UI. ScreenPlayManager has a list of
// WallpaperTimeline. Only the active timeline section has
// a filled vector of ScreenPlayWallpaper
struct WallpaperTimelineSection {
    // Is active is needed as an additional flag during switching.
    // When timeline A is no longer in the time range, then we can
    // use this flag to know that it was the last active timeline and
    // remove all active wallpaper.
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

    bool close();

    // Check if currentTime falls within the timeline section
    bool containsTime(const QTime& time) const
    {
        if (endTime < startTime) { // Timeline spans midnight
            return (time >= startTime || time < endTime);
        } else {
            return (time >= startTime && time < endTime);
        }
    }

    QJsonObject serialize() const
    {
        QJsonObject data;
        data.insert("isActive", isActive);
        data.insert("identifier", identifier);
        data.insert("index", index);
        data.insert("relativePosition", relativePosition);
        data.insert("startTime", startTime.toString());
        data.insert("endTime", endTime.toString());

               // Serialize vector<WallpaperData>
        QJsonArray wallpaperDataArray;
        for (const auto& wallpaper : wallpaperData) {
            QJsonObject wallpaperObject = wallpaper.serialize(); // Assuming WallpaperData has a serialize method
            wallpaperDataArray.append(wallpaperObject);
        }
        data.insert("wallpaperData", wallpaperDataArray);

               // Serialize vector<std::shared_ptr<ScreenPlayWallpaper>>
               // QJsonArray activeWallpaperArray;
               // for (const auto& wallpaper : activeWallpaperList) {
               //     QJsonObject wallpaperObject = wallpaper->serialize();  // Assuming ScreenPlayWallpaper has a serialize method
               //     activeWallpaperArray.append(wallpaperObject);
               // }
               // data.insert("activeWallpaperList", activeWallpaperArray);

        return data;
    }
};
}
