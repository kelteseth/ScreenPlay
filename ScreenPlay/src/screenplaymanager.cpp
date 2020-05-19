#include "screenplaymanager.h"

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
    const std::shared_ptr<GlobalVariables>& globalVariables,
    const std::shared_ptr<MonitorListModel>& mlm,
    const std::shared_ptr<SDKConnector>& sdkc,
    const std::shared_ptr<GAnalytics>& telemetry,
    const std::shared_ptr<Settings>& settings,
    QObject* parent)
    : QObject { parent }
    , m_globalVariables { globalVariables }
    , m_monitorListModel { mlm }
    , m_sdkconnector { sdkc }
    , m_telemetry { telemetry }
    , m_settings { settings }
{
    loadProfiles();
}

/*!
    \brief Creates a wallpaper with a given \a monitorIndex list, \a a absoluteStoragePath folder,
    a \a previewImage (relative path to the absoluteStoragePath), a  default \a volume,
    a \a fillMode, a \a type (htmlWallpaper, qmlWallpaper etc.), a \a saveToProfilesConfigFile bool only set to flase
    if we call the method when using via the settings on startup to skip a unnecessary save.
*/
void ScreenPlayManager::createWallpaper(
    const Enums::WallpaperType type,
    const Enums::FillMode fillMode,
    const QString& absoluteStoragePath,
    const QString& previewImage,
    const QString& file,
    QVector<int> monitorIndex,
    const float volume,
    const bool saveToProfilesConfigFile)
{
    if (m_telemetry) {
        m_telemetry->sendEvent("wallpaper", "start");
    }

    QJsonArray monitors;
    for (const int index : monitorIndex) {
        monitors.append(index);
    }

    QString path = QUrl::fromUserInput(absoluteStoragePath).toLocalFile();
    QString appID = Util::generateRandomString();

    std::shared_ptr<ScreenPlayWallpaper> wallpaper;
    wallpaper = std::make_shared<ScreenPlayWallpaper>(
        monitorIndex,
        m_globalVariables,
        appID,
        path,
        previewImage,
        file,
        volume,
        fillMode,
        type,
        m_settings->checkWallpaperVisible());

    // Only support remove wallpaper that spans over 1 monitor
    if (monitorIndex.length() == 1) {
        int i = 0;
        for (auto& wallpaperIterator : m_screenPlayWallpapers) {
            if (wallpaperIterator->screenNumber().length() == 1) {
                if (monitors.at(0) == wallpaperIterator->screenNumber().at(0)) {
                    if (!removeWallpaperAt(i)) {
                        qWarning() << "Could not remove wallpaper at index " << i;
                    }
                }
            }
            i++;
        }
    }

    m_screenPlayWallpapers.append(wallpaper);
    m_monitorListModel->setWallpaperActiveMonitor(wallpaper, monitorIndex);
    increaseActiveWallpaperCounter();

    // Do not save on app start
    if (saveToProfilesConfigFile) {
        saveProfiles();
    }
}

/*!
  \brief Creates a ScreenPlayWidget object via a \a absoluteStoragePath and a \a preview image (relative path).
*/
void ScreenPlayManager::createWidget(
    const Enums::WidgetType type,
    const QUrl& absoluteStoragePath,
    const QString& previewImage)
{
    if (m_telemetry) {
        m_telemetry->sendEvent("widget", "start");
    }
    increaseActiveWidgetsCounter();

    m_screenPlayWidgets.append(
        std::make_unique<ScreenPlayWidget>(
            Util::generateRandomString(),
            m_globalVariables,
            absoluteStoragePath.toLocalFile(),
            previewImage,
            absoluteStoragePath.toString(),
            type));
}

/*!
    \brief Removes all wallpaper entries in the profiles.json. This method will likely be removed
    when using nlohmann/json in the future.
*/
void ScreenPlayManager::removeAllWallpapers()
{
    if (!m_screenPlayWallpapers.empty()) {
        if (m_telemetry) {
            m_telemetry->sendEvent("wallpaper", "stopAll");
        }
        m_sdkconnector->closeAllWallpapers();
        m_screenPlayWallpapers.clear();

        m_monitorListModel->clearActiveWallpaper();

        saveProfiles();
        setActiveWallpaperCounter(0);
    } else {
        qWarning() << "Trying to remove all wallpapers while m_screenPlayWallpapers is not empty. Count: " << m_screenPlayWallpapers.size();
    }
}

/*!
    \brief Removes all widgets and resets the activeWidgetCounter to 0.
*/
void ScreenPlayManager::removeAllWidgets()
{
    if (!m_screenPlayWidgets.empty()) {
        if (m_telemetry) {
            m_telemetry->sendEvent("widgets", "stopAll");
        }
        m_sdkconnector->closeAllWidgets();
        saveProfiles();
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
    if (m_telemetry) {
        m_telemetry->sendEvent("wallpaper", "removeSingleWallpaper");
    }

    if (auto appID = m_monitorListModel->getAppIDByMonitorIndex(index)) {
        if (!saveProfiles()) {
            qWarning() << "Could not save profiles.json";
        }
        if (!m_sdkconnector->closeWallpaper(*appID)) {
            qWarning() << "Could not  close socket. Abort!";
            return false;
        }
        m_monitorListModel->closeWallpaper(*appID);

        const QString appIDCopy = *appID;
        if (!removeWallpaperByAppID(appIDCopy)) {
            qWarning() << "Could not remove Wallpaper " << appIDCopy << " from wallpaper list!";
            return false;
        }
        saveProfiles();
        return true;
    }
    qWarning() << "Could not remove Wallpaper at index:" << index;
    return false;
}

/*!
    \brief Request a spesific json profile to display in the active wallpaper popup on the right.
*/
void ScreenPlayManager::requestProjectSettingsListModelAt(const int index)
{
    for (const std::shared_ptr<ScreenPlayWallpaper>& uPtrWallpaper : qAsConst(m_screenPlayWallpapers)) {
        if (!uPtrWallpaper->screenNumber().empty() && uPtrWallpaper->screenNumber()[0] == index) {
            emit projectSettingsListModelResult(
                true,
                uPtrWallpaper->projectSettingsListModel().get(),
                uPtrWallpaper->type());
            return;
        }
    }
    emit projectSettingsListModelResult(false);
}

/*!
  \brief Set a wallpaper \a value at a given \a index and \a key.
*/
void ScreenPlayManager::setWallpaperValue(const int index, const QString& key, const QString& value)
{
    if (auto appID = m_monitorListModel->getAppIDByMonitorIndex(index)) {

        m_sdkconnector->setWallpaperValue(*appID, key, value);

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
        m_sdkconnector->setWallpaperValue(uPtrWallpaper->appID(), key, value);
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
    \brief Saves a given wallpaper \a newProfileObject to a \a profileName. We ignore the profileName argument
    because we currently only support one profile. Returns \c true if successfuly saved to profiles.json, otherwise \c false.
*/
bool ScreenPlayManager::saveProfiles()
{

    QJsonArray wallpaper {};

    for (const auto activeWallpaper : m_screenPlayWallpapers) {
        wallpaper.append(activeWallpaper->getActiveSettingsJson());
    }

    QJsonObject profileDefault;
    profileDefault.insert("widgets", QJsonArray {});
    profileDefault.insert("appdrawer", QJsonArray {});
    profileDefault.insert("wallpaper", wallpaper);
    profileDefault.insert("name", "default");

    QJsonArray activeProfileList;
    activeProfileList.append(profileDefault);

    QJsonObject profile;
    profile.insert("version", "1.0.0");
    profile.insert("profiles", activeProfileList);

    return Util::writeJsonObjectToFile({ m_globalVariables->localSettingsPath().toString() + "/profiles.json" }, profile);
}

bool ScreenPlayManager::removeWallpaperByAppID(const QString& appID)
{
    for (auto wallpaper : m_screenPlayWallpapers) {
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

            QString absolutePath = wallpaperObj.value("absolutePath").toString();
            QString fillModeString = wallpaperObj.value("fillMode").toString();
            QString previewImage = wallpaperObj.value("previewImage").toString();
            QString file = wallpaperObj.value("file").toString();
            QString typeString = wallpaperObj.value("type").toString();

            auto type = QStringToEnum<Enums::WallpaperType>(typeString, Enums::WallpaperType::VideoWallpaper);
            auto fillMode = QStringToEnum<Enums::FillMode>(fillModeString, Enums::FillMode::Cover);

            createWallpaper(type, fillMode, absolutePath, previewImage, file, monitors, volume, false);
            monitors.clear();
        }
    }
}
}
