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
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QObject>
#include <QProcess>
#include <QScopeGuard>
#include <QString>
#include <QThread>
#include <QtMath>

#include "util.h"

namespace ScreenPlay {

class CreateImportVideo : public QObject {
    Q_OBJECT
    Q_PROPERTY(float progress READ progress WRITE setProgress NOTIFY progressChanged)

public:
    CreateImportVideo() { }
    CreateImportVideo(QObject* parent = nullptr);
    explicit CreateImportVideo(const QString& videoPath, const QString& exportPath, const QStringList& codecs, QObject* parent = nullptr);

    enum class ImportVideoState {
        Idle,
        Started,
        AnalyseVideo,
        AnalyseVideoFinished,
        AnalyseVideoError,
        AnalyseVideoHasNoVideoStreamError,
        ConvertingPreviewVideo,
        ConvertingPreviewVideoFinished,
        ConvertingPreviewVideoError,
        ConvertingPreviewGif,
        ConvertingPreviewGifFinished, //10
        ConvertingPreviewGifError,
        ConvertingPreviewImage,
        ConvertingPreviewImageFinished,
        ConvertingPreviewImageError,
        ConvertingPreviewImageThumbnail,
        ConvertingPreviewImageThumbnailFinished,
        ConvertingPreviewImageThumbnailError,
        ConvertingAudio,
        ConvertingAudioFinished,
        ConvertingAudioError, //20
        ConvertingVideo,
        ConvertingVideoFinished,
        ConvertingVideoError,
        CopyFiles,
        CopyFilesFinished,
        CopyFilesError,
        CreateProjectFile,
        CreateProjectFileFinished,
        CreateProjectFileError,
        AbortCleanupError, //30
        CreateTmpFolderError,
        Finished,
    };
    Q_ENUM(ImportVideoState)

    float progress() const
    {
        return m_progress;
    }

    bool m_skipAudio { false };

    // If the video is < 1s in duration we cannot create a 5s preview
    bool m_smallVideo { false };

    // We do not get many infos with this
    bool m_isWebm { false };

    float m_progress { 0.0F };

    QString m_videoPath;
    QString m_exportPath;
    QString m_format;
    QStringList m_codecs;

    int m_numberOfFrames { 0 };
    int m_length { 0 };
    int m_framerate { 0 };

    enum class Executable {
        FFMPEG,
        FFPROBE
    };

signals:
    void createWallpaperStateChanged(CreateImportVideo::ImportVideoState state);
    void processOutput(QString text);
    void finished();
    void abortAndCleanup();
    void progressChanged(float progress);
    void ffmpegOutput(QString text);

public slots:
    void process();

    bool createWallpaperInfo();
    bool createWallpaperVideoPreview();
    bool createWallpaperGifPreview();
    bool createWallpaperImagePreview();
    bool createWallpaperVideo(const QString& codec);
    bool extractWallpaperAudio();
    bool createWallpaperImageThumbnailPreview();

    void setProgress(float progress)
    {
        if (progress < 0 || progress > 1)
            return;

        if (qFuzzyCompare(m_progress, progress))
            return;

        m_progress = progress;
        emit progressChanged(m_progress);
    }

private:
    QString waitForFinished(
            const QStringList& args,
            const QProcess::ProcessChannelMode processChannelMode = QProcess::ProcessChannelMode::SeparateChannels,
            const Executable executable = Executable::FFMPEG);

    bool analyzeWebmReadFrames(const QJsonObject& obj);
    bool analyzeVideo(const QJsonObject& obj);

    QString m_ffprobeExecutable;
    QString m_ffmpegExecutable;
};
}
Q_DECLARE_METATYPE(ScreenPlay::CreateImportVideo::ImportVideoState)
