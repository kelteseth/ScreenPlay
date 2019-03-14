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

#include <memory>

#include "createimportvideo.h"
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

    QString workingDir() const
    {
        return m_workingDir;
    }

    float progress() const
    {
        return m_progress;
    }

signals:
    void createWallpaperStateChanged(CreateImportVideo::State state);
    void processOutput(QString text);
    void workingDirChanged(QString workingDir);
    void progressChanged(float progress);
    void abortCreateWallpaper();

public slots:
    void copyProject(QString relativeProjectPath, QString toPath);
    bool copyRecursively(const QString& srcFilePath, const QString& tgtFilePath);
    void abortAndCleanup();
    void createWallpaperStart(QString videoPath);

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
    CreateImportVideo* m_createImportVideo;
    Settings* m_settings;
    QThread* m_createImportVideoThread;
    QMLUtilities* m_utils;
    CreateWallpaperData m_createWallpaperData;
    QString m_workingDir;
    float m_progress = 0.0f;

};
