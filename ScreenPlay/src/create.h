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
#include "globalvariables.h"
#include "util.h"

namespace ScreenPlay {

using std::shared_ptr,
    std::make_shared;

class Create : public QObject {
    Q_OBJECT

    Q_PROPERTY(QString workingDir READ workingDir WRITE setWorkingDir NOTIFY workingDirChanged)
    Q_PROPERTY(float progress READ progress WRITE setProgress NOTIFY progressChanged)
    Q_PROPERTY(QString ffmpegOutput READ ffmpegOutput WRITE appendFfmpegOutput NOTIFY ffmpegOutputChanged)

public:
    explicit Create(
        const shared_ptr<GlobalVariables> &globalVariables,
        QObject* parent = nullptr);
    Create();


    float progress() const
    {
        return m_progress;
    }

    QString workingDir() const
    {
        return m_workingDir;
    }

    QString ffmpegOutput() const
    {
        return m_ffmpegOutput;
    }

signals:
    void createWallpaperStateChanged(CreateImportVideo::ImportVideoState state);
    void progressChanged(float progress);
    void abortCreateWallpaper();
    void workingDirChanged(QString workingDir);
    void ffmpegOutputChanged(QString ffmpegOutput);

public slots:
    void createWidget(
        const QString &localStoragePath,
        const QString& title,
        const QString& previewThumbnail,
        const QString& createdBy,
        const QString& license,
        const QString& type,
        const QVector<QString>& tags) const;

    void createWallpaperStart(QString videoPath);
    void saveWallpaper(QString title, QString description, QString filePath, QString previewImagePath, QString youtube, QVector<QString> tags);
    void abortAndCleanup();


    void setProgress(float progress)
    {
        if (qFuzzyCompare(m_progress, progress))
            return;

        m_progress = progress;
        emit progressChanged(m_progress);
    }

    void setWorkingDir(const QString& workingDir)
    {
        if (m_workingDir == workingDir)
            return;

        m_workingDir = workingDir;
        emit workingDirChanged(m_workingDir);
    }

    void appendFfmpegOutput(QString ffmpegOutput)
    {
        m_ffmpegOutput += ffmpegOutput;
        emit ffmpegOutputChanged(m_ffmpegOutput);
    }

    void clearFfmpegOutput(){
        m_ffmpegOutput = "";
        emit ffmpegOutputChanged(m_ffmpegOutput);
    }

private:
    CreateImportVideo* m_createImportVideo {nullptr};
    QThread* m_createImportVideoThread {nullptr};

    const shared_ptr<GlobalVariables>& m_globalVariables;

    float m_progress { 0.0F };
    QString m_workingDir;
    QString m_ffmpegOutput;
};
}
