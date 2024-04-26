// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#pragma once

#include <QObject>
#include <QPoint>
#include <QProcess>
#include <QtWebSockets/QWebSocket>

#include "ScreenPlayUtil/projectfile.h"
#include "globalvariables.h"
#include "installedlistmodel.h"
#include "monitorlistmodel.h"
#include "profilelistmodel.h"
#include "projectsettingslistmodel.h"
#include "screenplaywallpaper.h"
#include "screenplaywidget.h"
#include "settings.h"
#include <memory>
#include <optional>

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


    // Timeline wallpaper updates:
    // 1. Disable timeline updates
    // Optional: 1.1 Insert new timeline
    // 2. Call setWallpaperAtTimelineIndex

    Q_INVOKABLE ScreenPlayWallpaper* getWallpaperByAppID(const QString& appID);

    void updateTimelineStartTime(const int index){

    }
    void updateTimelineEndTime(const int index){

    }

    // We always handle the endTimeString, because it is the handle for the
    // timeline. The last, default, timeline does not have a handle.
    Q_INVOKABLE bool moveTimelineAt(
        const int index,
        QString endTimeString){
        m_contentTimer.stop();
        auto updateTimer = qScopeGuard([this] {  m_contentTimer.start(); });

        auto& wallpapterTimelineSection = m_wallpaperTimelineSectionsList.at(index);
        QTime endTime = QTime::fromString(endTimeString, m_timelineTimeFormat);
        if (!endTime.isValid()){
            return false;
        }
        wallpapterTimelineSection->endTime = endTime;
        const auto timelineCount = m_wallpaperTimelineSectionsList.size();
        // Only update the next timeline startTime
        // if we are not end last wallpaper, that always
        // must end at 24:00
        if(index <= timelineCount){
            auto& wallpapterTimelineSectionNext = m_wallpaperTimelineSectionsList.at(index + 1);
            wallpapterTimelineSectionNext->startTime =endTime;
        }
        return true;
    }

    Q_INVOKABLE bool addTimelineAt(
        const int index,
        QString startTimeString,
        QString endTimeString,
        QString identitfier) {
        m_contentTimer.stop();
        auto updateTimer = qScopeGuard([this] {  m_contentTimer.start(); });

        QTime startTime = QTime::fromString(startTimeString, m_timelineTimeFormat);
        if (!startTime.isValid()){
            return false;
        }
        QTime endTime = QTime::fromString(endTimeString, m_timelineTimeFormat);
        if (!endTime.isValid()){
            return false;
        }
        auto timelineSection = std::make_shared<WallpaperTimelineSection>();
        timelineSection->index = index;
        timelineSection->startTime = startTime;
        timelineSection->endTime = endTime;
        m_wallpaperTimelineSectionsList.append(timelineSection);

        const auto timelineCount = m_wallpaperTimelineSectionsList.size();
        // Only update the next timeline startTime
        // if we are not end last wallpaper, that always
        // must end at 24:00
        if(index <= timelineCount){
            auto& wallpapterTimelineSectionNext = m_wallpaperTimelineSectionsList.at(index + 1);
            wallpapterTimelineSectionNext->startTime =endTime;
        }

        // Only update the next timeline startTime
        // if we are not end last wallpaper, that always
        // must end at 24:00
        if(index > 0){
            auto& wallpapterTimelineSectionBefore = m_wallpaperTimelineSectionsList.at(index - 1);
            wallpapterTimelineSectionBefore->endTime =startTime;
        }

        return true;
    }

    Q_INVOKABLE bool removeTimelineAt(const int index){
        m_contentTimer.stop();
        auto updateTimer = qScopeGuard([this] {  m_contentTimer.start(); });

        auto& wallpapterTimelineSection = m_wallpaperTimelineSectionsList.at(index);
        const auto timelineCount = m_wallpaperTimelineSectionsList.size();
        if(timelineCount  > 1){
            m_wallpaperTimelineSectionsList.at(timelineCount - 1)->endTime = m_wallpaperTimelineSectionsList.at(timelineCount  + 1)->startTime;
        }
        // Only update the next timeline startTime
        // if we are not end last wallpaper, that always
        // must end at 24:00
        // if(index <= timelineCount){
        //     auto& wallpapterTimelineSectionNext = m_wallpaperTimelineSectionsList.at(index + 1);
        //     wallpapterTimelineSectionNext->startTime = endTime;
        // }

        //        // Only update the next timeline startTime
        //        // if we are not end last wallpaper, that always
        //        // must end at 24:00
        // if(index > 0){
        //     auto& wallpapterTimelineSectionBefore = m_wallpaperTimelineSectionsList.at(index - 1);
        //     wallpapterTimelineSectionBefore->endTime =startTime;
        // }
        // First update the new times and then remove the old
        // timeline
        m_wallpaperTimelineSectionsList.removeAt(index);
        return true;

    }
    Q_INVOKABLE bool setWallpaperAtTimelineIndex(
        const ScreenPlay::ContentTypes::InstalledType type,
        const ScreenPlay::Video::FillMode fillMode,
        const QString& absolutePath,
        const QString& previewImage,
        const QString& file,
        const QVector<int>& monitorIndex,
        const float volume,
        const float playbackRate,
        const QJsonObject& properties,
        const int timelineIndex,
        const QString& startTimeString,
        const QString& endTimeString,
        const bool saveToProfilesConfigFile);

    Q_INVOKABLE bool createWidget(
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

    Q_INVOKABLE QVector<float> getRelativeSectionPositions(){
        QVector<float> sectionPositions;
        for (const auto &timelineSection : m_wallpaperTimelineSectionsList) {
            sectionPositions.append(timelineSection->relativePosition);
        }
        return sectionPositions;
    }

    bool removeWallpaperAtTimelineIndex(const int timelineIndex, const int monitorIndex);
signals:
    void activeWallpaperCounterChanged(int activeWallpaperCounter);
    void activeWidgetsCounterChanged(int activeWidgetsCounter);
    void monitorConfigurationChanged();

    void projectSettingsListModelResult(ScreenPlay::ProjectSettingsListModel* li = nullptr);
    void requestSaveProfiles();
    void requestRaise();
    void profilesSaved();
    void displayErrorPopup(const QString& msg);

private slots:
    bool saveProfiles();
    void checkActiveWallpaperTimeline();
    void newConnection();

public slots:

    void setActiveWallpaperCounter(int activeWallpaperCounter)
    {
        if (m_activeWallpaperCounter == activeWallpaperCounter)
            return;

        m_activeWallpaperCounter = activeWallpaperCounter;
        emit activeWallpaperCounterChanged(m_activeWallpaperCounter);
    }

    void setActiveWidgetsCounter(int activeWidgetsCounter)
    {
        if (m_activeWidgetsCounter == activeWidgetsCounter)
            return;

        m_activeWidgetsCounter = activeWidgetsCounter;
        emit activeWidgetsCounterChanged(m_activeWidgetsCounter);
    }

    void increaseActiveWidgetsCounter()
    {
        m_activeWidgetsCounter++;
        emit activeWidgetsCounterChanged(m_activeWidgetsCounter);
    }

    void decreaseActiveWidgetsCounter()
    {
        if (m_activeWidgetsCounter <= 0) {
            return;
        }
        m_activeWidgetsCounter--;
        emit activeWidgetsCounterChanged(m_activeWidgetsCounter);
    }

    void increaseActiveWallpaperCounter()
    {
        m_activeWallpaperCounter++;
        emit activeWallpaperCounterChanged(m_activeWallpaperCounter);
    }

    void decreaseActiveWallpaperCounter()
    {
        if (m_activeWallpaperCounter <= 0) {
            return;
        }
        m_activeWallpaperCounter--;
        emit activeWallpaperCounterChanged(m_activeWallpaperCounter);
    }

private:
    bool loadProfiles();
    bool loadWidgetConfig(const QJsonObject& widget);
    std::optional<WallpaperData> loadWallpaperConfig(const QJsonObject& wallpaper);
    bool checkIsAnotherScreenPlayInstanceRunning();
    bool removeWallpaper(const QString& appID);
    bool removeWidget(const QString& appID);
    std::optional<std::shared_ptr<WallpaperTimelineSection>> loadTimelineWallpaperConfig(const QJsonObject& timelineObj);
    std::shared_ptr<WallpaperTimelineSection> findActiveSection();

private:
    std::shared_ptr<GlobalVariables> m_globalVariables;
    std::shared_ptr<MonitorListModel> m_monitorListModel;
    std::shared_ptr<Settings> m_settings;
    std::unique_ptr<QLocalServer> m_server;
    std::unique_ptr<QWebSocketServer> m_websocketServer;
    QVector<QWebSocket*> m_connections;

    std::shared_ptr<WallpaperTimelineSection> m_activeWallpaperTimeline;
    QVector<std::shared_ptr<WallpaperTimelineSection>> m_wallpaperTimelineSectionsList;
    QVector<std::shared_ptr<ScreenPlayWidget>> m_screenPlayWidgets;
    std::vector<std::unique_ptr<SDKConnection>> m_unconnectedClients;

    int m_activeWallpaperCounter { 0 };
    int m_activeWidgetsCounter { 0 };

    QTimer m_saveLimiter;
    QTimer m_contentTimer;

    // We use a 24 hour system
    const QString m_timelineTimeFormat = "hh:mm:ss";
    const quint16 m_webSocketPort = 16395;
};
}
