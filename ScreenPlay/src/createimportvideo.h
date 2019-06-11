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
#include <QString>
#include <QThread>
#include <QtMath>
#include <QScopeGuard>

namespace ScreenPlay {

class CreateImportVideo : public QObject {
    Q_OBJECT

public:
    CreateImportVideo(QObject* parent = nullptr);
    explicit CreateImportVideo(const QString& videoPath, const QString& exportPath, QObject* parent = nullptr);

    enum class State {
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
    Q_ENUM(State)

    QString m_videoPath;
    QString m_exportPath;
    int m_length = 0;
    int m_framerate = 0;

signals:
    void createWallpaperStateChanged(CreateImportVideo::State state);
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
