#pragma once

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QObject>
#include <QProcess>
#include <QQmlEngine>
#include <QString>
#include <QStringList>
#include <QTime>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonArray>
#include <QtMath>

#include "qmlutilities.h"
#include "settings.h"

class Create : public QObject {
    Q_OBJECT
public:
    explicit Create(Settings* st, QMLUtilities* util,QObject *parent = nullptr);
    Create();
    ~Create();

    Q_PROPERTY(State importState READ importState WRITE setImportState NOTIFY importStateChanged)


    enum class State {
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
    Q_ENUM(State)



    State importState() const
    {
        return m_importState;
    }

signals:
    void localWorkshopCreationStatusChanged(State status);
    void importStateChanged(State importState);

public slots:
    void copyProject(QString relativeProjectPath, QString toPath);
    bool copyRecursively(const QString& srcFilePath, const QString& tgtFilePath);

    void importVideo(QString title, QUrl videoPath, QUrl previewPath, int videoDuration);
    void importVideo(QString title, QUrl videoPath, int timeStamp, int videoDuration, int videoFrameRate);
    void createVideoPreview(QString path, int frames, int length);
    void createWallpaper(QString videoPath);
    void setImportState(State importState)
    {
        if (m_importState == importState)
            return;

        m_importState = importState;
        emit importStateChanged(m_importState);
    }

private:
    Settings* m_settings;
    QMLUtilities* m_utils;

    State m_importState = State::Idle;
};
