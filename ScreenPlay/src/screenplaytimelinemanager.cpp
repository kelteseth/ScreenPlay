#include "ScreenPlay/screenplaytimelinemanager.h"
#include "ScreenPlay/screenplaywallpaper.h"
#include "ScreenPlay/wallpaperdata.h"
#include "ScreenPlayCore/util.h"
#include <QString>
#include <QStringList>
#include <QtGlobal>
#include <iostream>
#include <ranges>
#include <set>

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

std::shared_ptr<WallpaperTimelineSection> ScreenPlayTimelineManager::findStartingOrActiveWallpaperTimelineSection()
{
    for (const auto& section : m_wallpaperTimelineSectionsList) {
        if (section->state == WallpaperTimelineSection::State::Starting || section->state == WallpaperTimelineSection::State::Active) {
            return section;
        }
    }
    return nullptr;
}

/*!
  \brief Returns the current active timline. There must always be an active timeline!
*/
std::shared_ptr<WallpaperTimelineSection>
ScreenPlayTimelineManager::findTimelineSectionForCurrentTime()
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

std::shared_ptr<WallpaperTimelineSection> ScreenPlayTimelineManager::findTimelineSection(const int timelineIndex, const QString sectionIdentifier)
{
    for (const auto& section : m_wallpaperTimelineSectionsList) {
        if (section->identifier == sectionIdentifier && section->index == timelineIndex) {
            return section;
        }
    }
    qCritical() << "No active timeline";
    return nullptr;
}

/*!
  \brief Loads a single timeline from the timelineWallpaper array.

        {
        "endTime": "07:10:19",
        "startTime": "02:34:12",
        "wallpaper": [
            {
                [...]
            }
        ]
    },
*/
bool ScreenPlayTimelineManager::addTimelineFromSettings(const QJsonObject& timelineObj)
{
    const QTime startTime = QTime::fromString(timelineObj.value("startTime").toString(), m_timelineTimeFormat);
    const QTime endTime = QTime::fromString(timelineObj.value("endTime").toString(), m_timelineTimeFormat);
    if (startTime > endTime) {
        qCritical() << "Invalid time, start time is later than end time: " << startTime.toString() << endTime.toString();
        return false;
    }

    // If this is basic version and we already have a timeline, ignore additional ones
    if (m_globalVariables->isBasicVersion() && !m_wallpaperTimelineSectionsList.isEmpty()) {
        qInfo() << "Basic version: Ignoring additional timeline sections";
        return true;
    }

    // For basic version, force timeline to span whole day
    if (m_globalVariables->isBasicVersion()) {
        // Always spans 00:00:00 to 23:59:59 for basic version
        QTime defaultStartTime = QTime::fromString("00:00:00", m_timelineTimeFormat);
        QTime defaultEndTime = QTime::fromString("23:59:59", m_timelineTimeFormat);

        if (startTime != defaultStartTime || endTime != defaultEndTime) {
            qInfo() << "Basic version: Enforcing full day timeline span";
            const_cast<QTime&>(startTime) = defaultStartTime;
            const_cast<QTime&>(endTime) = defaultEndTime;
        }
    }

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
    newTimelineSection->relativePosition = m_util.calculateRelativePosition(endTime);
    newTimelineSection->index = m_wallpaperTimelineSectionsList.length();
    newTimelineSection->identifier = m_util.generateRandomString(4);

    qInfo() << newTimelineSection->index
            << newTimelineSection->startTime
            << newTimelineSection->endTime;

    newTimelineSection->init(timelineObj.value("wallpaper").toArray());
    m_wallpaperTimelineSectionsList.append(newTimelineSection);
    return true;
}

/*!
  \brief We always handle the endTimeString, because it is the handle for the
         timeline. The last, default, timeline does not have a handle.
*/
bool ScreenPlayTimelineManager::moveTimelineAt(const int index, const QString identifier, const float relativePosition, QString positionTimeString)
{
    // Q_ASSERT(m_util.getTimeString(relativePosition) == positionTimeString);
    // qDebug() << "Calculated time:" << m_util.getTimeString(relativePosition) << "Provided time:" << positionTimeString << relativePosition;

    m_contentTimer.stop();
    auto updateTimer = qScopeGuard([this] { m_contentTimer.start(); });

    auto& wallpapterTimelineSection = m_wallpaperTimelineSectionsList.at(index);
    const QTime newPositionTime = QTime::fromString(positionTimeString, "hh:mm:ss");
    if (!newPositionTime.isValid()) {
        qCritical() << "Unable to move with invalid time:" << positionTimeString;
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
    // TODO CRASH
    if (index <= timelineCount) {
        auto& wallpapterTimelineSectionNext = m_wallpaperTimelineSectionsList.at(index + 1);
        wallpapterTimelineSectionNext->startTime = newPositionTime;
    }

    // printTimelines();
    emit requestSaveProfiles();
    return true;
}

/*!
    \brief  Checks if we need to display a different wallpaper at the current time. This function is
            only used to handle regular updates. We still have to manually update the timeline,
            when the user alters a timeline WallpaperData, for example by setting a new wallpaper.
            This happens in \sa setWallpaperAtMonitorTimelineIndex(). During this time, the update timer
            that calls this function will be paused.
*/
QCoro::Task<void> ScreenPlayTimelineManager::checkActiveWallpaperTimeline()
{
    // Verify timeline states
    int activeCount = 0;
    for (const auto& section : m_wallpaperTimelineSectionsList) {
        if (section->state == WallpaperTimelineSection::State::Failed) {
            qDebug() << "Timeline" << section->identifier << " failed, removing";
            removeTimelineAt(section->index);
            co_return;
        }
        if (section->state == WallpaperTimelineSection::State::Active) {
            activeCount++;
        } else if (section->state != WallpaperTimelineSection::State::Inactive) {
            qDebug() << "Timeline" << section->identifier << "in transition state:" << section->state;
            co_return;
        }
    }

    if (activeCount > 1) {
        qCritical() << "Invalid timeline states - found" << activeCount << "active timelines, expected 1";
        printTimelines();
        co_return;
    }

    // Find current timelines
    std::shared_ptr<WallpaperTimelineSection> oldRunningTimeline = findActiveWallpaperTimelineSection();
    if (!oldRunningTimeline) {
        qDebug() << "No timeline with state active found.";
        printTimelines();
        co_return;
    }

    std::shared_ptr<WallpaperTimelineSection> newNotStartedTimeline = findTimelineSectionForCurrentTime();
    if (!newNotStartedTimeline) {
        qCritical() << "No current timeline found. There must always be an active timeline.";
        co_return;
    }

    // Default state - timeline hasn't changed
    if (newNotStartedTimeline->index == oldRunningTimeline->index) {
        co_return;
    }

    // Collect all monitored indices
    std::set<int> affectedMonitors;

    // Add monitors from old timeline
    for (const auto& wallpaper : oldRunningTimeline->wallpaperList) {
        for (const auto& monitor : wallpaper->monitors()) {
            affectedMonitors.insert(monitor);
        }
    }

    // Add monitors from new timeline
    for (const auto& data : newNotStartedTimeline->wallpaperData()) {
        for (const auto& monitor : data.monitors()) {
            affectedMonitors.insert(monitor);
        }
    }

    // Set transition states
    oldRunningTimeline->state = WallpaperTimelineSection::State::Closing;
    newNotStartedTimeline->state = WallpaperTimelineSection::State::Starting;

    // Handle each monitor transition
    for (const int monitorIndex : affectedMonitors) {
        auto oldWallpaperOpt = oldRunningTimeline->screenPlayWallpaperByMonitorIndex(monitorIndex);
        auto newWallpaperDataOpt = newNotStartedTimeline->getWallpaperDataForMonitor(monitorIndex);

        // Case 1: No old wallpaper, but new wallpaper needed
        if (!oldWallpaperOpt.has_value() && newWallpaperDataOpt.has_value()) {
            const bool startSuccess = co_await startWallpaper(
                newNotStartedTimeline->index,
                newNotStartedTimeline->identifier,
                QVector<int> { monitorIndex });

            if (!startSuccess) {
                qCritical() << "Failed to start new wallpaper for monitor" << monitorIndex;
                newNotStartedTimeline->state = WallpaperTimelineSection::State::Failed;
                co_return;
            }
            continue;
        }

        // Case 2: Old wallpaper exists, but no new wallpaper needed
        if (oldWallpaperOpt.has_value() && !newWallpaperDataOpt.has_value()) {
            const bool stopSuccess = co_await stopWallpaper(
                oldRunningTimeline->index,
                oldRunningTimeline->identifier,
                QVector<int> { monitorIndex });

            if (!stopSuccess) {
                qCritical() << "Failed to stop old wallpaper for monitor" << monitorIndex;
                // Continue anyway to try handling other monitors
            }
            continue;
        }

        // Case 3: Both old and new wallpapers exist
        if (oldWallpaperOpt.has_value() && newWallpaperDataOpt.has_value()) {
            auto oldWallpaper = oldWallpaperOpt.value();
            auto newWallpaperData = newWallpaperDataOpt.value();

            if (m_util.isSameWallpaperRuntime(oldWallpaper->type(), newWallpaperData.type())) {
                // Can reuse runtime - do live replacement
                const auto oldRunningTimelineWallpaperDataBackup = oldRunningTimeline->getWallpaperDataForMonitor(monitorIndex);
                auto runningScreenPlayWallpaperOpt = oldRunningTimeline->takeScreenPlayWallpaperByMonitorIndex(QVector<int> { monitorIndex });

                if (!runningScreenPlayWallpaperOpt.has_value()) {
                    qCritical() << "Failed to take existing wallpaper for monitor" << monitorIndex;
                    newNotStartedTimeline->state = WallpaperTimelineSection::State::Failed;
                    co_return;
                }

                auto runningScreenPlayWallpaper = runningScreenPlayWallpaperOpt.value();
                runningScreenPlayWallpaper->replaceLive(newWallpaperData);

                if (!newNotStartedTimeline->replaceScreenPlayWallpaperAtMonitorIndex(QVector<int> { monitorIndex }, runningScreenPlayWallpaper)) {
                    qCritical() << "Failed to replace wallpaper for monitor" << monitorIndex;
                    newNotStartedTimeline->state = WallpaperTimelineSection::State::Failed;
                    co_return;
                }

                // Restore the wallpaper in the old timeline
                if (oldRunningTimelineWallpaperDataBackup.has_value()) {
                    co_await setWallpaperAtMonitorTimelineIndex(
                        oldRunningTimelineWallpaperDataBackup.value(),
                        oldRunningTimeline->index,
                        oldRunningTimeline->identifier,
                        QVector<int> { monitorIndex });
                } else {
                    qCritical() << "Failed to restore wallpaper backup for monitor" << monitorIndex;
                }
            } else {
                // Different runtime - must stop old and start new
                const bool removeSuccess = co_await removeWallpaper(
                    oldRunningTimeline->index,
                    oldRunningTimeline->identifier,
                    QVector<int> { monitorIndex });

                if (!removeSuccess) {
                    qCritical() << "Failed to remove old wallpaper for monitor" << monitorIndex;
                    newNotStartedTimeline->state = WallpaperTimelineSection::State::Failed;
                    co_return;
                }

                const bool startSuccess = co_await startWallpaper(
                    newNotStartedTimeline->index,
                    newNotStartedTimeline->identifier,
                    QVector<int> { monitorIndex });

                if (!startSuccess) {
                    qCritical() << "Failed to start new wallpaper for monitor" << monitorIndex;
                    newNotStartedTimeline->state = WallpaperTimelineSection::State::Failed;
                    co_return;
                }
            }
        }
    }

    // Update final states
    oldRunningTimeline->state = WallpaperTimelineSection::State::Inactive;
    newNotStartedTimeline->state = WallpaperTimelineSection::State::Active;

    printTimelines();
}

std::optional<std::shared_ptr<WallpaperTimelineSection>> ScreenPlayTimelineManager::wallpaperSection(const int timelineIndex, const QString& sectionIdentifier)
{
    for (const auto& section : m_wallpaperTimelineSectionsList) {
        const bool indexMatches = section->index == timelineIndex;
        const bool sectionIdentifierMatches = section->identifier == sectionIdentifier;
        if (indexMatches && sectionIdentifierMatches) {
            return { section };
        }
    }
    qCritical() << "No matching timeline for index:" << timelineIndex << "sectionIdentifier: " << sectionIdentifier;

    return std::nullopt;
}

std::optional<std::shared_ptr<ScreenPlayWallpaper>> ScreenPlayTimelineManager::screenPlayWallpaperAt(
    const int timelineIndex,
    const QString timelineIdentifier,
    const QVector<int> monitorIndex)
{
    using enum WallpaperTimelineSection::State;
    auto timelineSectionOpt = wallpaperSection(timelineIndex, timelineIdentifier);
    if (!timelineSectionOpt.has_value()) {
        return std::nullopt;
    }
    auto timelineSection = timelineSectionOpt.value();
    return timelineSection->screenPlayWallpaperByMonitorIndex(monitorIndex.first());
}

void ScreenPlayTimelineManager::validateTimelineSections() const
{
    if (m_wallpaperTimelineSectionsList.empty()) {
        return; // Nothing to validate if the list is empty
    }

    // Check if the first section starts at 00:00:00
    if (m_wallpaperTimelineSectionsList.front()->startTime != QTime(0, 0, 0)) {
        printTimelines();
        Q_ASSERT_X(false, "validateTimelineSections", "First timeline section must start at 00:00:00");
    }

    for (size_t i = 0; i < m_wallpaperTimelineSectionsList.size() - 1; ++i) {
        const auto& current = m_wallpaperTimelineSectionsList[i];
        const auto& next = m_wallpaperTimelineSectionsList[i + 1];

        // Check that start time is before end time for each section
        if (!(current->startTime < current->endTime)) {
            printTimelines();
            Q_ASSERT_X(false, "validateTimelineSections", "Start time must be before end time");
        }

        // Check that end time of current section matches start time of next section
        if (current->endTime != next->startTime) {
            printTimelines();
            Q_ASSERT_X(false, "validateTimelineSections", "End time of current section must match start time of next section");
        }

        // Check that indices are consecutive and match their position in the list
        if (current->index != static_cast<int>(i)) {
            printTimelines();
            Q_ASSERT_X(false, "validateTimelineSections", "Timeline section index must match its position in the list");
        }

        // Check that relative positions are in ascending order
        if (!(current->relativePosition < next->relativePosition)) {
            printTimelines();
            Q_ASSERT_X(false, "validateTimelineSections", "Relative positions must be in ascending order");
        }
    }

    // Check the last section
    const auto& last = m_wallpaperTimelineSectionsList.back();
    if (!(last->startTime < last->endTime)) {
        printTimelines();
        Q_ASSERT_X(false, "validateTimelineSections", "Start time must be before end time for the last section");
    }

    if (last->index != static_cast<int>(m_wallpaperTimelineSectionsList.size() - 1)) {
        printTimelines();
        Q_ASSERT_X(false, "validateTimelineSections", "Last timeline section index must match its position");
    }

    // Check that the last section ends at 23:59:59
    if (last->endTime != QTime(23, 59, 59)) {
        printTimelines();
        Q_ASSERT_X(false, "validateTimelineSections", "Last timeline section must end at 23:59:59");
    }
}

void ScreenPlayTimelineManager::setSettings(const std::shared_ptr<Settings>& settings)
{
    m_settings = settings;
}

QCoro::Task<void> ScreenPlayTimelineManager::startup()
{
    std::shared_ptr<WallpaperTimelineSection> currentTimeline = findTimelineSectionForCurrentTime();
    if (!currentTimeline) {
        qCritical() << "No current timeline found. There must always be an active timeline.";
        co_return;
    }
    co_await activateTimeline(currentTimeline->index, currentTimeline->identifier);
    printTimelines();
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

    const auto& timeline = m_wallpaperTimelineSectionsList[selectedTimelineIndex];
    auto monitors = MonitorListModel::getSystemMonitors();

    // Iterate through each monitor for wallpaper data in case the timeline is not
    // the active one
    for (const auto& monitor : monitors) {
        QModelIndex modelIndex = m_monitorListModel->index(monitor.m_monitorIndex);
        auto wallpaperDataOpt = timeline->getWallpaperDataForMonitor(monitor.m_monitorIndex);
        if (wallpaperDataOpt.has_value()) {
            const auto wallpaperData = wallpaperDataOpt.value();
            const auto previewImg = wallpaperData.absolutePath() + "/"
                + wallpaperData.previewImage();
            m_monitorListModel->setData(modelIndex,
                previewImg,
                (int)MonitorListModel::MonitorRole::PreviewImage);
            m_monitorListModel->setData(modelIndex,
                (int)wallpaperData.type(),
                (int)MonitorListModel::MonitorRole::InstalledType);
        } else {
            m_monitorListModel->setData(modelIndex,
                "",
                (int)MonitorListModel::MonitorRole::PreviewImage);
            m_monitorListModel->setData(modelIndex,
                0,
                (int)MonitorListModel::MonitorRole::InstalledType);
        }
    }

    auto activeTimelineSection = findTimelineSectionForCurrentTime();
    const bool selectedTimelineIsActive = activeTimelineSection->index == selectedTimelineIndex;
    if (!selectedTimelineIsActive)
        return;

    // Now iterate again for active connection states for the running
    // wallpaper
    for (const auto& monitor : monitors) {
        QModelIndex modelIndex = m_monitorListModel->index(monitor.m_monitorIndex);
        auto activeWallpaperOpt = timeline->screenPlayWallpaperByMonitorIndex(monitor.m_monitorIndex);

        if (activeWallpaperOpt.has_value()) {
            auto activeWallpaper = activeWallpaperOpt.value();
            m_monitorListModel->setData(modelIndex,
                activeWallpaper->appID(),
                (int)MonitorListModel::MonitorRole::AppID);
            m_monitorListModel->setData(modelIndex,
                (int)activeWallpaper->state(),
                (int)MonitorListModel::MonitorRole::AppState);
        } else {
            m_monitorListModel->setData(modelIndex,
                "",
                (int)MonitorListModel::MonitorRole::AppID);
            m_monitorListModel->setData(modelIndex,
                (int)ScreenPlayEnums::AppState::NotSet,
                (int)MonitorListModel::MonitorRole::AppState);
        }
    }
}

void ScreenPlayTimelineManager::setGlobalVariables(const std::shared_ptr<GlobalVariables>& globalVariables)
{
    m_globalVariables = globalVariables;
}

const std::vector<WallpaperData> ScreenPlayTimelineManager::wallpaperData() const
{
    std::vector<WallpaperData> wallpaperDataFlatList;
    for (auto& timelineSection : m_wallpaperTimelineSectionsList) {
        for (auto& screenPlayWallpaper : timelineSection->wallpaperList) {
            wallpaperDataFlatList.push_back(screenPlayWallpaper->wallpaperData());
        }
    }

    return wallpaperDataFlatList;
}

/*!
  \brief Update m_wallpaperTimelineSectionsList index based on the startTime;
*/
void ScreenPlayTimelineManager::sortAndUpdateIndices()
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
    validateTimelineSections();
}

/*!
  \brief Adds a new timeline at relative position. We always shrink the timeline at the input
         position and append the new one to the left. There must always (lightning) an active
         timeline section.

 */
bool ScreenPlayTimelineManager::addTimelineAt(const int index, const float relativeLinePosition, QString identifier)
{
    m_contentTimer.stop();
    auto updateTimer = qScopeGuard([this] { m_contentTimer.start(); });

    // We always get the new endTime
    const QString newStopPosition = m_util.getTimeString(relativeLinePosition);
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

    if (m_wallpaperTimelineSectionsList.empty()) {
        newTimelineSection->startTime = QTime::fromString("00:00:00", m_timelineTimeFormat);
        m_wallpaperTimelineSectionsList.push_back(newTimelineSection);
    } else {
        // Find the correct position to insert the new section using C++23 ranges
        auto insertPosition = std::ranges::find_if(m_wallpaperTimelineSectionsList, [&](const auto& section) {
            return section->relativePosition > relativeLinePosition;
        });

        // Set the start time of the new section
        newTimelineSection->startTime = [&]() {
            if (insertPosition != m_wallpaperTimelineSectionsList.begin()) {
                return std::prev(insertPosition)->get()->endTime;
            }
            return QTime::fromString("00:00:00", m_timelineTimeFormat);
        }();

        // Adjust the start time of the next section (if it exists)
        if (insertPosition != m_wallpaperTimelineSectionsList.end()) {
            (*insertPosition)->startTime = newTimelineSection->endTime;
        }

        // Insert the new section at the correct position
        m_wallpaperTimelineSectionsList.insert(insertPosition, newTimelineSection);

        /* ASCII representation of the insertion process:
         *          * Case 1: Inserting at the beginning
         * Before:   |----A----|----B----|----C----|
         *           ^
         *        insertPosition
         *
         * After:    |--New--|----A----|----B----|----C----|
         *           ^       ^
         *           |       A.startTime = New.endTime
         *           New.startTime = 00:00:00
         *          * Case 2: Inserting in the middle
         * Before:   |----A----|----B----|----C----|
         *                     ^
         *                  insertPosition
         *
         * After:    |----A----|--New--|----B----|----C----|
         *                     ^       ^
         *                     |       B.startTime = New.endTime
         *                     New.startTime = A.endTime
         *          * Case 3: Inserting at the end
         * Before:   |----A----|----B----|----C----|
         *                                         ^
         *                                      insertPosition (end())
         *
         * After:    |----A----|----B----|----C----|--New--|
         *                                         ^
         *                                         New.startTime = C.endTime
         *          * Legend:
         * |     : Represents the start/end of a timeline section
         * ----  : Represents the duration of a timeline section
         * A,B,C : Existing timeline sections
         * New   : Newly inserted timeline section
         * ^     : Indicates the insertPosition
         * -->   : Indicates time adjustment
         */
    }

    sortAndUpdateIndices();
    printTimelines();
    emit requestSaveProfiles();
    return true;
}

/*!
  \brief Qml function that removes all Timeline sections. Qml then creates
         a new default section.
*/
QCoro::Task<bool> ScreenPlayTimelineManager::removeAllTimlineSections()
{
    m_contentTimer.stop();
    auto updateTimer = qScopeGuard([this] { m_contentTimer.start(); });

    // First check if there is any active wallpaper and disable it
    std::shared_ptr<WallpaperTimelineSection> activeTimelineSection = findActiveWallpaperTimelineSection();
    if (activeTimelineSection) {
        // First deactivate so the wallpaper has time to shutdown
        co_await deactivateTimeline(activeTimelineSection->index, activeTimelineSection->identifier);
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
        co_await deactivateTimeline(activeTimelineSection->index, activeTimelineSection->identifier);
    }
    co_return true;
}

/*!
  \brief Qml function that removes a wallpaper from  a Timelinesection.
*/
QCoro::Task<bool> ScreenPlayTimelineManager::removeWallpaperAt(const int timelineIndex, const QString sectionIdentifier, const int monitorIndex)
{
    m_contentTimer.stop();
    auto updateTimer = qScopeGuard([this] { m_contentTimer.start(); });

    QVector<int> monitorIndexList = { monitorIndex }; // TODO
    const bool success = co_await removeWallpaper(timelineIndex, sectionIdentifier, monitorIndexList);
    co_return success;
}

/*!
  \brief Removes a timeline at a given index. Expands the timeline next to it
         to fill the space.
*/
bool ScreenPlayTimelineManager::removeTimelineAt(const int index)
{
    m_contentTimer.stop();
    auto updateTimer = qScopeGuard([this] { m_contentTimer.start(); });
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
        sortAndUpdateIndices();
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

    // Check if there is a next timeline
    if (index + 1 >= timelineCount) {
        qCritical() << "Cannot remove the last timeline section";
        return false;
    }

    std::shared_ptr<WallpaperTimelineSection> timelineAfter = m_wallpaperTimelineSectionsList.at(index + 1);
    //    before      remove     <- expand
    // |-----------|-----------|-----------|
    //       0          1           2
    // Now when removing timeline at index 1, the next (after)
    // wallpaper gets the remaining space
    timelineAfter->startTime = endTime;
    m_wallpaperTimelineSectionsList.removeAt(index);
    sortAndUpdateIndices();
    printTimelines();

    return true;
}

QJsonArray ScreenPlayTimelineManager::timelineSections()
{
    QJsonArray sectionPositions;
    for (const std::shared_ptr<WallpaperTimelineSection>& timelineSection : m_wallpaperTimelineSectionsList) {
        sectionPositions.push_back(timelineSection->serialize());
    }
    return sectionPositions;
}

void ScreenPlayTimelineManager::printTimelines() const
{
    QString out = "\n";
    QTime currentTime = QTime::currentTime();
    for (const std::shared_ptr<WallpaperTimelineSection>& timeline : m_wallpaperTimelineSectionsList) {
        QString runningIndicator = timeline->containsTime(currentTime) ? "🟢" : "🔴";
        out += runningIndicator + " Timeline: " + QString::number(timeline->index) + ": "
            + timeline->identifier + "\t"
            + QString::number(timeline->relativePosition) + "\t"
            + QVariant::fromValue(timeline->state).toString() + "\t\t"
            + "wallpaper data count: " + QString::number(timeline->wallpaperData().size())
            + " start: " + timeline->startTime.toString() + " "
            + " end: " + timeline->endTime.toString() + "\n";

        if (timeline->wallpaperList.size() > 0) {
            for (const auto& wallpaper : timeline->wallpaperList) {
                const QString ptr = QString::number(reinterpret_cast<quintptr>(wallpaper.get()), 16);
                out += "   └─ 🖼️ ACTIVE WALLPAPER -> Monitor " + QString::number(wallpaper->monitors().first())
                    + " " + QVariant::fromValue(wallpaper->state()).toString() + "\t\t"
                    + "0x" + ptr
                    + " - " + wallpaper->absolutePath() + "\n";
            }
        }
        if (timeline->wallpaperData().size() > 0) {
            for (const auto& wallpaperData : timeline->wallpaperData()) {
                out += "   └─ 💾 WALLPAPER DATA   -> Monitor " + QString::number(wallpaperData.monitors().first()) + "\t\t"
                    + " - " + wallpaperData.absolutePath() + "\n";
            }
        }
    }
    qDebug().noquote() << out;
}

/*!
 * \brief ScreenPlayTimelineManager::setValueAtMonitorTimelineIndex
 *        sets the given key, value at the selected wallpaper.
 *        Note:
 * \return
 */
QCoro::Task<bool> ScreenPlayTimelineManager::setValueAtMonitorTimelineIndex(
    const int monitorIndex,
    const int timelineIndex,
    const QString sectionIdentifier,
    const QString& key,
    const QVariant& value,
    const QString& category)
{
    auto wallpaperSectionOpt = wallpaperSection(timelineIndex, sectionIdentifier);
    if (!wallpaperSectionOpt.has_value())
        co_return false;

    auto wallpaperSection = wallpaperSectionOpt.value();

    // Now set the value in the ScreenPlayWallpaper class so it does
    // get propagated to the running wallpaper.
    for (auto& activeWallpaper : wallpaperSection->wallpaperList) {
        if (activeWallpaper->monitors().contains(monitorIndex)) {
            const auto success = activeWallpaper->setWallpaperValue(key, value);
            co_return success;
        }
    }

    co_return false;
}

QCoro::Task<bool> ScreenPlayTimelineManager::updateCurrentlyActiveTimeline(
    WallpaperData wallpaperData,
    const int timelineIndex,
    const QString& sectionIdentifier)
{

    // Current timeline with the state active
    std::shared_ptr<WallpaperTimelineSection> activeTimeline = findActiveWallpaperTimelineSection();
    if (activeTimeline->index != timelineIndex || activeTimeline->identifier != sectionIdentifier) {
        co_return false;
    }

    // for (auto& activeWallpaper : activeTimeline->wallpaperList) {
    //     if (activeWallpaper->monitors() == wallpaperData.monitors()) {

    //         // Godot is a different runtime then all other wallpaper
    //         if (m_util.isSameWallpaperRuntime(activeWallpaper->type(), wallpaperData.type())) {
    //             // Current wallpaper will simply replace the content
    //             // in the currently run wallpaper
    //             activeWallpaper->replace(wallpaperData);
    //         } else {
    //             const bool removeWallpapersuccess = co_await activeTimeline->removeWallpaper(wallpaperData.monitors().first());
    //             if (!removeWallpapersuccess)
    //                 co_return false;
    //             const bool startWallpapersuccess = co_await activeTimeline->startWallpaper(wallpaperData);
    //             if (!startWallpapersuccess)
    //                 co_return false;
    //         }
    //     }
    // }

    co_return true;
}

QCoro::Task<bool> ScreenPlayTimelineManager::setWallpaperAtMonitorActiveTimelineIndex(
    WallpaperData wallpaperData,
    const int timelineIndex,
    const QString& sectionIdentifier,
    const QVector<int> monitorIndex)
{
    m_contentTimer.stop();
    auto updateTimer = qScopeGuard([this] {
        m_contentTimer.start();
    });

    // Do not use findActiveWallpaperTimelineSection here, because this function can also
    // be called when we are in transition from one timeline to the next.
    std::shared_ptr<WallpaperTimelineSection> oldRunningTimeline = findTimelineSectionForCurrentTime();
    if (!oldRunningTimeline) {
        co_return false;
    }
    auto newNotStartedTimelineOpt = wallpaperSection(timelineIndex, sectionIdentifier);
    if (!newNotStartedTimelineOpt.has_value()) {
        co_return false;
    }
    auto newNotStartedTimeline = newNotStartedTimelineOpt.value();
    auto newNotStartedWallpaperOpt = screenPlayWallpaperAt(timelineIndex, sectionIdentifier, wallpaperData.monitors());

    // When we have no wallpaper active, we can simply add and start
    if (!newNotStartedWallpaperOpt.has_value()) {
        newNotStartedTimeline->addWallpaper(wallpaperData);
        const bool startWallpapersuccess = co_await startWallpaper(
            timelineIndex,
            sectionIdentifier,
            monitorIndex);
        co_return startWallpapersuccess;
    }

    auto newNotStartedWallpaper = newNotStartedWallpaperOpt.value();

    for (auto& activeWallpaper : oldRunningTimeline->wallpaperList) {
        const auto monitorIndex = activeWallpaper->monitors().first();
        auto newNotStartedWallpaperDataOpt = newNotStartedTimeline->getWallpaperDataForMonitor(monitorIndex);
        if (!newNotStartedWallpaperDataOpt.has_value()) {
            continue;
        }
        auto newNotStartedWallpaperData = newNotStartedWallpaperDataOpt.value();

        // Godot is a different runtime then all other wallpaper
        if (m_util.isSameWallpaperRuntime(activeWallpaper->type(), newNotStartedWallpaperData.type())) {
            // Current wallpaper will simply replace the content
            // in the currently run wallpaper
            const auto oldRunningTimelineWallpaperDataBackup = oldRunningTimeline->getWallpaperDataForMonitor(activeWallpaper->monitors().first());
            auto runningScreenPlayWallpaperOpt = oldRunningTimeline->takeScreenPlayWallpaperByMonitorIndex(activeWallpaper->monitors());
            if (!runningScreenPlayWallpaperOpt.has_value()) {
                qCritical() << "No takeScreenPlayWallpaperByMonitorIndex";
                co_return false;
            }
            auto runningScreenPlayWallpaper = runningScreenPlayWallpaperOpt.value();
            runningScreenPlayWallpaper->replaceLive(newNotStartedWallpaperData);
            if (!newNotStartedTimeline->replaceScreenPlayWallpaperAtMonitorIndex(activeWallpaper->monitors(), runningScreenPlayWallpaper)) {
                qCritical() << "No replaceScreenPlayWallpaperAtMonitorIndex";
                co_return false;
            }
            co_return true;
        } else {
            const bool removeWallpapersuccess = co_await removeWallpaper(
                oldRunningTimeline->index,
                oldRunningTimeline->identifier,
                activeWallpaper->monitors());
            if (!removeWallpapersuccess)
                co_return false;
            const bool startWallpapersuccess = co_await startWallpaper(
                newNotStartedTimeline->index,
                newNotStartedTimeline->identifier,
                activeWallpaper->monitors());
            if (!startWallpapersuccess)
                co_return false;
        }
    }
    co_return true;
}

QCoro::Task<bool> ScreenPlayTimelineManager::setWallpaperAtMonitorInactiveTimelineIndex(
    WallpaperData wallpaperData,
    const int timelineIndex,
    const QString& sectionIdentifier,
    const QVector<int> monitorIndex)
{
    auto screenPlayWallpaperOpt = screenPlayWallpaperAt(timelineIndex, sectionIdentifier, monitorIndex);
    const bool doesWallpaperAlreadyExistAtThisMonitor = screenPlayWallpaperOpt.has_value();
    if (doesWallpaperAlreadyExistAtThisMonitor) {
        screenPlayWallpaperOpt.value()->setWallpaperData(wallpaperData);
    } else {
        auto timelineSection = findTimelineSection(timelineIndex, sectionIdentifier);
        if (!timelineSection)
            co_return false;
        timelineSection->addWallpaper(wallpaperData);
    }
    co_return true;
}

QCoro::Task<bool> ScreenPlayTimelineManager::setWallpaperAtMonitorTimelineIndex(
    WallpaperData wallpaperData,
    const int timelineIndex,
    const QString& sectionIdentifier,
    const QVector<int> monitorIndex)
{
    auto currentTimeline = findTimelineSectionForCurrentTime();
    if (!currentTimeline) {
        co_return false;
    }
    auto selectedTimelineOpt = findTimelineSection(timelineIndex, sectionIdentifier);
    if (!selectedTimelineOpt)
        co_return false;

    if (currentTimeline == selectedTimelineOpt) {
        const auto success = co_await setWallpaperAtMonitorActiveTimelineIndex(wallpaperData, timelineIndex, sectionIdentifier, monitorIndex);
        co_return success;
    } else {
        const auto success = co_await setWallpaperAtMonitorInactiveTimelineIndex(wallpaperData, timelineIndex, sectionIdentifier, monitorIndex);
        co_return success;
    }
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
        for (const auto& wallpaperData : activeTimelineWallpaper->wallpaperData()) {
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

// Start all ScreenPlayWallpaper processes of this current timeline
QCoro::Task<bool> ScreenPlayTimelineManager::activateTimeline(const int timelineIndex, const QString timelineIdentifier)
{
    using enum WallpaperTimelineSection::State;
    auto timelineSectionOpt = wallpaperSection(timelineIndex, timelineIdentifier);
    if (!timelineSectionOpt.has_value()) {
        co_return false;
    }
    auto timelineSection = timelineSectionOpt.value();
    qDebug() << "Attempting timeline activation:" << timelineIndex << timelineIdentifier << "current state:" << timelineSection->state;
    if (timelineSection->state == WallpaperTimelineSection::State::Closing) {
        qDebug() << "Timeline already Closing:" << timelineIndex << timelineIdentifier;
        co_return true;
    }
    // Allow activation if we're Inactive OR if we're already Starting
    if (timelineSection->state == Active) {
        qDebug() << "Timeline already active:" << timelineIndex << timelineIdentifier;
        co_return true; // Already active is a success case
    }

    if (timelineSection->state == Starting) {
        qDebug() << "Timeline already starting:" << timelineIndex << timelineIdentifier;
        co_return true; // Already starting is a success case
    }

    if (timelineSection->state != Inactive && timelineSection->state != Failed) {
        qCritical() << "Cannot activate timeline in state:" << timelineSection->state;
        co_return false;
    }

    timelineSection->state = Starting;
    qDebug() << "Activate timeline:" << timelineIndex
             << timelineIdentifier
             << timelineSection->relativePosition
             << wallpaperData().size();

    bool allWallpapersStarted = true;
    for (auto& wallpaper : timelineSection->wallpaperList) {
        const bool startSuccess = co_await startWallpaper(wallpaper);
        if (!startSuccess) {
            qCritical() << "Failed to start wallpaper in timeline:" << timelineIdentifier;
            allWallpapersStarted = false;
            break;
        }
    }

    if (allWallpapersStarted) {
        timelineSection->state = Active;
        qDebug() << "Timeline activated successfully:" << timelineIndex << timelineIdentifier;
        co_return true;
    } else {
        timelineSection->state = Failed; // Instead of Inactive
        qCritical() << "Timeline activation failed:" << timelineIndex << timelineIdentifier;
        co_return false;
    }
}

QCoro::Task<bool> ScreenPlayTimelineManager::deactivateTimeline(const int timelineIndex, const QString timelineIdentifier)
{
    using enum WallpaperTimelineSection::State;
    auto timelineSectionOpt = wallpaperSection(timelineIndex, timelineIdentifier);
    if (!timelineSectionOpt.has_value()) {
        co_return false;
    }
    auto timelineSection = timelineSectionOpt.value();
    if (timelineSection->state != Active) {
        qCritical() << " timeline:" << timelineIndex << timelineIdentifier << "is already active with state: " << timelineSection->state;
        co_return false;
    }

    timelineSection->state = Closing;
    if (timelineSection->wallpaperList.empty()) {
        timelineSection->state = Inactive;
        co_return true;
    }
    for (auto& activeWallpaper : timelineSection->wallpaperList) {
        const bool success = co_await activeWallpaper->close();
        if (!success) {
            qCritical() << " Unable to close activeWallpaper:" << activeWallpaper->absolutePath();
        }
    }
    timelineSection->wallpaperList.clear();
    timelineSection->state = Inactive;
    co_return false;
}

QCoro::Task<bool> ScreenPlayTimelineManager::stopWallpaper(const int timelineIndex, const QString timelineIdentifier, const QVector<int> monitorIndex)
{
    auto timelineSectionOpt = wallpaperSection(timelineIndex, timelineIdentifier);
    if (!timelineSectionOpt.has_value()) {
        co_return false;
    }
    auto timelineSection = timelineSectionOpt.value();
    std::shared_ptr<ScreenPlayWallpaper> wallpaperToClose;
    auto wallpaperIt = std::find_if(timelineSection->wallpaperList.begin(), timelineSection->wallpaperList.end(),
        [monitorIndex](const auto& screenPlayWallpaper) {
            return screenPlayWallpaper && screenPlayWallpaper->monitors() == monitorIndex;
        });

    if (wallpaperIt != timelineSection->wallpaperList.end()) {
        wallpaperToClose = *wallpaperIt;
    }
    if (!wallpaperToClose)
        co_return false;
    const bool success = co_await wallpaperToClose->close();
    co_return success;
}

// Remove the running wallpaper and the corresponding wallpaperData!
QCoro::Task<bool> ScreenPlayTimelineManager::removeWallpaper(const int timelineIndex, const QString timelineIdentifier, const QVector<int> monitorIndex)
{
    auto timelineSectionOpt = wallpaperSection(timelineIndex, timelineIdentifier);
    if (!timelineSectionOpt.has_value()) {
        co_return false;
    }
    auto timelineSection = timelineSectionOpt.value();
    // IMPORTANT: First find and store the wallpaper we need to close
    // before modifying any containers
    std::shared_ptr<ScreenPlayWallpaper> wallpaperToClose;
    auto wallpaperIt = std::find_if(timelineSection->wallpaperList.begin(), timelineSection->wallpaperList.end(),
        [monitorIndex](const auto& screenPlayWallpaper) {
            return screenPlayWallpaper && screenPlayWallpaper->monitors() == monitorIndex;
        });

    if (wallpaperIt != timelineSection->wallpaperList.end()) {
        wallpaperToClose = *wallpaperIt;
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
        timelineSection->wallpaperList.erase(
            std::remove_if(timelineSection->wallpaperList.begin(), timelineSection->wallpaperList.end(),
                [&wallpaperToClose](const auto& wp) {
                    return wp == wallpaperToClose;
                }),
            timelineSection->wallpaperList.end());
    }

    timelineSection->updateActiveWallpaperCounter();
    co_return true;
}

// QCoro::Task<bool> ScreenPlayTimelineManager::removeWallpaper(std::shared_ptr<ScreenPlayWallpaper> screenPlayWallpaper)
// {
// }

QCoro::Task<bool> ScreenPlayTimelineManager::startWallpaper(const int timelineIndex, const QString timelineIdentifier, const QVector<int> monitorIndex)
{
    auto screenPlayWallpaperOpt = screenPlayWallpaperAt(timelineIndex, timelineIdentifier, monitorIndex);
    if (!screenPlayWallpaperOpt.has_value()) {
        co_return false;
    }
    auto screenPlayWallpaper = screenPlayWallpaperOpt.value();
    const bool success = co_await startWallpaper(screenPlayWallpaper);
    co_return success;
}

QCoro::Task<bool> ScreenPlayTimelineManager::startWallpaper(std::shared_ptr<ScreenPlayWallpaper> screenPlayWallpaper)
{
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
            // timelineSection->updateActiveWallpaperCounter(); TODO?
            co_return true;
        }
    }
    co_return false;
}

}
