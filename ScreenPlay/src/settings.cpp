#include "settings.h"
#include <QGuiApplication>
#include <QStandardPaths>

Settings::Settings(ProfileListModel* plm, MonitorListModel* mlm, InstalledListModel* ilm, SDKConnector* sdkc, QGuiApplication* app, QObject* parent)
    : QObject(parent)
    , m_version(QVersionNumber(0, 0, 1))
    , m_qSettings(QSettings(QSettings::NativeFormat, QSettings::Scope::UserScope, app->organizationName(), app->applicationName()))
{

    m_plm = plm;
    m_mlm = mlm;
    m_ilm = ilm;
    m_sdkc = sdkc;
    m_app = app;

    if (m_qSettings.value("language").isNull()) {
        auto locale = QLocale::system().uiLanguages();
        auto localeSplits = locale.at(0).split("-");

        // Only install a translator if one exsists
        QFile tsFile;
        qDebug() << ":/translations/ScreenPlay_" + localeSplits.at(0) + ".qm";
        if (tsFile.exists(":/translations/ScreenPlay_" + localeSplits.at(0) + ".qm")) {
            m_translator.load(":/translations/ScreenPlay_" + localeSplits.at(0) + ".qm");
            m_qSettings.setValue("language", QVariant(localeSplits.at(0)));
            m_qSettings.sync();
            app->installTranslator(&m_translator);
        }
    } else {
        QFile tsFile;
        if (tsFile.exists(":/translations/ScreenPlay_" + m_qSettings.value("language").toString() + ".qm")) {
            m_translator.load(":/translations/ScreenPlay_" + m_qSettings.value("language").toString() + ".qm");
            app->installTranslator(&m_translator);
        }
    }

    if (m_qSettings.value("ScreenPlayExecutable").isNull()) {
        m_qSettings.setValue("ScreenPlayExecutable", QDir::toNativeSeparators(QCoreApplication::applicationFilePath()));
        m_qSettings.sync();
    }

    QFile configTmp;
    QString appConfigLocation = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    m_localSettingsPath = QUrl::fromUserInput(appConfigLocation);
    if (!QDir(appConfigLocation).exists()) {
        if (!QDir().mkpath(appConfigLocation)) {
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
    QJsonParseError parseError {};
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
    QVersionNumber fileVersion(major, minor, patch);

    //Checks if the settings file has the same version as ScreeenPlay
    if (fileVersion != m_version) {
        // TODO(Kelteseth): Display error message
        qWarning() << "Version missmatch fileVersion: " << fileVersion.toString() << "m_version: " << m_version.toString();
        return;
    }

    //If empty use steam workshop location
    if (QString(configObj.value("absoluteStoragePath").toString()).isEmpty()) {
        QDir steamTmpUrl = QDir::toNativeSeparators(QCoreApplication::applicationFilePath());
        steamTmpUrl.cdUp();
        steamTmpUrl.cdUp();
        steamTmpUrl.cdUp();
        steamTmpUrl.cd("workshop");
        steamTmpUrl.cd("content");
        steamTmpUrl.cd("672870");

        m_localStoragePath = steamTmpUrl.path();
    } else {
        m_localStoragePath = QUrl::fromUserInput(configObj.value("absoluteStoragePath").toString());
    }

    m_ilm->setabsoluteStoragePath(m_localStoragePath);
    m_plm->m_localStoragePath = m_localStoragePath;

    m_autostart = configObj.value("autostart").toBool();
    m_highPriorityStart = configObj.value("highPriorityStart").toBool();
    m_sendStatistics = configObj.value("sendStatistics").toBool();
    int renderer = static_cast<int>(configObj.value("renderer-value").toInt());

    m_checkForOtherFullscreenApplicationTimer = new QTimer(this);
    connect(m_checkForOtherFullscreenApplicationTimer, &QTimer::timeout, this, &Settings::checkForOtherFullscreenApplication);
    m_checkForOtherFullscreenApplicationTimer->start(1500);
}

Settings::~Settings()
{
}

QString Settings::loadProject(QString file)
{
    QFile configTmp;
    //file = file.replace("file:///", "");
    configTmp.setFileName(file);
    configTmp.open(QIODevice::ReadOnly | QIODevice::Text);
    return configTmp.readAll();
}

void Settings::loadActiveProfiles()
{
    QJsonDocument configJsonDocument;
    QJsonObject configObj;
    QJsonArray activeProfilesTmp;
    QFile configTmp;
    configTmp.setFileName(m_localSettingsPath.toLocalFile() + "/settings.json");

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
            //            auto spf = new QSharedPointer<ProjectFile>(new ProjectFile());

            //            if (!m_plm->getProfileByName(profileName, &profile))
            //                continue;

            //            if (!m_ilm->getProjectByAbsoluteStoragePath(&profile.m_absolutePath, spf))
            //                continue;

            //            constructWallpaper(profile, monitorID, spf);
        }
    }
}

void Settings::setGlobalVolume(float volume)
{
    //    for (int i = 0; i < m_wallpapers.size(); ++i) {
    //        m_wallpapers.at(i).data()->setVolume(volume);
    //    }
}

void Settings::setGlobalFillMode(QString fillMode)
{
    //    for (int i = 0; i < m_wallpapers.size(); ++i) {
    //        m_wallpapers.at(i).data()->setFillMode(fillMode);
    //    }
}

void Settings::writeSingleSettingConfig(QString name, QVariant value)
{

    QJsonDocument configJsonDocument;
    QJsonParseError parseError {};
    QJsonObject configObj;
    QFile configTmp;

    configTmp.setFileName(m_localSettingsPath.toLocalFile() + "/settings.json");
    configTmp.open(QIODevice::ReadOnly | QIODevice::Text);
    QString config = configTmp.readAll();
    configJsonDocument = QJsonDocument::fromJson(config.toUtf8(), &parseError);

    if (!(parseError.error == QJsonParseError::NoError)) {
        qWarning("Settings Json Parse Error ");
        return;
    }
    configObj = configJsonDocument.object();
    configObj.insert(name, value.toJsonValue());

    configTmp.close();
    // Reopen to empty the file via Truncate
    configTmp.open(QIODevice::ReadWrite | QIODevice::Truncate);
    QTextStream out(&configTmp);
    out << QJsonDocument(configObj).toJson();

    configTmp.close();
}

void Settings::requestAllLicenses()
{

    QtConcurrent::run([this]() {
        QString tmp;
        QFile file;
        QTextStream out(&file);

        file.setFileName(":/legal/Font Awesome Free License.txt");
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        tmp += out.readAll();
        file.close();

        file.setFileName(":/legal/gpl-3.0.txt");
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        tmp += out.readAll();
        file.close();

        file.setFileName(":/legal/gpl-3.0.txt");
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        tmp += out.readAll();
        file.close();

        file.setFileName(":/legal/OFL.txt");
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        tmp += out.readAll();
        file.close();

        file.setFileName(":/legal/OpenSSL.txt");
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        tmp += out.readAll();
        file.close();

        file.setFileName(":/legal/Qt LGPLv3.txt");
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        tmp += out.readAll();
        file.close();

        emit this->allLicenseLoaded(tmp);
    });
}

void Settings::requestAllLDataProtection()
{
    QtConcurrent::run([this]() {
        QString tmp;
        QFile file;
        QTextStream out(&file);

        file.setFileName(":/legal/DataProtection.txt");
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        tmp += out.readAll();
        file.close();

        emit this->allDataProtectionLoaded(tmp);
    });
}

void Settings::saveWallpaper(int monitorIndex, QUrl absoluteStoragePath, QStringList properties, QString type)
{
}

void Settings::setqSetting(const QString& key, const QString& value)
{
    m_qSettings.setValue(key, value);
    m_qSettings.sync();
}

void Settings::setMuteAll(bool isMuted)
{

    //    if (isMuted) {
    //        for (int i = 0; i < m_wallpapers.size(); ++i) {
    //            m_wallpapers.at(i).data()->setVolume(0.0f);
    //        }
    //    } else {
    //        for (int i = 0; i < m_wallpapers.size(); ++i) {
    //            m_wallpapers.at(i).data()->setVolume(1.0f);
    //        }
    //    }
}

void Settings::setPlayAll(bool isPlaying)
{
    //    if (isPlaying) {
    //        for (int i = 0; i < m_wallpapers.size(); ++i) {
    //            m_wallpapers.at(i).data()->setIsPlaying(true);
    //        }
    //    } else {
    //        for (int i = 0; i < m_wallpapers.size(); ++i) {
    //            m_wallpapers.at(i).data()->setIsPlaying(false);
    //        }
    //    }
}

void Settings::createDefaultConfig()
{

    QFile file(m_localSettingsPath.toLocalFile() + "/settings.json");
    QFile defaultSettings(":/settings.json");

    file.open(QIODevice::WriteOnly | QIODevice::Text);
    defaultSettings.open(QIODevice::ReadOnly | QIODevice::Text);

    QTextStream out(&file);
    QTextStream defaultOut(&defaultSettings);

    out << defaultOut.readAll();

    file.close();
    defaultSettings.close();
}

bool Settings::getOfflineMode() const
{
    return m_offlineMode;
}

QUrl Settings::getScreenPlayWidgetPath() const
{
    return m_screenPlayWidgetPath;
}

void Settings::setScreenPlayWidgetPath(const QUrl& screenPlayWidgetPath)
{
    m_screenPlayWidgetPath = screenPlayWidgetPath;
}

QUrl Settings::getScreenPlayBasePath() const
{
    return m_screenPlayBasePath;
}

void Settings::setScreenPlayBasePath(QUrl screenPlayBasePath)
{
    m_screenPlayBasePath = screenPlayBasePath;
}

QUrl Settings::getScreenPlayWindowPath() const
{
    return m_screenPlayWindowPath;
}

void Settings::setScreenPlayWindowPath(const QUrl& screenPlayWindowPath)
{
    m_screenPlayWindowPath = screenPlayWindowPath;
}

void Settings::checkForOtherFullscreenApplication()
{
#ifdef Q_OS_WIN

    HWND hWnd = GetForegroundWindow();
    RECT appBounds;
    RECT rc;
    GetWindowRect(GetDesktopWindow(), &rc);
#endif
    //    if(hWnd =! (HWND)GetDesktopWindow() && hWnd != (HWND)GetShellWindow())
    //    {
    //        GetWindowRect(hWnd, &appBounds);
    //    } else {

    //    }
}
