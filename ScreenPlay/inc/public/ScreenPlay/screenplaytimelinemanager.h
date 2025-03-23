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
    Q_PROPERTY(int activeTimelineIndex READ activeTimelineIndex WRITE setActiveTimelineIndex NOTIFY activeTimelineIndexChanged FINAL)

public:
    explicit ScreenPlayTimelineManager(QObject* parent = nullptr);

    bool addTimelineFromSettings(const QJsonObject& timelineObj);
    bool moveTimelineAt(const int index, const QString identifier, const float relativeLinePosition, QString positionTimeString);
    bool addTimelineAt(const int index, const float reltiaveLinePosition, QString identifier);
    QCoro::Task<Result> removeTimelineAt(const int index);

    QCoro::Task<void> startup();
    std::shared_ptr<WallpaperTimelineSection> findStartingOrActiveWallpaperTimelineSection();
    std::shared_ptr<WallpaperTimelineSection> findActiveWallpaperTimelineSection();
    std::shared_ptr<WallpaperTimelineSection> findTimelineSection(
        const int timelineIndex,
        const QString sectionIdentifier);
    QCoro::Task<Result> removeAllTimlineSections();
    QCoro::Task<Result> removeAllWallpaperFromActiveTimlineSections();
    QCoro::Task<Result> removeWallpaperAt(
        const int timelineIndex,
        const QString sectionIdentifier,
        const int monitorIndex);
    QCoro::Task<Result> setWallpaperAtMonitorTimelineIndex(
        WallpaperData wallpaperData,
        const int timelineIndex,
        const QString& sectionIdentifier,
        const QVector<int> monitorIndex);
    QCoro::Task<Result> setValueAtMonitorTimelineIndex(
        const int monitorIndex,
        const int timelineIndex,
        const QString sectionIdentifier,
        const QString& key,
        const QVariant& value,
        const QString& category);
    QJsonArray timelineSections();
    QJsonArray timelineWallpaperList();

    QCoro::Task<Result> activateTimeline(const int timelineIndex, const QString timelineIdentifier);
    QCoro::Task<Result> stopTimelineAndClearWallpaperData(const int timelineIndex, const QString timelineIdentifier, const bool disableTimeline = true);
    QCoro::Task<Result> stopWallpaper(const int timelineIndex, const QString timelineIdentifier, const QVector<int> monitorIndex);
    QCoro::Task<Result> removeWallpaper(const int timelineIndex, const QString timelineIdentifier, const QVector<int> monitorIndex);
    QCoro::Task<Result> startWallpaper(const int timelineIndex, const QString timelineIdentifier, const QVector<int> monitorIndex);
    QCoro::Task<Result> startWallpaper(std::shared_ptr<ScreenPlayWallpaper> screenPlayWallpaper);

    void setGlobalVariables(const std::shared_ptr<GlobalVariables>& globalVariables);
    void setSettings(const std::shared_ptr<Settings>& settings);
    void setMonitorListModel(const std::shared_ptr<MonitorListModel>& monitorListModel);
    int selectedTimelineIndex() const;
    int activeTimelineIndex() const;
    void printTimelines() const;
    void validateTimelineSections() const;
    void sortAndUpdateIndices();
    const std::vector<WallpaperData> wallpaperData() const;

public slots:
    void setSelectedTimelineIndex(int selectedTimelineIndex);
    void updateMonitorListModelData(const int selectedTimelineIndex);

private slots:
    QCoro::Task<void> checkActiveWallpaperTimeline();
    void setActiveTimelineIndex(int activeTimelineIndex);

signals:
    void requestSaveProfiles();
    void activeWallpaperCountChanged(const int count);
    void selectedTimelineIndexChanged(int selectedTimelineIndex);
    void activeTimelineIndexChanged(int activeTimelineIndex);

private:
    QCoro::Task<Result> setWallpaperAtMonitorActiveTimelineIndex(
        WallpaperData wallpaperData,
        const int timelineIndex,
        const QString& sectionIdentifier,
        const QVector<int> monitorIndex);
    QCoro::Task<Result> setWallpaperAtMonitorInactiveTimelineIndex(
        WallpaperData wallpaperData,
        const int timelineIndex,
        const QString& sectionIdentifier,
        const QVector<int> monitorIndex);
    std::shared_ptr<WallpaperTimelineSection> findTimelineSectionForCurrentTime();

    std::optional<std::shared_ptr<WallpaperTimelineSection>> wallpaperSection(
        const int timelineIndex,
        const QString& sectionIdentifier);
    std::optional<std::shared_ptr<ScreenPlayWallpaper>> screenPlayWallpaperAt(
        const int timelineIndex,
        const QString timelineIdentifier,
        const QVector<int> monitorIndex);
    QCoro::Task<Result> startAllWallpaperAtTimelineIndex(const int timelineIndex);

private:
    QVector<std::shared_ptr<WallpaperTimelineSection>> m_wallpaperTimelineSectionsList;
    std::shared_ptr<MonitorListModel> m_monitorListModel;

    // We use a 24 hour system
    const QString m_timelineTimeFormat = "hh:mm:ss";
    QTimer m_contentTimer;
    std::shared_ptr<GlobalVariables> m_globalVariables;
    std::shared_ptr<Settings> m_settings;
    int m_selectedTimelineIndex { 0 };
    int m_activeTimelineIndex { -1 };
    Util m_util;
};
}
