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
    ~CreateImportVideo()
    {
        qDebug() << "CreateImportVideo";
    }
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

    float m_progress { 0.0F };

    QString m_videoPath;
    QString m_exportPath;
    QString m_format;
    QStringList m_codecs;

    int m_numberOfFrames { 0 };
    int m_length { 0 };
    int m_framerate { 0 };

signals:
    void createWallpaperStateChanged(CreateImportVideo::ImportVideoState state);
    void processOutput(QString text);
    void finished();
    void abortAndCleanup();
    void progressChanged(float progress);

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
    void waitForFinished(std::shared_ptr<QProcess>& process);

    QString m_ffprobeExecutable;
    QString m_ffmpegExecutable;


};
}
Q_DECLARE_METATYPE(ScreenPlay::CreateImportVideo::ImportVideoState)
