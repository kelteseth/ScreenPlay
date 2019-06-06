#include "screenplay.h"

ScreenPlay::ScreenPlay(const shared_ptr<InstalledListModel> &ilm,
    const shared_ptr<Settings>& settings,
    const shared_ptr<MonitorListModel> &mlm,
    const shared_ptr<SDKConnector>&  sdkc,
    QObject* parent)
    : QObject { parent }
    , m_ilm { ilm }
    , m_settings { settings }
    , m_mlm { mlm }
    , m_sdkc { sdkc }
    , m_qGuiApplication { static_cast<QGuiApplication*>(QGuiApplication::instance()) }
{
}

void ScreenPlay::createWallpaper(
    const int monitorIndex, QUrl absoluteStoragePath,
    const QString& previewImage, const float volume,
    const QString& fillMode, const QString& type)
{
    ProjectFile project {};
    if (!m_ilm->getProjectByAbsoluteStoragePath(&absoluteStoragePath, &project)) {
        return;
    }

    // Remove previous wallpaper, if any
    this->removeWallpaperAt(0);
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

    m_mlm->setWallpaperActiveMonitor(m_qGuiApplication->screens().at(monitorIndex),
        QString { absoluteStoragePath.toLocalFile() + "/" + previewImage });
}

void ScreenPlay::createWidget(QUrl absoluteStoragePath, const QString& previewImage)
{
    ProjectFile project {};

    if (!m_ilm->getProjectByAbsoluteStoragePath(&absoluteStoragePath, &project)) {
        return;
    }

    m_screenPlayWidgets.emplace_back(
        make_unique<ScreenPlayWidget>(
            generateID(),
            m_settings,
            absoluteStoragePath.toLocalFile(),
            previewImage,
            QString { absoluteStoragePath.toLocalFile() + "/" + project.m_file.toString() },
            this));
}

void ScreenPlay::removeAllWallpaper() noexcept
{
    if (m_sdkc && m_settings && !m_screenPlayWallpapers.empty()) {
        m_sdkc->closeAllWallpapers();
        m_settings->setActiveWallpaperCounter(0);
        m_screenPlayWallpapers.clear();
        emit allWallpaperRemoved();
    }
    return;
}

void ScreenPlay::requestProjectSettingsListModelAt(const int index) noexcept
{
    for (const unique_ptr<ScreenPlayWallpaper>& uPtrWallpaper : m_screenPlayWallpapers) {
        if (!uPtrWallpaper->screenNumber().empty() && uPtrWallpaper->screenNumber()[0] == index) { // ??? only at index == 0
            emit projectSettingsListModelFound(
                uPtrWallpaper->projectSettingsListModel().data(),
                uPtrWallpaper->type());
            return;
        }
    }
    emit projectSettingsListModelNotFound();
}

void ScreenPlay::setWallpaperValue(const int at, const QString& key, const QString& value) noexcept
{
    Q_ASSERT(static_cast<size_t>(at) < m_screenPlayWallpapers.size() && m_sdkc);
    for (const unique_ptr<ScreenPlayWallpaper>& uPtrWallpaper : m_screenPlayWallpapers) {
        if (!uPtrWallpaper->screenNumber().empty() && m_sdkc && uPtrWallpaper->screenNumber()[0] == at) { // ??? only at index == 0
            m_sdkc->setWallpaperValue(uPtrWallpaper->appID(), key, value);
            return;
        }
    }
}

void ScreenPlay::setAllWallpaperValue(const QString& key, const QString& value) noexcept
{
    Q_ASSERT(m_sdkc);
    for (const unique_ptr<ScreenPlayWallpaper>& uPtrWallpaper : m_screenPlayWallpapers) {
        if (m_sdkc)
            m_sdkc->setWallpaperValue(uPtrWallpaper->appID(), key, value);
    }
}

void ScreenPlay::removeWallpaperAt(const int at)
{
    Q_ASSERT(m_sdkc);
    qDebug() << "No of walls in list: " << m_screenPlayWallpapers.size();
    if (m_screenPlayWallpapers.empty())
        return; // done here;

    const auto wallsToRemove = remove_if(
        m_screenPlayWallpapers.begin(), m_screenPlayWallpapers.end(),
        [&](const unique_ptr<ScreenPlayWallpaper>& uPtrWallpaper) noexcept->bool {
            const vector<int>& screenNumber = uPtrWallpaper->screenNumber();
            qDebug() << "Screen No. or vector size :" << screenNumber.size();
            const bool isFound = !screenNumber.empty() && screenNumber[0] == at;
            if (isFound) {
                // m_mlm
                m_sdkc->closeWallpapersAt(at);
                m_settings->decreaseActiveWallpaperCounter();
                qDebug() << "current wall count... " << m_settings->activeWallpaperCounter();
            }
            return isFound;
        });
    m_screenPlayWallpapers.erase(wallsToRemove, m_screenPlayWallpapers.end());
    qDebug() << "After removing: No of walls in list: " << m_screenPlayWallpapers.size();
}

vector<int> ScreenPlay::getMonitorByAppID(const QString& appID) const
{
    for (const unique_ptr<ScreenPlayWallpaper>& uPtrWallpaper : m_screenPlayWallpapers) {
        if (uPtrWallpaper->appID() == appID) {
            return uPtrWallpaper->screenNumber();
        }
    }
    return vector<int> {};
}

QString ScreenPlay::generateID() const
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
