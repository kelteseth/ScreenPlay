// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#pragma once

#include <QHash>
#include <QLocalServer>
#include <QObject>
#include <QPoint>
#include <QPointer>
#include <memory>

#include "ScreenPlay/globalvariables.h"
#include "ScreenPlay/monitorlistmodel.h"
#include "ScreenPlay/projectsettingslistmodel.h"
#include "ScreenPlay/screenplaytimelinemanager.h"
#include "ScreenPlay/screenplaywidget.h"
#include "ScreenPlay/settings.h"
#include "ScreenPlayCore/util.h"

namespace ScreenPlay {
class ErrorManager;

class ScreenPlayManager : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")

    Q_PROPERTY(int activeWallpaperCounter READ activeWallpaperCounter WRITE setActiveWallpaperCounter NOTIFY activeWallpaperCounterChanged FINAL)
    Q_PROPERTY(int activeWidgetsCounter READ activeWidgetsCounter WRITE setActiveWidgetsCounter NOTIFY activeWidgetsCounterChanged FINAL)
    Q_PROPERTY(int selectedTimelineIndex READ selectedTimelineIndex WRITE setSelectedTimelineIndex NOTIFY selectedTimelineIndexChanged FINAL)
    Q_PROPERTY(int activeTimelineIndex READ activeTimelineIndex WRITE setActiveTimelineIndex NOTIFY activeTimelineIndexChanged FINAL)
    Q_PROPERTY(int timelineSectionCount READ timelineSectionCount NOTIFY timelineSectionCountChanged FINAL)
    Q_PROPERTY(QJsonArray runningWallpapers READ runningWallpapers NOTIFY runningWallpapersChanged FINAL)
    Q_PROPERTY(bool isMuted READ isMuted WRITE setIsMuted NOTIFY isMutedChanged FINAL)
    Q_PROPERTY(bool isPaused READ isPaused WRITE setIsPaused NOTIFY isPausedChanged FINAL)
    Q_PROPERTY(ScreenPlay::ProjectSettingsListModel* projectSettingsListModel READ projectSettingsListModel WRITE setProjectSettingsListModel NOTIFY projectSettingsListModelChanged FINAL)

public:
    explicit ScreenPlayManager(QObject* parent = nullptr);
    void init(
        const std::shared_ptr<GlobalVariables>& globalVariables,
        const std::shared_ptr<MonitorListModel>& mlm,
        const std::shared_ptr<Settings>& settings,
        const std::shared_ptr<ErrorManager>& errorManager);

    QCoro::Task<Result> shutdown();

    Q_INVOKABLE QCoro::QmlTask removeAllRunningWallpapers(bool saveToProfile = false);
    Q_INVOKABLE bool removeAllRunningWidgets(bool saveToProfile = false);
    Q_INVOKABLE WallpaperData getWallpaperData(const int monitorIndex,
        const int timelineIndex,
        const QString sectionIdentifier);

    Q_INVOKABLE bool moveTimelineAt(
        const int timelineIndex,
        const QString identifier,
        const float relativePosition,
        QString positionTimeString);
    Q_INVOKABLE bool addTimelineAt(
        const int timelineIndex,
        const float reltiaveLinePosition,
        QString identifier);
    Q_INVOKABLE QCoro::QmlTask removeTimelineAt(const int timelineIndex, const QString identifier);
    Q_INVOKABLE QJsonArray timelineSections();
    Q_INVOKABLE void setWallpaperFrameStats(const bool visible);
    Q_INVOKABLE void setWallpaperFpsLimit(const int fps);
    QJsonArray runningWallpapers() const;
    Q_INVOKABLE QCoro::QmlTask removeAllTimlineSections();
    Q_INVOKABLE QCoro::QmlTask removeWallpaperAt(
        const int timelineIndex,
        const QString sectionIdentifier,
        const int monitorIndex);
    Q_INVOKABLE QCoro::QmlTask setWallpaperAtMonitorTimelineIndex(
        const QString& absoluteStoragePath,
        const QVector<int>& monitorIndex,
        const int timelineIndex,
        const QString& identifier,
        const bool saveToProfilesConfigFile);

    Q_INVOKABLE bool startWidget(
        const QString& absoluteStoragePath,
        const QPoint& position,
        const QJsonObject& properties,
        const bool saveToProfilesConfigFile);

    Q_INVOKABLE bool projectSettingsAtMonitorIndex(
        const int monitorIndex,
        const int timelineIndex,
        const QString& sectionIdentifier);
    Q_INVOKABLE QCoro::QmlTask setValueAtMonitorTimelineIndex(
        const int monitorIndex,
        const int timelineIndex,
        const QString& sectionIdentifier,
        const QString& key,
        const QVariant& value,
        const QString& category);
    Q_INVOKABLE QCoro::QmlTask setWallpaperFillModeAtMonitorIndex(
        const int monitorIndex,
        const int timelineIndex,
        const QString sectionIdentifier,
        const int fillmode);
    Q_INVOKABLE bool setAllWallpaperValue(const QString& key, const QVariant& value);
    Q_INVOKABLE int activeTimelineIndex();
    Q_INVOKABLE void requestSaveProfiles();
    int activeWallpaperCounter() const { return m_activeWallpaperCounter; }
    int activeWidgetsCounter() const { return m_activeWidgetsCounter; }
    int selectedTimelineIndex() const { return m_selectedTimelineIndex; }
    int activeTimelineIndex() const { return m_activeTimelineIndex; }
    bool isMuted() const { return m_isMuted; }
    bool isPaused() const { return m_isPaused; }
    int timelineSectionCount() const { return m_screenPlayTimelineManager.timelineSectionCount(); }

    ProjectSettingsListModel* projectSettingsListModel() const;
    void setProjectSettingsListModel(ProjectSettingsListModel* newProjectSettingsListModel);

public slots:
    void setSelectedTimelineIndex(int selectedTimelineIndex);
    void setIsMuted(bool isMuted);
    void setIsPaused(bool isPaused);

signals:
    void activeWallpaperCounterChanged(int activeWallpaperCounter);
    void activeWidgetsCounterChanged(int activeWidgetsCounter);
    void selectedTimelineIndexChanged(int selectedTimelineIndex);
    void activeTimelineIndexChanged(int activeTimelineIndex);
    void timelineSectionCountChanged(int count);
    void runningWallpapersChanged();
    void isMutedChanged(bool isMuted);
    void isPausedChanged(bool isPaused);
    void monitorConfigurationChanged();
    void requestRaise();
    void profilesSaved();
    void printQmlTimeline();
    void notifyUiReloadTimelinePreviewImage();
    void projectSettingsListModelChanged(ScreenPlay::ProjectSettingsListModel* projectSettingsListModel);

private slots:
    bool saveProfiles();
    void newConnection();

    /*!
        \brief Adds \a link to the appID registry used by newConnection. Must
               run before the process is started so a fast handshake cannot
               overtake the registration.
    */
    void registerLink(ScreenPlayExternalProcess* link);
    void setActiveWallpaperCounter(int activeWallpaperCounter);
    void setActiveWidgetsCounter(int activeWidgetsCounter);
    void setActiveTimelineIndex(int activeTimelineIndex);

private:
    bool loadProfiles();
    bool checkIsAnotherScreenPlayInstanceRunning();
    bool removeWidget(const QString& appID);
    bool loadWidgetConfig(const QJsonObject& widget);

    // Coroutine backends for the QmlTask wrappers above. They are member
    // functions taking arguments by value on purpose: a capturing lambda
    // coroutine stores its captures in the closure object, which dies at the
    // end of the wrapping full expression - any capture access after the
    // first co_await would be a dangling read. Member coroutine parameters
    // live in the coroutine frame instead.
    QCoro::Task<Result> setWallpaperAtMonitorTimelineIndexTask(
        QString absoluteStoragePath,
        QVector<int> monitorIndex,
        int timelineIndex,
        QString identifier,
        bool saveToProfilesConfigFile);
    QCoro::Task<Result> removeAllRunningWallpapersTask(bool saveToProfile);
    QCoro::Task<Result> removeWallpaperAtTask(int timelineIndex, QString sectionIdentifier, int monitorIndex);
    QCoro::Task<Result> setValueAtMonitorTimelineIndexTask(
        int monitorIndex,
        int timelineIndex,
        QString sectionIdentifier,
        QString key,
        QVariant value,
        QString category);
    QCoro::Task<Result> removeAllTimlineSectionsTask();
    QCoro::Task<Result> removeTimelineAtTask(int timelineIndex, QString identifier);

private:
    std::shared_ptr<GlobalVariables> m_globalVariables;
    std::shared_ptr<MonitorListModel> m_monitorListModel;
    std::shared_ptr<Settings> m_settings;
    std::shared_ptr<ErrorManager> m_errorManager;
    std::unique_ptr<QLocalServer> m_server;
    std::shared_ptr<ProjectSettingsListModel> m_projectSettingsListModel;
    QVector<std::shared_ptr<ScreenPlayWidget>> m_screenPlayWidgets;
    std::vector<std::unique_ptr<SDKConnection>> m_unconnectedClients;

    /*!
        \brief Every live wallpaper and widget, keyed by the appID its process
               was launched with. This is the ONLY thing newConnection needs to
               route an incoming socket: it no longer asks the timeline which
               section is starting, so a wallpaper that connects late (slow
               Godot/HTML start, or after its section already became Active)
               still finds its owner instead of being dropped as an orphan.

        Entries live for the lifetime of the link, not just until the first
        handshake: a crash-restart relaunches the same appID and must be
        matchable again. QPointer plus the destroyed() connection in
        registerLink() keeps the map free of dangling entries.
    */
    QHash<QString, QPointer<ScreenPlayExternalProcess>> m_links;
    ScreenPlayTimelineManager m_screenPlayTimelineManager;

    QTimer m_saveLimiter;
    Util m_util;

    int m_activeWallpaperCounter { 0 };
    int m_activeWidgetsCounter { 0 };
    int m_selectedTimelineIndex { 0 };
    int m_activeTimelineIndex { -1 };
    bool m_isMuted { false };
    bool m_isPaused { false };
};
}
