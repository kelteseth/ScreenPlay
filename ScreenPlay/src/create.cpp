#include "create.h"

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
    qRegisterMetaType<CreateImportVideo::ImportVideoState>("CreateImportVideo::ImportVideoState");
    qRegisterMetaType<Create::VideoCodec>("Create::VideoCodec");
    qmlRegisterUncreatableType<CreateImportVideo>("ScreenPlay.Create", 1, 0, "CreateImportVideo", "Error only for enums");
    qmlRegisterUncreatableType<Create>("ScreenPlay.Create", 1, 0, "VideoCodec", "Error only for enums");
    qmlRegisterType<Create>("ScreenPlay.Create", 1, 0, "Create");
}

/*!
  Constructor for the QMLEngine.
*/
Create::Create()
    : QObject(nullptr)
{
    qRegisterMetaType<CreateImportVideo::ImportVideoState>("CreateImportVideo::ImportVideoState");
    qRegisterMetaType<Create::VideoCodec>("Create::VideoCodec");
    qmlRegisterUncreatableType<ScreenPlay::CreateImportVideo>("ScreenPlay.Create", 1, 0, "CreateImportVideo", "Error only for enums");
    qmlRegisterUncreatableType<Create>("ScreenPlay.Create", 1, 0, "VideoCodec", "Error only for enums");
    qmlRegisterType<Create>("ScreenPlay.Create", 1, 0, "Create");
}

/*!
    \brief Starts the process.
*/
void Create::createWallpaperStart(QString videoPath, Create::VideoCodec codec, const int quality)
{
    clearFfmpegOutput();
    videoPath = ScreenPlayUtil::toLocal(videoPath);

    const QDir dir(m_globalVariables->localStoragePath().toLocalFile());

    // Create a temp dir so we can later alter it to the workshop id
    const auto folderName = QString("_tmp_" + QTime::currentTime().toString()).replace(":", "");

    if (!dir.mkdir(folderName)) {
        emit createWallpaperStateChanged(CreateImportVideo::ImportVideoState::CreateTmpFolderError);
        emit abortCreateWallpaper();
        return;
    }
    setWorkingDir(dir.path() + "/" + folderName);

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

    m_createImportVideoThread = std::make_unique<QThread>();
    m_createImportVideo = std::make_unique<CreateImportVideo>(videoPath, workingDir(), target_codec, quality);
    connect(m_createImportVideo.get(), &CreateImportVideo::processOutput, this, [this](QString text) {
        appendFfmpegOutput(text + "\n");
    });

    connect(m_createImportVideo.get(), &CreateImportVideo::createWallpaperStateChanged, this, &Create::createWallpaperStateChanged);
    connect(m_createImportVideo.get(), &CreateImportVideo::progressChanged, this, &Create::setProgress);
    connect(m_createImportVideoThread.get(), &QThread::started, m_createImportVideo.get(), &CreateImportVideo::process);
    connect(m_createImportVideo.get(), &CreateImportVideo::abortAndCleanup, this, &Create::abortAndCleanup);

    connect(m_createImportVideo.get(), &CreateImportVideo::finished, m_createImportVideoThread.get(), &QThread::quit);
    connect(m_createImportVideo.get(), &CreateImportVideo::finished, m_createImportVideo.get(), &QObject::deleteLater);
    connect(m_createImportVideoThread.get(), &QThread::finished, m_createImportVideoThread.get(), &QObject::deleteLater);

    m_createImportVideo->moveToThread(m_createImportVideoThread.get());
    m_createImportVideoThread->start();
}

/*!
    \brief When converting of the wallpaper steps where successful.
*/
void Create::saveWallpaper(QString title, QString description, QString filePath, QString previewImagePath, QString youtube, Create::VideoCodec codec, QVector<QString> tags)
{
    filePath = ScreenPlayUtil::toLocal(filePath);
    previewImagePath = ScreenPlayUtil::toLocal(previewImagePath);

    emit createWallpaperStateChanged(CreateImportVideo::ImportVideoState::CopyFiles);

    // Case when the selected users preview image has the same name as
    // our default "preview.jpg" name. QFile::copy does no override exsisting files
    // so we need to delete them first
    QFile userSelectedPreviewImage(previewImagePath);
    if (userSelectedPreviewImage.fileName() == "preview.jpg") {
        if (!userSelectedPreviewImage.remove()) {
            qDebug() << "Could remove" << previewImagePath;
            emit createWallpaperStateChanged(CreateImportVideo::ImportVideoState::CopyFilesError);
        }
    }

    QFileInfo previewImageFile(previewImagePath);
    if (previewImageFile.exists()) {
        if (!QFile::copy(previewImagePath, m_workingDir + "/" + previewImageFile.fileName())) {
            qDebug() << "Could not copy" << previewImagePath << " to " << m_workingDir + "/" + previewImageFile.fileName();
            emit createWallpaperStateChanged(CreateImportVideo::ImportVideoState::CopyFilesError);
            return;
        }
    }

    QFileInfo filePathFile(filePath);
    if (filePath.endsWith(".webm")) {
        if (!QFile::copy(filePath, m_workingDir + "/" + filePathFile.fileName())) {
            qDebug() << "Could not copy" << filePath << " to " << m_workingDir + "/" + filePathFile.fileName();
            emit createWallpaperStateChanged(CreateImportVideo::ImportVideoState::CopyFilesError);
            return;
        }
    }
    emit createWallpaperStateChanged(CreateImportVideo::ImportVideoState::CopyFilesFinished);
    emit createWallpaperStateChanged(CreateImportVideo::ImportVideoState::CreateProjectFile);

    QJsonObject obj;
    obj.insert("description", description);
    obj.insert("title", title);
    obj.insert("youtube", youtube);
    obj.insert("videoCodec", codec == Create::VideoCodec::VP8 ? "vp8" : "vp9");
    obj.insert("file", filePathFile.fileName());
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
        emit createWallpaperStateChanged(CreateImportVideo::ImportVideoState::CreateProjectFileError);
        return;
    }

    emit createWallpaperStateChanged(CreateImportVideo::ImportVideoState::CreateProjectFileFinished);

    m_createImportVideoThread->quit();
    m_createImportVideoThread->wait();
    m_createImportVideoThread.reset();
}

/*!
  \brief This method is called when the user manually aborts the wallpaper import.
*/
void Create::abortAndCleanup()
{
    qWarning() << "Abort and Cleanup!";

    if (m_createImportVideo == nullptr || m_createImportVideoThread == nullptr) {
        qDebug() << "Invalid thread pointer. Cancel abort!";
        return;
    }

    // Save to export path before aborting to be able to cleanup the tmp folder
    QString tmpExportPath = m_createImportVideo->m_exportPath;

    connect(m_createImportVideoThread.get(), &QThread::finished, this, [=]() {
        QDir exportPath(tmpExportPath);
        qWarning() << "Abort and Cleanup!" << exportPath;
        if (exportPath.exists()) {
            if (!exportPath.removeRecursively()) {
                emit createWallpaperStateChanged(CreateImportVideo::ImportVideoState::AbortCleanupError);
                qWarning() << "Could not delete temp exportPath: " << exportPath;
            } else {
                qDebug() << "cleanup " << tmpExportPath;
            }
        } else {
            qDebug() << "Could not cleanup and delete: " << exportPath;
        }
        m_createImportVideo.reset();
        m_createImportVideoThread.reset();
    });

    m_createImportVideoThread->quit();
    m_createImportVideoThread->wait();
}

}
