#include "settings.h"

Settings::Settings(ProfileListModel* plm, MonitorListModel* mlm, InstalledListModel* ilm, AppId_t steamID, QObject* parent)
    : QObject(parent)
{

    m_plm = plm;
    m_mlm = mlm;
    m_ilm = ilm;
    m_steamID = steamID;

    QFile configTmp;
    QString appConfigLocation = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    m_localSettingsPath = QUrl(appConfigLocation);
    if (!QDir(appConfigLocation).exists()) {
        if (!QDir().mkdir(appConfigLocation)) {
            qWarning("ERROR: Cloud not create install dir");
            return;
        }
    }

    configTmp.setFileName(appConfigLocation + "/settings.json");

    // App Settings
    if (!configTmp.exists()) {
        qWarning("No Settings found, creating default settings");
        createDefaultConfig();
    }

    QJsonDocument configJsonDocument;
    QJsonParseError parseError;
    QJsonObject configObj;

    configTmp.open(QIODevice::ReadOnly | QIODevice::Text);
    QString config = configTmp.readAll();
    configJsonDocument = QJsonDocument::fromJson(config.toUtf8(), &parseError);

    if (!(parseError.error == QJsonParseError::NoError)) {
        qWarning("Settings Json Parse Error ");
        return;
    }

    configObj = configJsonDocument.object();
    QString tmp(configObj.value("version").toVariant().toString());
    int major, minor, patch;
    major = QString(tmp.at(0)).toInt();
    minor = QString(tmp.at(2)).toInt();
    patch = QString(tmp.at(4)).toInt();

    //Checks if the settings file has the same version as ScreeenPlay
    if (!(major == m_version.major && minor == m_version.minor && patch == m_version.patch)) {
        //TODO Display error message
        qWarning("Version missmatch");
        return;
    }

    //If empty use steam workshop location
    if (QString(configObj.value("absoluteStoragePath").toString()).isEmpty()) {
        const uint32 size = 5000;
        char folder[size];
        SteamApps()->GetAppInstallDir(m_steamID, folder, 5000);
        QDir steamTmpUrl = QDir(QString(QByteArray(folder).data()));
        steamTmpUrl.cdUp();
        steamTmpUrl.cdUp();
        steamTmpUrl.cd("workshop");
        steamTmpUrl.cd("content");
        steamTmpUrl.cd("672870");

        m_localStoragePath = steamTmpUrl.path();
    } else {
        m_localStoragePath = configObj.value("absoluteStoragePath").toString();
    }

    m_ilm->setabsoluteStoragePath(m_localStoragePath);
    m_plm->m_localStoragePath = m_localStoragePath;

    m_autostart = configObj.value("autostart").toBool();
    m_highPriorityStart = configObj.value("highPriorityStart").toBool();
    m_sendStatistics = configObj.value("sendStatistics").toBool();
    m_renderer = static_cast<Renderer>(configObj.value("renderer-value").toInt());
}

Settings::~Settings()
{
}

void Settings::constructWallpaper(Profile profile, QString monitorID, ProjectFile pf)
{
    //m_wallpapers.append(QSharedPointer<Wallpaper>(new Wallpaper(profile, pf)));
}

void Settings::constructWallpaper(QString folder, int monitorListAt)
{
}

void Settings::setWallpaper(int monitorIndex, QUrl absoluteStoragePath)
{

    ProjectFile project;
    Monitor monitor;

    if (!m_mlm->getMonitorListItemAt(monitorIndex, &monitor)) {
        return;
    }

    if (!m_ilm->getProjectByAbsoluteStoragePath(&absoluteStoragePath, &project)) {
        return;
    }
    for(int i = 0; i < m_wallpapers.length();++i){
        if(m_wallpapers.at(i).data()->monitor().m_id == monitor.m_id){
            m_wallpapers.removeAt(i);
        }
    }
    m_wallpapers.append(QSharedPointer<Wallpaper>(new Wallpaper(project, monitor)));

}

void Settings::loadActiveProfiles()
{
    QJsonDocument configJsonDocument;
    QJsonObject configObj;
    QJsonArray activeProfilesTmp;
    QFile configTmp;
    configTmp.setFileName(m_localSettingsPath.toString() + "/settings.json");

    configTmp.open(QIODevice::ReadOnly | QIODevice::Text);
    QString config = configTmp.readAll();
    configJsonDocument = QJsonDocument::fromJson(config.toUtf8());
    configObj = configJsonDocument.object();

    activeProfilesTmp = configObj.value("profiles").toArray();
    int size = activeProfilesTmp.size();

    // Only load profiles if we have any
    if (size > 0) {
        for (int i = 0; i < size; i++) {
            QString profileName = activeProfilesTmp.at(i).toObject().value("profile").toString();
            QString monitorID = activeProfilesTmp.at(i).toObject().value("monitorID").toString();
            Profile profile;
            ProjectFile spf;

            if (!m_plm->getProfileByName(profileName, &profile))
                continue;
            if (!m_ilm->getProjectByAbsoluteStoragePath(&profile.m_absolutePath, &spf))
                continue;

            constructWallpaper(profile, monitorID, spf);
        }
    }
}

void Settings::removeAll()
{
    m_wallpapers.clear();
}

void Settings::createNewWallpaper(int monitorListPosition, Profile profile, ProjectFile projectFile)
{
}

void Settings::removeWallpaperAt(int pos)
{

    if (pos > 0 && pos > m_wallpapers.size())
        m_wallpapers.removeAt(pos);
}

void Settings::createDefaultConfig()
{

    QFile file(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/settings.json");
    QFile defaultSettings(":/settings.json");

    file.open(QIODevice::WriteOnly | QIODevice::Text);
    defaultSettings.open(QIODevice::ReadOnly | QIODevice::Text);

    QTextStream out(&file);
    QTextStream defaultOut(&defaultSettings);

    out << defaultOut.readAll();

    file.close();
    defaultSettings.close();
}

ActiveProfiles::ActiveProfiles()
{
}

ActiveProfiles::ActiveProfiles(QString wallpaperId, Profile profile)
{
    m_monitorId = wallpaperId;
    m_profile = profile;
}

QString ActiveProfiles::monitorId() const
{
    return m_monitorId;
}
