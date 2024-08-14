#include "ScreenPlay/screenplaytimelinemanager.h"
#include "ScreenPlay/screenplaywallpaper.h"
#include "ScreenPlay/wallpaperdata.h"
#include "ScreenPlayUtil/util.h"

#include <QString>
#include <QStringList>
#include <iostream>
#include <ranges>
#include <unordered_set>

namespace ScreenPlay {

/*!
 * \brief Calculates the relative position of a given time within a day.
 *      * This function takes a QTime object representing the end time and calculates its
 * position relative to a fixed start and end time on the same day. The relative position
 * is a normalized value between 0 and 1, rounded to four decimal places.
 *      * \param endTime The time for which to calculate the relative position.
 * \return A float representing the normalized relative position of endTime, rounded to four decimal places.
 */
ScreenPlayTimelineManager::ScreenPlayTimelineManager(
    QObject* parent)
    : QObject { parent }
{
    // Do not start the timer here. This will be done after
    // we have loaded all timeline wallpaper from the config.json
    QObject::connect(&m_contentTimer, &QTimer::timeout, this, &ScreenPlayTimelineManager::checkActiveWallpaperTimeline);
    m_contentTimer.setInterval(250);
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
std::shared_ptr<WallpaperTimelineSection> ScreenPlayTimelineManager::findActiveWallpaperTimelineSection()
{
    for (const auto& section : m_wallpaperTimelineSectionsList) {
        if (section->state == WallpaperTimelineSection::State::Active) {
            return section;
        }
    }
    return nullptr;
}
/*!
    \brief  Returns the wallpaper timeline that has the isActive
            flag enabled.
*/
std::optional<std::shared_ptr<WallpaperTimelineSection>> ScreenPlayTimelineManager::activeWallpaperSectionByAppID(const QString& appID)
{
    for (const auto& section : m_wallpaperTimelineSectionsList) {
        for (const auto& wallpaper : section->activeWallpaperList) {
            if (wallpaper->appID() == appID)
                return section;
        }
    }
    qCritical() << "No matching appID for:" << appID;
    return nullptr;
}

/*!
  \brief Returns the current active timline. There must always be an active timeline!
*/
std::shared_ptr<WallpaperTimelineSection> ScreenPlayTimelineManager::findTimelineForCurrentTime()
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

/*!
  \brief Loads all timelines.
*/
bool ScreenPlayTimelineManager::addTimelineFromSettings(const QJsonObject& timelineObj)
{
    const QTime startTime = QTime::fromString(timelineObj.value("startTime").toString(), m_timelineTimeFormat);
    const QTime endTime = QTime::fromString(timelineObj.value("endTime").toString(), m_timelineTimeFormat);
    if (startTime > endTime) {
        qCritical() << "Invalid time, start time is later than end time: " << startTime.toString() << endTime.toString();
        return false;
    }

    // TODO check license
    auto newTimelineSection = std::make_shared<WallpaperTimelineSection>();
    QObject::connect(newTimelineSection.get(), &WallpaperTimelineSection::requestUpdateMonitorListModel, this, [this]() {
        updateMonitorListModelData(selectedTimelineIndex());
    });
    QObject::connect(newTimelineSection.get(), &WallpaperTimelineSection::requestSaveProfiles, this, &ScreenPlayTimelineManager::requestSaveProfiles);
    QObject::connect(newTimelineSection.get(), &WallpaperTimelineSection::activeWallpaperCountChanged, this, &ScreenPlayTimelineManager::activeWallpaperCountChanged);
    newTimelineSection->startTime = startTime;
    newTimelineSection->endTime = endTime;
    newTimelineSection->settings = m_settings;
    newTimelineSection->globalVariables = m_globalVariables;
    newTimelineSection->relativePosition = Util().calculateRelativePosition(endTime);
    const auto wallpaperList = timelineObj.value("wallpaper").toArray();
    for (auto& wallpaper : wallpaperList) {
        std::optional<WallpaperData> wallpaperDataOpt = WallpaperData::loadWallpaperConfig(wallpaper.toObject());
        if (!wallpaperDataOpt.has_value())
            return false;
        newTimelineSection->wallpaperDataList.push_back(wallpaperDataOpt.value());
    }

    // Todo: Should we use addTimelineAt?
    newTimelineSection->index = m_wallpaperTimelineSectionsList.length();
    newTimelineSection->identifier = Util().generateRandomString(4);

    qInfo() << newTimelineSection->index
            << newTimelineSection->startTime
            << newTimelineSection->endTime;

    // Todo: Should we use addTimelineAt?
    m_wallpaperTimelineSectionsList.append(newTimelineSection);
    return true;
}

/*!
  \brief We always handle the endTimeString, because it is the handle for the
         timeline. The last, default, timeline does not have a handle.
*/
bool ScreenPlayTimelineManager::moveTimelineAt(const int index, const QString identifier, const float relativePosition, QString positionTimeString)
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

    // printTimelines();
    emit requestSaveProfiles();
    return true;
}

/*!
    \brief Checks if we need to display a different wallpaper at the current time.
*/
void ScreenPlayTimelineManager::checkActiveWallpaperTimeline()
{
    // Current timeline with the state active
    std::shared_ptr<WallpaperTimelineSection> activeTimeline = findActiveWallpaperTimelineSection();

    if (!activeTimeline) {
        std::shared_ptr<WallpaperTimelineSection> currentTimeline = findTimelineForCurrentTime();
        currentTimeline->activateTimeline();
        return;
    }

    // Current timline that should be active, based on the current time
    std::shared_ptr<WallpaperTimelineSection> currentTimeline = findTimelineForCurrentTime();
    if (!currentTimeline) {
        qCritical() << "No current timeline found. There must always be an active timeline.";
        return;
    }

    if (currentTimeline != activeTimeline) {
        activeTimeline->deactivateTimeline();
        currentTimeline->activateTimeline();
    } else {
        if (activeTimeline->state == WallpaperTimelineSection::State::Inactive) {
            currentTimeline->activateTimeline();
        }
    }
}

std::optional<std::shared_ptr<WallpaperTimelineSection>> ScreenPlayTimelineManager::wallpaperSection(const int timelineIndex, const QString timelineIdentifier)
{
    for (const auto& section : m_wallpaperTimelineSectionsList) {
        const bool indexMatches = section->index == timelineIndex;
        const bool timelineIdentifierMatches = section->identifier == timelineIdentifier;
        if (indexMatches && timelineIdentifierMatches) {
            return section;
        }
    }
    qCritical() << "No matching timeline for index:" << timelineIndex << "timelineIdentifier: " << timelineIdentifier;

    return std::nullopt;
}

void ScreenPlayTimelineManager::setSettings(const std::shared_ptr<Settings>& settings)
{
    m_settings = settings;
}

void ScreenPlayTimelineManager::startup()
{
    std::shared_ptr<WallpaperTimelineSection> currentTimeline = findTimelineForCurrentTime();
    if (!currentTimeline) {
        qCritical() << "No current timeline found. There must always be an active timeline.";
        return;
    }
    currentTimeline->activateTimeline();
    m_contentTimer.start();
}

void ScreenPlayTimelineManager::setMonitorListModel(const std::shared_ptr<MonitorListModel>& monitorListModel)
{
    m_monitorListModel = monitorListModel;
}

void ScreenPlayTimelineManager::updateMonitorListModelData(const int selectedTimelineIndex)
{
    if (m_wallpaperTimelineSectionsList.isEmpty()) {
        qCritical() << "No m_wallpaperTimelineSectionsList is empty";
        return;
    }

    auto selectedTimeline = m_wallpaperTimelineSectionsList | std::views::drop(selectedTimelineIndex) | std::views::take(1);
    if (selectedTimeline.empty()) {
        qCritical() << "No selectedTimelineIndex found" << selectedTimelineIndex;
        return;
    }
    std::shared_ptr<WallpaperTimelineSection>& timeline = selectedTimeline.front();

    // Create a set of monitor indices that have active wallpapers
    std::unordered_set<int> activeMonitors;
    for (const auto& activeWallpaper : timeline->activeWallpaperList) {
        for (const auto& monitorIndex : activeWallpaper->monitors()) {
            activeMonitors.insert(monitorIndex);
        }
    }

    for (int i = 0; i < m_monitorListModel->rowCount(); ++i) {
        bool ok;
        const int monitorIndex = m_monitorListModel->data(m_monitorListModel->index(i), (int)MonitorListModel::MonitorRole::Index).toInt(&ok);
        if (!ok) {
            qCritical() << "Invalid monitor index at: " << i;
            return;
        }

        const auto modelIndex = m_monitorListModel->index(0, monitorIndex);

        if (activeMonitors.find(monitorIndex) != activeMonitors.end()) {
            // Monitor has an active wallpaper
            for (const auto& activeWallpaper : timeline->activeWallpaperList) {
                if (activeWallpaper->monitors().contains(monitorIndex)) {
                    const auto previewImg = activeWallpaper->absolutePath() + "/" + activeWallpaper->previewImage();
                    m_monitorListModel->setData(modelIndex, activeWallpaper->appID(), (int)MonitorListModel::MonitorRole::AppID);
                    m_monitorListModel->setData(modelIndex, (int)activeWallpaper->state(), (int)MonitorListModel::MonitorRole::AppState);
                    m_monitorListModel->setData(modelIndex, previewImg, (int)MonitorListModel::MonitorRole::PreviewImage);
                    m_monitorListModel->setData(modelIndex, (int)activeWallpaper->type(), (int)MonitorListModel::MonitorRole::InstalledType);
                    break;
                }
            }
        } else {
            // Reset monitor data to empty values
            m_monitorListModel->setData(modelIndex, "", (int)MonitorListModel::MonitorRole::AppID);
            m_monitorListModel->setData(modelIndex, 0, (int)MonitorListModel::MonitorRole::AppState);
            m_monitorListModel->setData(modelIndex, "", (int)MonitorListModel::MonitorRole::PreviewImage);
            m_monitorListModel->setData(modelIndex, 0, (int)MonitorListModel::MonitorRole::InstalledType);
        }
    }
}
void ScreenPlayTimelineManager::setGlobalVariables(const std::shared_ptr<GlobalVariables>& globalVariables)
{
    m_globalVariables = globalVariables;
}

bool ScreenPlayTimelineManager::deactivateCurrentTimeline()
{
    std::shared_ptr<WallpaperTimelineSection> oldTimeline = findActiveWallpaperTimelineSection();
    if (!oldTimeline) {
        qCritical() << "No active timeline found. There must always be an active timeline.";
        return false;
    }
    oldTimeline->deactivateTimeline();
    return true;
}
/*!
  \brief Change active timeline. We need an extra flags to know if our
         timeline is active and out of time range.
*/
bool ScreenPlayTimelineManager::startTimeline()
{

    std::shared_ptr<WallpaperTimelineSection> newTimelineSection = findTimelineForCurrentTime();
    if (!newTimelineSection) {
        qCritical() << "No new timeline found. There must always be an active timeline.";
        return false;
    }
    if (!newTimelineSection->activateTimeline()) {
        qCritical() << "Unable activate timeline.";
        return false;
    }

    qInfo() << "Timeline switched successfully.";
    return true;
}

/*!
  \brief Update m_wallpaperTimelineSectionsList index based on the startTime;
*/
void ScreenPlayTimelineManager::updateIndices()
{
    // Sort the vector based on startTime
    std::sort(m_wallpaperTimelineSectionsList.begin(), m_wallpaperTimelineSectionsList.end(),
        [](const auto& a, const auto& b) {
            return a->startTime < b->startTime;
        });

    // Update the indices based on new order
    for (int i = 0; i < m_wallpaperTimelineSectionsList.size(); ++i) {
        m_wallpaperTimelineSectionsList[i]->index = i;
    }
}

/*!
  \brief Adds a new timeline at relative position. We always shrink the timeline at the input
         position and append the new one to the left. There must always (lightning) an active
         timeline section.

 */
bool ScreenPlayTimelineManager::addTimelineAt(const int index, const float relativeLinePosition, QString identifier)
{
    // We always get the new endTime
    const QString newStopPosition = Util().getTimeString(relativeLinePosition);
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
    QObject::connect(newTimelineSection.get(), &WallpaperTimelineSection::requestSaveProfiles, this, &ScreenPlayTimelineManager::requestSaveProfiles);
    QObject::connect(newTimelineSection.get(), &WallpaperTimelineSection::activeWallpaperCountChanged, this, &ScreenPlayTimelineManager::activeWallpaperCountChanged);
    newTimelineSection->settings = m_settings;
    newTimelineSection->globalVariables = m_globalVariables;
    newTimelineSection->index = index;
    newTimelineSection->relativePosition = relativeLinePosition;
    newTimelineSection->identifier = identifier;
    newTimelineSection->endTime = newStopPositionTime;
    // In case we do a full reset, we must set the start time manually
    if (m_wallpaperTimelineSectionsList.empty()) {
        newTimelineSection->startTime = QTime::fromString("00:00:00", m_timelineTimeFormat);
    } else {

        // We can use the given index here, because it points
        // the the current item at that index, and we have not yet
        // added our new timelineSection to our list.
        newTimelineSection->startTime = m_wallpaperTimelineSectionsList.at(index)->startTime;
    }

    const bool isLast = (m_wallpaperTimelineSectionsList.length() - 1) == index;
    if (isLast) {
        m_wallpaperTimelineSectionsList.last()->startTime = newTimelineSection->endTime;
    }

    m_wallpaperTimelineSectionsList.append(newTimelineSection);

    updateIndices();
    printTimelines();
    // emit requestSaveProfiles();
    return true;
}

/*!
  \brief Qml function that removes all Timeline sections. Qml then creates
         a new default section.
*/
QCoro::Task<bool> ScreenPlayTimelineManager::removeAllTimlineSections()
{

    // First check if there is any active wallpaper and disable it
    std::shared_ptr<WallpaperTimelineSection> activeTimelineSection = findActiveWallpaperTimelineSection();
    if (activeTimelineSection) {
        // First deactivate so the wallpaper has time to shutdown
        co_await activeTimelineSection->deactivateTimeline();
    }

    // After we have disconnected or timed out we remove the wallpaper
    m_wallpaperTimelineSectionsList.clear();

    // Do not call requestSaveProfiles, because qml will add
    // the default timeline after this function
    co_return true;
}

/*!
  \brief Qml function that removes all wallpaper for the current Timeline section.
*/
QCoro::Task<bool> ScreenPlayTimelineManager::removeAllWallpaperFromActiveTimlineSections()
{

    // First check if there is any active wallpaper and disable it
    std::shared_ptr<WallpaperTimelineSection> activeTimelineSection = findActiveWallpaperTimelineSection();
    if (activeTimelineSection) {
        // First deactivate so the wallpaper has time to shutdown
        activeTimelineSection->deactivateTimeline();
        activeTimelineSection->wallpaperDataList.clear();
    }
    co_return true;
}

/*!
  \brief Qml function that removes all wallpaper for the current Timeline section.
*/
QCoro::Task<bool> ScreenPlayTimelineManager::removeWallpaperAt(const int timelineIndex, const QString timelineIdentifier, const int monitorIndex)
{
    m_contentTimer.stop();
    auto updateTimer = qScopeGuard([this] {
        m_contentTimer.start();
    });

    std::optional<std::shared_ptr<WallpaperTimelineSection>> sectionOpt = wallpaperSection(timelineIndex, timelineIdentifier);
    if (!sectionOpt) {
        qCritical() << "No timeline section for  timelineIndex" << timelineIndex << "timelineIdentifier" << timelineIdentifier << "monitorIndex" << monitorIndex;
        co_return false;
    }

    const bool success = co_await sectionOpt.value()->removeWallpaper(monitorIndex);

    co_return success;
}

/*!
  \brief Removes a timeline at a given index. Expands the timeline next to it
         to fill the space.
*/
bool ScreenPlayTimelineManager::removeTimelineAt(const int index)
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

    const std::shared_ptr<WallpaperTimelineSection>& wallpapterTimelineSection = m_wallpaperTimelineSectionsList.at(index);

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
    if (index == 0) {
        endTime = QTime::fromString("00:00:00", m_timelineTimeFormat);
    } else {
        endTime = m_wallpaperTimelineSectionsList.at(index - 1)->endTime;
    }
    std::shared_ptr<WallpaperTimelineSection> timelineAfter = m_wallpaperTimelineSectionsList.at(index + 1);
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
}

void ScreenPlayTimelineManager::printTimelines()
{
    std::cout << "#############################\n";
    for (std::shared_ptr<WallpaperTimelineSection>& timeline : m_wallpaperTimelineSectionsList) {
        std::cout << timeline->index << ": " << timeline->identifier.toStdString() << "\t"
                  << timeline->relativePosition
                  << " start: " << timeline->startTime.toString().toStdString()
                  << " end: " << timeline->endTime.toString().toStdString() << std::endl;
    }
}

QCoro::Task<bool> ScreenPlayTimelineManager::setWallpaperAtTimelineIndex(
    WallpaperData wallpaperData,
    const int timelineIndex,
    const QString& identifier)
{
    bool found = false;
    for (std::shared_ptr<WallpaperTimelineSection>& timelineSection : m_wallpaperTimelineSectionsList) {
        const bool sameIndex = timelineSection->index == timelineIndex;
        const bool sameIdentifier = timelineSection->identifier == identifier;
        if (sameIndex && sameIdentifier) {
            found = true;

            // Check if we already have a wallpaper at the same position or on one or more of the specified monitors
            auto it = std::find_if(timelineSection->wallpaperDataList.begin(), timelineSection->wallpaperDataList.end(),
                [&wallpaperData](const WallpaperData& existingWallpaper) {
                    return std::any_of(wallpaperData.monitors.begin(), wallpaperData.monitors.end(),
                        [&existingWallpaper](int monitor) {
                            return existingWallpaper.monitors.contains(monitor);
                        });
                });

            if (it != timelineSection->wallpaperDataList.end()) {
                // TODO

                // Overwrite the existing wallpaper
                *it = wallpaperData;
                // TODO: Do not replace but
                co_await timelineSection->deactivateTimeline();
            } else {
                // IMPORTANT: Append the new wallpaper data,
                // but do not start it! The selected timelineSection
                // is not always be the currently running.
                // Besides for this we have m_contentTimer checkActiveWallpaperTimeline()
                timelineSection->wallpaperDataList.push_back(wallpaperData);

                // If the updated timeline section is already active, we
                // need to trigger a activateTimeline by deactivating it first
                if (timelineSection->state == WallpaperTimelineSection::State::Active) {
                    qDebug() << "Deactivate current timeline first";
                    co_await timelineSection->deactivateTimeline();
                }
            }

            break;
        }
    }
    co_return found;
}

QJsonArray ScreenPlayTimelineManager::timelineSections()
{
    QJsonArray sectionPositions;
    for (const std::shared_ptr<WallpaperTimelineSection>& timelineSection : m_wallpaperTimelineSectionsList) {
        sectionPositions.push_back(timelineSection->serialize());
    }
    return sectionPositions;
}

QJsonArray ScreenPlayTimelineManager::timelineWallpaperList()
{
    QJsonArray timelineWallpaperList {};
    for (const std::shared_ptr<WallpaperTimelineSection>& activeTimelineWallpaper : std::as_const(m_wallpaperTimelineSectionsList)) {
        QJsonObject timelineWallpaper;
        timelineWallpaper.insert("startTime", activeTimelineWallpaper->startTime.toString());
        timelineWallpaper.insert("endTime", activeTimelineWallpaper->endTime.toString());
        QJsonArray wallpaper;
        // Every timeline section can have multiple wallpaper
        for (const auto& wallpaperData : activeTimelineWallpaper->wallpaperDataList) {
            wallpaper.append(wallpaperData.serialize());
        }
        timelineWallpaper.insert("wallpaper", wallpaper);

        timelineWallpaperList.append(timelineWallpaper);
    }
    return timelineWallpaperList;
}

int ScreenPlayTimelineManager::selectedTimelineIndex() const
{
    return m_selectedTimelineIndex;
}

void ScreenPlayTimelineManager::setSelectedTimelineIndex(int selectedTimelineIndex)
{
    if (m_selectedTimelineIndex == selectedTimelineIndex)
        return;
    m_selectedTimelineIndex = selectedTimelineIndex;
    emit selectedTimelineIndexChanged(m_selectedTimelineIndex);
}
}
