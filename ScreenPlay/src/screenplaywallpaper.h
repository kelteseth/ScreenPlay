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

#include <QDebug>
#include <QJsonObject>
#include <QObject>
#include <QProcess>

#include <memory>

#include "globalvariables.h"
#include "projectsettingslistmodel.h"
#include "sdkconnection.h"
#include "util.h"

namespace ScreenPlay {

class ScreenPlayWallpaper : public QObject {
    Q_OBJECT

    Q_PROPERTY(QVector<int> screenNumber READ screenNumber WRITE setScreenNumber NOTIFY screenNumberChanged)

    Q_PROPERTY(float volume READ volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERTY(float playbackRate READ playbackRate WRITE setPlaybackRate NOTIFY playbackRateChanged)

    Q_PROPERTY(bool isLooping READ isLooping WRITE setIsLooping NOTIFY isLoopingChanged)

    Q_PROPERTY(QString file READ file WRITE setFile NOTIFY fileChanged)
    Q_PROPERTY(QString absolutePath READ absolutePath WRITE setAbsolutePath NOTIFY absolutePathChanged)
    Q_PROPERTY(QString previewImage READ previewImage WRITE setPreviewImage NOTIFY previewImageChanged)
    Q_PROPERTY(QString appID READ appID WRITE setAppID NOTIFY appIDChanged)

    Q_PROPERTY(FillMode::FillMode fillMode READ fillMode WRITE setFillMode NOTIFY fillModeChanged)
    Q_PROPERTY(InstalledType::InstalledType type READ type WRITE setType NOTIFY typeChanged)

public:
    explicit ScreenPlayWallpaper(const QVector<int>& screenNumber,
        const std::shared_ptr<GlobalVariables>& globalVariables,
        const QString& appID,
        const QString& absolutePath,
        const QString& previewImage,
        const QString& file,
        const float volume, const float playbackRate,
        const FillMode::FillMode fillMode,
        const InstalledType::InstalledType type, const QJsonObject& properties,
        const bool checkWallpaperVisible,
        QObject* parent = nullptr);

    void replace(const QString& absolutePath,
        const QString& previewImage,
        const QString& file,
        const float volume,
        const FillMode::FillMode fillMode,
        const InstalledType::InstalledType type,
        const bool checkWallpaperVisible);

    void setSDKConnection(const std::shared_ptr<SDKConnection>& connection);

    QJsonObject getActiveSettingsJson();

    QVector<int> screenNumber() const
    {
        return m_screenNumber;
    }

    QString previewImage() const
    {
        return m_previewImage;
    }

    QString appID() const
    {
        return m_appID;
    }

    InstalledType::InstalledType type() const
    {
        return m_type;
    }

    QString file() const
    {
        return m_file;
    }

    FillMode::FillMode fillMode() const
    {
        return m_fillMode;
    }

    QString absolutePath() const
    {
        return m_absolutePath;
    }

    float volume() const
    {
        return m_volume;
    }

    bool isLooping() const
    {
        return m_isLooping;
    }

    ProjectSettingsListModel* getProjectSettingsListModel()
    {
        return &m_projectSettingsListModel;
    }

    float playbackRate() const
    {
        return m_playbackRate;
    }

signals:
    void screenNumberChanged(QVector<int> screenNumber);
    void previewImageChanged(QString previewImage);
    void appIDChanged(QString appID);
    void typeChanged(InstalledType::InstalledType type);
    void fileChanged(QString file);
    void fillModeChanged(FillMode::FillMode fillMode);
    void absolutePathChanged(QString absolutePath);
    void profileJsonObjectChanged(QJsonObject profileJsonObject);
    void volumeChanged(float volume);
    void isLoopingChanged(bool isLooping);
    void requestSave();
    void playbackRateChanged(float playbackRate);

public slots:
    void processExit(int exitCode, QProcess::ExitStatus exitStatus);
    void processError(QProcess::ProcessError error);
    void setWallpaperValue(const QString& key, const QVariant& value, const bool save = false);

    void setScreenNumber(QVector<int> screenNumber)
    {
        if (m_screenNumber == screenNumber)
            return;

        m_screenNumber = screenNumber;
        emit screenNumberChanged(m_screenNumber);
    }

    void setPreviewImage(QString previewImage)
    {
        if (m_previewImage == previewImage)
            return;

        m_previewImage = previewImage;
        emit previewImageChanged(m_previewImage);
    }

    void setAppID(QString appID)
    {
        if (m_appID == appID)
            return;

        m_appID = appID;
        emit appIDChanged(m_appID);
    }

    void setType(InstalledType::InstalledType type)
    {
        if (m_type == type)
            return;

        m_type = type;
        emit typeChanged(m_type);
    }

    void setFile(QString file)
    {
        if (m_file == file)
            return;

        m_file = file;
        emit fileChanged(m_file);
    }

    void setFillMode(FillMode::FillMode fillMode)
    {
        if (m_fillMode == fillMode)
            return;

        m_fillMode = fillMode;
        emit fillModeChanged(m_fillMode);
    }

    void setAbsolutePath(QString absolutePath)
    {
        if (m_absolutePath == absolutePath)
            return;

        m_absolutePath = absolutePath;
        emit absolutePathChanged(m_absolutePath);
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

    void setIsLooping(bool isLooping)
    {
        if (m_isLooping == isLooping)
            return;

        m_isLooping = isLooping;
        emit isLoopingChanged(m_isLooping);
    }

    void setPlaybackRate(float playbackRate)
    {
        if (playbackRate < 0.0f || playbackRate > 1.0f)
            return;

        m_playbackRate = playbackRate;
        emit playbackRateChanged(m_playbackRate);
    }

private:
    const std::shared_ptr<GlobalVariables>& m_globalVariables;
    std::shared_ptr<SDKConnection> m_connection;

    ProjectSettingsListModel m_projectSettingsListModel;
    QVector<int> m_screenNumber;
    QProcess m_process;
    QString m_previewImage;
    InstalledType::InstalledType m_type;
    FillMode::FillMode m_fillMode;
    QString m_appID;
    QString m_absolutePath;
    QString m_file;
    float m_volume { 1.0f };
    bool m_isLooping { true };
    float m_playbackRate { 1.0f };
};
}
