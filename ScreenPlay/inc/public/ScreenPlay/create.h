// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#pragma once

#include <QFuture>
#include <QFutureWatcher>
#include <QObject>
#include <QQmlEngine>
#include <QString>
#include <memory>

#include "ScreenPlay/createimportstates.h"
#include "ScreenPlay/globalvariables.h"
#include "ScreenPlayCore/contenttypes.h"

namespace ScreenPlay {

class Create : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")

    Q_PROPERTY(QString workingDir READ workingDir WRITE setWorkingDir NOTIFY workingDirChanged)
    Q_PROPERTY(float progress READ progress WRITE setProgress NOTIFY progressChanged)
    Q_PROPERTY(QString ffmpegOutput READ ffmpegOutput WRITE appendFfmpegOutput NOTIFY ffmpegOutputChanged)

public:
    explicit Create(const std::shared_ptr<GlobalVariables>& globalVariables, QObject* parent = nullptr);

    Create(QObject* parent = nullptr);

    Q_INVOKABLE void cancel();

    Q_INVOKABLE void createWallpaperStart(
        QString videoPath,
        ScreenPlay::Video::VideoCodec codec,
        const int quality = 50);

    Q_INVOKABLE void saveWallpaper(const QString title,
        const QString description,
        QString filePath,
        QString previewImagePath,
        const QString youtube,
        const ScreenPlay::Video::VideoCodec codec,
        const QVector<QString> tags);

    Q_INVOKABLE void abortAndCleanup();

    float progress() const { return m_progress; }
    QString workingDir() const { return m_workingDir; }
    QString ffmpegOutput() const { return m_ffmpegOutput; }

signals:
    void createWallpaperStateChanged(ScreenPlay::Import::State state);
    void progressChanged(float progress);
    void abortCreateWallpaper();
    void workingDirChanged(QString workingDir);
    void ffmpegOutputChanged(QString ffmpegOutput);
    void widgetCreatedSuccessful(QString path);
    void htmlWallpaperCreatedSuccessful(QString path);
    void finished();

public slots:
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
