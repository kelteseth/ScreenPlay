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

class Create : public QObject {
    Q_OBJECT
public:
    explicit Create(Settings* st, QMLUtilities* util, QObject* parent = nullptr);

    Q_PROPERTY(float progress READ progress WRITE setProgress NOTIFY progressChanged)

    Create() {}
    ~Create() {}

    float progress() const
    {
        return m_progress;
    }

signals:
    void createWallpaperStateChanged(CreateImportVideo::State state);
    void processOutput(QString text);
    void progressChanged(float progress);
    void abortCreateWallpaper();

public slots:
    void copyProject(QString relativeProjectPath, QString toPath);
    bool copyRecursively(const QString& srcFilePath, const QString& tgtFilePath);
    void abortAndCleanup();
    void createWallpaperStart(QString videoPath);

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

    float m_progress = 0.0f;
};
