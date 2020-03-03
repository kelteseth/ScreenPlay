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
  Constructor
*/
ScreenPlayManager::ScreenPlayManager(
    const shared_ptr<GlobalVariables>& globalVariables,
    const shared_ptr<MonitorListModel>& mlm,
    const shared_ptr<SDKConnector>& sdkc,
    const shared_ptr<GAnalytics>& telemetry,
    const shared_ptr<Settings>& settings,
    QObject* parent)
    : QObject { parent }
    , m_globalVariables { globalVariables }
    , m_monitorListModel { mlm }
    , m_sdkconnector { sdkc }
    , m_telemetry { telemetry }
    , m_settings { settings }
{
    loadWallpaperProfiles();
}

/*!
    Creates a wallpaper with a given \a monitorIndex list, \a a absoluteStoragePath folder,
    a \a previewImage (relative path to the absoluteStoragePath), a  default \a volume,
    a \a fillMode, a \a type (htmlWallpaper, qmlWallpaper etc.), a \a saveToProfilesConfigFile bool only set to flase
    if we call the method when using via the settings on startup to skip a unnecessary save.
 */
void ScreenPlayManager::createWallpaper(
    QVector<int> monitorIndex,
    const QString& absoluteStoragePath,
    const QString& previewImage,
    const float volume,
    const QString& fillMode,
    const QString& type,
    const bool saveToProfilesConfigFile)
{
    if (m_telemetry) {
        m_telemetry->sendEvent("wallpaper", "start");
    }
    QString path = absoluteStoragePath;

    if (absoluteStoragePath.contains("file:///"))
        path = path.remove("file:///");

    std::sort(monitorIndex.begin(), monitorIndex.end());

    QJsonArray monitors;
    for (const int index : monitorIndex) {
        monitors.append(index);
    }

    std::shared_ptr<ScreenPlayWallpaper> wallpaper;
    wallpaper = make_shared<ScreenPlayWallpaper>(
        monitorIndex,
        m_globalVariables,
        Util::generateRandomString(),
        path,
        previewImage,
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
                    removeWallpaperAt(i);
                }
            }
            i++;
        }
    }

    m_screenPlayWallpapers.append(wallpaper);
    m_monitorListModel->setWallpaperActiveMonitor(wallpaper, monitorIndex);
    increaseActiveWallpaperCounter();

    if (saveToProfilesConfigFile && type == "videoWallpaper") {
        QJsonObject settings;
        settings.insert("monitors", monitors);
        settings.insert("type", type);
        settings.insert("volume", static_cast<double>(volume));
        settings.insert("isLooping", true);
        settings.insert("fillMode", fillMode);
        settings.insert("previewImage", previewImage);
        settings.insert("absolutePath", path);

        saveWallpaperProfile("default", settings);
    }
}

/*!
  Creates a ScreenPlayWidget object via a \a absoluteStoragePath and a \a preview image (relative path).
 */
void ScreenPlayManager::createWidget(const QUrl& absoluteStoragePath, const QString& previewImage, const QString& type)
{
    if (m_telemetry) {
        m_telemetry->sendEvent("widget", "start");
    }
    increaseActiveWidgetsCounter();

    m_screenPlayWidgets.append(
        make_unique<ScreenPlayWidget>(
            Util::generateRandomString(),
            m_globalVariables,
            absoluteStoragePath.toLocalFile(),
            previewImage,
            absoluteStoragePath.toString(),
            type));
}

/*!
    Removes all wallpaper entries in the profiles.json. This method will likely be removed
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

        QString absoluteProfilesFilePath = m_globalVariables->localSettingsPath().toLocalFile() + "/profiles.json";
        auto configOptional = Util::openJsonFileToObject(absoluteProfilesFilePath);

        if (!configOptional) {
            qWarning() << "Could not load active profiles.";
            return;
        }

        QJsonObject oldConfig = configOptional.value();
        QJsonObject oldConfigProfile = oldConfig.value("profiles").toArray().first().toObject();

        QJsonObject profileDefault;
        profileDefault.insert("widgets", oldConfigProfile.value("widgets").toArray());
        profileDefault.insert("appdrawer", oldConfigProfile.value("appdrawer").toArray());
        profileDefault.insert("wallpaper", QJsonArray());
        profileDefault.insert("name", "default");

        QJsonArray profiles;
        profiles.append(profileDefault);

        QJsonObject newConfig;
        newConfig.insert("version", oldConfig.value("version").toString());
        newConfig.insert("profiles", profiles);

        Util::writeJsonObjectToFile(absoluteProfilesFilePath, newConfig);
        setActiveWallpaperCounter(0);
    }
}

void ScreenPlayManager::removeAllWidgets()
{
    if (!m_screenPlayWidgets.empty()) {
        if (m_telemetry) {
            m_telemetry->sendEvent("widgets", "stopAll");
        }
        m_sdkconnector->closeAllWidgets();
        setActiveWidgetsCounter(0);
    }
}

/*!
    Removes a Wallpaper at the given monitor \a at (index). Internally searches for a appID at the
    given monitor index and then closes the sdk connection, removes the entries in the
    monitor list model and decreases the active wallpaper counter property of ScreenPlayManager.
*/
bool ScreenPlayManager::removeWallpaperAt(int at)
{
    if (m_telemetry) {
        m_telemetry->sendEvent("wallpaper", "removeSingleWallpaper");
    }
    if (auto appID = m_monitorListModel->getAppIDByMonitorIndex(at)) {

        m_sdkconnector->closeWallpaper(appID.value());
        m_monitorListModel->closeWallpaper(appID.value());
        decreaseActiveWallpaperCounter();
        return true;
    }
    return false;
}

/*!
 Request a spesific json profile to display in the active wallpaper popup on the right.

 */
void ScreenPlayManager::requestProjectSettingsListModelAt(const int index)
{
    for (const shared_ptr<ScreenPlayWallpaper>& uPtrWallpaper : m_screenPlayWallpapers) {
        if (!uPtrWallpaper->screenNumber().empty() && uPtrWallpaper->screenNumber()[0] == index) {
            emit projectSettingsListModelFound(
                uPtrWallpaper->projectSettingsListModel().get(),
                uPtrWallpaper->type());
            return;
        }
    }
    emit projectSettingsListModelNotFound();
}

/*!
  Set a wallpaper \a value at a given \a index and \a key.
 */
void ScreenPlayManager::setWallpaperValue(const int index, const QString& key, const QString& value)
{
    if (auto appID = m_monitorListModel->getAppIDByMonitorIndex(index)) {

        m_sdkconnector->setWallpaperValue(appID.value(), key, value);

        if (auto wallpaper = getWallpaperByAppID(appID.value())) {
        }
    }
}

/*!
  Convenient function to set a \a value at a given \a index and \a key for all wallaper. For exmaple used to mute all wallpaper.
 */
void ScreenPlayManager::setAllWallpaperValue(const QString& key, const QString& value)
{
    for (const shared_ptr<ScreenPlayWallpaper>& uPtrWallpaper : m_screenPlayWallpapers) {
        m_sdkconnector->setWallpaperValue(uPtrWallpaper->appID(), key, value);
    }
}

/*!
  Returns \c a ScreenPlayWallpaper if successful, otherwhise \c std::nullopt.
 */
std::optional<shared_ptr<ScreenPlayWallpaper>> ScreenPlayManager::getWallpaperByAppID(const QString& appID)
{
    for (auto& wallpaper : m_screenPlayWallpapers) {
        if (wallpaper->appID() == appID) {
            return wallpaper;
        }
    }
    return std::nullopt;
}

/*!
    Saves a given wallpaper \a newProfileObject to a \a profileName. We ignore the profileName argument
    because we currently only support one profile. Returns \c true if successfuly saved to profiles.json, otherwise \c false.
 */
bool ScreenPlayManager::saveWallpaperProfile(const QString& profileName, const QJsonObject& newProfileObject)
{
    // Remove when implementing profiles
    Q_UNUSED(profileName)

    QString absoluteProfilesFilePath = m_globalVariables->localSettingsPath().toLocalFile() + "/profiles.json";
    auto configOptional = Util::openJsonFileToObject(absoluteProfilesFilePath);

    if (!configOptional) {
        qWarning() << "Could not load active profiles.";
        return false;
    }

    QJsonObject oldConfig = configOptional.value();
    QJsonObject oldConfigProfile = oldConfig.value("profiles").toArray().first().toObject();
    QJsonArray oldWallpaperArray = oldConfigProfile.value("wallpaper").toArray();
    QJsonArray newWallpaperArray;

    if (oldWallpaperArray.empty()) {
        newWallpaperArray.append(newProfileObject);
    } else {
        for (QJsonValueRef wallpaper : oldWallpaperArray) {
            QJsonObject entry = wallpaper.toObject();

            if (entry.value("monitors").toArray() != newProfileObject.value("monitors").toArray()) {
                newWallpaperArray.append(entry);
                continue;
            }

            if ((entry.value("absolutePath").toString() == newProfileObject.value("absolutePath").toString())) {
                newWallpaperArray.append(newProfileObject);
            }
        }
    }

    QJsonObject profileDefault;
    profileDefault.insert("widgets", oldConfigProfile.value("widgets").toArray());
    profileDefault.insert("appdrawer", oldConfigProfile.value("appdrawer").toArray());
    profileDefault.insert("wallpaper", newWallpaperArray);
    profileDefault.insert("name", "default");

    QJsonArray profiles;
    profiles.append(profileDefault);

    QJsonObject newConfig;
    newConfig.insert("version", oldConfig.value("version").toString());
    newConfig.insert("profiles", profiles);

    return Util::writeJsonObjectToFile(absoluteProfilesFilePath, newConfig);
}

/*!
 Loads all wallpaper from profiles.json when the version number matches and starts the available wallpaper
 */
void ScreenPlayManager::loadWallpaperProfiles()
{

    auto configObj = Util::openJsonFileToObject(m_globalVariables->localSettingsPath().toLocalFile() + "/profiles.json");

    if (!configObj) {
        qWarning() << "Could not load active profiles at path: " << m_globalVariables->localSettingsPath().toLocalFile() + "/profiles.json";
        return;
    }

    std::optional<QVersionNumber> version = Util::getVersionNumberFromString(configObj.value().value("version").toString());

    if (version && version.value() != m_globalVariables->version()) {
        qWarning() << "Version missmatch fileVersion: " << version.value().toString() << "m_version: " << m_globalVariables->version().toString();
        return;
    }

    QJsonArray activeProfilesTmp = configObj.value().value("profiles").toArray();

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
            QString fillMode = wallpaperObj.value("fillMode").toString();
            QString previewImage = wallpaperObj.value("previewImage").toString();
            QString file = wallpaperObj.value("file").toString();
            QString type = wallpaperObj.value("type").toString();

            createWallpaper(monitors, absolutePath, previewImage, volume, fillMode, type, false);
            monitors.clear();
        }
    }
}
}
