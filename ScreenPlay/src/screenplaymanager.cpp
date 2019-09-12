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
    //loadActiveProfiles();
    QObject::connect(m_monitorListModel.get(), &MonitorListModel::monitorListChanged, this, &ScreenPlayManager::monitorListChanged);
}

void ScreenPlayManager::createWallpaper(
    QVector<int> monitorIndex,
    const QString& absoluteStoragePath,
    const QString& previewImage,
    const float volume,
    const QString& fillMode,
    const QString& type)
{
    increaseActiveWallpaperCounter();

    QString path = absoluteStoragePath;
    if (absoluteStoragePath.contains("file:///"))
        path = path.remove("file:///");

    std::sort(monitorIndex.begin(), monitorIndex.end());

    auto wallpaper = make_shared<ScreenPlayWallpaper>(
        monitorIndex,
        m_globalVariables,
        Util::generateRandomString(),
        path,
        previewImage,
        volume,
        fillMode,
        type,
        this);

    m_screenPlayWallpapers.append(wallpaper);
    m_monitorListModel->setWallpaperActiveMonitor(wallpaper, monitorIndex);

    QJsonObject settings;
    settings.insert("monitors", QJsonArray {});
    settings.insert("type", type);
    settings.insert("volume", volume);
    settings.insert("isLooping", true);

    saveConfigWallpaper("default", monitorIndex, settings);
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

void ScreenPlayManager::closeWallpaper(const QVector<int> screenNumber)
{
    for (shared_ptr<ScreenPlayWallpaper>& wallpaper : m_screenPlayWallpapers) {
        if (wallpaper->screenNumber() == screenNumber) {
        }
    }
}

void ScreenPlayManager::closeAllConnections()
{
    if (!m_screenPlayWidgets.empty() || !m_screenPlayWallpapers.empty()) {
        m_sdkconnector->closeAllConnections();
        setActiveWallpaperCounter(0);
        setActiveWidgetsCounter(0);
        m_screenPlayWallpapers.clear();
        m_screenPlayWidgets.clear();
        m_monitorListModel->clearActiveWallpaper();
    }
    return;
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
    if(auto appID = m_monitorListModel->getAppIDByMonitorIndex(index)){
        m_sdkconnector->setWallpaperValue(appID.value(), key, value);
    }
}

void ScreenPlayManager::setAllWallpaperValue(const QString& key, const QString& value)
{
    for (const shared_ptr<ScreenPlayWallpaper>& uPtrWallpaper : m_screenPlayWallpapers) {
        m_sdkconnector->setWallpaperValue(uPtrWallpaper->appID(), key, value);
    }
}

void ScreenPlayManager::removeWallpaperAt(int at)
{
    if (m_screenPlayWallpapers.empty())
        return;

    const auto wallsToRemove = remove_if(
        m_screenPlayWallpapers.begin(), m_screenPlayWallpapers.end(),
        [&](const shared_ptr<ScreenPlayWallpaper>& uPtrWallpaper) -> bool {
            const QVector<int>& screenNumber = uPtrWallpaper->screenNumber();
            const bool isFound = !screenNumber.empty() && screenNumber[0] == at;
            if (isFound) {
                m_sdkconnector->closeWallpapersAt(at);
                decreaseActiveWallpaperCounter();
            }
            return isFound;
        });

    m_screenPlayWallpapers.erase(wallsToRemove, m_screenPlayWallpapers.end());
}

void ScreenPlayManager::monitorListChanged()
{
    for (const auto& wallpaperUnique_ptr : m_screenPlayWallpapers) {
        if (wallpaperUnique_ptr->screenNumber().length() > 1) {
            for (const int moitor : wallpaperUnique_ptr->screenNumber()) {
                emit m_monitorListModel->setNewActiveMonitor(
                    moitor,
                    wallpaperUnique_ptr->previewImage());
            }
        }
    }
}

bool ScreenPlayManager::saveConfigWallpaper(const QString& profileName, const QVector<int>& monitors, const QJsonObject& content)
{
    auto configObj = Util::openJsonFileToObject(m_globalVariables->localSettingsPath().toLocalFile() + "/profiles.json");

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

    for (const QJsonValueRef wallpaper : activeProfilesTmp) {
        auto wallpaperList = wallpaper.toObject().value("wallpaper").toArray();

        for (const QJsonValueRef wallpaper : wallpaperList) {
            QJsonObject wallpaperObj = wallpaper.toObject();

            if (wallpaperObj.empty())
                return false;

            QJsonArray monitorsArray = wallpaperObj.value("monitors").toArray();

            // A wallpaper can span across multiple monitors
            // But first we need to convert the QJsonArray to an QVector<int>
            QVector<int> monitorsParsed;
            for (const QJsonValueRef item : monitorsArray) {
                int tmp = item.toInt(-1);
                if (tmp == -1) {
                    continue;
                }
                monitorsParsed.append(tmp);
            }

            // We need to find the right wallpaper with the same monitors.
            if (monitorsParsed != monitors) {
                continue;
            }

            return true;
        }
    }
    return true;
}

void ScreenPlayManager::loadActiveProfiles()
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
            QString name = wallpaperObj.value("name").toString();

            createWallpaper(monitors, absolutePath, previewImage, volume, fillMode, type);
            monitors.clear();
        }
    }
}
}
