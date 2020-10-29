/****************************************************************************
**
** Copyright (C) 2020 Elias Steurer (Kelteseth)
** Contact: https://screen-play.app
**
** This file is part of ScreenPlay. ScreenPlay is licensed under a dual license in
** order to ensure its sustainability. When you contribute to ScreenPlay
** you accept that your work will be available under the two following licenses:
**
** $SCREENPLAY_BEGIN_LICENSE$
**
** #### Affero General Public License Usage (AGPLv3)
** Alternatively, this file may be used under the terms of the GNU Affero
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file "ScreenPlay License.md" included in the
** packaging of this App. Please review the following information to
** ensure the GNU Affero Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/agpl-3.0.en.html.
**
** #### Commercial License
** This code is owned by Elias Steurer. By changing/adding to the code you agree to the
** terms written in:
**  * Legal/corporate_contributor_license_agreement.md - For corporate contributors
**  * Legal/individual_contributor_license_agreement.md - For individual contributors
**
** #### Additional Limitations to the AGPLv3 and Commercial Lincese
** This License does not grant any rights in the trademarks,
** service marks, or logos.
**
**
** $SCREENPLAY_END_LICENSE$
**
****************************************************************************/

#pragma once

#include <QApplication>
#include <QDir>
#include <QEasingCurve>
#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>
#include <QPropertyAnimation>
#include <QQmlContext>
#include <QQmlEngine>
#include <QScreen>
#include <QSharedPointer>
#include <QTimer>
#include <QWindow>
#include <QtGlobal>
#include <QtQuick/QQuickView>
#include <QtQuick/QQuickWindow>

#ifdef Q_OS_WIN
#include <qt_windows.h>
#endif

class MainWindow : public QQuickView {
    Q_OBJECT

public:
    explicit MainWindow(int screenAt, QString projectPath, QString id, QString decoder, QString volume, QString fillmode, QWindow* parent = nullptr);

    Q_PROPERTY(QVector<int> screenNumber READ screenNumber WRITE setScreenNumber NOTIFY screenNumberChanged)
    Q_PROPERTY(QString projectConfig READ projectConfig WRITE setProjectConfig NOTIFY projectConfigChanged)
    Q_PROPERTY(QString appID READ name WRITE setname NOTIFY nameChanged)

    Q_PROPERTY(QString fullContentPath READ fullContentPath WRITE setFullContentPath NOTIFY fullContentPathChanged)
    Q_PROPERTY(QString wallpaperType READ wallpaperType WRITE setWallpaperType NOTIFY wallpaperTypeChanged)
    Q_PROPERTY(QString fillMode READ fillMode WRITE setFillMode NOTIFY fillModeChanged)
    Q_PROPERTY(bool loops READ loops WRITE setLoops NOTIFY loopsChanged)
    Q_PROPERTY(float volume READ volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERTY(bool isPlaying READ isPlaying WRITE setIsPlaying NOTIFY isPlayingChanged)
    Q_PROPERTY(float playbackRate READ playbackRate WRITE setPlaybackRate NOTIFY playbackRateChanged)
    Q_PROPERTY(QString decoder READ decoder WRITE setDecoder NOTIFY decoderChanged) // Not yet needed

    QString projectConfig() const
    {
        return m_projectConfig;
    }

    QString name() const
    {
        return m_appID;
    }

    QVector<int> screenNumber() const
    {
        return m_screenNumber;
    }

    QUrl projectPath() const
    {
        return m_projectPath;
    }

    QString fillMode() const
    {
        return m_fillMode;
    }

    bool loops() const
    {
        return m_loops;
    }

    float volume() const
    {
        return m_volume;
    }

    QString fullContentPath() const
    {
        return m_fullContentPath;
    }

    bool isPlaying() const
    {
        return m_isPlaying;
    }
    float playbackRate() const
    {
        return m_playbackRate;
    }

    QString decoder() const
    {
        return m_decoder;
    }

    QString wallpaperType() const
    {
        return m_wallpaperType;
    }

public slots:
    void destroyThis();
    void init();
    void messageReceived(QString key, QString value);

    QString getApplicationPath()
    {
        return QApplication::applicationDirPath();
    }

    void setProjectConfig(QString projectConfig)
    {
        if (m_projectConfig == projectConfig)
            return;

        m_projectConfig = projectConfig;
        emit projectConfigChanged(m_projectConfig);
    }

    void setname(QString appID)
    {
        if (m_appID == appID)
            return;

        m_appID = appID;
        emit nameChanged(m_appID);
    }

    void setFillMode(QString fillMode)
    {
        if (m_fillMode == fillMode)
            return;

        m_fillMode = fillMode;
        emit fillModeChanged(m_fillMode);
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

    void setProjectPath(const QUrl& projectPath)
    {
        m_projectPath = projectPath;
    }

    void setScreenNumber(const QVector<int>& screenNumber)
    {
        m_screenNumber = screenNumber;
    }

    void setFullContentPath(QString fullContentPath)
    {
        if (m_fullContentPath == fullContentPath)
            return;

        m_fullContentPath = fullContentPath;
        emit fullContentPathChanged(m_fullContentPath);
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

    void setDecoder(QString decoder)
    {
        if (m_decoder == decoder)
            return;

        m_decoder = decoder;
        emit decoderChanged(m_decoder);
    }

    void setWallpaperType(QString wallpaperType)
    {
        if (m_wallpaperType == wallpaperType)
            return;

        m_wallpaperType = wallpaperType;
        emit wallpaperTypeChanged(m_wallpaperType);
    }

signals:
    void playVideo(QString path);
    void playQmlScene(QString file);
    void projectConfigChanged(QString projectConfig);
    void nameChanged(QString appID);
    void screenNumberChanged(QVector<int> screenNumber);

    void fillModeChanged(QString fillMode);
    void loopsChanged(bool loops);
    void volumeChanged(float volume);
    void fullContentPathChanged(QString fullContentPath);
    void isPlayingChanged(bool isPlaying);
    void playbackRateChanged(float playbackRate);
    void qmlSceneValueReceived(QString key, QString value);
    void decoderChanged(QString decoder);
    void qmlExit();

    void wallpaperTypeChanged(QString wallpaperType);

private:
#ifdef Q_OS_WIN
    HWND m_hwnd;
    HWND m_worker_hwnd;
#endif

    QUrl m_projectPath;
    QString m_projectFile;
    QJsonObject m_project;
    QString m_projectConfig;

    QString m_appID;
    QVector<int> m_screenNumber;

    QString m_fillMode;
    bool m_loops;
    float m_volume;
    QString m_fullContentPath;
    bool m_isPlaying;
    float m_playbackRate;
    QString m_decoder;
    QString m_wallpaperType;
};
