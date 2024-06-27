#include "screenplaytimeline.h"
#include "ScreenPlay/screenplaywallpaper.h"
#include "ScreenPlay/wallpaperdata.h"

namespace ScreenPlay {
/*!
 * \brief Calculates the relative position of a given time within a day.
 *      * This function takes a QTime object representing the end time and calculates its
 * position relative to a fixed start and end time on the same day. The relative position
 * is a normalized value between 0 and 1, rounded to four decimal places.
 *      * \param endTime The time for which to calculate the relative position.
 * \return A float representing the normalized relative position of endTime, rounded to four decimal places.
 */
float ScreenPlayTimeline::calculateRelativePosition(const QTime &endTime)
{
    QTime startTime(0, 0, 0); // Start of the day
    QTime maxTime(23, 59, 59); // End of the day range

    // Total number of seconds from startTime to maxTime
    int totalSeconds = startTime.secsTo(maxTime);

    // Seconds from startTime to the given endTime
    int endTimeSeconds = startTime.secsTo(endTime);

    // Calculate the relative position
    float relativePosition = static_cast<float>(endTimeSeconds) / totalSeconds;

    // Round to four decimal places
    return qRound(relativePosition * 10000.0) / 10000.0;
}

std::optional<std::shared_ptr<ScreenPlay::WallpaperTimelineSection> > ScreenPlay::ScreenPlayTimeline::loadTimelineWallpaperConfig(const QJsonObject &timelineObj)
{
    const QTime startTime = QTime::fromString(timelineObj.value("startTime").toString(), m_timelineTimeFormat);
    const QTime endTime = QTime::fromString(timelineObj.value("endTime").toString(), m_timelineTimeFormat);
    if (startTime > endTime) {
        qCritical() << "Invalid time, start time is later than end time: " << startTime.toString() << endTime.toString();
        return std::nullopt;
    }

    // TODO check license
    auto timelineSection = std::make_shared<WallpaperTimelineSection>();
    timelineSection->startTime = startTime;
    timelineSection->endTime = endTime;
    timelineSection->relativePosition = calculateRelativePosition(endTime);
    const auto wallpaperList = timelineObj.value("wallpaper").toArray();
    for (auto& wallpaper : wallpaperList) {
        std::optional<WallpaperData> wallpaperDataOpt = WallpaperData::loadWallpaperConfig(wallpaper.toObject());
        if (!wallpaperDataOpt.has_value())
            return std::nullopt;
        timelineSection->wallpaperData.push_back(wallpaperDataOpt.value());
    }
    return timelineSection;
}
/*!
  \brief Parses one timeline wallpaper:

        "timelineWallpaper": [
            {
                "endTime": "08:32:00",
                "startTime": "00:00:00",
                "wallpaper": [
                    [...]
                ]
            },
*/
std::shared_ptr<WallpaperTimelineSection> ScreenPlayTimeline::findActiveWallpaperTimelineSection()
{
    for (const auto& section : m_wallpaperTimelineSectionsList) {
        if (section->isActive) {
            return section;
        }
    }
    return nullptr;
}
/*!
    \brief  Returns the wallpaper timeline that has the isActive
            flag enabled.
*/
std::optional<std::shared_ptr<WallpaperTimelineSection> > ScreenPlayTimeline::activeWallpaperSectionByAppID(const QString &appID)
{
    for (const auto& section : m_wallpaperTimelineSectionsList) {
        for (const auto& wallpaper : section->activeWallpaperList) {
            if(wallpaper->appID() == appID)
                return section;
        }
    }
    qCritical() << "No matching appID for:"<< appID;
    return nullptr;
}
/*!
  \brief Returns the current active timline. There must always be an active timeline!
*/
std::shared_ptr<WallpaperTimelineSection> ScreenPlayTimeline::getCurrentTimeline()
{
    const QTime currentTime = QTime::currentTime();
    for (const auto& section : m_wallpaperTimelineSectionsList) {
        if (section->containsTime(currentTime)) {
            return section;
        }
    }
    qCritical() << "No active timeline";
    return nullptr;
}

}
