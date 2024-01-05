// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#pragma once

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
#include "ScreenPlayUtil/contenttypes.h"

namespace ScreenPlay {

class CreateImportVideo : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")
    Q_CLASSINFO("RegisterEnumClassesUnscoped", "false")
    Q_PROPERTY(float progress READ progress WRITE setProgress NOTIFY progressChanged)

public:
    explicit CreateImportVideo(
        const QString& videoPath,
        const QString& exportPath,
        const ScreenPlay::Video::VideoCodec targetCodec,
        const int quality,
        std::atomic<bool>& interrupt);

    enum class Executable {
        FFMPEG,
        FFPROBE
    };
    Q_ENUM(Executable)

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
    Video::VideoCodec m_targetCodec;
    Video::VideoCodec m_sourceCodec;

    const int m_quality = 50;
    int m_numberOfFrames { 0 };
    int m_length { 0 };
    int m_framerate { 0 };

signals:
    void createWallpaperStateChanged(ScreenPlay::Import::State state);
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
