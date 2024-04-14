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

    m_contentTimer.setInterval(1000);
    // Do not start the timer here. This will be done after
    // we have loaded all timeline wallpaper from the config.json
    QObject::connect(&m_contentTimer, &QTimer::timeout, this, &ScreenPlayManager::checkActiveWallpaperTimeline);
}

std::shared_ptr<WallpaperTimelineSection>  ScreenPlayManager::findActiveSection() {
    const QTime currentTime = QTime::currentTime();
    for (const auto& section : m_wallpaperTimelineSectionsList) {
        if (section->active && section->containsTime(currentTime)) {
            return section;
        }
    }
    return nullptr; // No active section contains the current time
}
/*!
    \brief Checks if we need to display a different wallpaper at the current time.
*/
void ScreenPlayManager::checkActiveWallpaperTimeline()
{
    auto activeTimelineSection= findActiveSection();
    if(!activeTimelineSection){
        qCritical() << "There must always be (lightning) an active timline";
        return;
    }

   // This should happen most of the time, where we are
   // during an active wallpaper section
    if(activeTimelineSection == m_activeWallpaperTimeline){
        return;
    }



    // This means the timeline section has never been active
    // and we must create new wallpaper
   // if(!activeTimelineSection->active){
    for (const auto& wallpaper : activeTimelineSection->wallpaperData){
        auto activeWallpaper = startWallpaper(wallpaper, false);
        if(activeWallpaper){
            activeTimelineSection->activeWallpaperList.push_back(activeWallpaper);
        }
    }
    return;
    //}

    // TODO: Check if we can reuse some active wallpaper
}

bool ScreenPlayManager::removeWallpaperAtTimelineIndex(const int timelineIndex, const int monitorIndex){
    return {};
}

/*!
    \brief Qml function, because we cannot create the WallpaperData in qml.
*/
bool ScreenPlayManager::addWallpaperAtTimelineIndex(
    const ContentTypes::InstalledType type,
    const Video::FillMode fillMode,
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
    const bool saveToProfilesConfigFile)
{
    WallpaperData wallpaperData;
    wallpaperData.type = type;
    wallpaperData.fillMode = fillMode;
    wallpaperData.absolutePath = absolutePath;
    wallpaperData.previewImage = previewImage;
    wallpaperData.file = file;
    wallpaperData.monitors = monitorIndex;
    wallpaperData.volume = volume;
    wallpaperData.playbackRate = playbackRate;
    wallpaperData.properties = properties;

    const QTime startTime = QTime::fromString(startTimeString, m_timelineTimeFormat);
    const QTime endTime = QTime::fromString(startTimeString, m_timelineTimeFormat);

    if(!startTime.isValid() || !endTime.isValid()){
        qWarning() << "Invalid time, startTime: " << startTime << " endTime " << endTime;
        return false;
    }

    auto wallpaper = startWallpaper(
        wallpaperData,
        saveToProfilesConfigFile);
    // TODO
    return true;
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

    // Reset to default settings if we are unable to load
    // the existing one
    if (!loadProfiles()) {
        qInfo() << "Reset default profiles.json at:" << m_globalVariables->localSettingsPath();
        m_settings->writeJsonFileFromResource("profiles");
    }
}

/*!
    \brief Creates a wallpaper with a given \a monitorIndex list, \a a absoluteStoragePath folder,
    a \a previewImage (relative path to the absoluteStoragePath), a  default \a volume,
    a \a fillMode, a \a type (htmlWallpaper, qmlWallpaper etc.), a \a saveToProfilesConfigFile bool only set to flase
    if we call the method when using via the settings on startup to skip a unnecessary save.
*/
std::shared_ptr<ScreenPlayWallpaper> ScreenPlayManager::startWallpaper(
    WallpaperData wallpaperData,
    const bool saveToProfilesConfigFile)
{

    const int screenCount = QGuiApplication::screens().count();

    QJsonArray monitors;
    for (const int index : wallpaperData.monitors) {
        monitors.append(index);
        if (index > screenCount - 1) {
            qWarning() << "Configuration contains invalid monitor with index: " << index << " screen count: " << screenCount;
            return nullptr;
        }
    }

    auto saveToProfile = qScopeGuard([=, this] {
        // Do not save on app start
        if (saveToProfilesConfigFile) {
            emit requestSaveProfiles();
        }
    });

    // Remove file:///
    wallpaperData.absolutePath = QUrl::fromUserInput(wallpaperData.absolutePath).toLocalFile();
    const QString appID = Util().generateRandomString();

    // Only support remove wallpaper that spans over 1 monitor
    // if (wallpaperData.monitors.length() == 1) {
    //     int i = 0;
    //     for (auto& wallpaper : m_screenPlayWallpapers) {
    //         if (wallpaper->monitors().length() == 1) {
    //             if (monitors.at(0) == wallpaper->monitors().at(0)) {
    //                 return wallpaper->replace(
    //                     wallpaperData,
    //                     m_settings->checkWallpaperVisible());
    //                 m_monitorListModel->setWallpaperMonitor(wallpaper, wallpaperData.monitors);
    //             }
    //         }
    //         i++;
    //     }
    // }

    auto wallpaper = std::make_shared<ScreenPlayWallpaper>(
        m_globalVariables,
        appID,
        wallpaperData,
        m_settings);


    QObject::connect(wallpaper.get(), &ScreenPlayWallpaper::requestSave, this, &ScreenPlayManager::requestSaveProfiles);
    QObject::connect(wallpaper.get(), &ScreenPlayWallpaper::requestClose, this, &ScreenPlayManager::removeWallpaper);
    QObject::connect(wallpaper.get(), &ScreenPlayWallpaper::error, this, &ScreenPlayManager::displayErrorPopup);
    if (!wallpaper->start()) {
        return nullptr;
    }
    m_monitorListModel->setWallpaperMonitor(wallpaper, wallpaperData.monitors);
    increaseActiveWallpaperCounter();
    return wallpaper;
}

/*!
  \brief Creates a ScreenPlayWidget object via a \a absoluteStoragePath and a \a preview image (relative path).
*/
bool ScreenPlayManager::createWidget(
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
    increaseActiveWidgetsCounter();
    m_screenPlayWidgets.append(widget);
    return true;
}

/*!
    \brief Removes all wallpaper entries in the profiles.json.
*/
bool ScreenPlayManager::removeAllWallpapers(bool saveToProfile)
{

    if (m_wallpaperTimelineSectionsList.empty()) {
        qWarning() << "Trying to remove all Wallpapers while m_screenPlayWallpapers is not empty.";
        return false;
    }

    QStringList appIDs;
    auto activeTimelineSection = findActiveSection();
    if(!activeTimelineSection){
        return false;
    }
      // Do not remove items from the vector you iterate on.
    for (auto& client : activeTimelineSection->activeWallpaperList) {
        appIDs.append(client->appID());
    }

    for (const auto& appID : appIDs) {
        if (!removeWallpaper(appID)) {
            return false;
        }
    }

    if (saveToProfile)
        emit requestSaveProfiles();

    return true;
}

/*!
    \brief Removes all widgets and resets the activeWidgetCounter to 0.
*/
bool ScreenPlayManager::removeAllWidgets(bool saveToProfile)
{
    if (m_screenPlayWidgets.empty()) {
        qWarning() << "Trying to remove all Widgets while m_screenPlayWidgets is empty.";
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
bool ScreenPlayManager::removeWallpaperAt(int index)
{

    if (auto appID = m_monitorListModel->getAppIDByMonitorIndex(index)) {
        if (removeWallpaper(*appID)) {
            emit requestSaveProfiles();
            return true;
        }
    }

    qWarning() << "Could not remove Wallpaper at index:" << index;
    return false;
}

/*!
    \brief Request a spesific json profile to display in the active wallpaper popup on the right.
*/
bool ScreenPlayManager::requestProjectSettingsAtMonitorIndex(const int index)
{
    auto activeTimelineSection = findActiveSection();
    if(!activeTimelineSection){
        return false;
    }
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
    auto activeTimelineSection = findActiveSection();
    if(!activeTimelineSection){
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

    auto activeTimelineSection = findActiveSection();
    if(!activeTimelineSection){
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

        auto activeTimelineSection = findActiveSection();
        if(!activeTimelineSection){
            return ;
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

/*!
    \brief Removes a wallpaper from the given appID. Returns true on success.
*/
bool ScreenPlayManager::removeWallpaper(const QString& appID)
{

    auto activeTimelineSection = findActiveSection();
    if(!activeTimelineSection){
        return false;
    }
    activeTimelineSection->activeWallpaperList.erase(
        std::remove_if(
            activeTimelineSection->activeWallpaperList.begin(),
            activeTimelineSection->activeWallpaperList.end(),
            [this, appID](std::shared_ptr<ScreenPlayWallpaper>& wallpaper) {
                if (wallpaper->appID() != appID) {
                    return false;
                }

                qInfo() << "Remove wallpaper " << wallpaper->file() << "at monitor " << wallpaper->monitors();

                // The MonitorListModel contains a shared_ptr of this object that needs to be removed
                // for shared_ptr to release the object.
                m_monitorListModel->setWallpaperMonitor({}, wallpaper->monitors());

                wallpaper->close();

                decreaseActiveWallpaperCounter();

                return true;
            }));

    if (activeWallpaperCounter() != activeTimelineSection->activeWallpaperList.size()) {
        qWarning() << "activeWallpaperCounter value: " << activeWallpaperCounter()
                   << "does not match m_screenPlayWallpapers length:" << activeTimelineSection->activeWallpaperList.size();
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

                decreaseActiveWidgetsCounter();

                return true;
            }));

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
    // for (auto& wallpaper : m_screenPlayWallpapers) {
    //     if (wallpaper->appID() == appID) {
    //         return wallpaper->setWallpaperValue(key, value, true);
    //     }
    // }
    return false;
}

/*!
    \brief Saves a given wallpaper \a newProfileObject to a \a profileName. We ignore the profileName argument
    because we currently only support one profile. Returns \c true if successfuly saved to profiles.json, otherwise \c false.
*/
bool ScreenPlayManager::saveProfiles()
{
    m_saveLimiter.stop();

    // QJsonArray wallpaper {};
    // for (const auto& activeWallpaper : std::as_const(m_screenPlayWallpapers)) {
    //     wallpaper.append(activeWallpaper->getActiveSettingsJson());
    // }

    QJsonArray widgets {};
    for (const auto& activeWidget : std::as_const(m_screenPlayWidgets)) {
        widgets.append(activeWidget->getActiveSettingsJson());
    }

    QJsonObject profileDefault;
    profileDefault.insert("appdrawer", QJsonArray {});
    profileDefault.insert("widgets", widgets);
    // profileDefault.insert("wallpaper", wallpaper);
    profileDefault.insert("name", "default");

    QJsonArray activeProfileList;
    activeProfileList.append(profileDefault);

    QJsonObject profile;
    profile.insert("version", "1.0.0");
    profile.insert("profiles", activeProfileList);

    if (Util().writeJsonObjectToFile({ m_globalVariables->localSettingsPath().toString() + "/profiles.json" }, profile)) {
        emit profilesSaved();
        return true;
    }
    return false;
}

/*!
 \brief Loads all wallpaper from profiles.json when the version number matches and starts the available wallpaper
*/
bool ScreenPlayManager::loadProfiles()
{
    Util util;
    const auto configObj = util.openJsonFileToObject(m_globalVariables->localSettingsPath().toString() + "/profiles.json");

    if (!configObj) {
        qWarning() << "Could not load active profiles at path: " << m_globalVariables->localSettingsPath().toString() + "/profiles.json";
        return false;
    }

    std::optional<QVersionNumber> version = util.getVersionNumberFromString(configObj->value("version").toString());
    QVersionNumber requiredVersion { 1, 0, 0 };
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

        // Dry run to check if the config is outdated. This is when a wallpaper is configured
        // but no longer available.
        for (QJsonValueRef wallpaper : wallpaper.toObject().value("wallpaper").toArray()) {
            QJsonObject wallpaperObj = wallpaper.toObject();

            if (wallpaperObj.empty())
                continue;

            const QString absolutePath = wallpaperObj.value("absolutePath").toString();
            if (!QFile(absolutePath).exists()) {
                qWarning() << "Specified file does not exist! This means the profiles.json is no longer valid.";
                return false;
            }
        }

        for (QJsonValueRef timelineWallpaper : wallpaper.toObject().value("timelineWallpaper").toArray()) {
            QJsonObject wallpaperObj = wallpaper.toObject();
            auto wallpaperDataOpt = loadTimelineWallpaperConfig(wallpaperObj);
            if (!wallpaperDataOpt.has_value()) {
                containsInvalidData = true;
                break;
            }
            m_wallpaperTimelineSectionsList.append(wallpaperDataOpt.value());
        }

        for (const QJsonValueRef widget : wallpaper.toObject().value("widgets").toArray()) {
            QJsonObject widgetObj = widget.toObject();
            if (!loadWidgetConfig(widgetObj))
                containsInvalidData = true;
        }
    }

    // The can happen if the user unpluggs a wallpaper but it still exists
    // in the settings.json. For this we save all profiles with now active
    // content.
    if (containsInvalidData)
        saveProfiles();

    checkActiveWallpaperTimeline();
    m_contentTimer.start();

    return true;
}

std::optional<std::shared_ptr<WallpaperTimelineSection>> ScreenPlayManager::loadTimelineWallpaperConfig(const QJsonObject& wallpaperObj)
{
    const QString name = wallpaperObj.value("name").toString();

    const QTime startTime = QTime::fromString(wallpaperObj.value("startTime").toString(), m_timelineTimeFormat);
    const QTime endTime = QTime::fromString(wallpaperObj.value("endTime").toString(), m_timelineTimeFormat);
    if (endTime > startTime) {
        qCritical() << "Invalid time, start time is greater then end time: " << startTime << endTime;
        return std::nullopt;
    }
    std::optional<WallpaperData> wallpaperDataOpt = loadWallpaperConfig(wallpaperObj);
    if (!wallpaperDataOpt.has_value())
        return std::nullopt;

    // TODO check license

    auto timelineSection = std::make_shared<WallpaperTimelineSection>();
    timelineSection->startTime = startTime;
    timelineSection->endTime = endTime;
    timelineSection->wallpaperData.push_back(wallpaperDataOpt.value());
    return timelineSection;
}

std::optional<WallpaperData> ScreenPlayManager::loadWallpaperConfig(const QJsonObject& wallpaperObj)
{
    if (wallpaperObj.empty())
        return std::nullopt;

    QJsonArray monitorsArray = wallpaperObj.value("monitors").toArray();

    QVector<int> monitors;
    for (const QJsonValueRef monitorNumber : monitorsArray) {
        int value = monitorNumber.toInt(-1);
        if (value == -1) {
            qWarning() << "Could not parse monitor number to display content at";
            return std::nullopt;
        }

        if (monitors.contains(value)) {
            qWarning() << "The monitor: " << value << " is sharing the config multiple times. ";
            return std::nullopt;
        }
        monitors.append(value);
    }

    float volume = static_cast<float>(wallpaperObj.value("volume").toDouble(-1.0));

    if (volume == -1.0F)
        volume = 1.0f;

    WallpaperData wallpaperData;
    wallpaperData.volume = volume;
    wallpaperData.absolutePath = wallpaperObj.value("absolutePath").toString();
    wallpaperData.previewImage = wallpaperObj.value("previewImage").toString();
    wallpaperData.playbackRate = wallpaperObj.value("playbackRate").toDouble(1.0);
    wallpaperData.file = wallpaperObj.value("file").toString();
    wallpaperData.properties = wallpaperObj.value("properties").toObject();

    const QString fillModeString = wallpaperObj.value("fillMode").toString();
    const QString typeString = wallpaperObj.value("type").toString();
    wallpaperData.type = QStringToEnum<ContentTypes::InstalledType>(typeString, ContentTypes::InstalledType::VideoWallpaper);
    wallpaperData.fillMode = QStringToEnum<Video::FillMode>(fillModeString, Video::FillMode::Cover);
    return wallpaperData;
}

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

    const bool success = createWidget(type, position, absolutePath, previewImage, properties, false);

    if (!success) {
        qWarning() << "Unable to start Widget! " << type << position << absolutePath;
        return false;
    }
    return true;
}
}

#include "moc_screenplaymanager.cpp"
