#include "screenplaymanager.h"

namespace ScreenPlay {

ScreenPlayManager::ScreenPlayManager(const shared_ptr<GlobalVariables>& globalVariables,
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
    const QString& type)
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
    if (type == "videoWallpaper") {
        wallpaper = make_shared<ScreenPlayWallpaper>(
            monitorIndex,
            m_globalVariables,
            Util::generateRandomString(),
            path,
            previewImage,
            volume,
            fillMode,
            type,
            this);

        settings.insert("monitors", monitors);
        settings.insert("type", type);
        settings.insert("volume", static_cast<double>(volume));
        settings.insert("isLooping", true);
        settings.insert("fillMode", fillMode);
        settings.insert("previeImage", previewImage);
        settings.insert("absolutePath", absoluteStoragePath);

    } else if(type == "qmlWallpaper"){
        wallpaper = make_shared<ScreenPlayWallpaper>(
            monitorIndex,
            m_globalVariables,
            Util::generateRandomString(),
            path,
            previewImage,
            type,
            this);
    }

    m_screenPlayWallpapers.append(wallpaper);
    m_monitorListModel->setWallpaperActiveMonitor(wallpaper, monitorIndex);



    saveWallpaperProfile("default", settings);
    increaseActiveWallpaperCounter();
}

void ScreenPlayManager::createWidget(const QUrl& absoluteStoragePath, const QString& previewImage)
{
    ProjectFile project {};

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
    if (!m_screenPlayWidgets.empty() || !m_screenPlayWallpapers.empty()) {
        m_sdkconnector->closeAllWallpapers();
        m_screenPlayWallpapers.clear();
        m_monitorListModel->clearActiveWallpaper();

        QString absoluteProfilesFilePath = m_globalVariables->localSettingsPath().toLocalFile() + "/profiles.json";
        auto configOptional = Util::openJsonFileToObject(absoluteProfilesFilePath);

        if (!configOptional.has_value()) {
            qWarning() << "Could not load active profiles.";
            return;
        }

        QJsonObject configObj = configOptional.value();

        // TODO right now we limit ourself to one default profile
        QJsonArray activeProfilesTmp = configObj.value("profiles").toArray();

        if (activeProfilesTmp.size() != 1) {
            qWarning() << "We currently only support one profile!";
            return;
        }

        for (const QJsonValueRef wallpaper : activeProfilesTmp) {
            wallpaper.toObject().remove("wallpaper");
            wallpaper.toObject().insert("wallpaper", QJsonArray());
        }

        configObj.remove("profiles");
        configObj.insert("profiles", activeProfilesTmp);
        //qDebug() << configObj;

        Util::writeJsonObjectToFile(absoluteProfilesFilePath, configObj);
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
    auto appID = m_monitorListModel->getAppIDByMonitorIndex(index);

    if (!appID.has_value()) {
        return;
    }

    m_sdkconnector->setWallpaperValue(appID.value(), key, value);

    for (auto& wallpaper : m_screenPlayWallpapers) {
        if (wallpaper->appID() == appID.value()) {
        }
    }
}

void ScreenPlayManager::setAllWallpaperValue(const QString& key, const QString& value)
{
    for (const shared_ptr<ScreenPlayWallpaper>& uPtrWallpaper : m_screenPlayWallpapers) {
        m_sdkconnector->setWallpaperValue(uPtrWallpaper->appID(), key, value);
    }
}

bool ScreenPlayManager::saveWallpaperProfile(const QString& profileName, const QJsonObject& newProfileObject)
{
    // Remove when implementing profiles
    Q_UNUSED(profileName)

    QString absoluteProfilesFilePath = m_globalVariables->localSettingsPath().toLocalFile() + "/profiles.json";
    auto configObj = Util::openJsonFileToObject(absoluteProfilesFilePath);

    if (!configObj.has_value()) {
        qWarning() << "Could not load active profiles.";
        return false;
    }

    // TODO right now we limit ourself to one default profile
    QJsonArray activeProfilesTmp = configObj.value().value("profiles").toArray();

    if (activeProfilesTmp.size() != 1) {
        qWarning() << "We currently only support one profile!";
        return false;
    }

    int i { 0 };
    for (const QJsonValueRef wallpaper : activeProfilesTmp) {
        auto wallpaperList = wallpaper.toObject().value("wallpaper").toArray();

        for (const QJsonValueRef wallpaper : wallpaperList) {
            QJsonObject currentProfileObject = wallpaper.toObject();

            if (currentProfileObject.empty())
                return false;

            if (currentProfileObject.value("monitors").toArray() != newProfileObject.value("monitors").toArray())
                continue;

            wallpaperList.removeAt(i);

            wallpaperList.append(newProfileObject);

            Util::writeJsonObjectToFile(absoluteProfilesFilePath, configObj.value());

            return true;
        }
        ++i;
    }
    return false;
}

void ScreenPlayManager::loadWallpaperProfiles()
{

    auto configObj = Util::openJsonFileToObject(m_globalVariables->localSettingsPath().toLocalFile() + "/profiles.json");

    if (!configObj.has_value()) {
        qWarning() << "Could not load active profiles.";
        return;
    }

    std::optional<QVersionNumber> version = Util::getVersionNumberFromString(configObj.value().value("version").toString());

    if (version.has_value() && version.value() != m_globalVariables->version()) {
        qWarning() << "Version missmatch fileVersion: " << version.value().toString() << "m_version: " << m_globalVariables->version().toString();
        return;
    }

    QJsonArray activeProfilesTmp = configObj.value().value("profiles").toArray();

    if (activeProfilesTmp.size() != 1) {
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

            createWallpaper(monitors, absolutePath, previewImage, volume, fillMode, type);
            monitors.clear();
        }
    }
}
}
