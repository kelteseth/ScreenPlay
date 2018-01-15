#pragma once

#include <QColor>
#include <QDebug>
#include <QDir>
#include <QGraphicsOpacityEffect>
#include <QQmlContext>
#include <QQuickView>
#include <QRect>
#include <QSharedPointer>
#include <QSurfaceFormat>
#include <QUrl>
#include <QWindow>
#include <QScreen>
#include <qt_windows.h>
#include <QSharedPointer>

#include "monitorlistmodel.h"
#include "profile.h"
#include "projectfile.h"

class Wallpaper : public QWindow {
    Q_OBJECT
public:
    explicit Wallpaper(QWindow* parent = 0);
    Wallpaper(ProjectFile project, Monitor monitor);
    ~Wallpaper();

    Q_PROPERTY(QString fillMode READ fillMode WRITE setFillMode NOTIFY fillModeChanged)
    Q_PROPERTY(QString absoluteFilePath READ absoluteFilePath WRITE setAbsoluteFilePath NOTIFY absoluteFilePathChanged)
    Q_PROPERTY(QString decoder READ decoder WRITE setDecoder NOTIFY decoderChanged)
    Q_PROPERTY(bool isPlaying READ isPlaying WRITE setIsPlaying NOTIFY isPlayingChanged)
    Q_PROPERTY(float volume READ volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERTY(float opacity READ opacity WRITE setOpacity NOTIFY opacityChanged)

    Q_INVOKABLE void setupWindow();

    QString absoluteFilePath() const
    {
        return m_absoluteFilePath;
    }

    bool isPlaying() const
    {
        return m_isPlaying;
    }

    Monitor monitor() const;

    void setMonitor(const Monitor& monitor);

    float volume() const
    {
        return m_volume;
    }

    float opacity() const
    {
        return m_opacity;
    }

    QString decoder() const
    {
        return m_decoder;
    }

    QString fillMode() const
    {
        return m_fillMode;
    }

public slots:

    void destroyWallpaper(){
        emit prepareDestroy();
    }

    void destroyWindow(){
        //deleteLater();
        setVisible(false);
        emit destroyThis(this);
    }

    void setAbsoluteFilePath(QString absoluteFilePath)
    {
        if (m_absoluteFilePath == absoluteFilePath)
            return;

        m_absoluteFilePath = absoluteFilePath;
        emit absoluteFilePathChanged(m_absoluteFilePath);
    }

    void setIsPlaying(bool isPlaying)
    {
        if (m_isPlaying == isPlaying)
            return;

        m_isPlaying = isPlaying;
        emit isPlayingChanged(m_isPlaying);
    }

    void setVisible(bool visible);

    void setVolume(float volume)
    {
        qWarning("Floating point comparison needs context sanity check");
        if (qFuzzyCompare(m_volume, volume))
            return;

        m_volume = volume;
        emit volumeChanged(m_volume);
    }

    void setOpacity(float opacity)
    {
        qDebug() << opacity;
        //this->setOpacity(opacity);
        qWarning("Floating point comparison needs context sanity check");
        if (qFuzzyCompare(m_opacity, opacity))
            return;

        m_opacity = opacity;
        emit opacityChanged(m_opacity);
    }

    void setDecoder(QString decoder)
    {
        if (m_decoder == decoder)
            return;

        m_decoder = decoder;
        emit decoderChanged(m_decoder);
    }

    void setFillMode(QString fillMode)
    {
        if (m_fillMode == fillMode)
            return;

        m_fillMode = fillMode;
        emit fillModeChanged(m_fillMode);
    }

signals:
    void absoluteFilePathChanged(QString absoluteFilePath);
    void isPlayingChanged(bool isPlaying);
    void volumeChanged(float volume);
    void opacityChanged(float opacity);
    void decoderChanged(QString decoder);
    void prepareDestroy();
    void destroyThis(Wallpaper* ref);
    void fillModeChanged(QString fillMode);

private:
    HWND m_hwnd;
    HWND m_worker_hwnd;
    QSharedPointer<QQuickView> m_quickRenderer = nullptr;
    QQmlContext* m_context = nullptr;

    Profile m_profile;
    ProjectFile m_project;
    Monitor m_monitor;
    QString m_absoluteFilePath;

    bool m_isPlaying = true;
    float m_volume = 1.0f;
    float m_opacity;
    QString m_decoder = "FFMPEG";
    QString m_fillMode = "Stretch";
};

