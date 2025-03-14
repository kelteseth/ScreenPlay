// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#include "ScreenPlay/create.h"
#include "ScreenPlay/createimportvideo.h"
#include "ScreenPlayCore/util.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QGuiApplication>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QStringList>
#include <QTime>
#include <QTimer>
#include <QUrl>
#include <QtMath>

namespace ScreenPlay {

/*!
    \class ScreenPlay::Create
    \inmodule ScreenPlay
    \brief  Baseclass for creating wallapers, widgets and the corresponding
            wizards.

    As for this writing (April 2019) it is solely used to import webm wallpaper
    and create the gif/web 5 second previews.
*/

/*!
  Constructor.
*/
Create::Create(const std::shared_ptr<GlobalVariables>& globalVariables, QObject* parent)
    : QObject(parent)
    , m_globalVariables(globalVariables)
{
}

/*!
  Constructor for the QMLEngine.
*/
Create::Create(QObject* parent)
    : QObject(parent)
{
}

void Create::reset()
{
    clearFfmpegOutput();
    m_interrupt = false;
    setProgress(0.);
    setWorkingDir({});
}

/*!
    \brief Starts the process.
*/
void Create::createWallpaperStart(QString videoPath, ScreenPlay::Video::VideoCodec target_codec, const int quality)
{
    reset();
    ScreenPlay::Util util;
    videoPath = util.toLocal(videoPath);

    const QDir installedDir = util.toLocal(m_globalVariables->localStoragePath().toString());

    // Create a temp dir so we can later alter it to the workshop id
    const QDateTime date = QDateTime::currentDateTime();
    const auto folderName = date.toString("ddMMyyyyhhmmss");
    setWorkingDir(installedDir.path() + "/" + folderName);

    if (!installedDir.mkdir(folderName)) {
        qInfo() << "Unable to create folder with name: " << folderName << " at: " << installedDir;
        emit createWallpaperStateChanged(Import::State::CreateTmpFolderError);
        emit abortCreateWallpaper();
        return;
    }

    m_createImportFuture = QtConcurrent::run(QThreadPool::globalInstance(), [videoPath, target_codec, quality, this]() {
        CreateImportVideo import(videoPath, workingDir(), target_codec, quality, m_interrupt);
        QObject::connect(&import, &CreateImportVideo::createWallpaperStateChanged, this, &Create::createWallpaperStateChanged, Qt::ConnectionType::QueuedConnection);
        QObject::connect(&import, &CreateImportVideo::abortAndCleanup, this, &Create::abortAndCleanup, Qt::ConnectionType::QueuedConnection);
        QObject::connect(
            &import, &CreateImportVideo::processOutput, this, [this](const QString text) {
                appendFfmpegOutput(text + "\n");
            },
            Qt::ConnectionType::QueuedConnection);

        if (!import.createWallpaperInfo() || m_interrupt) {
            emit createWallpaperStateChanged(Import::State::Failed);
            emit import.abortAndCleanup();
            return;
        }

        qInfo() << "createWallpaperImageThumbnailPreview()";
        if (!import.createWallpaperImageThumbnailPreview() || m_interrupt) {
            emit createWallpaperStateChanged(Import::State::Failed);
            emit import.abortAndCleanup();
            return;
        }

        qInfo() << "createWallpaperImagePreview()";
        if (!import.createWallpaperImagePreview() || m_interrupt) {
            emit createWallpaperStateChanged(Import::State::Failed);
            emit import.abortAndCleanup();
            return;
        }

        // Skip preview convert for webm
        if (!import.m_isWebm) {
            qInfo() << "createWallpaperVideoPreview()";
            if (!import.createWallpaperVideoPreview() || m_interrupt) {
                emit createWallpaperStateChanged(Import::State::Failed);
                emit import.abortAndCleanup();

                return;
            }
        }

        qInfo() << "createWallpaperGifPreview()";
        if (!import.createWallpaperGifPreview() || m_interrupt) {
            emit createWallpaperStateChanged(Import::State::Failed);
            emit import.abortAndCleanup();
            return;
        }

        // If the video has no audio we can skip the extraction
        if (!import.m_skipAudio) {
            qInfo() << "extractWallpaperAudio()";
            if (!import.extractWallpaperAudio() || m_interrupt) {
                emit createWallpaperStateChanged(Import::State::Failed);
                emit import.abortAndCleanup();
                return;
            }
        }

        qInfo() << "createWallpaperVideo";
        if (!import.createWallpaperVideo() || m_interrupt) {
            emit createWallpaperStateChanged(Import::State::Failed);
            emit import.abortAndCleanup();
            return;
        }
        emit createWallpaperStateChanged(Import::State::Finished);
    });

    QObject::connect(&m_createImportFutureWatcher, &QFutureWatcherBase::finished, this, [this]() {
        if (m_interrupt)
            abortAndCleanup();
    });

    m_createImportFutureWatcher.setFuture(m_createImportFuture);
}

/*!
    \brief When converting of the wallpaper steps where successful.
*/
void Create::saveWallpaper(
    const QString title,
    const QString description,
    QString filePath,
    QString previewImagePath,
    const QString youtube,
    const ScreenPlay::Video::VideoCodec codec,
    const QVector<QString> tags)
{
    ScreenPlay::Util util;
    filePath = util.toLocal(filePath);
    previewImagePath = util.toLocal(previewImagePath);

    emit createWallpaperStateChanged(Import::State::CopyFiles);

    // Case when the selected users preview image has the same name as
    // our default "preview.jpg" name. QFile::copy does no override exsisting files
    // so we need to delete them first
    QFile userSelectedPreviewImage(previewImagePath);
    if (userSelectedPreviewImage.fileName() == "preview.jpg") {
        if (!userSelectedPreviewImage.remove()) {
            qDebug() << "Could remove" << previewImagePath;
            emit createWallpaperStateChanged(Import::State::CopyFilesError);
        }
    }

    QFileInfo previewImageFile(previewImagePath);
    if (previewImageFile.exists()) {
        if (!QFile::copy(previewImagePath, m_workingDir + "/" + previewImageFile.fileName())) {
            qDebug() << "Could not copy" << previewImagePath << " to " << m_workingDir + "/" + previewImageFile.fileName();
            emit createWallpaperStateChanged(Import::State::CopyFilesError);
            return;
        }
    }

    QFileInfo filePathFile(filePath);
    // if (filePath.endsWith(".webm") || filePath.endsWith(".mp4")) {
    //     if (!QFile::copy(filePath, m_workingDir + "/" + filePathFile.fileName())) {
    //         qDebug() << "Could not copy" << filePath << " to " << m_workingDir + "/" + filePathFile.fileName();
    //         emit createWallpaperStateChanged(Import::State::CopyFilesError);
    //         return;
    //     }
    // }
    emit createWallpaperStateChanged(Import::State::CopyFilesFinished);
    emit createWallpaperStateChanged(Import::State::CreateProjectFile);

    QJsonObject obj;
    obj.insert("description", description);
    obj.insert("title", title);
    obj.insert("youtube", youtube);
    obj.insert("videoCodec", QVariant::fromValue<Video::VideoCodec>(codec).toString());

    QString fileEnding;
    if (codec == Video::VideoCodec::H264)
        fileEnding = ".mp4";
    if (codec == Video::VideoCodec::AV1)
        fileEnding = ".mkv";
    if (codec == Video::VideoCodec::VP8 || codec == Video::VideoCodec::VP9)
        fileEnding = ".webm";

    obj.insert("file", filePathFile.completeBaseName() + fileEnding);
    obj.insert("previewGIF", "preview.gif");
    obj.insert("previewWEBM", "preview.webm");
    obj.insert("preview", previewImageFile.exists() ? previewImageFile.fileName() : "preview.jpg");
    obj.insert("previewThumbnail", "previewThumbnail.jpg");
    obj.insert("type", "videoWallpaper");
    obj.insert("tags", util.fillArray(tags));

    QFile audioFile { m_workingDir + "/audio.mp3" };
    if (audioFile.exists() && audioFile.size() > 0) {
        obj.insert("audio", "audio.mp3");
        obj.insert("audioCodec", "mp3");
    }

    if (!util.writeSettings(std::move(obj), m_workingDir + "/project.json")) {
        emit createWallpaperStateChanged(Import::State::CreateProjectFileError);
        return;
    }

    emit createWallpaperStateChanged(Import::State::CreateProjectFileFinished);
    emit finished();
}

/*!
  \brief This method is called from qml.
*/
void Create::cancel()
{
    qInfo() << "cancel()";
    m_interrupt = true;
}

/*!
  \brief This method is called when the user manually aborts the wallpaper import.
*/
void Create::abortAndCleanup()
{
    QDir exportPath(m_workingDir);
    if (exportPath.exists()) {
        if (!exportPath.removeRecursively()) {
            emit createWallpaperStateChanged(Import::State::AbortCleanupError);
            qWarning() << "Could not delete temp exportPath: " << exportPath;
        }
    } else {
        qWarning() << "Could not cleanup video import. Export path does not exist: " << exportPath;
    }
}

}

#include "moc_create.cpp"
