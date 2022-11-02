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
#include <QQmlEngine>
#include <QScopeGuard>
#include <QString>
#include <QThread>
#include <QtMath>

#include "ScreenPlay/createimportstates.h"

namespace ScreenPlay {

class CreateImportVideo : public QObject {
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(float progress READ progress WRITE setProgress NOTIFY progressChanged)

public:
    explicit CreateImportVideo(const QString& videoPath, const QString& exportPath, const QString& codec, const int quality, std::atomic<bool>& interrupt);
    explicit CreateImportVideo(const QString& videoPath, const QString& exportPath, std::atomic<bool>& interrupt);

    float progress() const { return m_progress; }

    bool m_skipAudio { false };

    // If the video is < 1s in duration we cannot create a 5s preview
    bool m_smallVideo { false };

    // We do not get many infos with this
    bool m_isWebm { false };

    float m_progress { 0.0F };

    QString m_videoPath;
    QString m_exportPath;
    QString m_format;
    QString m_codec;

    const int m_quality = 50;
    int m_numberOfFrames { 0 };
    int m_length { 0 };
    int m_framerate { 0 };

    enum class Executable {
        FFMPEG,
        FFPROBE
    };

signals:
    void createWallpaperStateChanged(ImportVideoState::ImportVideoState state);
    void processOutput(QString text);
    void finished();
    void abortAndCleanup();
    void progressChanged(float progress);

public slots:
    bool createWallpaperInfo();
    bool createWallpaperVideoPreview();
    bool createWallpaperGifPreview();
    bool createWallpaperImagePreview();
    bool createWallpaperVideo();
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
    void setupFFMPEG();

private:
    QString m_ffprobeExecutable;
    QString m_ffmpegExecutable;
    std::unique_ptr<QProcess> m_process;
    std::atomic<bool>& m_interrupt;
};
}
Q_DECLARE_METATYPE(ScreenPlay::ImportVideoState::ImportVideoState)
