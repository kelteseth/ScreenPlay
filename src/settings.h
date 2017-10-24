#pragma once

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>
#include <QPair>
#include <QQmlPropertyMap>
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
#include "steam/steam_api.h"
#include "wallpaper.h"

class ActiveProfiles;




class Settings : public QObject {
    Q_OBJECT
public:
    explicit Settings(ProfileListModel* plm, MonitorListModel* mlm, InstalledListModel* ilm, AppId_t steamID, QObject* parent = nullptr);
    ~Settings();
    Q_PROPERTY(bool autostart READ autostart WRITE setAutostart NOTIFY autostartChanged)
    Q_PROPERTY(bool highPriorityStart READ highPriorityStart WRITE setHighPriorityStart NOTIFY highPriorityStartChanged)
    Q_PROPERTY(Renderer renderer READ renderer WRITE setRenderer NOTIFY rendererChanged)
    Q_PROPERTY(bool sendStatistics READ sendStatistics WRITE setSendStatistics NOTIFY sendStatisticsChanged)
    Q_PROPERTY(Version version READ version)
    Q_PROPERTY(QUrl localStoragePath READ localStoragePath WRITE setLocalStoragePath NOTIFY localStoragePathChanged)

    void loadActiveProfiles();
    void removeAll();

    enum LocalCopyResult {
        NoError,
        CopyError,
        NotEnoughDiskSpace,
        NameCollision,
    };
    Q_ENUM(LocalCopyResult)

    enum Renderer {
        OpenGL,
        Softare,
    };
    Q_ENUM(Renderer)

    struct Version {
        int major = 0;
        int minor = 0;
        int patch = 1;
    };

    bool autostart() const
    {
        return m_autostart;
    }

    bool highPriorityStart() const
    {
        return m_highPriorityStart;
    }

    Renderer renderer() const
    {
        return m_renderer;
    }

    bool sendStatistics() const
    {
        return m_sendStatistics;
    }

    Version version() const
    {
        return m_version;
    }

    QUrl localStoragePath() const
    {
        return m_localStoragePath;
    }


signals:

    void autostartChanged(bool autostart);

    void highPriorityStartChanged(bool highPriorityStart);

    void rendererChanged(Renderer renderer);

    void sendStatisticsChanged(bool status);

    void localStoragePathChanged(QUrl localStoragePath);

public slots:

    void setAutostart(bool autostart)
    {
        if (m_autostart == autostart)
            return;

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

    void setRenderer(Renderer renderer)
    {
        if (m_renderer == renderer)
            return;

        m_renderer = renderer;
        emit rendererChanged(m_renderer);
    }

    void setSendStatistics(bool sendStatistics)
    {
        if (m_sendStatistics == sendStatistics)
            return;

        m_sendStatistics = sendStatistics;
        emit sendStatisticsChanged(m_sendStatistics);
    }

    void createNewWallpaper(int monitorListPosition, Profile profile, ProjectFile projectFile);

    void removeWallpaperAt(int pos);

    void constructWallpaper(Profile profile, QString monitorID, ProjectFile spf);

    void constructWallpaper(QString folder, int monitorListAt);

    void setWallpaper(int monitorIndex, QString wallpaperID, QUrl absoluteStoragePath);

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
        QDir a = QDir(localStoragePath.toString());

        configObj.insert("absoluteStoragePath", cleanedPath);

        configTmp.close();
        configTmp.open(QIODevice::ReadWrite | QIODevice::Truncate);
        QTextStream out(&configTmp);
        out << QJsonDocument(configObj).toJson();

        configTmp.close();
        emit localStoragePathChanged(m_localStoragePath);
    }

private:
    void createDefaultConfig();
    void createProfileConfig();

    bool m_autostart = true;
    bool m_highPriorityStart = true;
    bool m_sendStatistics;

    AppId_t m_steamID;

    Renderer m_renderer = Renderer::OpenGL;
    Version m_version;
    ProfileListModel* m_plm;
    InstalledListModel* m_ilm;
    MonitorListModel* m_mlm;
    QThread m_thread;


    QVector<QSharedPointer<Wallpaper>> m_wallpapers;

    QUrl m_localStoragePath;
};

class ActiveProfiles {
public:
    ActiveProfiles();
    ActiveProfiles(QString monitorId, Profile profile);

    QString monitorId() const;

private:
    QString m_monitorId;
    Profile m_profile;
};

enum FillMode {
    Stretch,
    PreserveAspectFit,
    PreserveAspectCrop,
};

