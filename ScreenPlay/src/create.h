#pragma once

#include <QObject>
#include <QDir>
#include <QFileInfo>
#include <QStringList>
#include <QString>
#include <QFile>
#include <QProcess>
#include <QUrl>
#include <QQmlEngine>
#include <QTime>

#include "qmlutilities.h"
#include "settings.h"

namespace LocalWorkshopCreationStatus {
Q_NAMESPACE
enum Value {
    Idle,
    Started,
    CopyVideoFinished,
    CopyImageFinished,
    CopyConfigFinished,
    ConvertingPreviewImage,
    ConvertingPreviewImageFinished,
    ConvertingPreviewVideo,
    ConvertingPreviewVideoFinished,
    Finished,
    ErrorFolder,
    ErrorFolderCreation,
    ErrorDiskSpace,
    ErrorCopyVideo,
    ErrorCopyImage,
    ErrorCopyConfig,
    ErrorUnknown,
};
Q_ENUM_NS(Value)
}

class Create : public QObject
{
    Q_OBJECT
public:
    explicit Create(Settings* st, QMLUtilities* util,QObject *parent = nullptr);

signals:
    void localWorkshopCreationStatusChanged(LocalWorkshopCreationStatus::Value status);

public slots:
    void copyProject(QString relativeProjectPath, QString toPath);
    bool copyRecursively(const QString &srcFilePath, const QString &tgtFilePath);

    void importVideo(QString title, QUrl videoPath, QUrl previewPath, int videoDuration);
    void importVideo(QString title, QUrl videoPath, int timeStamp, int videoDuration, int videoFrameRate);
    void createVideoPreview(QString path, int framesToSkip);

private:
    Settings* m_settings;
    QMLUtilities* m_utils;
};
