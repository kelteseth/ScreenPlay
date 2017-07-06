#ifndef SCREENPLAYSETTINGS_H
#define SCREENPLAYSETTINGS_H

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QObject >
#include <QPair>
#include <QQmlPropertyMap>
#include <QStandardPaths>
#include <QString>
#include <QTextStream>
#include <QUrl>
#include <QVariant>
#include <QVector>
#include <QSharedPointer>

#include "monitorlistmodel.h"
#include "installedlistmodel.h"
#include "profilelistmodel.h"
#include "profile.h"
#include "wallpaper.h"

class ActiveProfiles;

class Settings : public QObject {
    Q_OBJECT
public:
    explicit Settings(ProfileListModel* plm, MonitorListModel* mlm, InstalledListModel* ilm, QObject* parent = nullptr);

    Q_PROPERTY(bool autostart READ autostart WRITE setAutostart NOTIFY autostartChanged)
    Q_PROPERTY(bool highPriorityStart READ highPriorityStart WRITE setHighPriorityStart NOTIFY highPriorityStartChanged)
    Q_PROPERTY(Renderer renderer READ renderer WRITE setRenderer NOTIFY rendererChanged)
    Q_PROPERTY(bool sendStatistics READ sendStatistics WRITE setSendStatistics NOTIFY sendStatisticsChanged)
    Q_PROPERTY(Version version READ version)

    Q_INVOKABLE void createNewProfile(int screenNumber);
    Q_INVOKABLE void constructWallpaper(Profile profile, QString monitorID);


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

signals:

    void autostartChanged(bool autostart);

    void highPriorityStartChanged(bool highPriorityStart);

    void rendererChanged(Renderer renderer);

    void sendStatisticsChanged(bool sendStatistics);

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

    void setsendStatistics(bool sendStatistics)
    {
        if (m_sendStatistics == sendStatistics)
            return;

        m_sendStatistics = sendStatistics;
        emit sendStatisticsChanged(m_sendStatistics);
    }

    void setSendStatistics(bool sendStatistics)
    {
        if (m_sendStatistics == sendStatistics)
            return;

        m_sendStatistics = sendStatistics;
        emit sendStatisticsChanged(m_sendStatistics);
    }

private:
    void createDefaultConfig();
    void createProfileConfig();

    bool m_autostart = true;
    bool m_highPriorityStart = true;
    bool m_sendStatistics;

    QUrl m_absoluteStoragePath;

    Renderer m_renderer = Renderer::OpenGL;
    Version m_version;
    ProfileListModel* m_plm;
    InstalledListModel* m_ilm;
    MonitorListModel* m_mlm;

    QVector<QSharedPointer<Wallpaper>> m_wallpapers;
    QVector<QSharedPointer<ActiveProfiles>> m_activeProfiles;
};

class ActiveProfiles {
public:
    ActiveProfiles();
    ActiveProfiles(QString monitorId, Profile profile);

private:
    QString m_monitorId;
    Profile m_profile;
};

enum FillMode {
    Stretch,
    PreserveAspectFit,
    PreserveAspectCrop,
};
#endif // SCREENPLAYSETTINGS_H
