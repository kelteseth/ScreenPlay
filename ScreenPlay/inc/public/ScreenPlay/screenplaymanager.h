// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#pragma once

#include <QObject>
#include <QPoint>
#include <QVariantMap>
#include <QProcess>
#include <QtWebSockets/QWebSocket>

#include "ScreenPlayUtil/projectfile.h"
#include "ScreenPlayUtil/util.h"
#include "globalvariables.h"
#include "installedlistmodel.h"
#include "monitorlistmodel.h"
#include "profilelistmodel.h"
#include "projectsettingslistmodel.h"
#include "screenplaywallpaper.h"
#include "screenplaywidget.h"
#include "settings.h"
#include <iostream>
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

    // We always handle the endTimeString, because it is the handle for the
    // timeline. The last, default, timeline does not have a handle.
    Q_INVOKABLE bool moveTimelineAt(
        const int index,
        const QString identifier,
        const float relativePosition,
        QString positionTimeString)
    {
        m_contentTimer.stop();
        auto updateTimer = qScopeGuard([this] { m_contentTimer.start(); });

        auto& wallpapterTimelineSection = m_wallpaperTimelineSectionsList.at(index);
        QTime newPositionTime = QTime::fromString(positionTimeString, "hh:mm");
        if (!newPositionTime.isValid()) {
            qWarning() << "Unable to move with invalid time:" << positionTimeString;
            return false;
        }
        wallpapterTimelineSection->endTime = newPositionTime;
        wallpapterTimelineSection->relativePosition = relativePosition;
        // We set the identifier here, because we generate it in qml
        // The identiefier is only used for debugging
        wallpapterTimelineSection->identifier = identifier;

        const auto timelineCount = m_wallpaperTimelineSectionsList.size();
        // Only update the next timeline startTime
        // if we are not end last wallpaper, that always
        // must end at 24:00
        if (index <= timelineCount) {
            auto& wallpapterTimelineSectionNext = m_wallpaperTimelineSectionsList.at(index + 1);
            wallpapterTimelineSectionNext->startTime = newPositionTime;
        }
        printTimelines();
        return true;
    }

    Q_INVOKABLE QString getTimeString(double relativeLinePosition)
    {
        const double totalHours = relativeLinePosition * 24;
        int hours = static_cast<int>(std::floor(totalHours)); // Gets the whole hour part
        double fractionalHours = totalHours - hours;
        int minutes = static_cast<int>(std::floor(fractionalHours * 60)); // Calculates the minutes
        double fractionalMinutes = fractionalHours * 60 - minutes;
        int seconds = static_cast<int>(std::round(fractionalMinutes * 60)); // Calculates the seconds

        // Adjust minutes and seconds if seconds rolled over to 60
        if (seconds == 60) {
            seconds = 0;
            minutes += 1;
        }

        // Adjust hours and minutes if minutes rolled over to 60
        if (minutes == 60) {
            minutes = 0;
            hours += 1;
        }

        // Ensure hours wrap correctly at 24
        if (hours == 24) {
            hours = 0;
        }

        // Format the output to "HH:MM:SS"
        return QString("%1:%2:%3").arg(hours, 2, 10, QChar('0')).arg(minutes, 2, 10, QChar('0')).arg(seconds, 2, 10, QChar('0'));
    }

    void updateIndices()
    {
        // Sort the vector based on relativePosition
        std::sort(m_wallpaperTimelineSectionsList.begin(), m_wallpaperTimelineSectionsList.end(),
            [](const auto& a, const auto& b) {
                return a->startTime < b->startTime;
            });

        // Update the indices based on new order
        for (int i = 0; i < m_wallpaperTimelineSectionsList.size(); ++i) {
            m_wallpaperTimelineSectionsList[i]->index = i;
        }
    }

    Q_INVOKABLE bool addTimelineAt(
        const int index,
        const float reltiaveLinePosition,
        QString identifier)
    {

        m_contentTimer.stop();
        auto updateTimer = qScopeGuard([this] { m_contentTimer.start(); });

        // We always get the new endTime
        const QString newStopPosition = getTimeString(reltiaveLinePosition);
        QTime newStopPositionTime = QTime::fromString(newStopPosition, m_timelineTimeFormat);
        if (!newStopPositionTime.isValid()) {
            return false;
        }

        // IMPORTANT: The new element is always on the left. The first
        // handle always persists  because the
        // user can never delete it. It only gets "pushed" further
        // to the right, by increasing the size.

        //                         | Insert here
        //                         ˇ
        // |-----------------------------------------------|
        //                      0 ID: AAA
        //
        //
        // |-----------------------|-----------------------|
        //          0 ID: BBB             1 - ID: AAA
        // We directly get the new index of 0 in this example from qml

        auto newTimelineSection = std::make_shared<WallpaperTimelineSection>();
        newTimelineSection->index = index;
        newTimelineSection->identifier = identifier;
        newTimelineSection->endTime = newStopPositionTime;
        // We can use the given index here, because it points
        // the the current item at that index, and we have not yet
        // added our new timelineSection to our list.
        newTimelineSection->startTime = m_wallpaperTimelineSectionsList.at(index)->startTime;

        const bool isLast = (m_wallpaperTimelineSectionsList.length() - 1) == index;
        if(isLast){
            m_wallpaperTimelineSectionsList.last()->startTime =  newTimelineSection->endTime;
        }

        m_wallpaperTimelineSectionsList.append(newTimelineSection);

        updateIndices();
        printTimelines();
        return true;
    }

    Q_INVOKABLE bool removeTimelineAt(const int index)
    {
        printTimelines();
        const auto timelineCount = m_wallpaperTimelineSectionsList.size();
        if (timelineCount == 0) {
            qCritical() << "Timeline empty";
            return false;
        }
        if (timelineCount == 1) {
            qCritical() << "Timeline must always have at least one element, that span across the whole timeline from 00:00:00 to 23:59:59.";
            return false;
        }
        m_contentTimer.stop();
        auto updateTimer = qScopeGuard([this] { m_contentTimer.start(); });

        auto& wallpapterTimelineSection = m_wallpaperTimelineSectionsList.at(index);

        // When we have two timelines, we know that only the first
        // timeline can be removed and the second one will then span
        // across the whole timeline
        //    remove     <- expand
        // |-----------|-----------|
        //       0          1
        if (timelineCount == 2) {
            if (index != 0) {
                qCritical() << "Removing the last timeline is not allowed. This must always span the whole timeline";
                return false;
            }
            m_wallpaperTimelineSectionsList.removeAt(index);
            m_wallpaperTimelineSectionsList.first()->startTime = QTime::fromString("00:00:00", m_timelineTimeFormat);
            updateIndices();
            printTimelines();
            return true;
        }

        // Now handle all states where we have more than two wallpaper
        // There is no timeline before if we want to remove
        // the timeline at index 0. We do not need to make the same
        // check for the timelineAfter, because the last timeline
        // cannot be deleted
        QTime endTime;
        if(index == 0){
            endTime = QTime::fromString("00:00:00", m_timelineTimeFormat);
        }else {
            endTime = m_wallpaperTimelineSectionsList.at(index - 1)->endTime;
        }
        auto timelineAfter = m_wallpaperTimelineSectionsList.at(index + 1);
        //    before      remove     <- expand
        // |-----------|-----------|-----------|
        //       0          1           2
        // Now when removing timeline at index 1, the next (after)
        // wallpaper gets the remaining space
        timelineAfter->startTime = endTime;
        m_wallpaperTimelineSectionsList.removeAt(index);
        updateIndices();
        printTimelines();
        return true;

        printTimelines();
        return true;
    }

    void printTimelines()
    {
        std::cout << "#############################\n";
        for (auto& timeline : m_wallpaperTimelineSectionsList) {
            std::cout <<timeline->index << ": " << timeline->identifier.toStdString() <<  "\t" << timeline->relativePosition << " start: " << timeline->startTime.toString().toStdString() << " end: " << timeline->endTime.toString().toStdString() << std::endl;
        }
    }

    Q_INVOKABLE QVariantMap initialStopPositions()
    {
        QVariantMap sectionPositions;
        for (const auto& timelineSection : m_wallpaperTimelineSectionsList) {
            sectionPositions.insert({timelineSection->identifier},{timelineSection->relativePosition});
        }
        return sectionPositions;
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
