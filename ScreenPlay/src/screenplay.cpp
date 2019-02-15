#include "screenplay.h"

ScreenPlay::ScreenPlay(InstalledListModel* ilm, Settings* set, MonitorListModel* mlm, SDKConnector* sdkc, QObject* parent)
    : QObject(parent)
{
    m_ilm = ilm;
    m_settings = set;
    m_mlm = mlm;
    m_qGuiApplication = static_cast<QGuiApplication*>(QGuiApplication::instance());
    m_sdkc = sdkc;
}

void ScreenPlay::createWallpaper(int monitorIndex, QUrl absoluteStoragePath, QString previewImage, float volume, QString fillMode, QString type)
{
    ProjectFile project;

    if (!m_ilm->getProjectByAbsoluteStoragePath(&absoluteStoragePath, &project)) {
        return;
    }

    // Remove previous wallpaper
    removeWallpaperAt(monitorIndex);

    m_settings->increaseActiveWallpaperCounter();
    QVector<int> tmpMonitorIndex;
    tmpMonitorIndex.append(monitorIndex);
    m_screenPlayWallpaperList.append(QSharedPointer<ScreenPlayWallpaper>(new ScreenPlayWallpaper(tmpMonitorIndex, absoluteStoragePath.toLocalFile(), previewImage, volume, fillMode, type, this)));

    m_mlm->setWallpaperActiveMonitor(m_qGuiApplication->screens().at(monitorIndex), absoluteStoragePath.toLocalFile() + "/" + previewImage);
}

void ScreenPlay::createWidget(QUrl absoluteStoragePath, QString previewImage)
{
    ProjectFile project;
    if (!m_ilm->getProjectByAbsoluteStoragePath(&absoluteStoragePath, &project)) {
        return;
    }

    QString fullPath = absoluteStoragePath.toLocalFile() + "/" + project.m_file.toString();

    m_screenPlayWidgetList.append(QSharedPointer<ScreenPlayWidget>(new ScreenPlayWidget(absoluteStoragePath.toLocalFile(), previewImage, fullPath, this)));
}

void ScreenPlay::removeAllWallpaper()
{
    m_sdkc->closeAllWallpapers();
    m_settings->setActiveWallpaperCounter(0);
    m_screenPlayWallpaperList.clear();
    emit allWallpaperRemoved();
}

void ScreenPlay::requestProjectSettingsListModelAt(int index)
{

    //Q_ASSERT(index > m_mlm->size());
    for (int i = 0; i < m_screenPlayWallpaperList.count(); ++i) {
        if (m_screenPlayWallpaperList.at(i).data()->screenNumber().at(0) == index) {
            emit projectSettingsListModelFound(m_screenPlayWallpaperList.at(i).data()->projectSettingsListModel().data(), m_screenPlayWallpaperList.at(i).data()->type());
            return;
        }
    }
    emit projectSettingsListModelNotFound();
}

QString ScreenPlay::generateID()
{
    const QString possibleCharacters("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
    const int randomStringLength = 32;

    QString randomString;
    for (int i = 0; i < randomStringLength; ++i) {
        int index = qrand() % possibleCharacters.length();
        QChar nextChar = possibleCharacters.at(index);
        randomString.append(nextChar);
    }
    return randomString;
}

void ScreenPlay::setWallpaperValue(int at, QString key, QString value)
{
    Q_ASSERT(at < m_screenPlayWallpaperList.size());
    for (int i = 0; i < m_screenPlayWallpaperList.count(); ++i) {
        if (m_screenPlayWallpaperList.at(i).data()->screenNumber().at(0) == at) {
            m_sdkc->setWallpaperValue(m_screenPlayWallpaperList.at(i).data()->appID(), key, value);
            return;
        }
    }
}

void ScreenPlay::setAllWallpaperValue(QString key, QString value)
{
    for (int i = 0; i < m_screenPlayWallpaperList.count(); ++i) {
        m_sdkc->setWallpaperValue(m_screenPlayWallpaperList.at(i).data()->appID(), key, value);
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

QVector<int> ScreenPlay::getMonitorByAppID(QString appID)
{
    for (int i = 0; i < m_screenPlayWallpaperList.length(); ++i) {
        if (m_screenPlayWallpaperList.at(i).data()->appID() == appID) {
            return m_screenPlayWallpaperList.at(i).data()->screenNumber();
        }
    }

    return QVector<int>();
}

Settings* ScreenPlay::settings() const
{
    return m_settings;
}

QSharedPointer<ProjectSettingsListModel> ScreenPlayWallpaper::projectSettingsListModel() const
{
    return m_projectSettingsListModel;
}
