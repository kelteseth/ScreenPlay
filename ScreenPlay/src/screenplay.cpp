#include "screenplay.h"

ScreenPlay::ScreenPlay(InstalledListModel* ilm, Settings* set, MonitorListModel* mlm, SDKConnector* sdkc, QObject* parent)
    : QObject{parent},
      m_ilm{ilm},
      m_settings{set},
      m_mlm{mlm},
      m_qGuiApplication{static_cast<QGuiApplication*>(QGuiApplication::instance())},
      m_sdkc{sdkc}
{}

void ScreenPlay::createWallpaper(int monitorIndex, QUrl absoluteStoragePath, QString previewImage, float volume, QString fillMode, QString type)
void ScreenPlay::createWallpaper(
        const int monitorIndex, QUrl absoluteStoragePath,
        const QString &previewImage, const float volume,
        const QString &fillMode, const QString &type)
{
    ProjectFile project{};
    if (!m_ilm->getProjectByAbsoluteStoragePath(&absoluteStoragePath, &project)) {
        return;
    }
    // Remove previous wallpaper, if any
    //this->removeAllWallpaper();
    this->removeWallpaperAt(0);
    m_settings->increaseActiveWallpaperCounter();
    m_screenPlayWallpaperList.emplace_back(
                RefSPWall::create(
                    QVector<int>{monitorIndex}, absoluteStoragePath.toLocalFile(),
                    previewImage, volume, fillMode, type, this)
                );
    m_mlm->setWallpaperActiveMonitor(m_qGuiApplication->screens().at(monitorIndex),
                                     QString{absoluteStoragePath.toLocalFile() + "/" + previewImage});
}

void ScreenPlay::createWidget(QUrl absoluteStoragePath, const QString &previewImage)
{
    ProjectFile project{};
    if (!m_ilm->getProjectByAbsoluteStoragePath(&absoluteStoragePath, &project)) {
        return;
    }
    m_screenPlayWidgetList.emplace_back(
                RefSPWidget::create(
                    absoluteStoragePath.toLocalFile(), previewImage,
                    QString{absoluteStoragePath.toLocalFile() + "/" + project.m_file.toString()},
                    this)
                );
}

void ScreenPlay::removeAllWallpaper() noexcept
{
    if(m_sdkc && m_settings && !m_screenPlayWallpaperList.empty()){
        m_sdkc->closeAllWallpapers();
        m_settings->setActiveWallpaperCounter(0);
        m_screenPlayWallpaperList.clear();
        emit allWallpaperRemoved();
    }
    return;
}

void ScreenPlay::requestProjectSettingsListModelAt(const int index) const noexcept
{
    for (const RefSPWall &refSPWallpaper: m_screenPlayWallpaperList) {
        if (!refSPWallpaper.data()->screenNumber().empty() &&
                refSPWallpaper.data()->screenNumber()[0] == index) { // ??? only at index == 0
            emit projectSettingsListModelFound(
                        refSPWallpaper.data()->projectSettingsListModel().data(),
                        refSPWallpaper.data()->type());
            return;
        }
    }
    emit projectSettingsListModelNotFound();
}

void ScreenPlay::setWallpaperValue(const int at, const QString &key, const QString &value) noexcept
{
    Q_ASSERT(static_cast<std::size_t>(at) < m_screenPlayWallpaperList.size() && m_sdkc);
    for (const RefSPWall &refSPWallpaper: m_screenPlayWallpaperList) {
        if (!refSPWallpaper.data()->screenNumber().empty() && m_sdkc &&
                refSPWallpaper.data()->screenNumber()[0] == at) { // ??? only at index == 0
            m_sdkc->setWallpaperValue(refSPWallpaper.data()->appID(), key, value);
            return;
        }
    }
}

void ScreenPlay::setAllWallpaperValue(const QString &key, const QString &value) noexcept
{
    Q_ASSERT(m_sdkc);
    for (const RefSPWall &refSPWallpaper: m_screenPlayWallpaperList) {
        if(m_sdkc) m_sdkc->setWallpaperValue(refSPWallpaper.data()->appID(), key, value);
    }
}

void ScreenPlay::removeWallpaperAt(int at)
{
    //Q_ASSERT(m_screenPlayWallpaperList.size() < at);

    if (m_screenPlayWallpaperList.isEmpty())
        return;

    for (int i = 0; i < m_screenPlayWallpaperList.length(); ++i) {

        if (m_screenPlayWallpaperList.at(i).data()->screenNumber().at(0) == at) {
            qDebug() << i << m_screenPlayWallpaperList.at(i).data()->screenNumber().at(0);
            m_sdkc->closeWallpapersAt(at);
            m_screenPlayWallpaperList.removeAt(i);
        }
    }
}

QVector<int> ScreenPlay::getMonitorByAppID(const QString &appID) const
{
    for (const RefSPWall &refSPWallpaper: m_screenPlayWallpaperList) {
        if (refSPWallpaper.data()->appID() == appID) {
            return refSPWallpaper.data()->screenNumber();
        }
    }
    return QVector<int>{};
}

QString ScreenPlay::generateID() const
{
    const QString possibleCharacters{
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"};
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

QSharedPointer<ProjectSettingsListModel> ScreenPlayWallpaper::projectSettingsListModel() const
{
    return m_projectSettingsListModel;
}
