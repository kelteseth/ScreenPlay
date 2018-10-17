#pragma once

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
#include <QScopedPointer>
#include <QString>
#include <QStringList>
#include <QTime>
#include <QTimer>
#include <QUrl>
#include <QtMath>

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

    Create() {}
    ~Create() {}

    enum class State {
        Idle,
        Started,
        AnalyseVideo,
        AnalyseVideoFinished,
        AnalyseVideoError,
        ConvertingPreviewImage,
        ConvertingPreviewImageFinished,
        ConvertingPreviewImageError,
        ConvertingPreviewVideo,
        ConvertingPreviewVideoFinished,
        ConvertingPreviewVideoError,
        ConvertingPreviewGif,
        ConvertingPreviewGifFinished,
        ConvertingPreviewGifError,
        Finished,
        ErrorUnknown,
    };
    Q_ENUM(State)


    QString workingDir() const
    {
        return m_workingDir;
    }

signals:
    void createWallpaperStateChanged(Create::State state);
    void processOutput(QString text);
    void workingDirChanged(QString workingDir);

public slots:
    void copyProject(QString relativeProjectPath, QString toPath);
    bool copyRecursively(const QString& srcFilePath, const QString& tgtFilePath);

    // Steps to convert any video to a wallpaper broken down into
    // several methods in this order:
    void createWallpaperStart(QString videoPath);
    bool createWallpaperInfo(CreateWallpaperData& createWallpaperData);
    bool createWallpaperVideoPreview(CreateWallpaperData& createWallpaperData);
    bool createWallpaperVideo(CreateWallpaperData& createWallpaperData);
    bool createWallpaperProjectFile(CreateWallpaperData& createWallpaperData);

    void setWorkingDir(QString workingDir)
    {
        if (m_workingDir == workingDir)
            return;

        m_workingDir = workingDir;
        emit workingDirChanged(m_workingDir);
    }

private:
    Settings* m_settings;
    QMLUtilities* m_utils;

    QString m_workingDir;
};
