#include "screenplaymanager.h"
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
  \brief Constructor-.
*/
ScreenPlayManager::ScreenPlayManager(
    QObject* parent)
    : QObject { parent }

    , m_server { std::make_unique<QLocalServer>() }
{

    if (checkIsAnotherScreenPlayInstanceRunning()) {
        m_isAnotherScreenPlayInstanceRunning = true;
        return;
    }

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
}

/*!
    \brief Checks if another ScreenPlay instance is running by trying to connect to a pipe
    with the name ScreenPlay.
    If successful we send a raise command and quit via m_isAnotherScreenPlayInstanceRunning = true.
*/
bool ScreenPlayManager::checkIsAnotherScreenPlayInstanceRunning()
{
    QLocalSocket socket;
    socket.connectToServer("ScreenPlay");

    if (!socket.isOpen()) {
        socket.close();
        return false;
    }

    qInfo("Another ScreenPlay app is already running!");
    QByteArray msg = "command=requestRaise";
    socket.write(msg);
    socket.waitForBytesWritten(500);
    socket.close();
    return true;
}

void ScreenPlayManager::init(
    const std::shared_ptr<GlobalVariables>& globalVariables,
    const std::shared_ptr<MonitorListModel>& mlm,
    const std::shared_ptr<GAnalytics>& telemetry,
    const std::shared_ptr<Settings>& settings)
{
    m_globalVariables = globalVariables;
    m_monitorListModel = mlm;
    m_telemetry = telemetry;
    m_settings = settings;
    loadProfiles();
}

/*!
    \brief Creates a wallpaper with a given \a monitorIndex list, \a a absoluteStoragePath folder,
    a \a previewImage (relative path to the absoluteStoragePath), a  default \a volume,
    a \a fillMode, a \a type (htmlWallpaper, qmlWallpaper etc.), a \a saveToProfilesConfigFile bool only set to flase
    if we call the method when using via the settings on startup to skip a unnecessary save.
*/
void ScreenPlayManager::createWallpaper(const InstalledType::InstalledType type,
    const FillMode::FillMode fillMode,
    const QString& absoluteStoragePath,
    const QString& previewImage,
    const QString& file,
    const QVector<int>& monitorIndex,
    const float volume,
    const float playbackRate, const QJsonObject& properties,
    const bool saveToProfilesConfigFile)
{
    auto saveToProfile = qScopeGuard([=, this] {
        // Do not save on app start
        if (saveToProfilesConfigFile) {
            emit requestSaveProfiles();
        }
    });

    if (m_telemetry) {
        m_telemetry->sendEvent("wallpaper", "start");
    }

    QJsonArray monitors;
    for (const int index : monitorIndex) {
        monitors.append(index);
    }

    const QString path = QUrl::fromUserInput(absoluteStoragePath).toLocalFile();
    const QString appID = Util::generateRandomString();

    // Only support remove wallpaper that spans over 1 monitor
    if (monitorIndex.length() == 1) {
        int i = 0;
        for (auto& wallpaper : m_screenPlayWallpapers) {
            if (wallpaper->screenNumber().length() == 1) {
                if (monitors.at(0) == wallpaper->screenNumber().at(0)) {
                    wallpaper->replace(
                        path,
                        previewImage,
                        file,
                        volume,
                        fillMode,
                        type,
                        m_settings->checkWallpaperVisible());
                    m_monitorListModel->setWallpaperActiveMonitor(wallpaper, monitorIndex);

                    return;
                }
            }
            i++;
        }
    }

    auto wallpaper = std::make_shared<ScreenPlayWallpaper>(
        monitorIndex,
        m_globalVariables,
        appID,
        path,
        previewImage,
        file,
        volume,
        playbackRate,
        fillMode,
        type,
        properties,
        m_settings->checkWallpaperVisible());

    QObject::connect(wallpaper.get(), &ScreenPlayWallpaper::requestSave, this, &ScreenPlayManager::requestSaveProfiles);
    m_screenPlayWallpapers.append(wallpaper);
    m_monitorListModel->setWallpaperActiveMonitor(wallpaper, monitorIndex);
    increaseActiveWallpaperCounter();
}

/*!
  \brief Creates a ScreenPlayWidget object via a \a absoluteStoragePath and a \a preview image (relative path).
*/
void ScreenPlayManager::createWidget(const InstalledType::InstalledType type,
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

    const QString appID = Util::generateRandomString();
    const QString path = QUrl::fromUserInput(absoluteStoragePath).toLocalFile();

    if (path.isEmpty()) {
        qInfo() << "Path is empty, Abort! String: " << absoluteStoragePath;
        return;
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
    increaseActiveWidgetsCounter();
    m_screenPlayWidgets.append(widget);
}

void ScreenPlayManager::appConnected(const std::shared_ptr<SDKConnection>& connection)
{
    for (const auto& item : m_screenPlayWidgets) {
        if (item->appID() == connection->appID()) {
            item->setSDKConnection(connection);
            return;
        }
    }

    for (const auto& item : m_screenPlayWallpapers) {
        if (item->appID() == connection->appID()) {
            item->setSDKConnection(connection);
            return;
        }
    }
}

/*!
    \brief Removes all wallpaper entries in the profiles.json. This method will likely be removed
    when using nlohmann/json in the future.
*/
void ScreenPlayManager::removeAllWallpapers()
{
    if (!m_screenPlayWallpapers.empty()) {

        closeAllWallpapers();
        m_screenPlayWallpapers.clear();

        m_monitorListModel->clearActiveWallpaper();

        emit requestSaveProfiles();
        setActiveWallpaperCounter(0);
        if (activeWallpaperCounter() != m_screenPlayWallpapers.length()) {
            if (m_telemetry) {
                m_telemetry->sendEvent("wallpaper", "error_removeAllWallpapers");
            }

            qWarning() << "activeWallpaperCounter value: " << activeWallpaperCounter()
                       << "does not match m_screenPlayWallpapers length:" << m_screenPlayWallpapers.length();
        }
    } else {
        if (m_telemetry) {
            m_telemetry->sendEvent("wallpaper", "error_removeAllWallpapers_m_screenPlayWallpapers_notEmpty");
        }
        qWarning() << "Trying to remove all wallpapers while m_screenPlayWallpapers is not empty. Count: " << m_screenPlayWallpapers.size();
    }
}

/*!
    \brief Removes all widgets and resets the activeWidgetCounter to 0.
*/
void ScreenPlayManager::removeAllWidgets()
{
    if (!m_screenPlayWidgets.empty()) {
        closeAllWidgets();
        m_screenPlayWidgets.clear();
        emit requestSaveProfiles();
        setActiveWidgetsCounter(0);
    }
}

/*!
    \brief Removes a Wallpaper at the given monitor \a at (index). Internally searches for a appID at the
    given monitor index and then closes the sdk connection, removes the entries in the
    monitor list model and decreases the active wallpaper counter property of ScreenPlayManager.
*/
bool ScreenPlayManager::removeWallpaperAt(int index)
{

    if (auto appID = m_monitorListModel->getAppIDByMonitorIndex(index)) {
        emit requestSaveProfiles();

        if (!closeWallpaper(*appID)) {
            qWarning() << "Could not  close socket. Abort!";
            return false;
        }
        m_monitorListModel->closeWallpaper(*appID);

        const QString appIDCopy = *appID;
        if (!removeWallpaperByAppID(appIDCopy)) {
            if (m_telemetry) {
                m_telemetry->sendEvent("wallpaper", "error_removeWallpaperAt_removeWallpaperByAppID");
            }
            qWarning() << "Could not remove Wallpaper " << appIDCopy << " from wallpaper list!";
            return false;
        }
        emit requestSaveProfiles();
        return true;
    }
    if (m_telemetry) {
        m_telemetry->sendEvent("wallpaper", "error_removeWallpaperAt");
    }
    qWarning() << "Could not remove Wallpaper at index:" << index;
    return false;
}

/*!
    \brief Request a spesific json profile to display in the active wallpaper popup on the right.
*/
void ScreenPlayManager::requestProjectSettingsAtMonitorIndex(const int index)
{
    for (const std::shared_ptr<ScreenPlayWallpaper>& uPtrWallpaper : qAsConst(m_screenPlayWallpapers)) {
        if (uPtrWallpaper->screenNumber()[0] == index) {

            emit projectSettingsListModelResult(
                uPtrWallpaper->getProjectSettingsListModel());
            return;
        }
    }
}

/*!
  \brief Set a wallpaper \a value at a given \a index and \a key.
*/
void ScreenPlayManager::setWallpaperValueAtMonitorIndex(const int index, const QString& key, const QString& value)
{
    if (auto appID = m_monitorListModel->getAppIDByMonitorIndex(index)) {

        setWallpaperValue(*appID, key, value);

        if (auto wallpaper = getWallpaperByAppID(*appID)) {
        }
    }
}

/*!
  \brief Convenient function to set a \a value at a given \a index and \a key for all wallaper. For exmaple used to mute all wallpaper.
*/
void ScreenPlayManager::setAllWallpaperValue(const QString& key, const QString& value)
{
    for (const std::shared_ptr<ScreenPlayWallpaper>& uPtrWallpaper : qAsConst(m_screenPlayWallpapers)) {
        setWallpaperValue(uPtrWallpaper->appID(), key, value);
    }
}

/*!
  \brief Returns \c a ScreenPlayWallpaper if successful, otherwhise \c std::nullopt.
*/
std::optional<std::shared_ptr<ScreenPlayWallpaper>> ScreenPlayManager::getWallpaperByAppID(const QString& appID)
{
    for (auto& wallpaper : m_screenPlayWallpapers) {
        if (wallpaper->appID() == appID) {
            return wallpaper;
        }
    }
    return std::nullopt;
}

/*!
    \brief Appends a new SDKConnection object shared_ptr to the m_clients list.
*/
void ScreenPlayManager::newConnection()
{
    auto connection = std::make_shared<SDKConnection>(m_server->nextPendingConnection());
    // Because user can close widgets by pressing x the widgets must send us the event
    QObject::connect(connection.get(), &SDKConnection::requestDecreaseWidgetCount, this, [this]() { setActiveWidgetsCounter(activeWallpaperCounter() - 1); });
    QObject::connect(connection.get(), &SDKConnection::requestRaise, this, &ScreenPlayManager::requestRaise);
    // Only after we receive the first message with appID and type we can set the shared reference to the
    // ScreenPlayWallpaper or ScreenPlayWidgets class
    QObject::connect(connection.get(), &SDKConnection::appConnected, this, [this](const SDKConnection* connection) {
        for (const auto& client : qAsConst(m_clients)) {
            if (client.get() == connection) {
                appConnected(client);
                return;
            }
        }
    });
    m_clients.append(connection);
}

/*!
 \brief Closes all wallpaper connection with the following type:
 \list
    \li videoWallpaper
    \li qmlWallpaper
    \li htmlWallpaper
    \li godotWallpaper
 \endlist
*/
void ScreenPlayManager::closeAllWallpapers()
{
    closeConntectionByType(GlobalVariables::getAvailableWallpaper());
}

/*!
 \brief Closes all widgets connection with the following type:
 \list
    \li qmlWidget
    \li htmlWidget
    \li standaloneWidget
 \endlist
*/
void ScreenPlayManager::closeAllWidgets()
{
    closeConntectionByType(GlobalVariables::getAvailableWidgets());
}

/*!
  \brief Closes a connection by type. Used only by closeAllWidgets() and closeAllWallpapers()
*/
void ScreenPlayManager::closeConntectionByType(const QStringList& list)
{
    for (auto& client : m_clients) {
        if (list.contains(client->type(), Qt::CaseInsensitive)) {
            client->close();
            m_clients.removeOne(client);
        }
    }
}

/*!
  \brief Closes a wallpaper by the given \a appID.
*/
bool ScreenPlayManager::closeWallpaper(const QString& appID)
{
    for (auto& client : m_clients) {
        if (client->appID() == appID) {
            client->close();
            m_clients.removeOne(client);
            return true;
        }
    }
    return false;
}

/*!
   \brief Sets a given \a value to a given \a key. The \a appID is used to identify the receiver socket.
*/
void ScreenPlayManager::setWallpaperValue(const QString& appID, const QString& key, const QVariant& value)
{
    for (const auto& wallpaper : m_screenPlayWallpapers) {
        if (wallpaper->appID() == appID) {
            wallpaper->setWallpaperValue(key, value, true);
        }
    }
}

/*!
    \brief Saves a given wallpaper \a newProfileObject to a \a profileName. We ignore the profileName argument
    because we currently only support one profile. Returns \c true if successfuly saved to profiles.json, otherwise \c false.
*/
void ScreenPlayManager::saveProfiles()
{
    m_saveLimiter.stop();
    qInfo() << "Save profiles!";
    QJsonArray wallpaper {};
    for (const auto& activeWallpaper : m_screenPlayWallpapers) {
        wallpaper.append(activeWallpaper->getActiveSettingsJson());
    }

    QJsonArray widgets {};
    for (const auto& activeWidget : m_screenPlayWidgets) {
        widgets.append(activeWidget->getActiveSettingsJson());
    }

    QJsonObject profileDefault;
    profileDefault.insert("appdrawer", QJsonArray {});
    profileDefault.insert("widgets", widgets);
    profileDefault.insert("wallpaper", wallpaper);
    profileDefault.insert("name", "default");

    QJsonArray activeProfileList;
    activeProfileList.append(profileDefault);

    QJsonObject profile;
    profile.insert("version", "1.0.0");
    profile.insert("profiles", activeProfileList);

    Util::writeJsonObjectToFile({ m_globalVariables->localSettingsPath().toString() + "/profiles.json" }, profile);
}

bool ScreenPlayManager::removeWallpaperByAppID(const QString& appID)
{
    for (auto& wallpaper : m_screenPlayWallpapers) {
        if (wallpaper->appID() == appID) {
            qInfo() << "Remove wallpaper " << wallpaper->file() << "at monitor " << wallpaper->screenNumber();
            decreaseActiveWallpaperCounter();
            m_screenPlayWallpapers.removeOne(wallpaper);
            if (activeWallpaperCounter() != m_screenPlayWallpapers.length()) {
                qWarning() << "activeWallpaperCounter value: " << activeWallpaperCounter()
                           << "does not match m_screenPlayWallpapers length:" << m_screenPlayWallpapers.length();
                return false;
            }
            return true;
        }
    }
    return false;
}

/*!
 \brief Loads all wallpaper from profiles.json when the version number matches and starts the available wallpaper
*/
void ScreenPlayManager::loadProfiles()
{

    auto configObj = Util::openJsonFileToObject(m_globalVariables->localSettingsPath().toString() + "/profiles.json");

    if (!configObj) {
        qWarning() << "Could not load active profiles at path: " << m_globalVariables->localSettingsPath().toString() + "/profiles.json";
        return;
    }

    std::optional<QVersionNumber> version = Util::getVersionNumberFromString(configObj->value("version").toString());

    if (version && *version != m_globalVariables->version()) {
        qWarning() << "Version missmatch fileVersion: " << version->toString() << "m_version: " << m_globalVariables->version().toString();
        return;
    }

    QJsonArray activeProfilesTmp = configObj->value("profiles").toArray();

    if (activeProfilesTmp.size() > 1) {
        qWarning() << "We currently only support one profile!";
        return;
    }

    for (const QJsonValueRef wallpaper : activeProfilesTmp) {

        // TODO right now we limit ourself to one default profile
        if (wallpaper.toObject().value("name").toString() != "default")
            continue;

        for (const QJsonValueRef wallpaper : wallpaper.toObject().value("wallpaper").toArray()) {
            QJsonObject wallpaperObj = wallpaper.toObject();

            if (wallpaperObj.empty())
                continue;

            QJsonArray monitorsArray = wallpaper.toObject().value("monitors").toArray();

            QVector<int> monitors;
            for (const QJsonValueRef monitorNumber : monitorsArray) {
                int value = monitorNumber.toInt(-1);
                if (value == -1) {
                    qWarning() << "Could not parse monitor number to display content at";
                    return;
                }

                if (monitors.contains(value)) {
                    qWarning() << "The monitor: " << value << " is sharing the config multiple times. ";
                    return;
                }
                monitors.append(value);
            }

            float volume = static_cast<float>(wallpaperObj.value("volume").toDouble(-1.0));

            if (volume == -1.0F)
                volume = 1.0f;

            const QString absolutePath = wallpaperObj.value("absolutePath").toString();
            const QString fillModeString = wallpaperObj.value("fillMode").toString();
            const QString previewImage = wallpaperObj.value("previewImage").toString();
            const float playbackRate = wallpaperObj.value("playbackRate").toDouble(1.0);
            const QString file = wallpaperObj.value("file").toString();
            const QString typeString = wallpaperObj.value("type").toString();
            const QJsonObject properties = wallpaperObj.value("properties").toObject();

            const auto type = QStringToEnum<InstalledType::InstalledType>(typeString, InstalledType::InstalledType::VideoWallpaper);
            const auto fillMode = QStringToEnum<FillMode::FillMode>(fillModeString, FillMode::FillMode::Cover);

            createWallpaper(type, fillMode, absolutePath, previewImage, file, monitors, volume, playbackRate, properties, false);
            monitors.clear();
        }

        for (const QJsonValueRef widget : wallpaper.toObject().value("widgets").toArray()) {
            QJsonObject widgetObj = widget.toObject();

            if (widgetObj.empty())
                continue;

            const QString absolutePath = widgetObj.value("absolutePath").toString();
            const QString previewImage = widgetObj.value("previewImage").toString();
            const QString typeString = widgetObj.value("type").toString();
            const int positionX = widgetObj.value("positionX").toInt(0);
            const int positionY = widgetObj.value("positionY").toInt(0);
            const QPoint position { positionX, positionY };
            const auto type = QStringToEnum<InstalledType::InstalledType>(typeString, InstalledType::InstalledType::QMLWidget);
            const QJsonObject properties = widgetObj.value("properties").toObject();

            createWidget(type, position, absolutePath, previewImage, properties, false);
        }
    }
}

}
