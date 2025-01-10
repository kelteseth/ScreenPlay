// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#pragma once
#include <QCoro/QCoroTask>
#include <QCoro/QCoroTimer>
#include <QJsonObject>
#include <QObject>
#include <QString>
#include <QTimer>

#include "ScreenPlay/globalvariables.h"
#include "ScreenPlay/settings.h"
#include "ScreenPlay/wallpaperdata.h"
#include "ScreenPlayCore/util.h"

namespace ScreenPlay {
class ScreenPlayWallpaper;
// Represents one timeline section line in the UI. ScreenPlayManager has a list of
// WallpaperTimeline. Only the active timeline section has
// a filled vector of ScreenPlayWallpaper
class WallpaperTimelineSection : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")
    Q_CLASSINFO("RegisterEnumClassesUnscoped", "false")
public:
    enum class State {
        Inactive, // Current time is outside startTime and endTime
        Starting, // Timeline is initializing wallpapers
        Active, // Running normally
        Closing, // Shutting down wallpapers
        Failed, // Failed to start/crashed - needs recovery
    };
    Q_ENUM(State)
    State state = State::Inactive;
    QString identifier;
    int index = 0; // Needed to check
    float relativePosition = 0.0f;
    QTime startTime;
    QTime endTime;

    // Data from the profiles.json that we need when we
    // enable this section of the pipeline. We keep a copy
    // here when this timeline needs to become active
    //
    // Note: We essentially have a copy of WallpaperData here and in the
    //       running activeWallpaperList. Should we make it a share_ptr?
    std::vector<WallpaperData> wallpaperDataList;
    // All active wallpaper.
    std::vector<std::shared_ptr<ScreenPlayWallpaper>> activeWallpaperList;

    std::shared_ptr<GlobalVariables> globalVariables;
    std::shared_ptr<Settings> settings;

    // Check if currentTime falls within the timeline section
    bool containsTime(const QTime& time) const;
    bool containsMonitor(const int monitor) const;
    std::optional<WallpaperData> getWallpaperDataForMonitor(const int monitorIndex) const;
    std::optional<std::shared_ptr<ScreenPlayWallpaper>> getActiveWallpaperForMonitor(const int monitorIndex) const;
    QJsonObject serialize() const;
    QCoro::Task<bool> activateTimeline();
    QCoro::Task<bool> activateTimeline(std::vector<std::shared_ptr<ScreenPlayWallpaper>> currentActiveWallpaperList);
    QCoro::Task<bool> deactivateTimeline();
    QCoro::Task<bool> stopWallpaper(const int monitorIndex);
    QCoro::Task<bool> removeWallpaper(const int monitorIndex);
    QCoro::Task<bool> startWallpaper(const WallpaperData wallpaperData);

signals:
    void requestSaveProfiles();
    void requestUpdateMonitorListModel();
    void activeWallpaperCountChanged(const int count);

private slots:
    void updateActiveWallpaperCounter();

private:
    Util m_util;
};
}
