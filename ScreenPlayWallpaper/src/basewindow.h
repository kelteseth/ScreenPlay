#pragma once

#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QObject>
#include <QString>
#include <QSysInfo>
#include <QtQml>

class BaseWindow : public QObject {
    Q_OBJECT

public:
    BaseWindow(QObject* parent = nullptr);
    BaseWindow(QString projectFilePath, QObject* parent = nullptr);

    Q_PROPERTY(int width READ width WRITE setWidth NOTIFY widthChanged)
    Q_PROPERTY(int height READ height WRITE setHeight NOTIFY heightChanged)

    Q_PROPERTY(QString appID READ appID WRITE setAppID NOTIFY appIDChanged)
    Q_PROPERTY(QString fullContentPath READ fullContentPath WRITE setFullContentPath NOTIFY fullContentPathChanged)

    Q_PROPERTY(bool loops READ loops WRITE setLoops NOTIFY loopsChanged)
    Q_PROPERTY(bool isPlaying READ isPlaying WRITE setIsPlaying NOTIFY isPlayingChanged)
    Q_PROPERTY(bool muted READ muted WRITE setMuted NOTIFY mutedChanged)
    Q_PROPERTY(bool canFade READ canFade WRITE setCanFade NOTIFY canFadeChanged)
    Q_PROPERTY(QString fillMode READ fillMode WRITE setFillMode NOTIFY fillModeChanged)

    Q_PROPERTY(float volume READ volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERTY(float playbackRate READ playbackRate WRITE setPlaybackRate NOTIFY playbackRateChanged)
    Q_PROPERTY(float currentTime READ currentTime WRITE setCurrentTime NOTIFY currentTimeChanged)

    Q_PROPERTY(WallpaperType type READ type WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(QString OSVersion READ OSVersion WRITE setOSVersion NOTIFY OSVersionChanged)

    QSysInfo m_sysinfo;

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

    QString OSVersion() const
    {
        return m_OSVersion;
    }

    bool muted() const
    {
        return m_muted;
    }

    float currentTime() const
    {
        return m_currentTime;
    }

    bool canFade() const
    {
        return m_canFade;
    }

    QString fillMode() const
    {
        return m_fillMode;
    }

    int width() const
    {
        return m_width;
    }

    int height() const
    {
        return m_height;
    }

signals:
    void qmlExit();

    void loopsChanged(bool loops);
    void volumeChanged(float volume);
    void isPlayingChanged(bool isPlaying);
    void playbackRateChanged(float playbackRate);
    void typeChanged(WallpaperType type);
    void fullContentPathChanged(QString fullContentPath);
    void appIDChanged(QString appID);
    void qmlSceneValueReceived(QString key, QString value);
    void OSVersionChanged(QString OSVersion);
    void mutedChanged(bool muted);
    void currentTimeChanged(float currentTime);
    void canFadeChanged(bool canFade);
    void fillModeChanged(QString fillMode);
    void widthChanged(int width);
    void heightChanged(int height);

public slots:
    virtual void destroyThis() {}
    virtual void setVisible(bool show) { Q_UNUSED(show) }
    virtual void messageReceived(QString key, QString value) final;

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
        if (volume < 0.0f || volume > 1.0f)
            return;

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
        if (playbackRate < 0.0f || playbackRate > 1.0f)
            return;

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

    void setOSVersion(QString OSVersion)
    {
        if (m_OSVersion == OSVersion)
            return;

        m_OSVersion = OSVersion;
        emit OSVersionChanged(m_OSVersion);
    }

    void setMuted(bool muted)
    {
        if (m_muted == muted)
            return;

        m_muted = muted;
        emit mutedChanged(m_muted);
    }

    void setCurrentTime(float currentTime)
    {
        if (currentTime < 0.0f || currentTime > 100000000000.0f)
            return;

        if (qFuzzyCompare(m_currentTime, currentTime))
            return;

        m_currentTime = currentTime;
        emit currentTimeChanged(m_currentTime);
    }

    void setCanFade(bool canFade)
    {
        if (m_canFade == canFade)
            return;

        m_canFade = canFade;
        emit canFadeChanged(m_canFade);
    }

    void setFillMode(QString fillMode)
    {
        if (m_fillMode == fillMode)
            return;

        fillMode = fillMode.toLower();

        QStringList availableFillModes { "stretch", "fill", "contain", "cover", "scale-down" };

        if (!availableFillModes.contains(fillMode)) {
            qWarning() << "Unable to set fillmode, the provided value did not match the available values"
                       << "Provided: " << fillMode
                       << "Available: " << availableFillModes;
            return;
        }

        // Actual web fillmodes are lowercase
        // https://developer.mozilla.org/en-US/docs/Web/CSS/object-fit
        m_fillMode = fillMode.toLower();
        emit fillModeChanged(m_fillMode);
    }

    void setWidth(int width)
    {
        if (m_width == width)
            return;

        m_width = width;
        emit widthChanged(m_width);
    }

    void setHeight(int height)
    {
        if (m_height == height)
            return;

        m_height = height;
        emit heightChanged(m_height);
    }

private:
    bool m_loops { true };
    bool m_isPlaying { true };
    bool m_muted { false };
    bool m_canFade { false };

    float m_volume { 1.0f };
    float m_playbackRate { 1.0f };
    float m_currentTime { 0.0f };

    QString m_fullContentPath;
    QString m_appID;

    WallpaperType m_type = BaseWindow::WallpaperType::Qml;
    QString m_OSVersion;
    QString m_fillMode;
    int m_width { 0 };
    int m_height { 0 };
};
