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

namespace ScreenPlay {
class ScreenPlayWallpaper;
// Represents one line in the UI. ScreenPlayManager has a list of
// WallpaperTimeline. Only the active timeline section has
// a filled vector of ScreenPlayWallpaper
class WallpaperTimelineSection : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")
    Q_CLASSINFO("RegisterEnumClassesUnscoped", "false")
public:
    enum class Status {
        Inactive,
        Starting,
        Closing,
        Active,
    };
    Q_ENUM(Status)
    Status status = Status::Inactive;
    QString identifier;
    int index = 0; // Needed to check
    float relativePosition = 0.0f;
    QTime startTime;
    QTime endTime;
    // Data from the profiles.json that we need when we
    // enable this section of the pipeline. We keep a copy
    // here when this timeline needs to become active
    std::vector<WallpaperData> wallpaperDataList;
    // All active wallpaper.
    std::vector<std::shared_ptr<ScreenPlayWallpaper>> activeWallpaperList;

    std::shared_ptr<GlobalVariables> globalVariables;
    std::shared_ptr<Settings> settings;

public:
    // Check if currentTime falls within the timeline section
    bool containsTime(const QTime& time) const;
    QJsonObject serialize() const;
    bool activateTimeline();

    QCoro::Task<bool> deactivateTimeline();
    QCoro::Task<bool> removeWallpaper(const int monitorIndex);
private slots:
    void updateActiveWallpaperCounter();

private:
    std::optional<std::shared_ptr<ScreenPlayWallpaper>> wallpaperByMonitorIndex(const int index);
signals:
    void requestSaveProfiles();
    void activeWallpaperCountChanged(const int count);
};
}
