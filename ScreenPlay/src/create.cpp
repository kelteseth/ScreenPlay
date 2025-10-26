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
#include <QProcess>
#include <QStringList>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(create, "screenplay.create")
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
        qCInfo(create) << "Unable to create folder with name: " << folderName << " at: " << installedDir;
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

        qCInfo(create) << "createWallpaperImageThumbnailPreview()";
        if (!import.createWallpaperImageThumbnailPreview() || m_interrupt) {
            emit createWallpaperStateChanged(Import::State::Failed);
            emit import.abortAndCleanup();
            return;
        }

        qCInfo(create) << "createWallpaperImagePreview()";
        if (!import.createWallpaperImagePreview() || m_interrupt) {
            emit createWallpaperStateChanged(Import::State::Failed);
            emit import.abortAndCleanup();
            return;
        }

        // Skip preview convert for webm
        if (!import.m_isWebm) {
            qCInfo(create) << "createWallpaperVideoPreview()";
            if (!import.createWallpaperVideoPreview() || m_interrupt) {
                emit createWallpaperStateChanged(Import::State::Failed);
                emit import.abortAndCleanup();

                return;
            }
        }

        qCInfo(create) << "createWallpaperWebpPreview()";
        if (!import.createWallpaperWebpPreview() || m_interrupt) {
            emit createWallpaperStateChanged(Import::State::Failed);
            emit import.abortAndCleanup();
            return;
        }

        // If the video has no audio we can skip the extraction
        if (!import.m_skipAudio) {
            qCInfo(create) << "extractWallpaperAudio()";
            if (!import.extractWallpaperAudio() || m_interrupt) {
                emit createWallpaperStateChanged(Import::State::Failed);
                emit import.abortAndCleanup();
                return;
            }
        }

        qCInfo(create) << "createWallpaperVideo";
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
    \brief Detects the video codec from a file.
           Returns the actual detected codec enum value (H264, VP9, etc.).
           Returns Unknown if detection fails or codec is not recognized.
*/
ScreenPlay::Video::VideoCodec Create::detectVideoCodec(const QString& videoPath)
{
    ScreenPlay::Util util;
    // Handle both URL strings and regular file paths
    QString localVideoPath;
    if (videoPath.startsWith("file://") || videoPath.startsWith("qrc:")) {
        localVideoPath = util.toLocal(videoPath);
    } else {
        // Already a local path, use as-is
        localVideoPath = QDir::toNativeSeparators(videoPath);
    }
    
    QProcess process;
    QString ffprobeExecutable;
    
#ifdef Q_OS_LINUX
    ffprobeExecutable = "ffprobe";
#else
    ffprobeExecutable = QGuiApplication::applicationDirPath() + "/ffprobe" + util.executableBinEnding();
#endif

#ifndef Q_OS_LINUX
    if (!QFileInfo::exists(ffprobeExecutable)) {
        qCWarning(create) << "FFPROBE executable not found!";
        return ScreenPlay::Video::VideoCodec::Unknown;
    }
#endif

    QStringList args;
    args.append("-v");
    args.append("error");
    args.append("-select_streams");
    args.append("v:0");
    args.append("-show_entries");
    args.append("stream=codec_name");
    args.append("-of");
    args.append("default=noprint_wrappers=1:nokey=1");
    args.append(localVideoPath);
    
    qCInfo(create) << "Running FFprobe:" << ffprobeExecutable;
    qCInfo(create) << "With args:" << args;
    qCInfo(create) << "Video path:" << localVideoPath;
    
    process.setProgram(ffprobeExecutable);
    process.setArguments(args);
    process.start();
    
    if (!process.waitForFinished(5000)) {
        qCWarning(create) << "FFprobe timeout while detecting codec";
        qCWarning(create) << "FFprobe error:" << process.errorString();
        return ScreenPlay::Video::VideoCodec::Unknown;
    }
    
    if (process.exitCode() != 0) {
        qCWarning(create) << "FFprobe exited with code:" << process.exitCode();
        qCWarning(create) << "FFprobe stderr:" << process.readAllStandardError();
        qCWarning(create) << "FFprobe stdout:" << process.readAllStandardOutput();
        return ScreenPlay::Video::VideoCodec::Unknown;
    }
    
    QString codecName = QString::fromUtf8(process.readAllStandardOutput()).trimmed();
    qCInfo(create) << "Detected codec:" << codecName;
    
    if (codecName.isEmpty()) {
        qCWarning(create) << "FFprobe returned empty codec name";
        qCWarning(create) << "Command was:" << ffprobeExecutable << args;
        return ScreenPlay::Video::VideoCodec::Unknown;
    }
    
    // Map codec names to enum values
    if (codecName == "vp8") {
        return ScreenPlay::Video::VideoCodec::VP8;
    } else if (codecName == "vp9") {
        return ScreenPlay::Video::VideoCodec::VP9;
    } else if (codecName == "av1") {
        return ScreenPlay::Video::VideoCodec::AV1;
    } else if (codecName == "h264") {
        return ScreenPlay::Video::VideoCodec::H264;
    } else if (codecName == "hevc" || codecName == "h265") {
        return ScreenPlay::Video::VideoCodec::H265;
    } else {
        qCInfo(create) << "Unknown or unplayable codec detected";
        return ScreenPlay::Video::VideoCodec::Unknown;
    }
}

/*!
    \brief Checks if the given codec can be played without conversion.
           Returns true for VP8, VP9, H264, H265, AV1.
*/
bool Create::canSkipConversion(ScreenPlay::Video::VideoCodec codec)
{
    switch (codec) {
    case ScreenPlay::Video::VideoCodec::VP8:
    case ScreenPlay::Video::VideoCodec::VP9:
    case ScreenPlay::Video::VideoCodec::AV1:
    case ScreenPlay::Video::VideoCodec::H264:
    case ScreenPlay::Video::VideoCodec::H265:
        return true;
    case ScreenPlay::Video::VideoCodec::Unknown:
    case ScreenPlay::Video::VideoCodec::NoConversion:
        return false;
    }
    return false;
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
    
    // If NoConversion was selected, detect the actual codec from the file
    // so we can save the correct codec to project.json
    ScreenPlay::Video::VideoCodec actualCodec = codec;
    if (codec == ScreenPlay::Video::VideoCodec::NoConversion) {
        actualCodec = detectVideoCodec(filePath);
        qCInfo(create) << "NoConversion selected, detected actual codec:" << QVariant::fromValue(actualCodec).toString();
    }

    emit createWallpaperStateChanged(Import::State::CopyFiles);

    // Case when the selected users preview image has the same name as
    // our default "preview.jpg" name. QFile::copy does no override exsisting files
    // so we need to delete them first
    QFile userSelectedPreviewImage(previewImagePath);
    if (userSelectedPreviewImage.fileName() == "preview.jpg") {
        if (!userSelectedPreviewImage.remove()) {
            qCDebug(create) << "Could remove" << previewImagePath;
            emit createWallpaperStateChanged(Import::State::CopyFilesError);
        }
    }

    QFileInfo previewImageFile(previewImagePath);
    if (previewImageFile.exists()) {
        if (!QFile::copy(previewImagePath, m_workingDir + "/" + previewImageFile.fileName())) {
            qCDebug(create) << "Could not copy" << previewImagePath << " to " << m_workingDir + "/" + previewImageFile.fileName();
            emit createWallpaperStateChanged(Import::State::CopyFilesError);
            return;
        }
    }

    QFileInfo filePathFile(filePath);
    // if (filePath.endsWith(".webm") || filePath.endsWith(".mp4")) {
    //     if (!QFile::copy(filePath, m_workingDir + "/" + filePathFile.fileName())) {
    //         qCDebug(create) << "Could not copy" << filePath << " to " << m_workingDir + "/" + filePathFile.fileName();
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
    obj.insert("videoCodec", QVariant::fromValue<Video::VideoCodec>(actualCodec).toString());

    QString fileEnding;
    if (actualCodec == Video::VideoCodec::H264)
        fileEnding = ".mp4";
    if (actualCodec == Video::VideoCodec::AV1)
        fileEnding = ".mkv";
    if (actualCodec == Video::VideoCodec::VP8 || actualCodec == Video::VideoCodec::VP9)
        fileEnding = ".webm";

    obj.insert("file", filePathFile.completeBaseName() + fileEnding);
    obj.insert("previewWEBP", "preview.webp");
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
    qCInfo(create) << "cancel()";
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
            qCWarning(create) << "Could not delete temp exportPath: " << exportPath;
        }
    } else {
        qCWarning(create) << "Could not cleanup video import. Export path does not exist: " << exportPath;
    }
}

}

#include "moc_create.cpp"
