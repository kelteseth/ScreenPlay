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


/*!
    \class CreateImportVideo
    \brief This class imports (copies) and creates wallaper previews. This
           class only exsits as long as the user creates a wallpaper and gets
           destroyed if the creation was successful or not.

           The state get propagated via createWallpaperStateChanged(CreateImportVideo::State state);

    \todo
            - Maybe: Replace with QThread to avoid running QCoreApplication::processEvents();?
*/

class CreateImportVideo : public QObject {
    Q_OBJECT

public:
    CreateImportVideo(QObject* parent = nullptr);
    explicit CreateImportVideo(QString videoPath, QString exportPath, QObject* parent = nullptr);

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
