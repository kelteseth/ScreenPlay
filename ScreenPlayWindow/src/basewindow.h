#pragma once

#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QObject>
#include <QString>
#include <qqml.h>

class BaseWindow : public QObject {
    Q_OBJECT

public:
    BaseWindow(QObject* parent = nullptr);
    BaseWindow(QString projectFilePath, QObject* parent = nullptr);

    Q_PROPERTY(QString appID READ appID WRITE setAppID NOTIFY appIDChanged)
    Q_PROPERTY(QString fullContentPath READ fullContentPath WRITE setFullContentPath NOTIFY fullContentPathChanged)
    Q_PROPERTY(bool loops READ loops WRITE setLoops NOTIFY loopsChanged)
    Q_PROPERTY(bool isPlaying READ isPlaying WRITE setIsPlaying NOTIFY isPlayingChanged)
    Q_PROPERTY(float volume READ volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERTY(float playbackRate READ playbackRate WRITE setPlaybackRate NOTIFY playbackRateChanged)
    Q_PROPERTY(WallpaperType type READ type WRITE setType NOTIFY typeChanged)

    enum class WallpaperType {
        Video,
        Html,
        ThreeJSScene,
        Qml,
    };
    Q_ENUM(WallpaperType)

    bool loops() const
    {
        return m_loops;
    }

    float volume() const
    {
        return m_volume;
    }

    bool isPlaying() const
    {
        return m_isPlaying;
    }

    float playbackRate() const
    {
        return m_playbackRate;
    }

    WallpaperType type() const
    {
        return m_type;
    }

    QString fullContentPath() const
    {
        return m_fullContentPath;
    }

    QString appID() const
    {
        return m_appID;
    }

signals:
    void loopsChanged(bool loops);
    void volumeChanged(float volume);
    void isPlayingChanged(bool isPlaying);
    void playbackRateChanged(float playbackRate);
    void typeChanged(WallpaperType type);
    void fullContentPathChanged(QString fullContentPath);
    void appIDChanged(QString appID);
    void qmlExit();
    void qmlSceneValueReceived(QString key, QString value);

public slots:
    virtual void destroyThis() {}
    virtual void setVisible(bool show) { Q_UNUSED(show) }
    virtual void messageReceived(QString key, QString value)
    {
        Q_UNUSED(key);
        Q_UNUSED(value)
    }

    QString getApplicationPath()
    {
        return QApplication::applicationDirPath();
    }

    void setLoops(bool loops)
    {
        if (m_loops == loops)
            return;

        m_loops = loops;
        emit loopsChanged(m_loops);
    }
    void setVolume(float volume)
    {

        if (qFuzzyCompare(m_volume, volume))
            return;

        m_volume = volume;
        emit volumeChanged(m_volume);
    }
    void setIsPlaying(bool isPlaying)
    {
        if (m_isPlaying == isPlaying)
            return;

        m_isPlaying = isPlaying;
        emit isPlayingChanged(m_isPlaying);
    }
    void setPlaybackRate(float playbackRate)
    {
        qWarning("Floating point comparison needs context sanity check");
        if (qFuzzyCompare(m_playbackRate, playbackRate))
            return;

        m_playbackRate = playbackRate;
        emit playbackRateChanged(m_playbackRate);
    }

    void setType(WallpaperType type)
    {
        if (m_type == type)
            return;

        m_type = type;
        emit typeChanged(m_type);
    }

    void setFullContentPath(QString fullContentPath)
    {
        if (m_fullContentPath == fullContentPath)
            return;

        m_fullContentPath = fullContentPath;
        emit fullContentPathChanged(m_fullContentPath);
    }

    void setAppID(QString appID)
    {
        if (m_appID == appID)
            return;

        m_appID = appID;
        emit appIDChanged(m_appID);
    }

private:
    bool m_loops = true;
    bool m_isPlaying = true;

    float m_volume = 1.0f;
    float m_playbackRate = 1.0f;

    QString m_fullContentPath;
    QString m_appID;

    WallpaperType m_type = BaseWindow::WallpaperType::Qml;
};
