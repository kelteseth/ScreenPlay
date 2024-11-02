#include "ScreenPlay/screenplaytimelinemanager.h"
#include "ScreenPlay/screenplaywallpaper.h"
#include "ScreenPlay/wallpaperdata.h"
#include "ScreenPlayUtil/util.h"
#include <QString>
#include <QStringList>
#include <QtGlobal>
#include <iostream>
#include <ranges>

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

std::shared_ptr<WallpaperTimelineSection> ScreenPlayTimelineManager::findTimelineSection(
    const int monitorIndex, const int timelineIndex, const QString sectionIdentifier)
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

    const auto wallpaperList = timelineObj.value("wallpaper").toArray();
    for (auto& wallpaper : wallpaperList) {
        std::optional<WallpaperData> wallpaperDataOpt = WallpaperData::loadTimelineWallpaperConfig(wallpaper.toObject());
        if (!wallpaperDataOpt.has_value())
            return false;
        newTimelineSection->wallpaperDataList.push_back(wallpaperDataOpt.value());
    }

    newTimelineSection->index = m_wallpaperTimelineSectionsList.length();
    newTimelineSection->identifier = m_util.generateRandomString(4);

    qInfo() << newTimelineSection->index
            << newTimelineSection->startTime
            << newTimelineSection->endTime;

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
        std::shared_ptr<WallpaperTimelineSection> currentTimeline
            = findTimelineSectionForCurrentTime();
        if (!currentTimeline) {
            qCritical() << "No current timeline found. There must always be an active timeline.";
        }
        currentTimeline->activateTimeline();
        return;
    }

    // Current timline that should be active, based on the current time
    std::shared_ptr<WallpaperTimelineSection> currentTimeline = findTimelineSectionForCurrentTime();
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

void ScreenPlayTimelineManager::startup()
{
    std::shared_ptr<WallpaperTimelineSection> currentTimeline = findTimelineSectionForCurrentTime();
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
    const auto& timeline = m_wallpaperTimelineSectionsList[selectedTimelineIndex];
    const bool isTimelineActive = !timeline->activeWallpaperList.empty();

    auto updateActiveMonitor = [&](int monitorIndex, const QModelIndex& modelIndex) -> bool {
        for (const auto& wallpaper : timeline->activeWallpaperList) {
            if (wallpaper->monitors().contains(monitorIndex)) {
                const auto& wallpaperData = wallpaper->wallpaperData();
                const auto previewImg = wallpaperData.absolutePath() + "/"
                    + wallpaperData.previewImage();
                m_monitorListModel->setData(modelIndex, wallpaper->appID(), (int)MonitorListModel::MonitorRole::AppID);
                m_monitorListModel->setData(modelIndex, (int)wallpaper->state(), (int)MonitorListModel::MonitorRole::AppState);
                m_monitorListModel->setData(modelIndex, previewImg, (int)MonitorListModel::MonitorRole::PreviewImage);
                m_monitorListModel->setData(modelIndex,
                    (int)wallpaperData.type(),
                    (int)MonitorListModel::MonitorRole::InstalledType);
                return true;
            }
        }
        return false;
    };

    auto updatePlannedMonitor = [&](int monitorIndex, const QModelIndex& modelIndex) -> bool {
        for (const auto& wallpaperData : timeline->wallpaperDataList) {
            if (wallpaperData.monitors().contains(monitorIndex)) {
                const auto previewImg = wallpaperData.absolutePath() + "/"
                    + wallpaperData.previewImage();
                m_monitorListModel->setData(modelIndex,
                    wallpaperData.file(),
                    (int)MonitorListModel::MonitorRole::AppID);
                m_monitorListModel->setData(modelIndex, (int)timeline->state, (int)MonitorListModel::MonitorRole::AppState);
                m_monitorListModel->setData(modelIndex, previewImg, (int)MonitorListModel::MonitorRole::PreviewImage);
                m_monitorListModel->setData(modelIndex,
                    (int)wallpaperData.type(),
                    (int)MonitorListModel::MonitorRole::InstalledType);
                return true;
            }
        }
        return false;
    };

    auto resetMonitorData = [&](const QModelIndex& modelIndex) {
        m_monitorListModel->setData(modelIndex, "", (int)MonitorListModel::MonitorRole::AppID);
        m_monitorListModel->setData(modelIndex, 0, (int)MonitorListModel::MonitorRole::AppState);
        m_monitorListModel->setData(modelIndex, "", (int)MonitorListModel::MonitorRole::PreviewImage);
        m_monitorListModel->setData(modelIndex, 0, (int)MonitorListModel::MonitorRole::InstalledType);
    };

    for (int i = 0; i < m_monitorListModel->rowCount(); ++i) {
        const int monitorIndex = m_monitorListModel->data(m_monitorListModel->index(i), (int)MonitorListModel::MonitorRole::MonitorIndex).toInt();
        const auto modelIndex = m_monitorListModel->index(i, 0);

        bool updated = isTimelineActive
            ? updateActiveMonitor(monitorIndex, modelIndex)
            : updatePlannedMonitor(monitorIndex, modelIndex);

        if (!updated) {
            resetMonitorData(modelIndex);
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
    std::shared_ptr<WallpaperTimelineSection> currentTimeline = findTimelineSectionForCurrentTime();
    if (!currentTimeline) {
        qCritical() << "No new timeline found. There must always be an active timeline.";
        return false;
    }
    if (!currentTimeline->activateTimeline()) {
        qCritical() << "Unable activate timeline.";
        return false;
    }

    qInfo() << "Timeline switched successfully.";
    return true;
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
QCoro::Task<bool> ScreenPlayTimelineManager::removeWallpaperAt(const int timelineIndex, const QString sectionIdentifier, const int monitorIndex)
{
    m_contentTimer.stop();
    auto updateTimer = qScopeGuard([this] {
        m_contentTimer.start();
    });

    std::optional<std::shared_ptr<WallpaperTimelineSection>> sectionOpt = wallpaperSection(timelineIndex, sectionIdentifier);
    if (!sectionOpt) {
        qCritical() << "No timeline section for  timelineIndex" << timelineIndex << "sectionIdentifier" << sectionIdentifier << "monitorIndex" << monitorIndex;
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

void ScreenPlayTimelineManager::printTimelines() const
{
    std::cout << "#############################\n";
    for (const std::shared_ptr<WallpaperTimelineSection>& timeline : m_wallpaperTimelineSectionsList) {
        std::cout << timeline->index << ": " << timeline->identifier.toStdString() << "\t"
                  << timeline->relativePosition
                  << " start: " << timeline->startTime.toString().toStdString()
                  << " end: " << timeline->endTime.toString().toStdString() << std::endl;
    }
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
    const QString& value)
{
    auto wallpaperSectionOpt = wallpaperSection(timelineIndex, sectionIdentifier);
    if (!wallpaperSectionOpt.has_value())
        co_return false;

    auto wallpaperSection = wallpaperSectionOpt.value();
    bool found = false;
    for (auto& wallpaperData : wallpaperSection->wallpaperDataList) {
        if (wallpaperData.monitors().contains(monitorIndex)) {
            if (key == "volume") {
                wallpaperData.setVolume(value.toFloat());
            }
            if (key == "playbackRate") {
                wallpaperData.setPlaybackRate(value.toFloat());
            }
            if (key == "fillmode") {
                wallpaperData.setFillMode(
                    QStringToEnum<Video::FillMode>(value, Video::FillMode::Cover));
            }
            found = true;
        }
    }

    // Skip when the values was changed on an not running wallpaper
    if (!found) {
        co_return true;
    }

    // Now set the value in the ScreenPlayWallpaper class so it does
    // get propagated to the running wallpaper.
    for (auto& activeWallpaper : wallpaperSection->activeWallpaperList) {
        if (activeWallpaper->monitors().contains(monitorIndex)) {
            activeWallpaper->setWallpaperValue(key, value);
        }
    }

    co_return true;
}

QCoro::Task<bool> ScreenPlayTimelineManager::setWallpaperAtTimelineIndex(
    WallpaperData wallpaperData,
    const int timelineIndex,
    const QString& sectionIdentifier)
{
    bool found = false;
    for (std::shared_ptr<WallpaperTimelineSection>& timelineSection : m_wallpaperTimelineSectionsList) {
        const bool sameIndex = timelineSection->index == timelineIndex;
        const bool sameIdentifier = timelineSection->identifier == sectionIdentifier;
        if (sameIndex && sameIdentifier) {
            found = true;

            // Check if we already have a wallpaper at the same position or on one or more of the specified monitors
            auto it = std::find_if(timelineSection->wallpaperDataList.begin(),
                timelineSection->wallpaperDataList.end(),
                [&wallpaperData](const WallpaperData& existingWallpaper) {
                    return std::any_of(wallpaperData.monitors().begin(),
                        wallpaperData.monitors().end(),
                        [&existingWallpaper](int monitor) {
                            return existingWallpaper.monitors()
                                .contains(monitor);
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
