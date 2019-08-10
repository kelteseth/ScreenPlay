#include "screenplaymanager.h"

namespace ScreenPlay {

ScreenPlayManager::ScreenPlayManager(const shared_ptr<InstalledListModel>& ilm,
    const shared_ptr<Settings>& settings,
    const shared_ptr<MonitorListModel>& mlm,
    const shared_ptr<SDKConnector>& sdkc, const shared_ptr<ProfileListModel>& plm,
    QObject* parent)
    : QObject { parent }
    , m_installedListModel { ilm }
    , m_settings { settings }
    , m_monitorListModel { mlm }
    , m_sdkconnector { sdkc }
    , m_profileListModel { plm }
    , m_qGuiApplication { static_cast<QGuiApplication*>(QGuiApplication::instance()) }
{
   loadActiveProfiles();
}

void ScreenPlayManager::createWallpaper(
    const int monitorIndex, const QString& absoluteStoragePath,
    const QString& previewImage, const float volume,
    const QString& fillMode, const QString& type)
{

    removeWallpaperAt(monitorIndex);

    m_settings->increaseActiveWallpaperCounter();

    QString path = absoluteStoragePath;
    if(absoluteStoragePath.contains("file:///"))
        path = path.remove("file:///");

    m_screenPlayWallpapers.emplace_back(
        make_unique<ScreenPlayWallpaper>(
            QVector<int> { monitorIndex },
            m_settings,
            generateID(),
            path,
            previewImage,
            volume,
            fillMode,
            type,
            this));

    m_monitorListModel->setWallpaperActiveMonitor(m_qGuiApplication->screens().at(monitorIndex),
        QString { path + "/" + previewImage });

    m_settings->saveWallpaperToConfig(monitorIndex, path, type);
}

void ScreenPlayManager::createWallpaper( QVector<int> monitorIndex, const QString &absoluteStoragePath, const QString &previewImage, const float volume, const QString &fillMode, const QString &type)
{
    m_settings->increaseActiveWallpaperCounter();

    QString path = absoluteStoragePath;
    if(absoluteStoragePath.contains("file:///"))
        path = path.remove("file:///");

    std::sort(monitorIndex.begin(), monitorIndex.end());

    m_screenPlayWallpapers.emplace_back(
        make_unique<ScreenPlayWallpaper>(
            monitorIndex,
            m_settings,
            generateID(),
            path,
            previewImage,
            volume,
            fillMode,
            type,
            this));


}

void ScreenPlayManager::createWidget(QUrl absoluteStoragePath, const QString& previewImage)
{
    ProjectFile project {};

    m_settings->increaseActiveWidgetsCounter();

    m_screenPlayWidgets.emplace_back(
        make_unique<ScreenPlayWidget>(
            generateID(),
            m_settings,
            absoluteStoragePath.toLocalFile(),
            previewImage,
            absoluteStoragePath.toString(),
            this));
}

void ScreenPlayManager::closeAllConnections()
{
    if (!m_screenPlayWidgets.empty() || !m_screenPlayWallpapers.empty()) {
        m_sdkconnector->closeAllConnections();
        m_settings->setActiveWallpaperCounter(0);
        m_settings->setActiveWidgetsCounter(0);
        m_screenPlayWallpapers.clear();
        m_screenPlayWidgets.clear();
        emit allWallpaperRemoved();
    }
    return;
}

void ScreenPlayManager::requestProjectSettingsListModelAt(const int index)
{
    for (const unique_ptr<ScreenPlayWallpaper>& uPtrWallpaper : m_screenPlayWallpapers) {
        if (!uPtrWallpaper->screenNumber().empty() && uPtrWallpaper->screenNumber()[0] == index) { // ??? only at index == 0
            emit projectSettingsListModelFound(
                uPtrWallpaper->projectSettingsListModel().get(),
                uPtrWallpaper->type());
            return;
        }
    }
    emit projectSettingsListModelNotFound();
}

void ScreenPlayManager::setWallpaperValue(const int at, const QString& key, const QString& value)
{
    Q_ASSERT(static_cast<size_t>(at) < m_screenPlayWallpapers.size() && m_sdkconnector);
    for (const unique_ptr<ScreenPlayWallpaper>& uPtrWallpaper : m_screenPlayWallpapers) {
        if (!uPtrWallpaper->screenNumber().empty() && m_sdkconnector && uPtrWallpaper->screenNumber()[0] == at) { // ??? only at index == 0
            m_sdkconnector->setWallpaperValue(uPtrWallpaper->appID(), key, value);
            return;
        }
    }
}

void ScreenPlayManager::setAllWallpaperValue(const QString& key, const QString& value)
{
    for (const unique_ptr<ScreenPlayWallpaper>& uPtrWallpaper : m_screenPlayWallpapers) {
        m_sdkconnector->setWallpaperValue(uPtrWallpaper->appID(), key, value);
    }
}

void ScreenPlayManager::removeWallpaperAt(const int at)
{
    if (m_screenPlayWallpapers.empty())
        return;

    const auto wallsToRemove = remove_if(
        m_screenPlayWallpapers.begin(), m_screenPlayWallpapers.end(),
        [&](const unique_ptr<ScreenPlayWallpaper>& uPtrWallpaper) -> bool {
            const QVector<int>& screenNumber = uPtrWallpaper->screenNumber();
            const bool isFound = !screenNumber.empty() && screenNumber[0] == at;
            if (isFound) {
                m_sdkconnector->closeWallpapersAt(at);
                m_settings->decreaseActiveWallpaperCounter();
            }
            return isFound;
        });

    m_screenPlayWallpapers.erase(wallsToRemove, m_screenPlayWallpapers.end());
}

QString ScreenPlayManager::generateID() const
{
    const QString possibleCharacters {
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"
    };
    const int randomStringLength = 32;
    const auto radomGen = QRandomGenerator::system();

    QString randomString;
    for (int i = 0; i < randomStringLength; ++i) {
        const int index = radomGen->bounded(possibleCharacters.length());
        const QChar nextChar = possibleCharacters.at(index);
        randomString.append(nextChar);
    }
    return randomString;
}

void ScreenPlayManager::saveConfigWallpaper(const QString& monitorID)
{

}

void ScreenPlayManager::loadActiveProfiles()
{
    QJsonDocument configJsonDocument;
    QJsonObject configObj;
    QJsonArray activeProfilesTmp;
    QFile configTmp;
    configTmp.setFileName(m_settings->localSettingsPath().toLocalFile() + "/profiles.json");

    configTmp.open(QIODevice::ReadOnly | QIODevice::Text);
    QString config = configTmp.readAll();
    configJsonDocument = QJsonDocument::fromJson(config.toUtf8());
    configObj = configJsonDocument.object();

    activeProfilesTmp = configObj.value("profilesWallpaper").toArray();

    for (const QJsonValueRef wallpaper : activeProfilesTmp) {
        QJsonObject wallpaperObj = wallpaper.toObject();

        if (wallpaperObj.empty())
            continue;

        bool parsing = true;
        bool isLooping = wallpaperObj.value("isLooping").toBool(parsing);

        if (!parsing)
            continue;

        float volume = static_cast<float>(wallpaperObj.value("volume").toDouble(parsing));

        if (!parsing)
            continue;

        QString absolutePath = wallpaperObj.value("absolutePath").toString();
        QString fillMode = wallpaperObj.value("fillMode").toString();
        QString previewImage = wallpaperObj.value("previewImage").toString();
        QString file = wallpaperObj.value("file").toString();
        QString type = wallpaperObj.value("type").toString();

        QJsonArray monitorsArray = wallpaper.toObject().value("monitors").toArray();

        if (monitorsArray.empty())
            continue;

        QMap<QString, int> monitorMap;

        // A wallpaper can span across multiple monitors
        for (const QJsonValueRef monitor : monitorsArray) {
            QJsonObject obj = monitor.toObject();

            int monitorIndex = obj.value("index").toInt(-1);
            if (monitorIndex == -1)
                continue;

            monitorMap.insert(obj.value("monitorID").toString(), monitorIndex);
        }

        createWallpaper(0, absolutePath, previewImage, volume, fillMode, type);
    }
}
}
