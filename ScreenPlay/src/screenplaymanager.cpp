// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#include "ScreenPlay/screenplaymanager.h"
#include "ScreenPlay/errormanager.h"
#include "ScreenPlay/widgetdata.h"
#include "ScreenPlayCore/projectfile.h"
#include "ScreenPlayCore/util.h"
#include "core/qcorothread.h"
#include "qcorotask.h"

#include <QScopeGuard>
namespace ScreenPlay {

/*!
    \class ScreenPlay::ScreenPlayManager
    \inmodule ScreenPlay
    \brief The ScreenPlayManager is used to manage multiple ScreenPlayWallpaper and ScreenPlayWidget.

    Creates and (indirectly) destroys Wallpaper and Widgets via opening and closing QLocalPipe connectons of the ScreenPlaySDK.
    Also responsible to set the current active wallpaper to the monitorListModel.
*/

/*!
  \brief Constructor that checks if another ScreenPlay instance is running via a localsocket. Starts a save timer
         to limit the amount of times to save. This is used for limiting slider small value save spam.
*/
ScreenPlayManager::ScreenPlayManager(
    QObject* parent)
    : QObject { parent }
{
    m_server = std::make_unique<QLocalServer>();
    QObject::connect(m_server.get(), &QLocalServer::newConnection, this, &ScreenPlayManager::newConnection);
    QObject::connect(&m_screenPlayTimelineManager, &ScreenPlayTimelineManager::activeTimelineIndexChanged, this, &ScreenPlayManager::setActiveTimelineIndex);
    QObject::connect(&m_screenPlayTimelineManager, &ScreenPlayTimelineManager::requestSaveProfiles, this, &ScreenPlayManager::requestSaveProfiles);
    QObject::connect(&m_screenPlayTimelineManager, &ScreenPlayTimelineManager::activeWallpaperCountChanged, this, &ScreenPlayManager::setActiveWallpaperCounter);
    QObject::connect(&m_screenPlayTimelineManager, &ScreenPlayTimelineManager::wallpaperRestartFailed, this, [this](const QString& appID, const QString& message) {
        if (m_errorManager) {
            m_errorManager->displayError(message);
        }
    });

    QObject::connect(this, &ScreenPlayManager::selectedTimelineIndexChanged, &m_screenPlayTimelineManager, &ScreenPlayTimelineManager::setSelectedTimelineIndex);
    QObject::connect(this, &ScreenPlayManager::selectedTimelineIndexChanged, &m_screenPlayTimelineManager, &ScreenPlayTimelineManager::updateMonitorListModelData);

    m_server->setSocketOptions(QLocalServer::WorldAccessOption);
    if (!m_server->listen("ScreenPlay")) {
        qCritical("Could not open Local Socket with the name ScreenPlay!");
    }

    // We limit the time we save via a Qtimer. This is because we can live update values like
    // volume to the wallpaper very fast. This would be very imperformant for HDD user, so
    // we limit ourself to m_saveLimiters interval below:
    m_saveLimiter.setInterval(1000);
    QObject::connect(&m_saveLimiter, &QTimer::timeout, this, &ScreenPlayManager::saveProfiles);
}

/*!
    \brief Inits this class instead of init in the constructor. This is because we need to check
           first if another ScreenPlay instance is running. If it is not the case we call this function.
*/
void ScreenPlayManager::init(
    const std::shared_ptr<GlobalVariables>& globalVariables,
    const std::shared_ptr<MonitorListModel>& mlm,
    const std::shared_ptr<Settings>& settings,
    const std::shared_ptr<ErrorManager>& errorManager)
{
    m_globalVariables = globalVariables;
    m_monitorListModel = mlm;
    m_settings = settings;
    m_errorManager = errorManager;

    m_screenPlayTimelineManager.setGlobalVariables(m_globalVariables);
    m_screenPlayTimelineManager.setSettings(m_settings);
    m_screenPlayTimelineManager.setMonitorListModel(m_monitorListModel);

    // Reset to default settings if we are unable to load
    // the existing one
    if (!loadProfiles()) {
        qInfo() << "Reset default profiles.json at:" << m_globalVariables->localSettingsPath();
        m_settings->writeDefaultProfiles();
    }
}

/*!
    \brief Shutdown
*/
QCoro::Task<Result> ScreenPlayManager::shutdown()
{
    removeAllRunningWidgets(false);
    const auto result = co_await m_screenPlayTimelineManager.removeAllWallpaperFromActiveTimlineSections();
    co_return result;
}

/*!
    \brief Sets the wallpaper at a specific timeline.
*/
QCoro::QmlTask ScreenPlayManager::setWallpaperAtMonitorTimelineIndex(
    const QString& absoluteStoragePath,
    const QVector<int>& monitorIndex,
    const int timelineIndex,
    const QString& identifier,
    const bool saveToProfilesConfigFile)
{
    return QCoro::QmlTask([this, absoluteStoragePath, monitorIndex, timelineIndex, identifier, saveToProfilesConfigFile]() -> QCoro::Task<Result> {
        const QString path = QUrl::fromUserInput(absoluteStoragePath).toLocalFile();
        
        if (path.isEmpty()) {
            Result result;
            result.setSuccess(false);
            result.setMessage("AbsolutePath to project is empty");
            co_return result;
        }

        // Load project data from project.json
        using namespace ScreenPlay;
        ProjectFile projectFile;
        projectFile.projectJsonFilePath = QFileInfo(path + "/project.json");
        if (!projectFile.init()) {
            Result result;
            result.setSuccess(false);
            result.setMessage(QString("Invalid project at %1").arg(path));
            co_return result;
        }

        WallpaperData wallpaperData;

        if (m_settings->startWallpaperMuted()) {
            wallpaperData.setVolume(0.0f);
        } else {
            wallpaperData.setVolume(1.0f);
        }

        // Set data from project file
        wallpaperData.setType(projectFile.type);
        wallpaperData.setAbsolutePath(path);
        wallpaperData.setPreviewImage(projectFile.preview);
        wallpaperData.setFile(projectFile.file);
        wallpaperData.setTitle(projectFile.title);
        wallpaperData.setMonitors(monitorIndex);
        wallpaperData.setFillMode(m_settings->videoFillMode());
        if (wallpaperData.absolutePath().isEmpty()) {
            const QString msg = QString("AbsolutePath to project is empty");
            Result result;
            result.setSuccess(false);
            result.setMessage(msg);
            co_return result;
        }

        if (timelineIndex < 0 || identifier.isEmpty()) {
            const QString msg = QString("Invalid timeline index %1 identifier %2")
                                    .arg(QString::number(timelineIndex), identifier);
            Result result;
            result.setSuccess(false);
            result.setMessage(msg);
            co_return result;
        }

        auto result = co_await m_screenPlayTimelineManager
                          .setWallpaperAtMonitorTimelineIndex(wallpaperData, timelineIndex, identifier, monitorIndex)
                          .then([this, timelineIndex, saveToProfilesConfigFile](Result result) -> QCoro::Task<Result> {
                              if (!result.success()) {
                                  m_screenPlayTimelineManager.printTimelines();
                                  emit printQmlTimeline();
                                  co_return result;
                              }
                              // We do not start the wallpaper here, but let
                              // ScreenPlayTimelineManager::checkActiveWallpaperTimeline decide
                              // if the wallpaper
                              // m_screenPlayTimelineManager.updateMonitorListModelData(selectedTimelineIndex());
                              if (saveToProfilesConfigFile) {
                                  requestSaveProfiles();
                              }
                              m_screenPlayTimelineManager.updateMonitorListModelData(timelineIndex);
                              emit this->notifyUiReloadTimelinePreviewImage();
                              co_return result;
                          });
        co_return result;
    }());
}

/*!
  \brief Creates a ScreenPlayWidget object via a \a absoluteStoragePath and a \a preview image (relative path).
*/
bool ScreenPlayManager::startWidget(
    const QString& absoluteStoragePath,
    const QPoint& position,
    const QJsonObject& properties,
    const bool saveToProfilesConfigFile)
{
    auto saveToProfile = qScopeGuard([=, this] {
        // Do not save on app start
        if (saveToProfilesConfigFile) {
            requestSaveProfiles();
        }
    });

    const QString appID = Util().generateRandomString();
    const QString path = QUrl::fromUserInput(absoluteStoragePath).toLocalFile();

    if (path.isEmpty()) {
        qWarning() << "Path is empty, Abort! Path: " << absoluteStoragePath;
        return false;
    }

    // Load project data from project.json
    using namespace ScreenPlay;
    ProjectFile projectFile;
    projectFile.projectJsonFilePath = QFileInfo(path + "/project.json");
    if (!projectFile.init()) {
        qWarning() << "Invalid project at " << path;
        return false;
    }

    // Create WidgetData object from project file
    WidgetData widgetData;
    widgetData.setAbsolutePath(path);
    widgetData.setTitle(projectFile.title);
    widgetData.setPreviewImage(projectFile.preview);
    widgetData.setFile(projectFile.file);
    widgetData.setType(projectFile.type);
    widgetData.setPosition(position);
    
    // Use provided properties or load from project file
    if (!properties.isEmpty()) {
        widgetData.setProperties(properties);
    } else {
        // Load properties from project.json
        if (auto projectObj = Util().openJsonFileToObject(path + "/project.json")) {
            QJsonObject projectProperties = projectObj->value("properties").toObject();
            widgetData.setProperties(projectProperties);
        }
    }

    auto widget = std::make_shared<ScreenPlayWidget>(
        appID,
        m_globalVariables,
        widgetData);

    QObject::connect(widget.get(), &ScreenPlayWidget::requestSave, this, &ScreenPlayManager::requestSaveProfiles);
    QObject::connect(widget.get(), &ScreenPlayWidget::requestClose, this, &ScreenPlayManager::removeWidget);
    QObject::connect(widget.get(), &ScreenPlayWidget::error, this, &ScreenPlayManager::displayErrorPopup);
    QObject::connect(widget.get(), &ScreenPlayWidget::restartFailed, this, [this](const QString& appID, const QString& message) {
        if (m_errorManager) {
            m_errorManager->displayError(message);
        }
    });
    if (!widget->start()) {
        return false;
    }
    setActiveWidgetsCounter(activeWidgetsCounter() + 1);
    m_screenPlayWidgets.append(widget);
    return true;
}

/*!
    \brief Removes all wallpaper entries in the profiles.json.
*/
QCoro::QmlTask ScreenPlayManager::removeAllRunningWallpapers(bool saveToProfile)
{
    return QCoro::QmlTask([this, saveToProfile]() -> QCoro::Task<Result> {
        auto result = co_await m_screenPlayTimelineManager.removeAllWallpaperFromActiveTimlineSections()
                          .then([this, saveToProfile](Result result) -> QCoro::Task<Result> {
                              if (!result.success()) {
                                  const QString msg = QString("Unable to remove all Wallpaper");
                                  m_screenPlayTimelineManager.printTimelines();
                                  emit printQmlTimeline();
                                  Result result;
                                  result.setSuccess(false);
                                  result.setMessage(msg);
                                  co_return result;
                              }
                              qDebug() << "Task: removeAllWallpaperFromActiveTimlineSections" << result.success();
                              if (saveToProfile)
                                  requestSaveProfiles();
                              co_return result;
                          });
        co_return result;
    }());
}

/*!
    \brief Removes all widgets and resets the activeWidgetCounter to 0.
*/
bool ScreenPlayManager::removeAllRunningWidgets(bool saveToProfile)
{
    if (m_screenPlayWidgets.empty()) {
        return false;
    }

    QStringList appIDs;
    // Do not remove items from the vector you iterate on.
    for (auto& client : m_screenPlayWidgets) {
        appIDs.append(client->appID());
    }

    for (const auto& appID : appIDs) {
        if (!removeWidget(appID)) {
            return false;
        }
    }

    if (saveToProfile)
        requestSaveProfiles();

    return true;
}

/*!
    \brief Removes a Wallpaper at the given monitor  \a timelineIndex, \a sectionIdentifier, and \a monitorIndex.
    Internally searches for a appID at the
    given monitor index and then closes the sdk connection, removes the entries in the
    monitor list model and decreases the active wallpaper counter property of ScreenPlayManager.
*/
QCoro::QmlTask ScreenPlayManager::removeWallpaperAt(int timelineIndex, QString sectionIdentifier, int monitorIndex)
{
    qInfo() << "this: " << this;
    return QCoro::QmlTask([this, timelineIndex, sectionIdentifier, monitorIndex]() -> QCoro::Task<Result> {
        auto result = co_await m_screenPlayTimelineManager.removeWallpaperAt(timelineIndex, sectionIdentifier, monitorIndex)
                          .then([this, timelineIndex, monitorIndex](Result result) -> QCoro::Task<Result> {
                              if (!result.success()) {
                                  const QString msg = QString(
                                      "Unable to remove wallpaper at timeline index: %1 at monitor index: %2")
                                                          .arg(timelineIndex, monitorIndex);
                                  m_screenPlayTimelineManager.printTimelines();
                                  emit printQmlTimeline();
                                  co_return result;
                              }
                              m_screenPlayTimelineManager.updateMonitorListModelData(timelineIndex);
                              emit this->notifyUiReloadTimelinePreviewImage();
                              requestSaveProfiles();
                              co_return result;
                          });
        co_return result;
    }());
}

/*!
    \brief Request a spesific json profile to display in the active wallpaper popup on the right.
*/
bool ScreenPlayManager::projectSettingsAtMonitorIndex(
    const int monitorIndex,
    const int timelineIndex,
    const QString& sectionIdentifier)
{
    auto timelineSection = m_screenPlayTimelineManager.findTimelineSection(timelineIndex, sectionIdentifier);
    if (!timelineSection) {
        return false;
    }

    for (const auto& wallpaper : std::as_const(timelineSection->wallpaperList)) {
        if (wallpaper->monitors().contains(monitorIndex)) {
            m_projectSettingsListModel = wallpaper->getProjectSettingsListModel();
            emit projectSettingsListModelChanged(m_projectSettingsListModel.get());
            return true;
        }
    }
    return false;
}

/*!
  \brief Set a wallpaper \a value at a given \a monitorIndex, \a timelineIndex and \a sectionIdentifier.
*/
QCoro::QmlTask ScreenPlayManager::setValueAtMonitorTimelineIndex(
    const int monitorIndex,
    const int timelineIndex,
    const QString& sectionIdentifier,
    const QString& key,
    const QVariant& value,
    const QString& category)
{
    return QCoro::QmlTask(
        [this, monitorIndex, timelineIndex, sectionIdentifier, key, value, category]() -> QCoro::Task<Result> {
            auto result = co_await m_screenPlayTimelineManager
                              .setValueAtMonitorTimelineIndex(
                                  monitorIndex,
                                  timelineIndex,
                                  sectionIdentifier,
                                  key,
                                  value,
                                  category)
                              .then([this, timelineIndex, monitorIndex](Result result) -> QCoro::Task<Result> {
                                  if (!result.success()) {
                                      const QString msg = QString(
                                          "Unable to set Value at monitor %1 timeline index: %2")
                                                              .arg(timelineIndex, monitorIndex);
                                      m_screenPlayTimelineManager.printTimelines();
                                      emit printQmlTimeline();
                                      co_return result;
                                  }
                                  // m_screenPlayTimelineManager.updateMonitorListModelData(selectedTimelineIndex());
                                  emit requestSaveProfiles();
                                  co_return result;
                              });
            co_return result;
        }());
}

/*!
  \brief Set a wallpaper \a fillmode at a given \a index and converts the qml enum (int)
         into the c++ enum class value.
*/
QCoro::QmlTask ScreenPlayManager::setWallpaperFillModeAtMonitorIndex(
    const int monitorIndex,
    const int timelineIndex,
    const QString sectionIdentifier,
    const int fillmode)
{
    const auto fillModeTyped = static_cast<Video::FillMode>(fillmode);
    const QString value = QVariant::fromValue<Video::FillMode>(fillModeTyped).toString();
    const QString key = "fillmode";
    qDebug() << monitorIndex << timelineIndex << sectionIdentifier << fillmode << key << value;

    return setValueAtMonitorTimelineIndex(monitorIndex, timelineIndex, sectionIdentifier, key, value, "No property category");
}

/*!
  \brief Convenient function to set a \a value at a given \a index and \a key for all wallaper. For exmaple used to mute all wallpaper.
*/
bool ScreenPlayManager::setAllWallpaperValue(const QString& key, const QVariant& value)
{
    auto activeTimelineSection = m_screenPlayTimelineManager.findActiveWallpaperTimelineSection();
    if (!activeTimelineSection) {
        return false;
    }
    for (auto& wallpaper : activeTimelineSection->wallpaperList) {
        if (!wallpaper->setWallpaperValue(key, value, "", true))
            return false;
    }
    return true;
}

/*!
  \brief Returns \c a ScreenPlayWallpaper if successful, otherwhise \c std::nullopt.
         This function is only used in QML.
*/
WallpaperData ScreenPlayManager::getWallpaperData(const int monitorIndex,
    const int timelineIndex,
    const QString sectionIdentifier)
{
    auto timelineSection = m_screenPlayTimelineManager.findTimelineSection(
        timelineIndex,
        sectionIdentifier);
    if (!timelineSection) {
        return {};
    }
    for (const auto& wallpaperData : timelineSection->wallpaperData()) {
        if (wallpaperData.monitors().first() == monitorIndex) {
            // TODO COPY?
            return wallpaperData;
        }
    }
    return {};
}

/*!
  \brief We always handle the endTimeString, because it is the handle for the
         timeline. The last, default, timeline does not have a handle.
*/
bool ScreenPlayManager::moveTimelineAt(const int timelineIndex, const QString identifier, const float relativePosition, QString positionTimeString)
{
    return m_screenPlayTimelineManager.moveTimelineAt(timelineIndex, identifier, relativePosition, positionTimeString);
}

bool ScreenPlayManager::addTimelineAt(const int timelineIndex, const float reltiaveLinePosition, QString identifier)
{

    const bool success = m_screenPlayTimelineManager.addTimelineAt(timelineIndex, reltiaveLinePosition, identifier);
    return success;
}

QCoro::QmlTask ScreenPlayManager::removeAllTimlineSections()
{
    return QCoro::QmlTask([this]() -> QCoro::Task<Result> {
        // call with coro
        auto result = co_await m_screenPlayTimelineManager.removeAllTimlineSections()
                          .then([this](Result result) -> QCoro::Task<Result> {
                              qDebug() << "Task: removeAllTimlineSections" << result.success();
                              if (result.success())
                                  requestSaveProfiles();
                              co_return result;
                          });

        co_return result;
    }());
}

QCoro::QmlTask ScreenPlayManager::removeTimelineAt(const int timelineIndex)
{
    return QCoro::QmlTask([this, timelineIndex]() -> QCoro::Task<Result> {
        auto result = co_await m_screenPlayTimelineManager.removeTimelineAt(timelineIndex)
                          .then([this](Result result) -> QCoro::Task<Result> {
                              qDebug() << "Task: removeTimelineAt" << result.success();
                              if (result.success())
                                  requestSaveProfiles();
                              co_return result;
                          });

        co_return result;
    }());
}

/*!
  \brief Qml function to build our timeline on creation in qml.
*/
QJsonArray ScreenPlayManager::timelineSections()
{
    return m_screenPlayTimelineManager.timelineSections();
}

/*!
    \brief Appends a new SDKConnection object shared_ptr to the m_clients list.
*/
void ScreenPlayManager::newConnection()
{
    qInfo() << "[1/4] SDKConnection incomming";
    auto connection = std::make_unique<SDKConnection>(m_server->nextPendingConnection());
    QObject::connect(connection.get(), &SDKConnection::requestRaise, this, &ScreenPlayManager::requestRaise);

    // Only after we receive the first message with appID and type we can set the SDKConnection to the
    // ScreenPlayWallpaper or ScreenPlayWidgets class.
    QObject::connect(connection.get(), &SDKConnection::appConnected, this, [this](const SDKConnection* connection) {
        if (m_unconnectedClients.empty()) {
            qWarning() << "Unable to match a connection. UnconnectedClients list is empty!";
            return;
        }

        std::unique_ptr<SDKConnection> matchingConnection;
        for (int i = 0; i < m_unconnectedClients.size(); ++i) {
            if (m_unconnectedClients.at(i).get() == connection) {
                matchingConnection = std::move(m_unconnectedClients.at(i));
                m_unconnectedClients.erase(m_unconnectedClients.begin() + i);
            }
        }

        if (!matchingConnection) {
            qWarning() << "Unable to match a connection! Aborting!";
            return;
        }

        auto startingTimelineSection = m_screenPlayTimelineManager.findStartingOrActiveWallpaperTimelineSection();
        if (!startingTimelineSection) {
            qWarning() << "Unable to findStartingOrActiveWallpaperTimelineSection! Aborting!";
            return;
        }
        auto& activeWallpaperList = startingTimelineSection->wallpaperList;

        for (int i = 0; i < activeWallpaperList.size(); ++i) {
            if (activeWallpaperList.at(i)->appID() == matchingConnection->appID()) {
                qInfo() << "[3/4] Matching Wallpaper found!";
                activeWallpaperList.at(i)->setSDKConnection(std::move(matchingConnection));
                return;
            }
        }

        for (int i = 0; i < m_screenPlayWidgets.size(); ++i) {
            if (m_screenPlayWidgets.at(i)->appID() == matchingConnection->appID()) {
                qInfo() << "[3/4] Matching Widget found!";
                m_screenPlayWidgets.at(i)->setSDKConnection(std::move(matchingConnection));
                return;
            }
        }

        qWarning() << "No matching connection found!"
                   << "m_screenPlayWallpapers count:  " << activeWallpaperList.size()
                   << "m_screenPlayWidgets count:     " << m_screenPlayWidgets.size()
                   << "m_unconnectedClients count:    " << m_unconnectedClients.size();
    });
    m_unconnectedClients.push_back(std::move(connection));
}

void ScreenPlayManager::setActiveWallpaperCounter(int activeWallpaperCounter)
{
    if (m_activeWallpaperCounter == activeWallpaperCounter)
        return;

    m_activeWallpaperCounter = activeWallpaperCounter;
    emit activeWallpaperCounterChanged(m_activeWallpaperCounter);
}

void ScreenPlayManager::setActiveWidgetsCounter(int activeWidgetsCounter)
{
    if (m_activeWidgetsCounter == activeWidgetsCounter)
        return;

    m_activeWidgetsCounter = activeWidgetsCounter;
    emit activeWidgetsCounterChanged(m_activeWidgetsCounter);
}

void ScreenPlayManager::setActiveTimelineIndex(int activeTimelineIndex)
{
    if (m_activeTimelineIndex == activeTimelineIndex)
        return;

    m_activeTimelineIndex = activeTimelineIndex;
    emit activeTimelineIndexChanged(m_activeTimelineIndex);
}

/*!
    \brief Removes a Widget from the given appID. Returns true on success.
*/
bool ScreenPlayManager::removeWidget(const QString& appID)
{
    m_screenPlayWidgets.erase(
        std::remove_if(
            m_screenPlayWidgets.begin(),
            m_screenPlayWidgets.end(),
            [this, appID](std::shared_ptr<ScreenPlayWidget>& widget) {
                if (widget->appID() != appID) {
                    qInfo() << "No match " << widget->appID();
                    return false;
                }

                widget->close();

                qInfo() << "Remove widget " << appID;

                setActiveWidgetsCounter(activeWidgetsCounter() - 1);

                return true;
            }),
        m_screenPlayWidgets.end());

    if (activeWidgetsCounter() != m_screenPlayWidgets.length()) {
        qWarning() << "activeWidgetsCounter value: " << activeWidgetsCounter()
                   << "does not match m_screenPlayWidgets length:" << m_screenPlayWidgets.length();
        return false;
    }

    return true;
}

int ScreenPlayManager::activeTimelineIndex()
{
    std::shared_ptr<WallpaperTimelineSection> activeTimelineSection = m_screenPlayTimelineManager.findActiveWallpaperTimelineSection();
    if (!activeTimelineSection) {
        qCritical() << "ActiveTimelineIndex failed, because no active timeline section was found";
        m_screenPlayTimelineManager.printTimelines();
        return -1;
    }
    return activeTimelineSection->index;
}

void ScreenPlayManager::requestSaveProfiles()
{
    m_saveLimiter.start();
}

/*!
    \brief Saves a given wallpaper \a newProfileObject to a \a profileName. We ignore the profileName argument
    because we currently only support one profile. Returns \c true if successfuly saved to profiles.json, otherwise \c false.
*/
bool ScreenPlayManager::saveProfiles()
{
    m_saveLimiter.stop();

    QJsonArray timelineWallpaperList = m_screenPlayTimelineManager.timelineWallpaperList();

    QJsonArray widgetList {};
    for (const auto& activeWidget : std::as_const(m_screenPlayWidgets)) {
        widgetList.append(activeWidget->getActiveSettingsJson());
    }

    QJsonObject profileDefault;
    profileDefault.insert("widgets", widgetList);
    profileDefault.insert("timelineWallpaper", timelineWallpaperList);
    profileDefault.insert("name", "default");

    QJsonArray activeProfileList;
    activeProfileList.append(profileDefault);

    QJsonObject profile;
    profile.insert("version", m_settings->getProfilesVersion().toString());
    profile.insert("profiles", activeProfileList);

    if (m_util.writeJsonObjectToFile({ m_globalVariables->localSettingsPath().toString() + "/profiles.json" }, profile)) {
        emit profilesSaved();
        return true;
    }
    return false;
}

/*!
 \brief Loads all wallpaper from C:\Users\XXX\AppData\Local\ScreenPlay\ScreenPlay\profiles.json
        when the version number matches and starts the available wallpaper
*/
bool ScreenPlayManager::loadProfiles()
{
    const auto configObj = m_util.openJsonFileToObject(m_globalVariables->localSettingsPath().toString() + "/profiles.json");

    if (!configObj) {
        qWarning() << "Could not load active profiles at path: " << m_globalVariables->localSettingsPath().toString() + "/profiles.json";
        if (m_errorManager) {
            m_errorManager->displayError("Could not load profiles.json file. Using default settings.");
        }
        return false;
    }
    std::optional<QVersionNumber> version = m_util.getVersionNumberFromString(configObj->value("version").toString());
    QVersionNumber requiredVersion = m_settings->getProfilesVersion();
    if (version && *version != requiredVersion) {
        qWarning() << "Version missmatch fileVersion: " << version->toString() << "m_version: " << requiredVersion.toString();
        if (m_errorManager) {
            m_errorManager->displayError(QString("Profile version mismatch. Expected %1, found %2. Some settings may be reset.")
                    .arg(requiredVersion.toString(), version->toString()));
        }
        return false;
    }

    QJsonArray activeProfilesTmp = configObj->value("profiles").toArray();

    if (activeProfilesTmp.size() > 1) {
        qWarning() << "We currently only support one profile!";
        return false;
    }

    bool containsInvalidData = false;
    bool hasTimelines = false;
    
    for (const QJsonValueRef profile : activeProfilesTmp) {

        // TODO right now we limit ourself to one default profile
        if (profile.toObject().value("name").toString() != "default")
            continue;

        for (QJsonValueRef timelineWallpaper : profile.toObject().value("timelineWallpaper").toArray()) {
            QJsonObject wallpaperObj = timelineWallpaper.toObject();
            auto result = m_screenPlayTimelineManager.addTimelineFromSettings(wallpaperObj);
            if (!result.has_value()) {
                QString errorMessage = QString("Failed to load timeline wallpaper: %1")
                    .arg(ScreenPlayTimelineManager::timelineManagerErrorToString(result.error()));
                qCritical() << "Unable to add wallpaper timeline:" << errorMessage;
                if (m_errorManager) {
                    m_errorManager->displayError(errorMessage);
                }
                containsInvalidData = true;
                continue;
            }
            hasTimelines = true;
        }

        for (const QJsonValueRef widget : profile.toObject().value("widgets").toArray()) {
            QJsonObject widgetObj = widget.toObject();
            if (!loadWidgetConfig(widgetObj)) {
                QString widgetPath = widgetObj.value("absolutePath").toString();
                QString errorMessage = QString("Failed to load widget from: %1").arg(widgetPath);
                qWarning() << errorMessage;
                if (m_errorManager) {
                    m_errorManager->displayError(errorMessage);
                }
                containsInvalidData = true;
            }
        }
    }

    // Create default timeline if no timelines were loaded
    if (!hasTimelines) {
        qInfo() << "No timelines found in profiles, creating default timeline";
        QJsonObject defaultTimelineObj;
        defaultTimelineObj.insert("startTime", "00:00:00");
        defaultTimelineObj.insert("endTime", "23:59:59");
        defaultTimelineObj.insert("wallpaper", QJsonArray());
        
        auto result = m_screenPlayTimelineManager.addTimelineFromSettings(defaultTimelineObj);
        if (!result.has_value()) {
            QString errorMessage = QString("Failed to create default timeline: %1")
                .arg(ScreenPlayTimelineManager::timelineManagerErrorToString(result.error()));
            qCritical() << errorMessage;
            if (m_errorManager) {
                m_errorManager->displayError(errorMessage);
            }
            return false;
        }
        
        // Save the default timeline to profiles
        saveProfiles();
    }

    // The can happen if the user unpluggs a wallpaper but it still exists
    // in the profiles.json. For this we save all profiles with now active
    // content.
    if (containsInvalidData) {
        if (m_errorManager) {
            m_errorManager->displayError("Some wallpapers or widgets from profiles.json could not be loaded. This may happen if files have been moved or monitors have been disconnected. The configuration will be updated automatically.");
        }
        saveProfiles();
    } else {
        m_screenPlayTimelineManager.startup();
    }

    return true;
}

/*!
  \brief Loads a widget from C:\Users\XXX\AppData\Local\ScreenPlay\ScreenPlay\profiles.json
*/
bool ScreenPlayManager::loadWidgetConfig(const QJsonObject& widgetObj)
{
    if (widgetObj.empty())
        return false;

    const QString absolutePath = widgetObj.value("absolutePath").toString();
    const int positionX = widgetObj.value("positionX").toInt(0);
    const int positionY = widgetObj.value("positionY").toInt(0);
    const QPoint position { positionX, positionY };
    const QJsonObject properties = widgetObj.value("properties").toObject();

    const bool success = startWidget(absolutePath, position, properties, false);

    if (!success) {
        qWarning() << "Unable to start Widget! " << position << absolutePath;
        return false;
    }
    return true;
}

void ScreenPlayManager::setSelectedTimelineIndex(int selectedTimelineIndex)
{
    // TODO ?
    // if (m_selectedTimelineIndex == selectedTimelineIndex)
    //     return;
    m_selectedTimelineIndex = selectedTimelineIndex;
    emit selectedTimelineIndexChanged(m_selectedTimelineIndex);
}

ProjectSettingsListModel* ScreenPlayManager::projectSettingsListModel() const
{
    return m_projectSettingsListModel.get();
}

void ScreenPlayManager::setProjectSettingsListModel(ProjectSettingsListModel* newProjectSettingsListModel)
{
    if (m_projectSettingsListModel.get() == newProjectSettingsListModel)
        return;
    m_projectSettingsListModel.reset(newProjectSettingsListModel);
    emit projectSettingsListModelChanged(m_projectSettingsListModel.get());
}
}

#include "moc_screenplaymanager.cpp"
