#include "screenplay.h"

ScreenPlay::ScreenPlay(InstalledListModel* ilm, Settings* set, MonitorListModel* mlm, QGuiApplication* qGuiApplication, SDKConnector* sdkc, QObject* parent)
    : QObject(parent)
{
    m_ilm = ilm;
    m_settings = set;
    m_mlm = mlm;
    m_qGuiApplication = qGuiApplication;
    m_sdkc = sdkc;
}

void ScreenPlay::createWallpaper(int monitorIndex, QUrl absoluteStoragePath, QString previewImage, float volume, QString fillMode)
{
    ProjectFile project;

    if (!m_ilm->getProjectByAbsoluteStoragePath(&absoluteStoragePath, &project)) {
        return;
    }

    // Remove previous wallpaper
//    for (int i = 0; i < m_screenPlayWallpaperList.length(); ++i) {
//        if(m_screenPlayWallpaperList.at(i).data()->screenNumber().at(0) == monitorIndex){
//            m_sdkc->closeWallpapersAt(i);
//        }
//    }

    m_settings->increaseActiveWallpaperCounter();
    QVector<int> tmpMonitorIndex;
    tmpMonitorIndex.append(monitorIndex);
    m_screenPlayWallpaperList.append(QSharedPointer<ScreenPlayWallpaper>(new ScreenPlayWallpaper(tmpMonitorIndex, absoluteStoragePath.toString(), previewImage, volume, fillMode, this)));

    m_mlm->setWallpaperActiveMonitor(m_qGuiApplication->screens().at(monitorIndex), absoluteStoragePath.toString() + "/" + previewImage);
}

void ScreenPlay::createWidget(QUrl absoluteStoragePath, QString previewImage)
{
    ProjectFile project;
    if (!m_ilm->getProjectByAbsoluteStoragePath(&absoluteStoragePath, &project)) {
        return;
    }

    if (project.m_file.toString().endsWith(".exe")) {

    } else if (project.m_file.toString().endsWith(".qml")) {
    }

    qDebug() << absoluteStoragePath << previewImage;
    QString fullPath = absoluteStoragePath.toString() + "/" + project.m_file.toString();

    m_screenPlayWidgetList.append(QSharedPointer<ScreenPlayWidget>(new ScreenPlayWidget(absoluteStoragePath.toString(), previewImage, fullPath, this)));
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
    for (int i = 0; i < m_screenPlayWallpaperList.count(); ++i) {
        if (m_screenPlayWallpaperList.at(i).data()->screenNumber().at(0) == index) {
            emit projectSettingsListModelFound(m_screenPlayWallpaperList.at(i).data()->projectSettingsListModel().data());
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
    return "appID="+randomString;
}

void ScreenPlay::setWallpaperValue(int at, QString key, QString value)
{

    for (int i = 0; i < m_screenPlayWallpaperList.count(); ++i) {
        if(m_screenPlayWallpaperList.at(i).data()->screenNumber().at(0) == at){

            m_sdkc->setWallpaperValue(m_screenPlayWallpaperList.at(i).data()->appID(), key, value);
            return;
        }
    }
}

Settings* ScreenPlay::settings() const
{
    return m_settings;
}

QSharedPointer<ProjectSettingsListModel> ScreenPlayWallpaper::projectSettingsListModel() const
{
    return m_projectSettingsListModel;
}
