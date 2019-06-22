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

public:
    CreateImportVideo(){}
    CreateImportVideo(QObject* parent = nullptr);
    explicit CreateImportVideo(const QString& videoPath, const QString& exportPath, QObject* parent = nullptr);
    ~CreateImportVideo(){}

    QString m_videoPath;
    QString m_exportPath;
    int m_length = 0;
    int m_framerate = 0;
    enum class ImportVideoState {
        Idle,
        Started,
        AnalyseVideo,
        AnalyseVideoFinished,
        AnalyseVideoError,
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
        //      Oh well... Due to so many patents around video codecs
        //      the user has to convert the video on his own :(
        //        ConvertingVideo,
        //        ConvertingVideoFinished,
        //        ConvertingVideoError,
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

signals:
    void createWallpaperStateChanged(CreateImportVideo::ImportVideoState state);
    void processOutput(QString text);
    void finished();
    void canceled();

public slots:
    void process();
    void requestInterruption() {}

    bool createWallpaperInfo();
    bool createWallpaperVideoPreview();
    bool createWallpaperGifPreview();
    bool createWallpaperImagePreview();
    bool extractWallpaperAudio();
};
}
Q_DECLARE_METATYPE(ScreenPlay::CreateImportVideo::ImportVideoState)
