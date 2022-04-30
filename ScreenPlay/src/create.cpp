#include "ScreenPlay/create.h"

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
Create::Create(const std::shared_ptr<GlobalVariables>& globalVariables)
    : QObject(nullptr)
    , m_globalVariables(globalVariables)
{
    init();
}

/*!
  Constructor for the QMLEngine.
*/
Create::Create()
    : QObject(nullptr)
{
    init();
}

void Create::init()
{
    qRegisterMetaType<Create::VideoCodec>("Create::VideoCodec");
    qmlRegisterUncreatableType<Create>("ScreenPlay.Create", 1, 0, "VideoCodec", "Error only for enums");
    qmlRegisterType<Create>("ScreenPlay.Create", 1, 0, "Create");

    qRegisterMetaType<ImportVideoState::ImportVideoState>("ImportVideoState::ImportVideoState");
    qmlRegisterUncreatableMetaObject(ScreenPlay::ImportVideoState::staticMetaObject,
        "ScreenPlay.Enums.ImportVideoState",
        1, 0,
        "ImportVideoState",
        "Error: only enums");
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
void Create::createWallpaperStart(QString videoPath, Create::VideoCodec codec, const int quality)
{
    reset();
    videoPath = ScreenPlayUtil::toLocal(videoPath);

    const QDir installedDir = ScreenPlayUtil::toLocal(m_globalVariables->localStoragePath().toString());

    // Create a temp dir so we can later alter it to the workshop id
    const QDateTime date = QDateTime::currentDateTime();
    const auto folderName = date.toString("ddMMyyyyhhmmss");
    setWorkingDir(installedDir.path() + "/" + folderName);

    if (!installedDir.mkdir(folderName)) {
        qInfo() << "Unable to create folder with name: " << folderName << " at: " << installedDir;
        emit createWallpaperStateChanged(ImportVideoState::ImportVideoState::CreateTmpFolderError);
        emit abortCreateWallpaper();
        return;
    }

    QString target_codec;
    switch (codec) {
    case Create::VideoCodec::VP8:
        target_codec = "vp8";
        break;
    case Create::VideoCodec::VP9:
        target_codec = "vp9";
        break;
    case Create::VideoCodec::AV1:
        target_codec = "av1";
        break;
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
            emit createWallpaperStateChanged(ImportVideoState::ImportVideoState::Failed);
            emit import.abortAndCleanup();
            return;
        }

        qInfo() << "createWallpaperImageThumbnailPreview()";
        if (!import.createWallpaperImageThumbnailPreview() || m_interrupt) {
            emit createWallpaperStateChanged(ImportVideoState::ImportVideoState::Failed);
            emit import.abortAndCleanup();
            return;
        }

        qInfo() << "createWallpaperImagePreview()";
        if (!import.createWallpaperImagePreview() || m_interrupt) {
            emit createWallpaperStateChanged(ImportVideoState::ImportVideoState::Failed);
            emit import.abortAndCleanup();
            return;
        }

        // Skip preview convert for webm
        if (!import.m_isWebm) {
            qInfo() << "createWallpaperVideoPreview()";
            if (!import.createWallpaperVideoPreview() || m_interrupt) {
                emit createWallpaperStateChanged(ImportVideoState::ImportVideoState::Failed);
                emit import.abortAndCleanup();

                return;
            }
        }

        qInfo() << "createWallpaperGifPreview()";
        if (!import.createWallpaperGifPreview() || m_interrupt) {
            emit createWallpaperStateChanged(ImportVideoState::ImportVideoState::Failed);
            emit import.abortAndCleanup();
            return;
        }

        // If the video has no audio we can skip the extraction
        if (!import.m_skipAudio) {
            qInfo() << "extractWallpaperAudio()";
            if (!import.extractWallpaperAudio() || m_interrupt) {
                emit createWallpaperStateChanged(ImportVideoState::ImportVideoState::Failed);
                emit import.abortAndCleanup();
                return;
            }
        }

        // Skip convert for webm
        if (import.m_isWebm) {
            emit createWallpaperStateChanged(ImportVideoState::ImportVideoState::Finished);
            return;
        }

        qInfo() << "createWallpaperVideo()";
        if (!import.createWallpaperVideo() || m_interrupt) {
            emit createWallpaperStateChanged(ImportVideoState::ImportVideoState::Failed);
            emit import.abortAndCleanup();
            return;
        }
        emit createWallpaperStateChanged(ImportVideoState::ImportVideoState::Finished);
    });

    QObject::connect(&m_createImportFutureWatcher, &QFutureWatcherBase::finished, this, [this]() {
        if (m_interrupt)
            abortAndCleanup();
    });

    m_createImportFutureWatcher.setFuture(m_createImportFuture);
}

void Create::importH264(QString videoPath)
{
    reset();
    videoPath = ScreenPlayUtil::toLocal(videoPath);

    const QDir installedDir = ScreenPlayUtil::toLocal(m_globalVariables->localStoragePath().toString());

    // Create a temp dir so we can later alter it to the workshop id
    const QDateTime date = QDateTime::currentDateTime();
    const auto folderName = date.toString("ddMMyyyyhhmmss");
    setWorkingDir(installedDir.path() + "/" + folderName);

    if (!installedDir.mkdir(folderName)) {
        qInfo() << "Unable to create folder with name: " << folderName << " at: " << installedDir;
        emit createWallpaperStateChanged(ImportVideoState::ImportVideoState::CreateTmpFolderError);
        emit abortCreateWallpaper();
        return;
    }

    m_createImportFuture = QtConcurrent::run(QThreadPool::globalInstance(), [videoPath, this]() {
        CreateImportVideo import(videoPath, workingDir(), m_interrupt);
        QObject::connect(&import, &CreateImportVideo::createWallpaperStateChanged, this, &Create::createWallpaperStateChanged, Qt::ConnectionType::QueuedConnection);
        QObject::connect(&import, &CreateImportVideo::abortAndCleanup, this, &Create::abortAndCleanup, Qt::ConnectionType::QueuedConnection);
        QObject::connect(
            &import, &CreateImportVideo::processOutput, this, [this](const QString text) {
                appendFfmpegOutput(text + "\n");
            },
            Qt::ConnectionType::QueuedConnection);

        if (!import.createWallpaperInfo() || m_interrupt) {
            emit createWallpaperStateChanged(ImportVideoState::ImportVideoState::Failed);
            emit import.abortAndCleanup();
            return;
        }

        qInfo() << "createWallpaperImageThumbnailPreview()";
        if (!import.createWallpaperImageThumbnailPreview() || m_interrupt) {
            emit createWallpaperStateChanged(ImportVideoState::ImportVideoState::Failed);
            emit import.abortAndCleanup();
            return;
        }

        qInfo() << "createWallpaperImagePreview()";
        if (!import.createWallpaperImagePreview() || m_interrupt) {
            emit createWallpaperStateChanged(ImportVideoState::ImportVideoState::Failed);
            emit import.abortAndCleanup();
            return;
        }

        // Skip preview convert for webm
        if (!import.createWallpaperVideoPreview() || m_interrupt) {
            emit createWallpaperStateChanged(ImportVideoState::ImportVideoState::Failed);
            emit import.abortAndCleanup();
            return;
        }

        qInfo() << "createWallpaperGifPreview()";
        if (!import.createWallpaperGifPreview() || m_interrupt) {
            emit createWallpaperStateChanged(ImportVideoState::ImportVideoState::Failed);
            emit import.abortAndCleanup();
            return;
        }

        // If the video has no audio we can skip the extraction
        if (!import.m_skipAudio) {
            qInfo() << "extractWallpaperAudio()";
            if (!import.extractWallpaperAudio() || m_interrupt) {
                emit createWallpaperStateChanged(ImportVideoState::ImportVideoState::Failed);
                emit import.abortAndCleanup();
                return;
            }
        }

        emit createWallpaperStateChanged(ImportVideoState::ImportVideoState::Finished);
        return;
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
    const Create::VideoCodec codec,
    const QVector<QString> tags)
{
    filePath = ScreenPlayUtil::toLocal(filePath);
    previewImagePath = ScreenPlayUtil::toLocal(previewImagePath);

    emit createWallpaperStateChanged(ImportVideoState::ImportVideoState::CopyFiles);

    // Case when the selected users preview image has the same name as
    // our default "preview.jpg" name. QFile::copy does no override exsisting files
    // so we need to delete them first
    QFile userSelectedPreviewImage(previewImagePath);
    if (userSelectedPreviewImage.fileName() == "preview.jpg") {
        if (!userSelectedPreviewImage.remove()) {
            qDebug() << "Could remove" << previewImagePath;
            emit createWallpaperStateChanged(ImportVideoState::ImportVideoState::CopyFilesError);
        }
    }

    QFileInfo previewImageFile(previewImagePath);
    if (previewImageFile.exists()) {
        if (!QFile::copy(previewImagePath, m_workingDir + "/" + previewImageFile.fileName())) {
            qDebug() << "Could not copy" << previewImagePath << " to " << m_workingDir + "/" + previewImageFile.fileName();
            emit createWallpaperStateChanged(ImportVideoState::ImportVideoState::CopyFilesError);
            return;
        }
    }

    QFileInfo filePathFile(filePath);
    if (filePath.endsWith(".webm") || filePath.endsWith(".mp4")) {
        if (!QFile::copy(filePath, m_workingDir + "/" + filePathFile.fileName())) {
            qDebug() << "Could not copy" << filePath << " to " << m_workingDir + "/" + filePathFile.fileName();
            emit createWallpaperStateChanged(ImportVideoState::ImportVideoState::CopyFilesError);
            return;
        }
    }
    emit createWallpaperStateChanged(ImportVideoState::ImportVideoState::CopyFilesFinished);
    emit createWallpaperStateChanged(ImportVideoState::ImportVideoState::CreateProjectFile);

    QJsonObject obj;
    obj.insert("description", description);
    obj.insert("title", title);
    obj.insert("youtube", youtube);
    obj.insert("videoCodec", QVariant::fromValue<VideoCodec>(codec).toString());
    obj.insert("file", filePathFile.completeBaseName() + (codec == VideoCodec::H264 ? ".mp4" : ".webm"));
    obj.insert("previewGIF", "preview.gif");
    obj.insert("previewWEBM", "preview.webm");
    obj.insert("preview", previewImageFile.exists() ? previewImageFile.fileName() : "preview.jpg");
    obj.insert("previewThumbnail", "previewThumbnail.jpg");
    obj.insert("type", "videoWallpaper");
    obj.insert("tags", ScreenPlayUtil::fillArray(tags));

    QFile audioFile { m_workingDir + "/audio.mp3" };
    if (audioFile.exists() && audioFile.size() > 0) {
        obj.insert("audio", "audio.mp3");
        obj.insert("audioCodec", "mp3");
    }

    if (!Util::writeSettings(std::move(obj), m_workingDir + "/project.json")) {
        emit createWallpaperStateChanged(ImportVideoState::ImportVideoState::CreateProjectFileError);
        return;
    }

    emit createWallpaperStateChanged(ImportVideoState::ImportVideoState::CreateProjectFileFinished);
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
            emit createWallpaperStateChanged(ImportVideoState::ImportVideoState::AbortCleanupError);
            qWarning() << "Could not delete temp exportPath: " << exportPath;
        }
    } else {
        qWarning() << "Could not cleanup video import. Export path does not exist: " << exportPath;
    }
}

}
