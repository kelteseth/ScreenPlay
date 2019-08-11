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

namespace ScreenPlay {

class CreateImportVideo : public QObject {
    Q_OBJECT
    Q_PROPERTY(float progress READ progress WRITE setProgress NOTIFY progressChanged)

public:
    CreateImportVideo() {}
    CreateImportVideo(QObject* parent = nullptr);
    explicit CreateImportVideo(const QString& videoPath, const QString& exportPath, QObject* parent = nullptr);
    ~CreateImportVideo() {}

    QString m_videoPath;
    QString m_exportPath;
    QString m_format;

    int m_numberOfFrames {0};
    int m_length {0};
    int m_framerate  {0};

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
        ConvertingPreviewGifFinished,
        ConvertingPreviewGifError,
        ConvertingPreviewImage,
        ConvertingPreviewImageFinished,
        ConvertingPreviewImageError,
        ConvertingAudio,
        ConvertingAudioFinished,
        ConvertingAudioError,
        ConvertingVideo,
        ConvertingVideoFinished,
        ConvertingVideoError,
        CopyFiles,
        CopyFilesFinished,
        CopyFilesError,
        CreateProjectFile,
        CreateProjectFileFinished,
        CreateProjectFileError,
        AbortCleanupError,
        CreateTmpFolderError,
        Finished,
    };
    Q_ENUM(ImportVideoState)

    float progress() const
    {
        return m_progress;
    }

signals:
    void createWallpaperStateChanged(CreateImportVideo::ImportVideoState state);
    void processOutput(QString text);
    void finished();
    void canceled();

    void progressChanged(float progress);

public slots:
    void process();
    void requestInterruption() {}

    bool createWallpaperInfo();
    bool createWallpaperVideoPreview();
    bool createWallpaperGifPreview();
    bool createWallpaperImagePreview();
    bool createWallpaperVideo();
    bool extractWallpaperAudio();

    void setProgress(float progress)
    {
        if(progress < 0 || progress > 1)
            return;

        if (qFuzzyCompare(m_progress, progress))
            return;

        m_progress = progress;
        emit progressChanged(m_progress);
    }

private:
    bool m_skipAudio {false};

    float m_progress;
};
}
Q_DECLARE_METATYPE(ScreenPlay::CreateImportVideo::ImportVideoState)
