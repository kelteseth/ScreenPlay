// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

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

#include "ScreenPlay/createimportvideo.h"
#include "ScreenPlay/globalvariables.h"

namespace ScreenPlay {

class Create : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")

    Q_PROPERTY(QString workingDir READ workingDir WRITE setWorkingDir NOTIFY workingDirChanged)
    Q_PROPERTY(float progress READ progress WRITE setProgress NOTIFY progressChanged)
    Q_PROPERTY(QString ffmpegOutput READ ffmpegOutput WRITE appendFfmpegOutput NOTIFY ffmpegOutputChanged)

public:
    explicit Create(const std::shared_ptr<GlobalVariables>& globalVariables);

    Create();

    enum class VideoCodec {
        VP8,
        VP9,
        AV1,
        H264
    };
    Q_ENUM(VideoCodec)

    float progress() const { return m_progress; }
    QString workingDir() const { return m_workingDir; }
    QString ffmpegOutput() const { return m_ffmpegOutput; }

signals:
    void createWallpaperStateChanged(ImportVideoState::ImportVideoState state);
    void progressChanged(float progress);
    void abortCreateWallpaper();
    void workingDirChanged(QString workingDir);
    void ffmpegOutputChanged(QString ffmpegOutput);
    void widgetCreatedSuccessful(QString path);
    void htmlWallpaperCreatedSuccessful(QString path);
    void finished();

public slots:
    void cancel();

    void createWallpaperStart(QString videoPath, Create::VideoCodec codec = Create::VideoCodec::VP9, const int quality = 50);

    void importH264(QString videoPath);

    void saveWallpaper(const QString title,
        const QString description,
        QString filePath,
        QString previewImagePath,
        const QString youtube,
        const ScreenPlay::Create::VideoCodec codec,
        const QVector<QString> tags);

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

    void clearFfmpegOutput()
    {
        m_ffmpegOutput = "";
        emit ffmpegOutputChanged(m_ffmpegOutput);
    }

private:
    void init();
    void reset();

private:
    const std::shared_ptr<GlobalVariables> m_globalVariables;

    float m_progress { 0.0F };
    QString m_workingDir;
    QString m_ffmpegOutput;

    std::atomic<bool> m_interrupt;
    QFuture<void> m_createImportFuture;
    QFutureWatcher<void> m_createImportFutureWatcher;
};
}
