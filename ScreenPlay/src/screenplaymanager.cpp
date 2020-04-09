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
    loadWallpaperProfiles();
}

/*!
    \brief Creates a wallpaper with a given \a monitorIndex list, \a a absoluteStoragePath folder,
    a \a previewImage (relative path to the absoluteStoragePath), a  default \a volume,
    a \a fillMode, a \a type (htmlWallpaper, qmlWallpaper etc.), a \a saveToProfilesConfigFile bool only set to flase
    if we call the method when using via the settings on startup to skip a unnecessary save.
*/
void ScreenPlayManager::createWallpaper(
    QVector<int> monitorIndex,
    const GlobalVariables::WallpaperType type,
    const QString& absoluteStoragePath,
    const QString& previewImage,
    const float volume,
    const QString& fillMode,
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
    wallpaper = std::make_shared<ScreenPlayWallpaper>(
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
                    if (!removeWallpaperAt(i)) {
                        qWarning() << "Could not remove wallpaper at index " << i;
                    }
                }
            }
            i++;
        }
    }

    // Do not save on app start
    if (saveToProfilesConfigFile) {
        if (type == GlobalVariables::WallpaperType::VideoWallpaper) {
            QJsonObject settings = wallpaper->getActiveSettingsJson();

            saveWallpaperProfile("default", settings);
            return;
        }
        if (type == GlobalVariables::WallpaperType::QMLWallpaper) {
            QJsonObject settings;
            settings.insert("monitors", monitors);
            settings.insert("type", QVariant::fromValue(type).toString());
            settings.insert("previewImage", previewImage);
            settings.insert("absolutePath", path);

            saveWallpaperProfile("default", settings);
        }
    }

    m_screenPlayWallpapers.append(wallpaper);
    m_monitorListModel->setWallpaperActiveMonitor(wallpaper, monitorIndex);
    increaseActiveWallpaperCounter();
}

/*!
  \brief Creates a ScreenPlayWidget object via a \a absoluteStoragePath and a \a preview image (relative path).
*/
void ScreenPlayManager::createWidget(const QUrl& absoluteStoragePath, const QString& previewImage, const QString& type)
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

        QString absoluteProfilesFilePath = m_globalVariables->localSettingsPath().toString() + "/profiles.json";
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
        setActiveWidgetsCounter(0);
    }
}

/*!
    \brief Removes a Wallpaper at the given monitor \a at (index). Internally searches for a appID at the
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
    \brief Request a spesific json profile to display in the active wallpaper popup on the right.
*/
void ScreenPlayManager::requestProjectSettingsListModelAt(const int index)
{
    for (const std::shared_ptr<ScreenPlayWallpaper>& uPtrWallpaper : qAsConst(m_screenPlayWallpapers)) {
        if (!uPtrWallpaper->screenNumber().empty() && uPtrWallpaper->screenNumber()[0] == index) {
            emit projectSettingsListModelFound(
                uPtrWallpaper->projectSettingsListModel().get(),
                QVariant::fromValue(uPtrWallpaper->type()).toString());
            return;
        }
    }
    emit projectSettingsListModelNotFound();
}

/*!
  \brief Set a wallpaper \a value at a given \a index and \a key.
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
bool ScreenPlayManager::saveWallpaperProfile(const QString& profileName, const QJsonObject& newProfileObject)
{
    // Remove when implementing profiles
    Q_UNUSED(profileName)

    QJsonArray wallpaper {};

    for (const auto activeWallpaper : m_screenPlayWallpapers) {
        wallpaper.append(activeWallpaper->getActiveSettingsJson());
    }

    QJsonObject profileDefault;
    profileDefault.insert("widgets", QJsonArray {});
    profileDefault.insert("appdrawer", QJsonArray {});
    profileDefault.insert("wallpaper", wallpaper);
    profileDefault.insert("name", "default");

    QJsonArray profiles;
    profiles.append(profileDefault);

    QJsonObject profile;
    profile.insert("version", "1.0.0");
    profile.insert("profiles", profiles);

    return Util::writeJsonObjectToFile({ m_globalVariables->localSettingsPath().toString() + "/profiles.json" }, profile);
}

/*!
 \brief Loads all wallpaper from profiles.json when the version number matches and starts the available wallpaper
*/
void ScreenPlayManager::loadWallpaperProfiles()
{

    auto configObj = Util::openJsonFileToObject(m_globalVariables->localSettingsPath().toString() + "/profiles.json");

    if (!configObj) {
        qWarning() << "Could not load active profiles at path: " << m_globalVariables->localSettingsPath().toString() + "/profiles.json";
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

            createWallpaper(monitors, QStringToEnum<GlobalVariables::WallpaperType>(type, GlobalVariables::WallpaperType::VideoWallpaper), absolutePath, previewImage, volume, fillMode, false);
            monitors.clear();
        }
    }
}
}
