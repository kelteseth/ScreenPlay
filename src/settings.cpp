#include "settings.h"

Settings::Settings(ProfileListModel* plm, QObject* parent)
    : QObject(parent)
{

    m_plm = plm;

    QFile configTmp;
    QString appConfigLocation = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
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

    m_autostart = configObj.value("autostart").toBool();
    m_highPriorityStart = configObj.value("highPriorityStart").toBool();
    m_sendStatistics = configObj.value("sendStatistics").toBool();
    m_renderer = static_cast<Renderer>(configObj.value("renderer-value").toInt());

    /*
     *  LOAD ACTIVE PROFILES
     */

    QJsonArray activeProfilesTmp;
    activeProfilesTmp = configObj.value("profiles").toArray();
    int size = activeProfilesTmp.size();

    // Only load profiles if we have any
    if (size > 0) {
        for (int i = 0; i < size; i++) {
            QString profileName = activeProfilesTmp.at(i).toObject().value("profile").toString();
            QString monitorID = activeProfilesTmp.at(i).toObject().value("monitorID").toString();
            Profile profile;
            if (!m_plm->getProfileByName(profileName, &profile))
                continue;
            m_activeProfiles.append(ActiveProfiles(monitorID, profile));
            constructWallpaper(profile, monitorID);
        }
    }
}

void Settings::createNewProfile(int screenNumber)
{
}

void Settings::constructWallpaper(Profile profile, QString monitorID)
{
    qDebug() << monitorID;
    //m_wallpapers.append(Wallpaper(                            ));
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
