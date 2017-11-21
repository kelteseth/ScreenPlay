#ifndef WALLPAPER_H
#define WALLPAPER_H

#include <QDebug>
#include <QQmlContext>
#include <QQuickView>
#include <QRect>
#include <QWindow>
#include <qt_windows.h>
#include <QUrl>
#include <QDir>

#include "profile.h"
#include "projectfile.h"
#include "monitorlistmodel.h"

class Wallpaper : public QWindow {
    Q_OBJECT
public:
    explicit Wallpaper(QWindow* parent = 0);
    Wallpaper( ProjectFile project, Monitor monitor);
    ~Wallpaper();
    Q_PROPERTY(QString absoluteFilePath READ absoluteFilePath WRITE setAbsoluteFilePath NOTIFY absoluteFilePathChanged)
    Q_PROPERTY(bool isPlaying READ isPlaying WRITE setIsPlaying NOTIFY isPlayingChanged)
    Q_PROPERTY(float volume READ volume WRITE setVolume NOTIFY volumeChanged)



    QString absoluteFilePath() const
    {
        return m_absoluteFilePath;
    }

    bool isPlaying() const
    {
        return m_isPlaying;
    }

    Monitor monitor() const;

    void setMonitor(const Monitor &monitor);

    float volume() const
    {
        return m_volume;
    }

public slots:
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

signals:
    void absoluteFilePathChanged(QString absoluteFilePath);

    void isPlayingChanged(bool isPlaying);

    void volumeChanged(float volume);

private:
    HWND m_hwnd;
    HWND m_worker_hwnd;
    QQuickView* m_quickRenderer = nullptr;
    QQmlContext* m_context = nullptr;

    Profile m_profile;
    ProjectFile m_project;
    Monitor m_monitor;
    QString m_absoluteFilePath;

    bool m_isPlaying;
    float m_volume;
};

#endif // WALLPAPER_H
