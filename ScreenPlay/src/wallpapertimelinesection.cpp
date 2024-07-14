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

    // Serialize vector<WallpaperData>
    QJsonArray wallpaperDataArray;
    for (const auto& wallpaper : wallpaperDataList) {
        QJsonObject wallpaperObject = wallpaper.serialize(); // Assuming WallpaperData has a serialize method
        wallpaperDataArray.append(wallpaperObject);
    }
    data.insert("wallpaperData", wallpaperDataArray);

    // Serialize vector<std::shared_ptr<ScreenPlayWallpaper>>
    // QJsonArray activeWallpaperArray;
    // for (const auto& wallpaper : activeWallpaperList) {
    //     QJsonObject wallpaperObject = wallpaper->serialize();  // Assuming ScreenPlayWallpaper has a serialize method
    //     activeWallpaperArray.append(wallpaperObject);
    // }
    // data.insert("activeWallpaperList", activeWallpaperArray);

    return data;
}

// Start all ScreenPlayWallpaper processes of this current timeline
bool WallpaperTimelineSection::activateTimeline()
{
    status = Status::Active;
    // TODO: this can be called if the timeline is already active.
    // Add a check to only launch new
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

        // Only support remove wallpaper that spans over 1 monitor
        // if (wallpaper.monitors.length() == 1) {
        //     int i = 0;
        //     for (auto& wallpaper : m_screenPlayWallpapers) {
        //         if (wallpaper->monitors().length() == 1) {
        //             if (monitors.at(0) == wallpaper->monitors().at(0)) {
        //                 return wallpaper->replace(
        //                     wallpaper,
        //                     settings->checkWallpaperVisible());
        //                 m_monitorListModel->setWallpaperMonitor(wallpaper, wallpaper.monitors);
        //             }
        //         }
        //         i++;
        //     }
        // }

        auto screenPlayWallpaper = std::make_shared<ScreenPlayWallpaper>(
            globalVariables,
            appID,
            wallpaperData,
            settings);

        QObject::connect(screenPlayWallpaper.get(), &ScreenPlayWallpaper::requestSave, this, [this]() {
            // &ScreenPlayManager::requestSaveProfiles
            emit requestSaveProfiles();
        });
        QObject::connect(screenPlayWallpaper.get(), &ScreenPlayWallpaper::requestClose, this, []() {
            // , &ScreenPlayManager::removeWallpaper);
        });

        // QObject::connect(screenPlayWallpaper.get(), &ScreenPlayWallpaper::error, this, this, []() {
        //     // , &ScreenPlayManager::displayErrorPopup);
        // });
        if (!screenPlayWallpaper->start()) {
            return false;
        }
        // m_monitorListModel->setWallpaperMonitor(wallpaper, wallpaperData.monitors);
        activeWallpaperList.push_back(screenPlayWallpaper);
    }
    return true;
}

QCoro::Task<bool> WallpaperTimelineSection::deactivateTimeline()
{

    status = Status::Closing;
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
        for (auto& activeWallpaper : activeWallpaperList) {
            if (activeWallpaper->isConnected()) {
                wallpaperStillActive = true;
                break;
            }
        }
        if (!wallpaperStillActive) {
            status = Status::Inactive;
            // Clear all active wallpaper connections, but do not
            // clear WallpaperData in case we need it again later
            activeWallpaperList.clear();
            co_return true;
        }
    }
    co_return false;
}

}
