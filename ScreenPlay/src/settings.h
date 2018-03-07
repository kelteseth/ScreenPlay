#pragma once

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>
#include <QPair>
#include <QProcess>
#include <QProcessEnvironment>
#include <QQmlPropertyMap>
#include <QSettings>
#include <QSharedPointer>
#include <QStandardPaths>
#include <QString>
#include <QTextStream>
#include <QThread>
#include <QUrl>
#include <QVariant>
#include <QVector>

#include "installedlistmodel.h"
#include "monitorlistmodel.h"
#include "profile.h"
#include "profilelistmodel.h"
#include "projectsettingslistmodel.h"
#include "sdkconnector.h"
#include "steam/steam_api.h"
#include "wallpaper.h"

class ActiveProfile;

class Settings : public QObject {
    Q_OBJECT

public:
    explicit Settings(ProfileListModel* plm, MonitorListModel* mlm, InstalledListModel* ilm, SDKConnector* sdkc, AppId_t steamID, QGuiApplication* app, QObject* parent = nullptr);
    ~Settings();

    Q_PROPERTY(Version version READ version)
    Q_PROPERTY(bool hasWorkshopBannerSeen READ hasWorkshopBannerSeen WRITE setHasWorkshopBannerSeen NOTIFY hasWorkshopBannerSeenChanged)
    Q_PROPERTY(bool autostart READ autostart WRITE setAutostart NOTIFY autostartChanged)
    Q_PROPERTY(bool highPriorityStart READ highPriorityStart WRITE setHighPriorityStart NOTIFY highPriorityStartChanged)
    Q_PROPERTY(bool sendStatistics READ sendStatistics WRITE setSendStatistics NOTIFY sendStatisticsChanged)
    Q_PROPERTY(bool pauseWallpaperWhenIngame READ pauseWallpaperWhenIngame WRITE setPauseWallpaperWhenIngame NOTIFY pauseWallpaperWhenIngameChanged)
    Q_PROPERTY(bool offlineMode READ offlineMode WRITE setOfflineMode NOTIFY offlineModeChanged)
    Q_PROPERTY(QUrl localStoragePath READ localStoragePath WRITE setLocalStoragePath NOTIFY localStoragePathChanged)
    Q_PROPERTY(QString decoder READ decoder WRITE setDecoder NOTIFY decoderChanged)
    Q_PROPERTY(int activeWallpaperCounter READ activeWallpaperCounter WRITE setActiveWallpaperCounter NOTIFY activeWallpaperCounterChanged)

    void loadActiveProfiles();

    enum LocalCopyResult {
        NoError,
        CopyError,
        NotEnoughDiskSpace,
        NameCollision,
    };
    Q_ENUM(LocalCopyResult)

    struct Version {
        int major = 0;
        int minor = 0;
        int patch = 1;
    };


    Version version() const
    {
        return m_version;
    }

    QUrl localStoragePath() const
    {
        return m_localStoragePath;
    }

    bool hasWorkshopBannerSeen() const
    {
        return m_hasWorkshopBannerSeen;
    }


    int activeWallpaperCounter() const
    {
        return m_activeWallpaperCounter;
    }

    QUrl getScreenPlayWindowPath() const;
    void setScreenPlayWindowPath(const QUrl& screenPlayWindowPath);

    bool pauseWallpaperWhenIngame() const
    {
        return m_pauseWallpaperWhenIngame;
    }

    bool offlineMode() const
    {
        return m_offlineMode;
    }

signals:
    void autostartChanged(bool autostart);
    void highPriorityStartChanged(bool highPriorityStart);
    void sendStatisticsChanged(bool status);
    void localStoragePathChanged(QUrl localStoragePath);
    void hasWorkshopBannerSeenChanged(bool hasWorkshopBannerSeen);
    void decoderChanged(QString decoder);
    void setMainWindowVisible(bool visible);
    void activeWallpaperCounterChanged(int activeWallpaperCounter);
    void pauseWallpaperWhenIngameChanged(bool pauseWallpaperWhenIngame);
    void allWallpaperRemoved();
    void offlineModeChanged(bool offlineMode);

public slots:
    void removeAll();
    void setMuteAll(bool isMuted);
    void setPlayAll(bool isPlaying);
    QUrl getPreviewImageByMonitorID(QString id);
    QString fixWindowsPath(QString url);
    void openFolderInExplorer(QString url);
    void openLicenceFolder();
    void checkForOtherFullscreenApplication();

    //Global settings
    void setGlobalVolume(float volume);
    void setGlobalFillMode(QString fillMode);

    void writeSingleSettingConfig(QString name, QVariant value);

    bool autostart() const
    {
        return m_autostart;
    }

    bool highPriorityStart() const
    {
        return m_highPriorityStart;
    }

    bool sendStatistics() const
    {
        return m_sendStatistics;
    }

    QString decoder() const
    {
        return m_decoder;
    }
    void setAutostart(bool autostart)
    {
        if (m_autostart == autostart)
            return;

        if (autostart) {
#ifdef Q_OS_WIN
            QSettings settings("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
            settings.setValue("ScreenPlay", QDir::toNativeSeparators(QCoreApplication::applicationFilePath()) + " -silent");
            settings.sync();
#endif
        } else {
#ifdef Q_OS_WIN
            QSettings settings("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
            settings.remove("ScreenPlay");
#endif
        }

        m_autostart = autostart;
        emit autostartChanged(m_autostart);
    }

    void setHighPriorityStart(bool highPriorityStart)
    {
        if (m_highPriorityStart == highPriorityStart)
            return;

        m_highPriorityStart = highPriorityStart;
        emit highPriorityStartChanged(m_highPriorityStart);
    }

    void setSendStatistics(bool sendStatistics)
    {
        if (m_sendStatistics == sendStatistics)
            return;

        m_sendStatistics = sendStatistics;
        emit sendStatisticsChanged(m_sendStatistics);
    }

    void removeWallpaperAt(int pos);

    void setWallpaper(int monitorIndex, QUrl absoluteStoragePath, QString previewImage);

    void setWidget(QUrl absoluteStoragePath);

    QString loadProject(QString file);

    void setLocalStoragePath(QUrl localStoragePath)
    {
        if (m_localStoragePath == localStoragePath)
            return;

        QJsonValue cleanedPath = QJsonValue(QString(localStoragePath.toString()).remove(0, 8));

        m_localStoragePath = cleanedPath.toString();

        QFile configTmp;
        QJsonDocument configJsonDocument;
        QJsonObject configObj;
        configTmp.setFileName(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/settings.json");
        configTmp.open(QIODevice::ReadOnly | QIODevice::Text);
        QString config = configTmp.readAll();

        configJsonDocument = QJsonDocument::fromJson(config.toUtf8());
        configObj = configJsonDocument.object();
        configObj.insert("absoluteStoragePath", cleanedPath);

        configTmp.close();
        configTmp.open(QIODevice::ReadWrite | QIODevice::Truncate);
        QTextStream out(&configTmp);
        out << QJsonDocument(configObj).toJson();

        configTmp.close();
        m_ilm->setabsoluteStoragePath(m_localStoragePath);
        emit localStoragePathChanged(m_localStoragePath);
        m_ilm->reset();
        m_ilm->loadScreens();
    }

    void setDecoder(QString decoder)
    {
        if (m_decoder == decoder)
            return;

        m_decoder = decoder;
        //        for (int i = 0; i < m_wallpapers.size(); ++i) {
        //            m_wallpapers.at(i).data()->setDecoder(decoder);
        //        }
        emit decoderChanged(m_decoder);
    }

    void setHasWorkshopBannerSeen(bool hasWorkshopBannerSeen)
    {
        if (m_hasWorkshopBannerSeen == hasWorkshopBannerSeen)
            return;

        m_hasWorkshopBannerSeen = hasWorkshopBannerSeen;
        emit hasWorkshopBannerSeenChanged(m_hasWorkshopBannerSeen);
    }

    void setActiveWallpaperCounter(int activeWallpaperCounter)
    {
        if (m_activeWallpaperCounter == activeWallpaperCounter)
            return;

        m_activeWallpaperCounter = activeWallpaperCounter;
        emit activeWallpaperCounterChanged(m_activeWallpaperCounter);
    }

    void increaseActiveWallpaperCounter()
    {
        m_activeWallpaperCounter++;
        emit activeWallpaperCounterChanged(m_activeWallpaperCounter);
    }

    void decreaseActiveWallpaperCounter()
    {
        if (m_activeWallpaperCounter <= 0) {
            return;
        }
        m_activeWallpaperCounter--;
        emit activeWallpaperCounterChanged(m_activeWallpaperCounter);
    }

    void setPauseWallpaperWhenIngame(bool pauseWallpaperWhenIngame)
    {
        if (m_pauseWallpaperWhenIngame == pauseWallpaperWhenIngame)
            return;

        m_pauseWallpaperWhenIngame = pauseWallpaperWhenIngame;
        emit pauseWallpaperWhenIngameChanged(m_pauseWallpaperWhenIngame);
    }

    void setOfflineMode(bool offlineMode)
    {
        if (m_offlineMode == offlineMode)
            return;

        m_offlineMode = offlineMode;
        emit offlineModeChanged(m_offlineMode);
    }

private:
    void createDefaultConfig();
    void createProfileConfig();

    Version m_version;
    ProfileListModel* m_plm;
    InstalledListModel* m_ilm;
    MonitorListModel* m_mlm;
    SDKConnector* m_sdkc;

    QTimer* m_checkForOtherFullscreenApplicationTimer;

    QVector<QProcess*> m_widgets;
    QVector<QProcess*> m_windows;
    QVector<QSharedPointer<ActiveProfile>> m_activeProfiles;

    QUrl m_localStoragePath;
    QUrl m_localSettingsPath;
    QUrl m_screenPlayWindowPath;

    bool m_hasWorkshopBannerSeen = false;
    bool m_pauseWallpaperWhenIngame = true;
    bool m_autostart = true;
    bool m_highPriorityStart = true;
    bool m_sendStatistics;

    AppId_t m_steamID;
    QString m_decoder;
    int m_activeWallpaperCounter = 0;
    QGuiApplication* m_qGuiApplication;

    bool m_offlineMode = false;
};

class ActiveProfile {
public:
    ActiveProfile();
    ActiveProfile(QString monitorId, Profile profile);
    ActiveProfile(int monitorIndex, QString absoluteStoragePath);
    QString monitorId() const;

private:
    QString m_monitorId, m_absoluteStoragePath;
    int m_monitorIndex;
    Profile m_profile;
    QProcess* m_process;
    ProjectSettingsListModel m_proSettingsListModel;
};

enum FillMode {
    Stretch,
    PreserveAspectFit,
    PreserveAspectCrop,
};
