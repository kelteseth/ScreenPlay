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
        AbortCleanupError,
        CreateTmpFolderError,
        Finished,
        ErrorUnknown,
    };
    Q_ENUM(State)

    QProcess m_process;
    QString m_videoPath;
    QString m_exportPath;
    int m_length = 0;
    int m_framerate = 0;

signals:
    void createWallpaperStateChanged(CreateImportVideo::State state);
    void processOutput(QString text);
    void finished();

public slots:
    void process();
    void requestInterruption()
    {
    }

    bool createWallpaperInfo();
    bool createWallpaperVideoPreview();
    bool createWallpaperGifPreview();
    bool createWallpaperImagePreview();
    bool extractWallpaperAudio();
    bool createWallpaperProjectFile(const QString title, const QString description);
};
