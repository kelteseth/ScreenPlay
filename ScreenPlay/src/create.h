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
#include <QTimer>

#include "qmlutilities.h"
#include "settings.h"

struct CreateWallpaperData {
    CreateWallpaperData() {}

    QString videoPath = "";
    QString exportPath = "";
    int length = 0;
    int framerate = 0;
};

class Create : public QObject {
    Q_OBJECT
public:
    explicit Create(Settings* st, QMLUtilities* util, QObject* parent = nullptr);

    Q_PROPERTY(QString workingDir READ workingDir WRITE setWorkingDir NOTIFY workingDirChanged)
    Q_PROPERTY(float progress READ progress WRITE setProgress NOTIFY progressChanged)

    Create() {}
    ~Create() {}

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
        Finished,
        ErrorUnknown,
    };
    Q_ENUM(State)

    QString workingDir() const
    {
        return m_workingDir;
    }

    float progress() const
    {
        return m_progress;
    }

signals:
    void createWallpaperStateChanged(Create::State state);
    void processOutput(QString text);
    void workingDirChanged(QString workingDir);
    void progressChanged(float progress);
    void abortCreateWallpaper();

public slots:
    void copyProject(QString relativeProjectPath, QString toPath);
    bool copyRecursively(const QString& srcFilePath, const QString& tgtFilePath);

    // Steps to convert any video to a wallpaper broken down into
    // several methods in this order:
    void createWallpaperStart(QString videoPath);
    bool createWallpaperInfo();
    bool createWallpaperVideoPreview();
    bool createWallpaperGifPreview();
    bool createWallpaperImagePreview();
    bool extractWallpaperAudio();
    bool createWallpaperProjectFile(const QString title, const QString description);
    void abortAndCleanup();

    void setWorkingDir(QString workingDir)
    {
        if (m_workingDir == workingDir)
            return;

        m_workingDir = workingDir;
        emit workingDirChanged(m_workingDir);
    }

    void setProgress(float progress)
    {
        if (qFuzzyCompare(m_progress, progress))
            return;

        m_progress = progress;
        emit progressChanged(m_progress);
    }

private:
    Settings* m_settings;
    QMLUtilities* m_utils;
    CreateWallpaperData m_createWallpaperData;
    QString m_workingDir;
    float m_progress = 0.0f;
    QFuture<void> m_future;
    QFutureWatcher<void> m_futureWatcher;
};
