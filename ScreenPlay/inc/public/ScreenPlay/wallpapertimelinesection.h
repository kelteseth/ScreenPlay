// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#pragma once
#include <QCoro/QCoroTask>
#include <QCoro/QCoroTimer>
#include <QJsonObject>
#include <QObject>
#include <QString>
#include <QTimer>
#include <expected>

#include "ScreenPlay/globalvariables.h"
#include "ScreenPlay/settings.h"
#include "ScreenPlay/wallpaperdata.h"
#include "ScreenPlayCore/util.h"

namespace ScreenPlay {
class ScreenPlayWallpaper;
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

    enum class InitError {
        None,
        WallpaperDataLoadFailed
    };
    Q_ENUM(InitError)

    static QString initErrorToString(InitError error);
    State state = State::Inactive;
    QString identifier;
    int index = 0; // Needed to check
    float relativePosition = 0.0f;
    QTime startTime;
    QTime endTime;
    const std::vector<WallpaperData> wallpaperData() const;
    std::vector<std::shared_ptr<ScreenPlayWallpaper>> wallpaperList;

    std::shared_ptr<GlobalVariables> globalVariables;
    std::shared_ptr<Settings> settings;

    // Check if currentTime falls within the timeline section
    bool containsTime(const QTime& time) const;
    std::expected<bool, InitError> init(const QJsonArray wallpaperConfig);
    std::shared_ptr<ScreenPlayWallpaper> addWallpaper(const WallpaperData wallpaperData);
    std::optional<WallpaperData> getWallpaperDataForMonitor(const int monitorIndex) const;
    std::optional<std::shared_ptr<ScreenPlayWallpaper>> screenPlayWallpaperByMonitorIndex(const int monitorIndex) const;
    std::optional<std::shared_ptr<ScreenPlayWallpaper>> takeScreenPlayWallpaperByMonitorIndex(const QVector<int>& monitors);
    bool replaceScreenPlayWallpaperAtMonitorIndex(const QVector<int>& monitors, std::shared_ptr<ScreenPlayWallpaper> screenPlayWallpaper);
    QJsonObject serialize() const;

signals:
    void requestSaveProfiles();
    void requestUpdateMonitorListModel();
    void activeWallpaperCountChanged(const int count);
    void wallpaperRestartFailed(const QString& appID, const QString& message);

public slots:
    void updateActiveWallpaperCounter();

private:
    Util m_util;
};
}
