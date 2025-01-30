// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#pragma once

#include <QObject>
#include <QTimer>
#include <memory>

#include "ScreenPlay/monitorlistmodel.h"
#include "ScreenPlay/wallpapertimelinesection.h"
#include "ScreenPlayCore/util.h"

namespace ScreenPlay {

class ScreenPlayTimelineManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(int selectedTimelineIndex READ selectedTimelineIndex WRITE setSelectedTimelineIndex NOTIFY selectedTimelineIndexChanged FINAL)

public:
    explicit ScreenPlayTimelineManager(QObject* parent = nullptr);

    bool addTimelineFromSettings(const QJsonObject& timelineObj);
    bool moveTimelineAt(const int index, const QString identifier, const float relativeLinePosition, QString positionTimeString);
    bool addTimelineAt(const int index, const float reltiaveLinePosition, QString identifier);
    bool removeTimelineAt(const int index);

    QCoro::Task<void> startup();
    std::shared_ptr<WallpaperTimelineSection> findStartingOrActiveWallpaperTimelineSection();
    std::shared_ptr<WallpaperTimelineSection> findActiveWallpaperTimelineSection();
    std::shared_ptr<WallpaperTimelineSection> findTimelineSection(
        const int timelineIndex,
        const QString sectionIdentifier);
    QCoro::Task<bool> removeAllTimlineSections();
    QCoro::Task<bool> removeAllWallpaperFromActiveTimlineSections();
    QCoro::Task<bool> removeWallpaperAt(
        const int timelineIndex,
        const QString sectionIdentifier,
        const int monitorIndex);
    QCoro::Task<bool> setWallpaperAtMonitorTimelineIndex(
        WallpaperData wallpaperData,
        const int timelineIndex,
        const QString& sectionIdentifier,
        const QVector<int> monitorIndex);
    QCoro::Task<bool> setValueAtMonitorTimelineIndex(const int monitorIndex,
        const int timelineIndex,
        const QString sectionIdentifier,
        const QString& key,
        const QVariant& value,
        const QString& category);
    QJsonArray timelineSections();
    QJsonArray timelineWallpaperList();

    QCoro::Task<bool> activateTimeline(const int timelineIndex, const QString timelineIdentifier);
    // QCoro::Task<bool> activateTimeline(std::vector<std::shared_ptr<ScreenPlayWallpaper>> currentActiveWallpaperList);
    QCoro::Task<bool> deactivateTimeline(const int timelineIndex, const QString timelineIdentifier);
    QCoro::Task<bool> stopWallpaper(const int timelineIndex, const QString timelineIdentifier, const QVector<int> monitorIndex);
    QCoro::Task<bool> removeWallpaper(const int timelineIndex, const QString timelineIdentifier, const QVector<int> monitorIndex);
    // QCoro::Task<bool> removeWallpaper(std::shared_ptr<ScreenPlayWallpaper> screenPlayWallpaper);
    QCoro::Task<bool> startWallpaper(const int timelineIndex, const QString timelineIdentifier, const QVector<int> monitorIndex);
    QCoro::Task<bool> startWallpaper(std::shared_ptr<ScreenPlayWallpaper> screenPlayWallpaper);

    void setGlobalVariables(const std::shared_ptr<GlobalVariables>& globalVariables);
    void setSettings(const std::shared_ptr<Settings>& settings);
    void setMonitorListModel(const std::shared_ptr<MonitorListModel>& monitorListModel);
    int selectedTimelineIndex() const;
    void setSelectedTimelineIndex(int selectedTimelineIndex);
    void printTimelines() const;
    void validateTimelineSections() const;
    void sortAndUpdateIndices();
    const std::vector<WallpaperData> wallpaperData() const;

public slots:
    void updateMonitorListModelData(const int selectedTimelineIndex);

private slots:
    QCoro::Task<void> checkActiveWallpaperTimeline();

signals:
    void requestSaveProfiles();
    void activeWallpaperCountChanged(const int count);
    void selectedTimelineIndexChanged(int selectedTimelineIndex);

private:
    QCoro::Task<bool> setWallpaperAtMonitorActiveTimelineIndex(
        WallpaperData wallpaperData,
        const int timelineIndex,
        const QString& sectionIdentifier,
        const QVector<int> monitorIndex);
    QCoro::Task<bool> setWallpaperAtMonitorInactiveTimelineIndex(
        WallpaperData wallpaperData,
        const int timelineIndex,
        const QString& sectionIdentifier,
        const QVector<int> monitorIndex);
    // bool replaceRunningWallpaper(std::shared_ptr<ScreenPlayWallpaper> runningWallpaper, WallpaperData wallpaperData);
    std::shared_ptr<WallpaperTimelineSection> findTimelineSectionForCurrentTime();
    QCoro::Task<bool> updateCurrentlyActiveTimeline(WallpaperData wallpaperData,
        const int timelineIndex,
        const QString& sectionIdentifier);
    std::optional<std::shared_ptr<WallpaperTimelineSection>> wallpaperSection(
        const int timelineIndex,
        const QString& sectionIdentifier);
    std::optional<std::shared_ptr<ScreenPlayWallpaper>> screenPlayWallpaperAt(const int timelineIndex, const QString timelineIdentifier, const QVector<int> monitorIndex);

private:
    QVector<std::shared_ptr<WallpaperTimelineSection>> m_wallpaperTimelineSectionsList;
    std::shared_ptr<MonitorListModel> m_monitorListModel;

    // We use a 24 hour system
    const QString m_timelineTimeFormat = "hh:mm:ss";
    QTimer m_contentTimer;
    std::shared_ptr<GlobalVariables> m_globalVariables;
    std::shared_ptr<Settings> m_settings;
    int m_selectedTimelineIndex { 0 };
    Util m_util;
};
}
