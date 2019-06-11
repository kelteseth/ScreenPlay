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

namespace ScreenPlay {



using std::shared_ptr,
    std::make_shared;

class Create : public QObject {
    Q_OBJECT

    Q_PROPERTY(QString workingDir READ workingDir WRITE setWorkingDir NOTIFY workingDirChanged)
    Q_PROPERTY(float progress READ progress WRITE setProgress NOTIFY progressChanged)

public:
    explicit Create(const shared_ptr<Settings>& settings,
        QObject* parent = nullptr);
    Create();
    ~Create() {}

    float progress() const
    {
        return m_progress;
    }

    QString workingDir() const
    {
        return m_workingDir;
    }

signals:
    void createWallpaperStateChanged(CreateImportVideo::State state);
    void processOutput(QString text);
    void progressChanged(float progress);
    void abortCreateWallpaper();
    void workingDirChanged(QString workingDir);

public slots:
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

    void setWorkingDir(QString workingDir)
    {
        if (m_workingDir == workingDir)
            return;

        m_workingDir = workingDir;
        emit workingDirChanged(m_workingDir);
    }

private:
    CreateImportVideo* m_createImportVideo;
    QThread* m_createImportVideoThread;

    const shared_ptr<Settings> m_settings;

    float m_progress = 0.0f;
    QString m_workingDir;
};
}
