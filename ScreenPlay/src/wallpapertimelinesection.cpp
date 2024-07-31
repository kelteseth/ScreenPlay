#include "ScreenPlay/wallpapertimelinesection.h"
#include "ScreenPlay/screenplaywallpaper.h"
#include "ScreenPlay/wallpaperdata.h"
#include "ScreenPlayUtil/util.h"
#include <QDebug>
#include <QDir>
#include <QFileInfoList>
#include <QGuiApplication>
#include <QObject>

namespace ScreenPlay {

bool WallpaperTimelineSection::containsTime(const QTime& time) const
{
    if (endTime < startTime) { // Timeline spans midnight
        return (time >= startTime || time < endTime);
    } else {
        return (time >= startTime && time < endTime);
    }
}

QJsonObject WallpaperTimelineSection::serialize() const
{
    QJsonObject data;
    data.insert("identifier", identifier);
    data.insert("index", index);
    data.insert("relativePosition", relativePosition);
    data.insert("startTime", startTime.toString());
    data.insert("endTime", endTime.toString());

    QJsonArray wallpaperDataArray;
    for (const auto& wallpaper : wallpaperDataList) {
        QJsonObject wallpaperObject = wallpaper.serialize();
        wallpaperDataArray.append(wallpaperObject);
    }
    data.insert("wallpaperData", wallpaperDataArray);

    return data;
}

// Start all ScreenPlayWallpaper processes of this current timeline
bool WallpaperTimelineSection::activateTimeline()
{
    if (state == State::Active) {
        qCritical() << " timeline:" << index << identifier << "is already active with state: " << state;
        return false;
    }

    state = WallpaperTimelineSection::State::Starting;
    qDebug() << "Activate timeline:" << index
             << identifier
             << relativePosition
             << wallpaperDataList.size();

    for (auto& wallpaperData : wallpaperDataList) {

        const int screenCount = QGuiApplication::screens().count();

        QJsonArray monitors;
        for (const int index : wallpaperData.monitors) {
            monitors.append(index);
            if (index > screenCount - 1) {
                qWarning() << "Configuration contains invalid monitor with index: " << index << " screen count: " << screenCount;
                return false;
            }
        }

        // Remove file:///
        wallpaperData.absolutePath = QUrl::fromUserInput(wallpaperData.absolutePath).toLocalFile();
        const QString appID = Util().generateRandomString();
        qInfo() << "Start wallpaper" << wallpaperData.absolutePath << appID;

        auto screenPlayWallpaper = std::make_shared<ScreenPlayWallpaper>(
            globalVariables,
            appID,
            wallpaperData,
            settings);

        QObject::connect(screenPlayWallpaper.get(), &ScreenPlayWallpaper::requestSave, this, [this]() {
            // &ScreenPlayManager::requestSaveProfiles
            emit requestSaveProfiles();
        });
        QObject::connect(screenPlayWallpaper.get(), &ScreenPlayWallpaper::requestClose, this, [this]() {
            // , &ScreenPlayManager::removeWallpaper);
        });
        QObject::connect(screenPlayWallpaper.get(), &ScreenPlayWallpaper::isConnectedChanged, this, [this]() {
            updateActiveWallpaperCounter();
        });
        // QObject::connect(screenPlayWallpaper.get(), &ScreenPlayWallpaper::error, this, this, []() {
        //     // , &ScreenPlayManager::displayErrorPopup);
        // });
        if (!screenPlayWallpaper->start()) {
            return false;
        }
        activeWallpaperList.push_back(screenPlayWallpaper);
    }
    state = WallpaperTimelineSection::State::Active;
    return true;
}

QCoro::Task<bool> WallpaperTimelineSection::deactivateTimeline()
{
    state = WallpaperTimelineSection::State::Closing;
    if (activeWallpaperList.empty()) {
        state = State::Inactive;
        co_return true;
    }
    for (auto& activeWallpaper : activeWallpaperList) {
        activeWallpaper->close();
    }
    QTimer timer;
    timer.start(250);
    const int maxRetries = 30;
    for (int i = 1; i <= maxRetries; ++i) {
        // Wait for the timer to tick
        co_await timer;
        bool wallpaperStillActive = false;

        // Check again after the co_await!
        if (activeWallpaperList.empty()) {
            state = State::Inactive;
            co_return true;
        }

        for (std::shared_ptr<ScreenPlayWallpaper>& activeWallpaper : activeWallpaperList) {
            if (!activeWallpaper.get()) {
                Q_ASSERT(!activeWallpaper.get());
                state = State::Inactive;
                co_return false;
            }
            if (activeWallpaper->isConnected()) {
                wallpaperStillActive = true;
                break;
            }
        }
        if (!wallpaperStillActive) {
            // Clear all active wallpaper connections, but do not
            // clear WallpaperData in case we need it again later
            activeWallpaperList.clear();
            state = State::Inactive;
            co_return true;
        }
    }
    state = State::Inactive;
    co_return false;
}

QCoro::Task<bool> WallpaperTimelineSection::removeWallpaper(const int monitorIndex)
{
    auto wallpaperOpt = wallpaperByMonitorIndex(monitorIndex);
    if (!wallpaperOpt.has_value()) {
        qCritical() << "No wallpaper found for monitor index:" << monitorIndex;
        co_return false;
    }

    QTimer timer;
    timer.start(250);
    const int maxRetries = 30;
    wallpaperOpt.value()->close();
    for (int i = 1; i <= maxRetries; ++i) {
        // Wait for the timer to tick
        co_await timer;
        if (!wallpaperOpt.value()->isConnected()) {
            co_return true;
        }
    }
    co_return false;
}

void WallpaperTimelineSection::updateActiveWallpaperCounter()
{
    quint64 activeWallpaperCount = 0;
    for (const auto& screenPlayWallpaper : activeWallpaperList) {
        if (screenPlayWallpaper->isConnected())
            activeWallpaperCount++;
    }
    emit activeWallpaperCountChanged(activeWallpaperCount);
}

std::optional<std::shared_ptr<ScreenPlayWallpaper>> WallpaperTimelineSection::wallpaperByMonitorIndex(const int monitorIndex)
{
    for (const auto& screenPlayWallpaper : activeWallpaperList) {
        if (screenPlayWallpaper->monitors().contains(monitorIndex))
            return screenPlayWallpaper;
    }
    return std::nullopt;
}
}
