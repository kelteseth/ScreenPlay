// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#pragma once

#include <QObject>
#include <QTimer>
#include <memory>

#include "ScreenPlay/monitorlistmodel.h"
#include "ScreenPlay/wallpapertimelinesection.h"

namespace ScreenPlay {

class ScreenPlayTimelineManager : public QObject {
    Q_OBJECT

public:
    explicit ScreenPlayTimelineManager(QObject* parent = nullptr);

    std::optional<std::shared_ptr<WallpaperTimelineSection>> activeWallpaperSectionByAppID(const QString& appID);
    std::shared_ptr<WallpaperTimelineSection> findActiveWallpaperTimelineSection();
    std::shared_ptr<WallpaperTimelineSection> findTimelineForCurrentTime();

    void startup();
    bool addTimelineFromSettings(const QJsonObject& timelineObj);
    bool deactivateCurrentTimeline();
    bool moveTimelineAt(const int index, const QString identifier, const float relativeLinePosition, QString positionTimeString);
    bool startTimeline();
    bool addTimelineAt(const int index, const float reltiaveLinePosition, QString identifier);
    bool removeTimelineAt(const int index);
    QCoro::Task<bool> removeAllTimlineSections();
    QCoro::Task<bool> removeAllWallpaperFromActiveTimlineSections();
    QCoro::Task<bool> removeWallpaperAt(
        const int timelineIndex,
        const QString timelineIdentifier,
        const int monitorIndex);
    void updateIndices();
    void printTimelines();
    QCoro::Task<bool> setWallpaperAtTimelineIndex(
        WallpaperData wallpaperData,
        const int timelineIndex,
        const QString& identifier);
    QJsonArray initialSectionsList();
    QJsonArray timelineWallpaperList();
    void setGlobalVariables(const std::shared_ptr<GlobalVariables>& globalVariables);
    void setSettings(const std::shared_ptr<Settings>& settings);
    void setMonitorListModel(const std::shared_ptr<MonitorListModel>& monitorListModel);
    void updateMonitorListModelData(const int selectedTimelineIndex);

private slots:
    void checkActiveWallpaperTimeline();

signals:
    void requestSaveProfiles();
    void activeWallpaperCountChanged(const int count);

private:
    std::optional<std::shared_ptr<WallpaperTimelineSection>> activeWallpaperSection(const int timelineIndex, const QString timelineIdentifier);

private:
    QVector<std::shared_ptr<WallpaperTimelineSection>> m_wallpaperTimelineSectionsList;
    std::shared_ptr<MonitorListModel> m_monitorListModel;

    // We use a 24 hour system
    const QString m_timelineTimeFormat = "hh:mm:ss";
    QTimer m_contentTimer;
    std::shared_ptr<GlobalVariables> m_globalVariables;
    std::shared_ptr<Settings> m_settings;
};
}
