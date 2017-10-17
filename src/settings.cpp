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
        uint32 size = 5000;
        char folder[size];
        SteamApps()->GetAppInstallDir(m_steamID,folder,5000);
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

    /*
    //Create default folders
    if (!QDir(m_absoluteStoragePath.toString() + "/ProfilePackages").exists()) {
        if (!QDir().mkdir(appConfigLocation + "/ProfilePackages")) {
            qWarning("ERROR: Cloud not create ProfilePackages dir");
            return;
        }
    }
    if (!QDir(m_absoluteStoragePath.toString() + "/Profiles").exists()) {
        if (!QDir().mkdir(appConfigLocation + "/Profiles")) {
            qWarning("ERROR: Cloud not create  Profiles dir");
            return;
        }
    }
    if (!QDir(m_absoluteStoragePath.toString() + "/Wallpaper").exists()) {
        if (!QDir().mkdir(appConfigLocation + "/Wallpaper")) {
            qWarning("ERROR: Cloud not create Wallpaper dir");
            return;
        }
    }
    if (!QDir(m_absoluteStoragePath.toString() + "/Widgets").exists()) {
        if (!QDir().mkdir(appConfigLocation + "/Widgets")) {
            qWarning("ERROR: Cloud not create Widgets dir");
            return;
        }
    }*/

    m_autostart = configObj.value("autostart").toBool();
    m_highPriorityStart = configObj.value("highPriorityStart").toBool();
    m_sendStatistics = configObj.value("sendStatistics").toBool();
    m_renderer = static_cast<Renderer>(configObj.value("renderer-value").toInt());
}

Settings::~Settings()
{
}

void Settings::createNewProfile(int screenNumber)
{
}

void Settings::constructWallpaper(Profile profile, QString monitorID, ProjectFile pf)
{

    m_wallpapers.append(QSharedPointer<Wallpaper>(new Wallpaper(profile,pf)));
}

void Settings::setWallpaper(int monitorIndex, QString wallpaperID)
{

}

void Settings::loadActiveProfiles()
{
    QJsonDocument configJsonDocument;
    QJsonObject configObj;
    QJsonArray activeProfilesTmp;
    QFile configTmp;
    configTmp.setFileName(m_localStoragePath.toString() + "/settings.json");
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
            if (!m_ilm->getProjectByName(profile.m_wallpaperId, &spf))
                continue;


            constructWallpaper(profile, monitorID, spf);
        }
    }
}

void Settings::removeAll()
{
    m_wallpapers.clear();
}

void Settings::updateSettinsValue(QString newValue)
{

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

void Settings::updateSettingsLocalPath(QUrl newPath)
{

}

ActiveProfiles::ActiveProfiles()
{
}

ActiveProfiles::ActiveProfiles(QString wallpaperId, Profile profile)
{
    m_monitorId = wallpaperId;
    m_profile = profile;
}
