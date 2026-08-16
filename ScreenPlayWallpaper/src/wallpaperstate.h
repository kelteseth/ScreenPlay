// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#pragma once

#include <QObject>
#include <QString>
#include <QtQml/qqml.h>

namespace ScreenPlay {

/*!
 * \brief WallpaperState holds the playback/rendering state for a wallpaper.
 *
 * This QObject manages transitions between wallpapers:
 * - BaseWindow has `currentState` (active/outgoing settings)
 * - BaseWindow has `targetState` (incoming settings during crossfade)
 *
 * After a transition completes, targetState values are copied to currentState.
 */
class WallpaperState : public QObject {
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(float volume READ volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERTY(QString fillMode READ fillMode WRITE setFillMode NOTIFY fillModeChanged)
    Q_PROPERTY(bool muted READ muted WRITE setMuted NOTIFY mutedChanged)
    Q_PROPERTY(bool loops READ loops WRITE setLoops NOTIFY loopsChanged)
    Q_PROPERTY(bool isPlaying READ isPlaying WRITE setIsPlaying NOTIFY isPlayingChanged)
    Q_PROPERTY(bool visualsPaused READ visualsPaused WRITE setVisualsPaused NOTIFY visualsPausedChanged)
    Q_PROPERTY(bool checkWallpaperVisible READ checkWallpaperVisible WRITE setCheckWallpaperVisible NOTIFY checkWallpaperVisibleChanged)

public:
    explicit WallpaperState(QObject* parent = nullptr)
        : QObject(parent)
    {
    }

    float volume() const { return m_volume; }
    QString fillMode() const { return m_fillMode; }
    bool muted() const { return m_muted; }
    bool loops() const { return m_loops; }
    bool isPlaying() const { return m_isPlaying; }
    bool visualsPaused() const { return m_visualsPaused; }
    bool checkWallpaperVisible() const { return m_checkWallpaperVisible; }

    // Copy all values from another state
    void copyFrom(const WallpaperState* other)
    {
        if (!other)
            return;
        setVolume(other->volume());
        setFillMode(other->fillMode());
        setMuted(other->muted());
        setLoops(other->loops());
        setIsPlaying(other->isPlaying());
        setVisualsPaused(other->visualsPaused());
        setCheckWallpaperVisible(other->checkWallpaperVisible());
    }

public slots:
    void setVolume(float volume)
    {
        if (volume < 0.0f || volume > 1.0f)
            return;
        if (qFuzzyCompare(m_volume, volume))
            return;
        m_volume = volume;
        emit volumeChanged(m_volume);
    }

    void setFillMode(QString fillMode)
    {
        fillMode = fillMode.toLower();
        if (m_fillMode == fillMode)
            return;
        m_fillMode = fillMode;
        emit fillModeChanged(m_fillMode);
    }

    void setMuted(bool muted)
    {
        if (m_muted == muted)
            return;
        m_muted = muted;
        emit mutedChanged(m_muted);
    }

    void setLoops(bool loops)
    {
        if (m_loops == loops)
            return;
        m_loops = loops;
        emit loopsChanged(m_loops);
    }

    void setIsPlaying(bool isPlaying)
    {
        if (m_isPlaying == isPlaying)
            return;
        m_isPlaying = isPlaying;
        emit isPlayingChanged(m_isPlaying);
    }

    void setVisualsPaused(bool visualsPaused)
    {
        if (m_visualsPaused == visualsPaused)
            return;
        m_visualsPaused = visualsPaused;
        emit visualsPausedChanged(m_visualsPaused);
    }

    void setCheckWallpaperVisible(bool checkWallpaperVisible)
    {
        if (m_checkWallpaperVisible == checkWallpaperVisible)
            return;
        m_checkWallpaperVisible = checkWallpaperVisible;
        emit checkWallpaperVisibleChanged(m_checkWallpaperVisible);
    }

signals:
    void volumeChanged(float volume);
    void fillModeChanged(QString fillMode);
    void mutedChanged(bool muted);
    void loopsChanged(bool loops);
    void isPlayingChanged(bool isPlaying);
    void visualsPausedChanged(bool visualsPaused);
    void checkWallpaperVisibleChanged(bool checkWallpaperVisible);

private:
    float m_volume = 1.0f;
    QString m_fillMode = QStringLiteral("fill");
    bool m_muted = false;
    bool m_loops = true;
    bool m_isPlaying = true;
    bool m_visualsPaused = false;
    bool m_checkWallpaperVisible = false;
};

} // namespace ScreenPlay

Q_DECLARE_METATYPE(ScreenPlay::WallpaperState)
