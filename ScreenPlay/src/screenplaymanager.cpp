// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#include "ScreenPlay/screenplaymanager.h"
#include "ScreenPlayUtil/util.h"

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
    QObject::connect(&m_screenPlayTimelineManager, &ScreenPlayTimelineManager::requestSaveProfiles, this, &ScreenPlayManager::requestSaveProfiles);
    QObject::connect(&m_screenPlayTimelineManager, &ScreenPlayTimelineManager::activeWallpaperCountChanged, this, &ScreenPlayManager::setActiveWallpaperCounter);
    m_server->setSocketOptions(QLocalServer::WorldAccessOption);
    if (!m_server->listen("ScreenPlay")) {
        qCritical("Could not open Local Socket with the name ScreenPlay!");
    }

    // We limit the time we save via a Qtimer. This is because we can live update values like
    // volume to the wallpaper very fast. This would be very imperformant for HDD user, so
    // we limit ourself to m_saveLimiters interval below:
    m_saveLimiter.setInterval(1000);
    QObject::connect(&m_saveLimiter, &QTimer::timeout, this, &ScreenPlayManager::saveProfiles);
    QObject::connect(this, &ScreenPlayManager::requestSaveProfiles, this, [this]() {
        m_saveLimiter.start();
    });
}

/*!
    \brief Inits this class instead of init in the constructor. This is because we need to check
           first if another ScreenPlay instance is running. If it is not the case we call this function.
*/
void ScreenPlayManager::init(
    const std::shared_ptr<GlobalVariables>& globalVariables,
    const std::shared_ptr<MonitorListModel>& mlm,
    const std::shared_ptr<Settings>& settings)
{
    m_globalVariables = globalVariables;
    m_monitorListModel = mlm;
    m_settings = settings;

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
    \brief Sets the wallpaper at a spesific timeline.
*/
QCoro::QmlTask ScreenPlayManager::setWallpaperAtTimelineIndex(
    const ScreenPlay::ContentTypes::InstalledType type,
    const QString& absolutePath,
    const QString& previewImage,
    const QString& file,
    const QVector<int>& monitorIndex,
    const int timelineIndex,
    const QString& identifier,
    const bool saveToProfilesConfigFile)
{
    WallpaperData wallpaperData;
    wallpaperData.type = type;
    wallpaperData.absolutePath = absolutePath;
    wallpaperData.previewImage = previewImage;
    wallpaperData.file = file;
    wallpaperData.monitors = monitorIndex;
    wallpaperData.fillMode = m_settings->videoFillMode();

    return QCoro::QmlTask([this, wallpaperData, timelineIndex, identifier]() -> QCoro::Task<Result> {
        if (timelineIndex < 0 || identifier.isEmpty()) {

            co_return Result { false };
        }

        const bool success = co_await m_screenPlayTimelineManager.setWallpaperAtTimelineIndex(wallpaperData, timelineIndex, identifier);

        if (!success) {
            qCritical() << "Invalid timeline index or identifier: " << timelineIndex << identifier;
            m_screenPlayTimelineManager.printTimelines();
            emit printQmlTimeline();
            co_return Result { success };
        }

        // We do not start the wallpaper here, but let
        // ScreenPlayTimelineManager::checkActiveWallpaperTimeline decide
        // if the wallpaper
        emit requestSaveProfiles();
        co_return Result { success };
    }());
}

/*!
  \brief Creates a ScreenPlayWidget object via a \a absoluteStoragePath and a \a preview image (relative path).
*/
bool ScreenPlayManager::startWidget(
    const ContentTypes::InstalledType type,
    const QPoint& position,
    const QString& absoluteStoragePath,
    const QString& previewImage,
    const QJsonObject& properties,
    const bool saveToProfilesConfigFile)
{
    auto saveToProfile = qScopeGuard([=, this] {
        // Do not save on app start
        if (saveToProfilesConfigFile) {
            emit requestSaveProfiles();
        }
    });

    const QString appID = Util().generateRandomString();
    const QString path = QUrl::fromUserInput(absoluteStoragePath).toLocalFile();

    if (path.isEmpty()) {
        qWarning() << "Path is empty, Abort! Path: " << absoluteStoragePath;
        return false;
    }

    auto widget = std::make_shared<ScreenPlayWidget>(
        appID,
        m_globalVariables,
        position,
        path,
        previewImage,
        properties,
        type);

    QObject::connect(widget.get(), &ScreenPlayWidget::requestSave, this, &ScreenPlayManager::requestSaveProfiles);
    QObject::connect(widget.get(), &ScreenPlayWidget::requestClose, this, &ScreenPlayManager::removeWidget);
    QObject::connect(widget.get(), &ScreenPlayWidget::error, this, &ScreenPlayManager::displayErrorPopup);
    if (!widget->start()) {
        return false;
    }
    setActiveWidgetsCounter(activeWidgetsCounter() + 1);
    m_screenPlayWidgets.append(widget);
    return true;
}

void ScreenPlayManager::setSelectedTimelineIndex(const int selectedTimelineIndex)
{
    m_screenPlayTimelineManager.updateMonitorListModelData(selectedTimelineIndex);
}

/*!
    \brief Removes all wallpaper entries in the profiles.json.
*/
QCoro::QmlTask ScreenPlayManager::removeAllRunningWallpapers(bool saveToProfile)
{
    return QCoro::QmlTask([this, saveToProfile]() -> QCoro::Task<Result> {
        // call with coro
        const bool success = co_await m_screenPlayTimelineManager.removeAllWallpaperFromActiveTimlineSections();
        qDebug() << "Task: removeAllWallpaperFromActiveTimlineSections" << success;
        if (saveToProfile)
            emit requestSaveProfiles();
        co_return Result { success };
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
        emit requestSaveProfiles();

    return true;
}

/*!
    \brief Removes a Wallpaper at the given monitor \a at (index). Internally searches for a appID at the
    given monitor index and then closes the sdk connection, removes the entries in the
    monitor list model and decreases the active wallpaper counter property of ScreenPlayManager.
*/
QCoro::QmlTask ScreenPlayManager::removeWallpaperAt(int timelineIndex, QString timelineIdentifier, int monitorIndex)
{

    return QCoro::QmlTask([this, timelineIndex, timelineIdentifier, monitorIndex]() -> QCoro::Task<Result> {
        // call with coro
        const bool success = co_await m_screenPlayTimelineManager.removeWallpaperAt(timelineIndex, timelineIdentifier, monitorIndex);
        qDebug() << "Task: removeAllWallpaperFromActiveTimlineSections" << success;
        emit requestSaveProfiles();
        co_return Result { success };
    }());
}

/*!
    \brief Request a spesific json profile to display in the active wallpaper popup on the right.
*/
bool ScreenPlayManager::requestProjectSettingsAtMonitorIndex(const int index)
{
    auto activeTimelineSection = m_screenPlayTimelineManager.findActiveWallpaperTimelineSection();
    if (!activeTimelineSection) {
        return false;
    }

    // TODO CHANGE TO WP SECTION
    for (const auto& wallpaper : std::as_const(activeTimelineSection->activeWallpaperList)) {
        if (wallpaper->monitors()[0] == index) {

            emit projectSettingsListModelResult(
                wallpaper->getProjectSettingsListModel());
            return true;
        }
    }
    return false;
}

/*!
  \brief Set a wallpaper \a value at a given \a index and \a key.
*/
bool ScreenPlayManager::setWallpaperValueAtMonitorIndex(const int index, const QString& key, const QString& value)
{
    if (auto appID = m_monitorListModel->getAppIDByMonitorIndex(index)) {
        return setWallpaperValue(*appID, key, value);
    }

    qWarning() << "Could net get appID from m_monitorListModel!";
    return false;
}

/*!
  \brief Set a wallpaper \a fillmode at a given \a index and converts the qml enum (int)
         into the c++ enum class value.
*/
bool ScreenPlayManager::setWallpaperFillModeAtMonitorIndex(const int index, const int fillmode)
{
    const auto fillModeTyped = static_cast<Video::FillMode>(fillmode);

    if (auto appID = m_monitorListModel->getAppIDByMonitorIndex(index)) {
        return setWallpaperValue(*appID, "fillmode", QVariant::fromValue<Video::FillMode>(fillModeTyped).toString());
    }

    qWarning() << "Could net get appID from m_monitorListModel!";
    return false;
}

/*!
  \brief Convenient function to set a \a value at a given \a index and \a key for all wallaper. For exmaple used to mute all wallpaper.
*/
bool ScreenPlayManager::setAllWallpaperValue(const QString& key, const QString& value)
{
    auto activeTimelineSection = m_screenPlayTimelineManager.findActiveWallpaperTimelineSection();
    if (!activeTimelineSection) {
        return false;
    }
    for (auto& wallpaper : activeTimelineSection->activeWallpaperList) {
        if (!wallpaper->setWallpaperValue(key, value, true))
            return false;
    }
    return true;
}

/*!
  \brief Returns \c a ScreenPlayWallpaper if successful, otherwhise \c std::nullopt.
         This function is only used in QML.
*/
ScreenPlayWallpaper* ScreenPlayManager::getWallpaperByAppID(const QString& appID)
{

    auto activeTimelineSection = m_screenPlayTimelineManager.findActiveWallpaperTimelineSection();
    if (!activeTimelineSection) {
        return nullptr;
    }
    for (auto& wallpaper : activeTimelineSection->activeWallpaperList) {
        if (wallpaper->appID() == appID) {
            return wallpaper.get();
        }
    }
    return nullptr;
}

/*!
  \brief We always handle the endTimeString, because it is the handle for the
         timeline. The last, default, timeline does not have a handle.
*/
bool ScreenPlayManager::moveTimelineAt(const int index, const QString identifier, const float relativePosition, QString positionTimeString)
{
    return m_screenPlayTimelineManager.moveTimelineAt(index, identifier, relativePosition, positionTimeString);
}

bool ScreenPlayManager::addTimelineAt(const int index, const float reltiaveLinePosition, QString identifier)
{

    const bool success = m_screenPlayTimelineManager.addTimelineAt(index, reltiaveLinePosition, identifier);
    return success;
}

QCoro::QmlTask ScreenPlayManager::removeAllTimlineSections()
{
    return QCoro::QmlTask([this]() -> QCoro::Task<Result> {
        // call with coro
        const bool success = co_await m_screenPlayTimelineManager.removeAllTimlineSections();
        qDebug() << "Task: removeAllTimlineSections" << success;
        // emit requestSaveProfiles();
        // removeAllRunningWallpapers();
        co_return Result { success };
    }());
}

bool ScreenPlayManager::removeTimelineAt(const int index)
{
    const bool success = m_screenPlayTimelineManager.removeTimelineAt(index);
    emit requestSaveProfiles();
    return success;
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

        auto activeTimelineSection = m_screenPlayTimelineManager.findActiveWallpaperTimelineSection();
        if (!activeTimelineSection) {
            return;
        }
        auto& activeWallpaperList = activeTimelineSection->activeWallpaperList;

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

/*!
    \brief Removes a wallpaper from the given appID. Returns true on success.
*/
bool ScreenPlayManager::removeWallpaper(const QString& appID)
{

    auto wallpaperSectionOpt = m_screenPlayTimelineManager.activeWallpaperSectionByAppID(appID);
    if (!wallpaperSectionOpt.has_value()) {
        qCritical() << "No wallpaper found.";
        return false;
    }
    if (!wallpaperSectionOpt.value()) {
        qCritical() << "No wallpaperSectionOpt invalid.";
        return false;
    }
    auto& wallpaperSection = wallpaperSectionOpt.value();

    wallpaperSection->activeWallpaperList.erase(
        std::remove_if(
            wallpaperSection->activeWallpaperList.begin(),
            wallpaperSection->activeWallpaperList.end(),
            [this, appID](std::shared_ptr<ScreenPlayWallpaper>& wallpaper) {
                if (wallpaper->appID() != appID) {
                    return false;
                }

                qInfo() << "Remove wallpaper " << wallpaper->file() << "at monitor " << wallpaper->monitors();

                // The MonitorListModel contains a shared_ptr of this object that needs to be removed
                // for shared_ptr to release the object.
                // m_monitorListModel->setWallpaperMonitor({}, wallpaper->monitors());

                wallpaper->close();

                return true;
            }));

    if (activeWallpaperCounter() != wallpaperSection->activeWallpaperList.size()) {
        qWarning() << "activeWallpaperCounter value: " << activeWallpaperCounter()
                   << "does not match m_screenPlayWallpapers length:" << wallpaperSection->activeWallpaperList.size();
        return false;
    }

    return true;
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

/*!
   \brief Sets a given \a value to a given \a key. The \a appID is used to identify the receiver socket.
*/
bool ScreenPlayManager::setWallpaperValue(const QString& appID, const QString& key, const QString& value)
{
    std::shared_ptr<WallpaperTimelineSection> activeTimelineSection = m_screenPlayTimelineManager.findActiveWallpaperTimelineSection();
    if (!activeTimelineSection) {
        qCritical() << "setWallpaperValue failed, because no active timeline section was found";
        return false;
    }

    for (auto& wallpaper : activeTimelineSection->activeWallpaperList) {
        if (wallpaper->appID() == appID) {
            return wallpaper->setWallpaperValue(key, value, true);
        }
    }
    qCritical() << "No wallpaper with matching appID was found";
    return false;
}

int ScreenPlayManager::activeTimelineIndex()
{
    std::shared_ptr<WallpaperTimelineSection> activeTimelineSection = m_screenPlayTimelineManager.findActiveWallpaperTimelineSection();
    if (!activeTimelineSection) {
        qCritical() << "setWallpaperValue failed, because no active timeline section was found";
        return -1;
    }
    return activeTimelineSection->index;
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
        return false;
    }

    std::optional<QVersionNumber> version = m_util.getVersionNumberFromString(configObj->value("version").toString());
    QVersionNumber requiredVersion = m_settings->getProfilesVersion();
    if (version && *version != requiredVersion) {
        qWarning() << "Version missmatch fileVersion: " << version->toString() << "m_version: " << requiredVersion.toString();
        return false;
    }

    QJsonArray activeProfilesTmp = configObj->value("profiles").toArray();

    if (activeProfilesTmp.size() > 1) {
        qWarning() << "We currently only support one profile!";
        return false;
    }

    bool containsInvalidData = false;
    for (const QJsonValueRef wallpaper : activeProfilesTmp) {

        // TODO right now we limit ourself to one default profile
        if (wallpaper.toObject().value("name").toString() != "default")
            continue;

        for (QJsonValueRef timelineWallpaper : wallpaper.toObject().value("timelineWallpaper").toArray()) {
            QJsonObject wallpaperObj = timelineWallpaper.toObject();
            if (!m_screenPlayTimelineManager.addTimelineFromSettings(wallpaperObj)) {
                qCritical() << "Unable to add wallpaper timeline";
                containsInvalidData = true;
                continue;
            }
        }

        for (const QJsonValueRef widget : wallpaper.toObject().value("widgets").toArray()) {
            QJsonObject widgetObj = widget.toObject();
            if (!loadWidgetConfig(widgetObj))
                containsInvalidData = true;
        }
    }

    // The can happen if the user unpluggs a wallpaper but it still exists
    // in the profiles.json. For this we save all profiles with now active
    // content.
    if (containsInvalidData) {
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
    const QString previewImage = widgetObj.value("previewImage").toString();
    const QString typeString = widgetObj.value("type").toString();
    const int positionX = widgetObj.value("positionX").toInt(0);
    const int positionY = widgetObj.value("positionY").toInt(0);
    const QPoint position { positionX, positionY };
    const auto type = QStringToEnum<ContentTypes::InstalledType>(typeString, ContentTypes::InstalledType::QMLWidget);
    const QJsonObject properties = widgetObj.value("properties").toObject();

    const bool success = startWidget(type, position, absolutePath, previewImage, properties, false);

    if (!success) {
        qWarning() << "Unable to start Widget! " << type << position << absolutePath;
        return false;
    }
    return true;
}
}

#include "moc_screenplaymanager.cpp"
