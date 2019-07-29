#include "screenplaymanager.h"

namespace ScreenPlay {

ScreenPlayManager::ScreenPlayManager(const shared_ptr<InstalledListModel>& ilm,
    const shared_ptr<Settings>& settings,
    const shared_ptr<MonitorListModel>& mlm,
    const shared_ptr<SDKConnector>& sdkc,
    QObject* parent)
    : QObject { parent }
    , m_installedListModel { ilm }
    , m_settings { settings }
    , m_monitorListModel { mlm }
    , m_sdkconnector { sdkc }
    , m_qGuiApplication { static_cast<QGuiApplication*>(QGuiApplication::instance()) }
{
}

void ScreenPlayManager::createWallpaper(
    const int monitorIndex, QUrl absoluteStoragePath,
    const QString& previewImage, const float volume,
    const QString& fillMode, const QString& type)
{

    removeWallpaperAt(monitorIndex);

    ProjectFile project {};
    if (!m_installedListModel->getProjectByAbsoluteStoragePath(&absoluteStoragePath, &project)) {
        qWarning() << "Failed to receive getProjectByAbsoluteStoragePath";
        return;
    }

    m_settings->increaseActiveWallpaperCounter();

    m_screenPlayWallpapers.emplace_back(
        make_unique<ScreenPlayWallpaper>(
            vector<int> { monitorIndex },
            m_settings,
            generateID(),
            absoluteStoragePath.toLocalFile(),
            previewImage,
            volume,
            fillMode,
            type,
            this));

    m_monitorListModel->setWallpaperActiveMonitor(m_qGuiApplication->screens().at(monitorIndex),
        QString { absoluteStoragePath.toLocalFile() + "/" + previewImage });

    m_settings->saveWallpaperToConfig(monitorIndex, absoluteStoragePath, type);
}

void ScreenPlayManager::createWidget(QUrl absoluteStoragePath, const QString& previewImage)
{
    ProjectFile project {};

    if (!m_installedListModel->getProjectByAbsoluteStoragePath(&absoluteStoragePath, &project)) {
        return;
    }

    m_settings->increaseActiveWidgetsCounter();

    m_screenPlayWidgets.emplace_back(
        make_unique<ScreenPlayWidget>(
            generateID(),
            m_settings,
            absoluteStoragePath.toLocalFile(),
            previewImage,
            QString { absoluteStoragePath.toLocalFile() + "/" + project.m_file.toString() },
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
            const vector<int>& screenNumber = uPtrWallpaper->screenNumber();
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
}
