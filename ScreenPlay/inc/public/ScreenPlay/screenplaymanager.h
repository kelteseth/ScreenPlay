// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#pragma once

#include <QObject>
#include <QPoint>
#include <QProcess>
#include <QVariantMap>

#include "ScreenPlay/screenplaytimeline.h"
#include "ScreenPlayUtil/util.h"
#include "globalvariables.h"
#include "monitorlistmodel.h"
#include "projectsettingslistmodel.h"
#include "screenplaywallpaper.h"
#include "screenplaywidget.h"
#include "settings.h"
#include <memory>

namespace ScreenPlay {



class ScreenPlayManager : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")

    Q_PROPERTY(int activeWallpaperCounter READ activeWallpaperCounter WRITE setActiveWallpaperCounter NOTIFY activeWallpaperCounterChanged)
    Q_PROPERTY(int activeWidgetsCounter READ activeWidgetsCounter WRITE setActiveWidgetsCounter NOTIFY activeWidgetsCounterChanged)

public:
    explicit ScreenPlayManager(QObject* parent = nullptr);

    void init(
        const std::shared_ptr<GlobalVariables>& globalVariables,
        const std::shared_ptr<MonitorListModel>& mlm,
        const std::shared_ptr<Settings>& settings);

    int activeWallpaperCounter() const { return m_activeWallpaperCounter; }
    int activeWidgetsCounter() const { return m_activeWidgetsCounter; }

    std::shared_ptr<ScreenPlayWallpaper> startWallpaper(
        WallpaperData wallpaperData,
        const bool saveToProfilesConfigFile);

    Q_INVOKABLE bool removeAllWallpapers(bool saveToProfile = false);
    Q_INVOKABLE bool removeAllWidgets(bool saveToProfile = false);
    Q_INVOKABLE bool removeWallpaperAt(const int index);

    Q_INVOKABLE ScreenPlayWallpaper* getWallpaperByAppID(const QString& appID);

    Q_INVOKABLE bool moveTimelineAt(
        const int index,
        const QString identifier,
        const float relativePosition,
        QString positionTimeString);

    Q_INVOKABLE QString getTimeString(double relativeLinePosition);

    Q_INVOKABLE bool addTimelineAt(
        const int index,
        const float reltiaveLinePosition,
        QString identifier);
    Q_INVOKABLE void removeAllTimlineSections();
    Q_INVOKABLE bool removeTimelineAt(const int index);
    Q_INVOKABLE QJsonArray initialSectionsList();
    Q_INVOKABLE bool setWallpaperAtTimelineIndex(
        const ScreenPlay::ContentTypes::InstalledType type,
        const QString& absolutePath,
        const QString& previewImage,
        const QString& file,
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

    Q_INVOKABLE bool requestProjectSettingsAtMonitorIndex(const int index);
    Q_INVOKABLE bool setWallpaperValueAtMonitorIndex(const int index, const QString& key, const QString& value);
    Q_INVOKABLE bool setWallpaperFillModeAtMonitorIndex(const int index, const int fillmode);
    Q_INVOKABLE bool setAllWallpaperValue(const QString& key, const QString& value);
    Q_INVOKABLE bool setWallpaperValue(const QString& appID, const QString& key, const QString& value);
 signals:
    void activeWallpaperCounterChanged(int activeWallpaperCounter);
    void activeWidgetsCounterChanged(int activeWidgetsCounter);
    void monitorConfigurationChanged();

    void projectSettingsListModelResult(ScreenPlay::ProjectSettingsListModel* li = nullptr);
    void requestSaveProfiles();
    void requestRaise();
    void profilesSaved();
    void printQmlTimeline();
    void displayErrorPopup(const QString& msg);

private slots:
    bool saveProfiles();
    void checkActiveWallpaperTimeline();
    void newConnection();
    void setActiveWallpaperCounter(int activeWallpaperCounter);
    void setActiveWidgetsCounter(int activeWidgetsCounter);

private:
    void printTimelines();
    bool loadProfiles();
    void updateQmlTimelines();

    void updateIndices();
    bool checkIsAnotherScreenPlayInstanceRunning();
    bool removeWallpaper(const QString& appID);
    bool removeWidget(const QString& appID);
    void activateNewTimeline();

    bool loadWidgetConfig(const QJsonObject& widget);
    std::shared_ptr<GlobalVariables> m_globalVariables;
    std::shared_ptr<MonitorListModel> m_monitorListModel;
    std::shared_ptr<Settings> m_settings;
    std::unique_ptr<QLocalServer> m_server;
    QVector<std::shared_ptr<ScreenPlayWidget>> m_screenPlayWidgets;
    std::vector<std::unique_ptr<SDKConnection>> m_unconnectedClients;
    ScreenPlayTimeline m_screenPlayTimeline;

    int m_activeWallpaperCounter { 0 };
    int m_activeWidgetsCounter { 0 };

    QTimer m_saveLimiter;
    QTimer m_contentTimer;

    Util m_util;

    const quint16 m_webSocketPort = 16395;
};

}
