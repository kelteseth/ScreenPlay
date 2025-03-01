// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#pragma once

#include <QLocalServer>
#include <QObject>
#include <QPoint>
#include <memory>

#include "ScreenPlay/globalvariables.h"
#include "ScreenPlay/monitorlistmodel.h"
#include "ScreenPlay/projectsettingslistmodel.h"
#include "ScreenPlay/screenplaytimelinemanager.h"
#include "ScreenPlay/screenplaywallpaper.h"
#include "ScreenPlay/screenplaywidget.h"
#include "ScreenPlay/settings.h"
#include "ScreenPlayCore/util.h"

namespace ScreenPlay {

class ScreenPlayManager : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")

    Q_PROPERTY(int activeWallpaperCounter READ activeWallpaperCounter WRITE setActiveWallpaperCounter NOTIFY activeWallpaperCounterChanged FINAL)
    Q_PROPERTY(int activeWidgetsCounter READ activeWidgetsCounter WRITE setActiveWidgetsCounter NOTIFY activeWidgetsCounterChanged FINAL)
    Q_PROPERTY(int selectedTimelineIndex READ selectedTimelineIndex WRITE setSelectedTimelineIndex NOTIFY selectedTimelineIndexChanged FINAL)
    Q_PROPERTY(int activeTimelineIndex READ activeTimelineIndex WRITE setActiveTimelineIndex NOTIFY activeTimelineIndexChanged FINAL)
    Q_PROPERTY(ProjectSettingsListModel* projectSettingsListModel READ projectSettingsListModel WRITE setProjectSettingsListModel NOTIFY projectSettingsListModelChanged FINAL)

public:
    explicit ScreenPlayManager(QObject* parent = nullptr);

    void init(
        const std::shared_ptr<GlobalVariables>& globalVariables,
        const std::shared_ptr<MonitorListModel>& mlm,
        const std::shared_ptr<Settings>& settings);

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
    Q_INVOKABLE QCoro::QmlTask removeTimelineAt(const int timelineIndex);
    Q_INVOKABLE QJsonArray timelineSections();
    Q_INVOKABLE QCoro::QmlTask removeAllTimlineSections();
    Q_INVOKABLE QCoro::QmlTask removeWallpaperAt(
        const int timelineIndex,
        const QString sectionIdentifier,
        const int monitorIndex);
    Q_INVOKABLE QCoro::QmlTask setWallpaperAtMonitorTimelineIndex(
        const ScreenPlay::ContentTypes::InstalledType type,
        const QString& absolutePath,
        const QString& previewImage,
        const QString& file,
        const QString& title,
        const QVector<int>& monitorIndex,
        const int timelineIndex,
        const QString& identifier,
        const bool saveToProfilesConfigFile);

    Q_INVOKABLE bool startWidget(
        // moc needs full enum namespace info see QTBUG-58454
        const ScreenPlay::ContentTypes::InstalledType type,
        const QPoint& position,
        const QString& absoluteStoragePath,
        const QString& previewImage,
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

    ProjectSettingsListModel* projectSettingsListModel() const;
    void setProjectSettingsListModel(ProjectSettingsListModel* newProjectSettingsListModel);

public slots:
    void setSelectedTimelineIndex(int selectedTimelineIndex);

signals:
    void activeWallpaperCounterChanged(int activeWallpaperCounter);
    void activeWidgetsCounterChanged(int activeWidgetsCounter);
    void selectedTimelineIndexChanged(int selectedTimelineIndex);
    void activeTimelineIndexChanged(int activeTimelineIndex);
    void monitorConfigurationChanged();
    void projectSettingsListModelResult(ScreenPlay::ProjectSettingsListModel* li = nullptr);
    void requestRaise();
    void profilesSaved();
    void printQmlTimeline();
    void displayErrorPopup(const QString& msg);
    void notifyUiWallpaperAdded();
    void projectSettingsListModelChanged(ProjectSettingsListModel* projectSettingsListModel);

private slots:
    bool saveProfiles();
    void newConnection();
    void setActiveWallpaperCounter(int activeWallpaperCounter);
    void setActiveWidgetsCounter(int activeWidgetsCounter);
    void setActiveTimelineIndex(int activeTimelineIndex);

private:
    bool loadProfiles();
    bool checkIsAnotherScreenPlayInstanceRunning();
    bool removeWidget(const QString& appID);
    bool loadWidgetConfig(const QJsonObject& widget);

private:
    std::shared_ptr<GlobalVariables> m_globalVariables;
    std::shared_ptr<MonitorListModel> m_monitorListModel;
    std::shared_ptr<Settings> m_settings;
    std::unique_ptr<QLocalServer> m_server;
    std::shared_ptr<ProjectSettingsListModel> m_projectSettingsListModel;
    QVector<std::shared_ptr<ScreenPlayWidget>> m_screenPlayWidgets;
    std::vector<std::unique_ptr<SDKConnection>> m_unconnectedClients;
    ScreenPlayTimelineManager m_screenPlayTimelineManager;

    QTimer m_saveLimiter;
    Util m_util;

    int m_activeWallpaperCounter { 0 };
    int m_activeWidgetsCounter { 0 };
    int m_selectedTimelineIndex { 0 };
    int m_activeTimelineIndex { -1 };
};
}
