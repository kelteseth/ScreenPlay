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

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFuture>
#include <QFutureWatcher>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QObject>
#include <QProcess>
#include <QQmlEngine>
#include <QScopedPointer>
#include <QString>
#include <QStringList>
#include <QTime>
#include <QTimer>
#include <QUrl>
#include <QtMath>

#include <memory>

#include "ScreenPlayUtil/util.h"
#include "ScreenPlay/createimportvideo.h"
#include "ScreenPlay/globalvariables.h"
#include "ScreenPlay/util.h"

namespace ScreenPlay {

class Create : public QObject {
    Q_OBJECT

    Q_PROPERTY(QString workingDir READ workingDir WRITE setWorkingDir NOTIFY workingDirChanged)
    Q_PROPERTY(float progress READ progress WRITE setProgress NOTIFY progressChanged)
    Q_PROPERTY(QString ffmpegOutput READ ffmpegOutput WRITE appendFfmpegOutput NOTIFY ffmpegOutputChanged)

public:
    explicit Create(const std::shared_ptr<GlobalVariables>& globalVariables);

    Create();

    enum class VideoCodec {
        VP8,
        VP9,
        AV1,
        H264
    };
    Q_ENUM(VideoCodec)

    float progress() const { return m_progress; }
    QString workingDir() const { return m_workingDir; }
    QString ffmpegOutput() const { return m_ffmpegOutput; }

signals:
    void createWallpaperStateChanged(ImportVideoState::ImportVideoState state);
    void progressChanged(float progress);
    void abortCreateWallpaper();
    void workingDirChanged(QString workingDir);
    void ffmpegOutputChanged(QString ffmpegOutput);
    void widgetCreatedSuccessful(QString path);
    void htmlWallpaperCreatedSuccessful(QString path);
    void finished();

public slots:
    void cancel();

    void createWallpaperStart(QString videoPath, Create::VideoCodec codec = Create::VideoCodec::VP9, const int quality = 50);

    void importH264(QString videoPath);

    void saveWallpaper(const QString title,
        const QString description,
        QString filePath,
        QString previewImagePath,
        const QString youtube,
        const ScreenPlay::Create::VideoCodec codec,
        const QVector<QString> tags);

    void abortAndCleanup();

    void setProgress(float progress)
    {
        if (qFuzzyCompare(m_progress, progress))
            return;

        m_progress = progress;
        emit progressChanged(m_progress);
    }

    void setWorkingDir(const QString& workingDir)
    {
        if (m_workingDir == workingDir)
            return;

        m_workingDir = workingDir;
        emit workingDirChanged(m_workingDir);
    }

    void appendFfmpegOutput(QString ffmpegOutput)
    {
        m_ffmpegOutput += ffmpegOutput;
        emit ffmpegOutputChanged(m_ffmpegOutput);
    }

    void clearFfmpegOutput()
    {
        m_ffmpegOutput = "";
        emit ffmpegOutputChanged(m_ffmpegOutput);
    }

private:
    void init();
    void reset();

private:
    const std::shared_ptr<GlobalVariables> m_globalVariables;

    float m_progress { 0.0F };
    QString m_workingDir;
    QString m_ffmpegOutput;

    std::atomic<bool> m_interrupt;
    QFuture<void> m_createImportFuture;
    QFutureWatcher<void> m_createImportFutureWatcher;
};
}
