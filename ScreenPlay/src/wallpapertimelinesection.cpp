// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#include "ScreenPlay/wallpapertimelinesection.h"
#include "ScreenPlay/screenplaywallpaper.h"
#include "ScreenPlay/wallpaperdata.h"
#include "ScreenPlayCore/util.h"
#include "wallpapertimelinesection.h"
#include <QDebug>
#include <QDir>
#include <QFileInfoList>
#include <QGuiApplication>
#include <QObject>
#include <ranges>

namespace ScreenPlay {

const std::vector<WallpaperData> WallpaperTimelineSection::wallpaperData() const
{
    std::vector<WallpaperData> wallpaperData;
    for (auto& screenplayWallpaper : wallpaperList) {
        wallpaperData.push_back(screenplayWallpaper->wallpaperData());
    }
    return wallpaperData;
}

bool WallpaperTimelineSection::containsTime(const QTime& time) const
{
    if (endTime < startTime) { // Timeline spans midnight
        return (time >= startTime || time < endTime);
    } else {
        return (time >= startTime && time < endTime);
    }
}

std::optional<WallpaperData> WallpaperTimelineSection::getWallpaperDataForMonitor(const int monitorIndex) const
{
    for (const auto& wallpaper : wallpaperData()) {
        const auto& monitors = wallpaper.monitors();
        if (std::find(monitors.begin(), monitors.end(), monitorIndex) != monitors.end()) {
            return wallpaper;
        }
    }
    return std::nullopt;
}

std::optional<std::shared_ptr<ScreenPlayWallpaper>> WallpaperTimelineSection::screenPlayWallpaperByMonitorIndex(const int monitorIndex) const // TODO replace with QVector<int>
{
    for (const auto& wallpaper : wallpaperList) {
        const auto monitors = wallpaper->monitors();
        if (std::find(monitors.begin(), monitors.end(), monitorIndex) != monitors.end()) {
            return wallpaper;
        }
    }
    return {};
}

std::optional<std::shared_ptr<ScreenPlayWallpaper>> WallpaperTimelineSection::takeScreenPlayWallpaperByMonitorIndex(const QVector<int>& monitors)
{
    for (auto it = wallpaperList.begin(); it != wallpaperList.end(); ++it) {
        const auto wallpaperMonitors = (*it)->monitors();
        for (const auto& monitor : monitors) {
            if (std::find(wallpaperMonitors.begin(), wallpaperMonitors.end(), monitor) != wallpaperMonitors.end()) {
                auto wallpaper = *it;
                wallpaperList.erase(it);
                return wallpaper;
            }
        }
    }
    return {};
}

bool WallpaperTimelineSection::replaceScreenPlayWallpaperAtMonitorIndex(const QVector<int>& monitors, std::shared_ptr<ScreenPlayWallpaper> screenPlayWallpaper)
{
    for (auto it = wallpaperList.begin(); it != wallpaperList.end(); ++it) {
        const auto wallpaperMonitors = (*it)->monitors();
        for (const auto& monitor : monitors) {
            if (std::find(wallpaperMonitors.begin(), wallpaperMonitors.end(), monitor) != wallpaperMonitors.end()) {
                wallpaperList.erase(it);
                wallpaperList.push_back(screenPlayWallpaper);
                return true;
            }
        }
    }
    return false;
}

bool WallpaperTimelineSection::init(const QJsonArray wallpaperConfigList)
{
    for (const auto& wallpaperConfig : wallpaperConfigList) {
        auto wallpaperDataOpt = WallpaperData::loadTimelineWallpaperConfig(wallpaperConfig.toObject());
        if (!wallpaperDataOpt.has_value()) {
            continue;
        }
        auto wallpaperData = wallpaperDataOpt.value();
        addWallpaper(wallpaperData);
    }
    return true;
}

QJsonObject WallpaperTimelineSection::serialize() const
{
    QJsonObject data;
    data.insert("identifier", identifier);
    data.insert("index", index);
    data.insert("relativePosition", relativePosition);
    data.insert("startTime", startTime.toString());
    data.insert("endTime", endTime.toString());

    QJsonArray wallpaperDataArray;
    for (const auto& wallpaper : wallpaperData()) {
        QJsonObject wallpaperObject = wallpaper.serialize();
        wallpaperDataArray.append(wallpaperObject);
    }
    data.insert("wallpaperData", wallpaperDataArray);

    return data;
}

std::shared_ptr<ScreenPlayWallpaper> WallpaperTimelineSection::addWallpaper(const WallpaperData wallpaperData)
{
    // This id is used for IPC identification to send commands
    // between ScreenPlay and the wallpaper
    const QString appID = Util().generateRandomString(8);
    qInfo() << "Add wallpaper" << wallpaperData.absolutePath() << appID;

    auto screenPlayWallpaper = std::make_shared<ScreenPlayWallpaper>(
        globalVariables,
        appID,
        wallpaperData,
        settings);

    QObject::connect(screenPlayWallpaper.get(), &ScreenPlayWallpaper::stateChanged, this, &WallpaperTimelineSection::requestUpdateMonitorListModel);
    QObject::connect(screenPlayWallpaper.get(), &ScreenPlayWallpaper::requestClose, this, [this]() {
        // , &ScreenPlayManager::removeWallpaper); TODO
    });
    QObject::connect(screenPlayWallpaper.get(), &ScreenPlayWallpaper::isConnectedChanged, this, [this]() {
        updateActiveWallpaperCounter();
    });
    wallpaperList.push_back(screenPlayWallpaper);
    return screenPlayWallpaper;
}

void WallpaperTimelineSection::updateActiveWallpaperCounter()
{
    quint64 activeWallpaperCount = 0;
    for (const auto& screenPlayWallpaper : wallpaperList) {
        if (screenPlayWallpaper->isConnected())
            activeWallpaperCount++;
    }
    emit activeWallpaperCountChanged(activeWallpaperCount);
}
}
