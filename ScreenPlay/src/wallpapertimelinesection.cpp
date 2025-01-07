#include "ScreenPlay/wallpapertimelinesection.h"
#include "ScreenPlay/screenplaywallpaper.h"
#include "ScreenPlay/wallpaperdata.h"
#include "ScreenPlayCore/util.h"
#include <QDebug>
#include <QDir>
#include <QFileInfoList>
#include <QGuiApplication>
#include <QObject>
#include <ranges>

namespace ScreenPlay {

bool WallpaperTimelineSection::containsTime(const QTime& time) const
{
    if (endTime < startTime) { // Timeline spans midnight
        return (time >= startTime || time < endTime);
    } else {
        return (time >= startTime && time < endTime);
    }
}

std::optional<WallpaperData> WallpaperTimelineSection::getWallpaperDataForMonitor(const int monitorIndex) const
{
    for (const auto& wallpaper : wallpaperDataList) {
        const auto& monitors = wallpaper.monitors();
        if (std::find(monitors.begin(), monitors.end(), monitorIndex) != monitors.end()) {
            return wallpaper;
        }
    }
    return std::nullopt;
}
std::optional<std::shared_ptr<ScreenPlayWallpaper>> WallpaperTimelineSection::getActiveWallpaperForMonitor(const int monitorIndex) const
{
    for (const auto& activeWallpaper : activeWallpaperList) {
        const auto monitors = activeWallpaper->monitors();
        if (std::find(monitors.begin(), monitors.end(), monitorIndex) != monitors.end()) {
            return activeWallpaper;
        }
    }
    return {};
}

bool WallpaperTimelineSection::containsMonitor(const int monitor) const
{
    auto allMonitors = wallpaperDataList
        | std::views::transform([](const WallpaperData& wallpaperData) { return wallpaperData.monitors(); })
        | std::views::join
        | std::ranges::to<std::vector<int>>();

    return std::ranges::contains(allMonitors, monitor);
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
QCoro::Task<bool> WallpaperTimelineSection::activateTimeline()
{
    qDebug() << "Attempting timeline activation:" << index << identifier << "current state:" << state;
    if (state == State::Closing) {
        qDebug() << "Timeline already Closing:" << index << identifier;
        co_return true;
    }
    // Allow activation if we're Inactive OR if we're already Starting
    if (state == State::Active) {
        qDebug() << "Timeline already active:" << index << identifier;
        co_return true; // Already active is a success case
    }

    if (state == State::Starting) {
        qDebug() << "Timeline already starting:" << index << identifier;
        co_return true; // Already starting is a success case
    }

    if (state != State::Inactive && state != State::Failed) {
        qCritical() << "Cannot activate timeline in state:" << state;
        co_return false;
    }

    state = WallpaperTimelineSection::State::Starting;
    qDebug() << "Activate timeline:" << index
             << identifier
             << relativePosition
             << wallpaperDataList.size();

    bool allWallpapersStarted = true;
    for (auto& wallpaperData : wallpaperDataList) {
        const bool startSuccess = co_await startWallpaper(wallpaperData);
        if (!startSuccess) {
            qCritical() << "Failed to start wallpaper in timeline:" << identifier;
            allWallpapersStarted = false;
            break;
        }
    }

    if (allWallpapersStarted) {
        state = WallpaperTimelineSection::State::Active;
        qDebug() << "Timeline activated successfully:" << index << identifier;
        co_return true;
    } else {
        state = WallpaperTimelineSection::State::Failed; // Instead of Inactive
        qCritical() << "Timeline activation failed:" << index << identifier;
        co_return false;
    }
}

// Reuse running wallpaper
QCoro::Task<bool> WallpaperTimelineSection::activateTimeline(std::vector<std::shared_ptr<ScreenPlayWallpaper>> currentActiveWallpaperList)
{
    if (currentActiveWallpaperList.empty()) {
        const bool success = co_await activateTimeline();
        co_return success;
    }

    namespace ranges = std::ranges;
    qDebug() << " activateTimeline:" << index << identifier << activeWallpaperList.size();
    if (state == State::Closing) {
        qDebug() << "Cannot activate timeline in Closing state:" << index << identifier;
        co_return false;
    }
    if (state == State::Active) {
        qDebug() << "Timeline already active:" << index << identifier;
        co_return true;
    }

    if (state == State::Starting) {
        qDebug() << "Timeline already starting:" << index << identifier;
        co_return true;
    }
    if (state != State::Inactive && state != State::Failed) {
        qCritical() << "Cannot activate timeline in state:" << state;
        co_return false;
    }

    if (!activeWallpaperList.empty()) {
        qCritical() << " activeWallpaperList not empty with size:" << activeWallpaperList.size();
        state = WallpaperTimelineSection::State::Inactive;
        co_return false;
    }
    state = WallpaperTimelineSection::State::Starting;

    auto same_monitor = [](const auto& running, const auto& data) {
        return running->monitors().first() == data.monitors().first();
    };

    auto same_runtime = [this](const auto& running, const auto& data) {
        return m_util.isSameWallpaperRuntime(running->type(), data.type());
    };

    // Process wallpapers that need updating or replacing
    bool allWallpapersHandled = true;
    for (auto wallpaperIt = currentActiveWallpaperList.begin();
        wallpaperIt != currentActiveWallpaperList.end();) {
        auto& runningWallpaper = *wallpaperIt;

        // Find matching wallpaper data using views
        auto matchingData = wallpaperDataList
            | std::views::filter([&](const WallpaperData& data) {
                  return same_monitor(runningWallpaper, data);
              })
            | std::views::take(1);

        if (matchingData.empty()) {
            ++wallpaperIt;
            continue;
        }

        const auto& newData = *matchingData.begin();

        if (same_runtime(runningWallpaper, newData)) {
            if (!runningWallpaper->replace(newData)) {
                qCritical() << "Unable to replace running wallpaper";
                allWallpapersHandled = false;
            }
            // TODO fix other timeline sectino wallpaperData
            activeWallpaperList.push_back(runningWallpaper);
            ++wallpaperIt;
        } else {
            co_await runningWallpaper->close();
            wallpaperIt = currentActiveWallpaperList.erase(wallpaperIt);
            if (!co_await startWallpaper(newData)) {
                qCritical() << "Failed to start new wallpaper";
                allWallpapersHandled = false;
            }
        }
    }

    // Start any remaining wallpapers that weren't replacements
    for (const auto& wallpaperData : wallpaperDataList) {
        bool alreadyHandled = ranges::any_of(activeWallpaperList,
            [&wallpaperData](const auto& wp) {
                return wp->monitors().first() == wallpaperData.monitors().first();
            });

        if (!alreadyHandled) {
            if (!co_await startWallpaper(wallpaperData)) {
                qCritical() << "Failed to start additional wallpaper";
                allWallpapersHandled = false;
            }
        }
    }

    if (allWallpapersHandled) {
        state = WallpaperTimelineSection::State::Active;
        qDebug() << "Timeline activated successfully with reused wallpapers:" << index << identifier;
        co_return true;
    } else {
        state = WallpaperTimelineSection::State::Failed;
        qCritical() << "Timeline activation failed with reused wallpapers:" << index << identifier;
        co_return false;
    }
}

QCoro::Task<bool> WallpaperTimelineSection::deactivateTimeline()
{
    if (state != State::Active) {
        qCritical() << " timeline:" << index << identifier << "is already active with state: " << state;
        co_return false;
    }

    state = WallpaperTimelineSection::State::Closing;
    if (activeWallpaperList.empty()) {
        state = State::Inactive;
        co_return true;
    }
    for (auto& activeWallpaper : activeWallpaperList) {
        co_await activeWallpaper->close();
    }

    state = State::Inactive;
    co_return false;
}

QCoro::Task<bool> WallpaperTimelineSection::stopWallpaper(const int monitorIndex)
{
    std::shared_ptr<ScreenPlayWallpaper> wallpaperToClose;
    auto wallpaperIt = std::find_if(activeWallpaperList.begin(), activeWallpaperList.end(),
        [monitorIndex](const auto& screenPlayWallpaper) {
            return screenPlayWallpaper && screenPlayWallpaper->monitors().contains(monitorIndex);
        });

    if (wallpaperIt != activeWallpaperList.end()) {
        wallpaperToClose = *wallpaperIt;
    }
    if (!wallpaperToClose)
        co_return false;
    const bool success = co_await wallpaperToClose->close();
    co_return success;
}

// Remove the running wallpaper and the corresponding wallpaperData!
QCoro::Task<bool> WallpaperTimelineSection::removeWallpaper(const int monitorIndex)
{
    // IMPORTANT: First find and store the wallpaper we need to close
    // before modifying any containers
    std::shared_ptr<ScreenPlayWallpaper> wallpaperToClose;
    auto wallpaperIt = std::find_if(activeWallpaperList.begin(), activeWallpaperList.end(),
        [monitorIndex](const auto& screenPlayWallpaper) {
            return screenPlayWallpaper && screenPlayWallpaper->monitors().contains(monitorIndex);
        });

    if (wallpaperIt != activeWallpaperList.end()) {
        wallpaperToClose = *wallpaperIt;
    }

    // Now safely remove from wallpaperDataList
    size_t removedCount = std::erase_if(wallpaperDataList,
        [monitorIndex](const auto& wallpaperData) {
            return wallpaperData.monitors().contains(monitorIndex);
        });

    // Handle case where we have no data but might have running wallpaper
    if (removedCount == 0) {
        qWarning() << "No wallpaper data found for monitor index:" << monitorIndex;
        // Continue execution to handle potential running wallpaper
    }

    // If we found a wallpaper to close earlier, close it safely
    if (wallpaperToClose) {
        // Ensure the wallpaper isn't already closing
        if (wallpaperToClose->state() != ScreenPlay::ScreenPlayEnums::AppState::Closing) {
            const bool closingSuccess = co_await wallpaperToClose->close();
            if (!closingSuccess) {
                qCritical() << "Failed to close wallpaper for monitor index:" << monitorIndex;
                // Continue to remove from list even if close failed
            }
        }

        // Remove from active list after close attempt
        activeWallpaperList.erase(
            std::remove_if(activeWallpaperList.begin(), activeWallpaperList.end(),
                [&wallpaperToClose](const auto& wp) {
                    return wp == wallpaperToClose;
                }),
            activeWallpaperList.end());
    }

    updateActiveWallpaperCounter();
    co_return true;
}

QCoro::Task<bool> WallpaperTimelineSection::startWallpaper(WallpaperData wallpaperData)
{
    // Remove file:///
    wallpaperData.setAbsolutePath(
        QUrl::fromUserInput(wallpaperData.absolutePath()).toLocalFile());
    const QString appID = Util().generateRandomString(8);
    qInfo() << "Start wallpaper" << wallpaperData.absolutePath() << appID;

    auto screenPlayWallpaper = std::make_shared<ScreenPlayWallpaper>(
        globalVariables,
        appID,
        wallpaperData,
        settings);

    QObject::connect(screenPlayWallpaper.get(), &ScreenPlayWallpaper::requestSave, this, [this]() {
        // &ScreenPlayManager::requestSaveProfiles
        emit requestSaveProfiles();
    });
    QObject::connect(screenPlayWallpaper.get(), &ScreenPlayWallpaper::stateChanged, this, &WallpaperTimelineSection::requestUpdateMonitorListModel);
    QObject::connect(screenPlayWallpaper.get(), &ScreenPlayWallpaper::requestClose, this, [this]() {
        // , &ScreenPlayManager::removeWallpaper);
    });
    QObject::connect(screenPlayWallpaper.get(), &ScreenPlayWallpaper::isConnectedChanged, this, [this]() {
        updateActiveWallpaperCounter();
    });
    activeWallpaperList.push_back(screenPlayWallpaper);

    QTimer timer;
    timer.start(250);
    const int maxRetries = 30;
    if (!screenPlayWallpaper->start()) {
        co_return false;
    }
    for (int i = 1; i <= maxRetries; ++i) {
        // Wait for the timer to tick
        co_await timer;
        if (screenPlayWallpaper->isConnected()) {
            updateActiveWallpaperCounter();
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
}
