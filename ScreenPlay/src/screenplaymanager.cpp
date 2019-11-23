#include "screenplaymanager.h"

namespace ScreenPlay {

ScreenPlayManager::ScreenPlayManager(
    const shared_ptr<GlobalVariables>& globalVariables,
    const shared_ptr<MonitorListModel>& mlm,
    const shared_ptr<SDKConnector>& sdkc,
    QObject* parent)
    : QObject { parent }
    , m_globalVariables { globalVariables }
    , m_monitorListModel { mlm }
    , m_sdkconnector { sdkc }
{
    loadWallpaperProfiles();
}

void ScreenPlayManager::createWallpaper(
    QVector<int> monitorIndex,
    const QString& absoluteStoragePath,
    const QString& previewImage,
    const float volume,
    const QString& fillMode,
    const QString& type,
    const bool saveToProfilesConfigFile)
{

    QString path = absoluteStoragePath;

    if (absoluteStoragePath.contains("file:///"))
        path = path.remove("file:///");

    std::sort(monitorIndex.begin(), monitorIndex.end());

    QJsonArray monitors;
    for (const int index : monitorIndex) {
        monitors.append(index);
    }

    QJsonObject settings;

    std::shared_ptr<ScreenPlayWallpaper> wallpaper;

    qDebug() << type;
    if (type == "videoWallpaper") {
        wallpaper = make_shared<ScreenPlayWallpaper>(
            monitorIndex,
            m_globalVariables,
            Util::generateRandomString(),
            path,
            previewImage,
            volume,
            fillMode,
            type);

        settings.insert("monitors", monitors);
        settings.insert("type", type);
        settings.insert("volume", static_cast<double>(volume));
        settings.insert("isLooping", true);
        settings.insert("fillMode", fillMode);
        settings.insert("previewImage", previewImage);
        settings.insert("absolutePath", path);

    } else if (type == "qmlWallpaper") {
        wallpaper = make_shared<ScreenPlayWallpaper>(
            monitorIndex,
            m_globalVariables,
            Util::generateRandomString(),
            path,
            previewImage,
            type,
            settings);

        settings.insert("monitors", monitors);
        settings.insert("type", type);
        settings.insert("previewImage", previewImage);
        settings.insert("absolutePath", path);
    }

    m_screenPlayWallpapers.append(wallpaper);
    m_monitorListModel->setWallpaperActiveMonitor(wallpaper, monitorIndex);

    if (saveToProfilesConfigFile) {
        saveWallpaperProfile("default", settings);
    }
    increaseActiveWallpaperCounter();
}

void ScreenPlayManager::createWidget(const QUrl& absoluteStoragePath, const QString& previewImage)
{
    increaseActiveWidgetsCounter();

    m_screenPlayWidgets.append(
        make_unique<ScreenPlayWidget>(
            Util::generateRandomString(),
            m_globalVariables,
            absoluteStoragePath.toLocalFile(),
            previewImage,
            absoluteStoragePath.toString(),
            this));
}

void ScreenPlayManager::removeAllWallpapers()
{
    if (!m_screenPlayWallpapers.empty()) {
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

void ScreenPlayManager::removeWallpaperAt(int at)
{
    if (auto appID = m_monitorListModel->getAppIDByMonitorIndex(at)) {
        m_sdkconnector->closeWallpaper(appID.value());
        m_monitorListModel->closeWallpaper(appID.value());
        decreaseActiveWallpaperCounter();
    }
}

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

void ScreenPlayManager::setWallpaperValue(const int index, const QString& key, const QString& value)
{
    if (auto appID = m_monitorListModel->getAppIDByMonitorIndex(index)) {

        m_sdkconnector->setWallpaperValue(appID.value(), key, value);

        if (auto wallpaper = getWallpaperByAppID(appID.value())) {
        }
    }
}

void ScreenPlayManager::setAllWallpaperValue(const QString& key, const QString& value)
{
    for (const shared_ptr<ScreenPlayWallpaper>& uPtrWallpaper : m_screenPlayWallpapers) {
        m_sdkconnector->setWallpaperValue(uPtrWallpaper->appID(), key, value);
    }
}

std::optional<shared_ptr<ScreenPlayWallpaper>> ScreenPlayManager::getWallpaperByAppID(const QString& appID)
{
    for (auto& wallpaper : m_screenPlayWallpapers) {
        if (wallpaper->appID() == appID) {
            return wallpaper;
        }
    }
    return std::nullopt;
}

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

void ScreenPlayManager::loadWallpaperProfiles()
{

    auto configObj = Util::openJsonFileToObject(m_globalVariables->localSettingsPath().toLocalFile() + "/profiles.json");

    if (!configObj) {
        qWarning() << "Could not load active profiles.";
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

            bool parsing = true;
            bool isLooping = wallpaperObj.value("isLooping").toBool(parsing);

            if (!parsing)
                continue;

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
