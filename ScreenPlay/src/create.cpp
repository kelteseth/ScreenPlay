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
Create::Create(const shared_ptr<GlobalVariables>& globalVariables, QObject* parent)
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

void Create::createWidget(const QString& localStoragePath, const QString& title, const QString& previewThumbnail, const QString& createdBy, const QString& license, const QString& type, const QVector<QString>& tags)
{
    QtConcurrent::run([=]() {
        QUrl localStoragePathUrl { localStoragePath };
        QDir dir;
        dir.cd(localStoragePathUrl.toLocalFile());
        // Create a temp dir so we can later alter it to the workshop id
        auto folderName = QString("_tmp_" + QTime::currentTime().toString()).replace(":", "");
        QString workingPath = dir.path() + "/" + folderName;

        if (!dir.mkdir(folderName)) {
            qDebug() << "Could create folder: " << folderName;
            return;
        }

        QJsonObject obj;
        obj.insert("license", license);
        obj.insert("title", title);
        obj.insert("createdBy", createdBy);

        if (type == "QML") {
            obj.insert("type", "qmlWidget");
            obj.insert("file", "main.qml");

            QFile fileMainQML(workingPath + "/main.qml");
            if (!fileMainQML.open(QIODevice::WriteOnly | QIODevice::Text)) {
                qDebug() << "Could not open /main.qml";
                return;
            }

            QTextStream outMainQML(&fileMainQML);
            outMainQML.setCodec("UTF-8");
            outMainQML << "import QtQuick 2.14 \n\n Item {\n id:root \n}";
            fileMainQML.close();

        } else {
            obj.insert("type", "htmlWidget");
            obj.insert("file", "index.html");

            QFile fileMainHTML(workingPath + "/index.html");
            if (!fileMainHTML.open(QIODevice::WriteOnly | QIODevice::Text)) {
                qDebug() << "Could not open /index.html";
                return;
            }

            QTextStream outMainHTML(&fileMainHTML);
            outMainHTML.setCodec("UTF-8");
            outMainHTML << "<html>\n<head></head>\n<body></body>\n</html>";
            fileMainHTML.close();
        }

        QJsonArray tagsJsonArray;
        for (const QString& tmp : tags) {
            tagsJsonArray.append(tmp);
        }
        obj.insert("tags", tagsJsonArray);

        QFile file(workingPath + "/project.json");
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qDebug() << "Could not open /project.json";
            return;
        }

        QUrl previewThumbnailUrl { previewThumbnail };
        QFileInfo previewImageFile(previewThumbnailUrl.toLocalFile());

        if (!previewThumbnail.isEmpty()) {
            obj.insert("previewThumbnail", previewImageFile.fileName());
            obj.insert("preview", previewImageFile.fileName());
            if (!QFile::copy(previewThumbnailUrl.toLocalFile(), workingPath + "/" + previewImageFile.fileName())) {
                qDebug() << "Could not copy" << previewThumbnailUrl.toLocalFile() << " to " << workingPath + "/" + previewImageFile.fileName();
                return;
            }
        }

        QTextStream out(&file);
        out.setCodec("UTF-8");
        QJsonDocument doc(obj);
        out << doc.toJson();
        file.close();

        emit widgetCreatedSuccessful(workingPath);
    });
}

void Create::createHTMLWallpaper(
    const QString& localStoragePath,
    const QString& title,
    const QString& previewThumbnail,
    const QString& license,
    const QVector<QString>& tags)
{
    QtConcurrent::run([=]() {
        QUrl localStoragePathUrl { localStoragePath };
        QDir dir;
        dir.cd(localStoragePathUrl.toLocalFile());
        // Create a temp dir so we can later alter it to the workshop id
        auto folderName = QString("_tmp_" + QTime::currentTime().toString()).replace(":", "");
        QString workingPath = dir.path() + "/" + folderName;

        if (!dir.mkdir(folderName)) {
            qDebug() << "Could create folder: " << folderName;
            return;
        }

        QJsonObject obj;
        obj.insert("license", license);
        obj.insert("title", title);

        obj.insert("type", "htmlWallpaper");
        obj.insert("file", "index.html");

        QFile fileMainHTML(workingPath + "/index.html");
        if (!fileMainHTML.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qDebug() << "Could not open /index.html";
            return;
        }

        QTextStream outMainHTML(&fileMainHTML);
        outMainHTML.setCodec("UTF-8");
        outMainHTML << "<html>\n<head></head>\n"
                       "<h1>This is an empty html Wallpaper!</h1>"
                       "<body></body>\n</html>";
        fileMainHTML.close();

        QJsonArray tagsJsonArray;
        for (const QString& tmp : tags) {
            tagsJsonArray.append(tmp);
        }
        obj.insert("tags", tagsJsonArray);

        QFile file(workingPath + "/project.json");
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qDebug() << "Could not open /project.json";
            return;
        }

        QUrl previewThumbnailUrl { previewThumbnail };
        QFileInfo previewImageFile(previewThumbnailUrl.toLocalFile());

        if (!previewThumbnail.isEmpty()) {
            obj.insert("previewThumbnail", previewImageFile.fileName());
            obj.insert("preview", previewImageFile.fileName());
            if (!QFile::copy(previewThumbnailUrl.toLocalFile(), workingPath + "/" + previewImageFile.fileName())) {
                qDebug() << "Could not copy" << previewThumbnailUrl.toLocalFile() << " to " << workingPath + "/" + previewImageFile.fileName();
                return;
            }
        }

        QTextStream out(&file);
        out.setCodec("UTF-8");
        QJsonDocument doc(obj);
        out << doc.toJson();
        file.close();

        emit widgetCreatedSuccessful(workingPath);
    });
}

/*!
    Starts the process.
*/
void Create::createWallpaperStart(QString videoPath, Create::VideoCodec codec)
{
    clearFfmpegOutput();
    videoPath.remove("file:///");

    QDir dir;
    dir.cd(m_globalVariables->localStoragePath().toLocalFile());

    // Create a temp dir so we can later alter it to the workshop id
    auto folderName = QString("_tmp_" + QTime::currentTime().toString()).replace(":", "");

    if (!dir.mkdir(folderName)) {
        emit createWallpaperStateChanged(CreateImportVideo::ImportVideoState::CreateTmpFolderError);
        emit abortCreateWallpaper();
        return;
    }
    setWorkingDir(dir.path() + "/" + folderName);
    QStringList codecs;
    if (codec == Create::VideoCodec::VP8) {
        codecs.append("vp8");
    } else {
        codecs.append("vp9");
    }

    m_createImportVideoThread = new QThread();
    m_createImportVideo = new CreateImportVideo(videoPath, workingDir(), codecs);
    connect(m_createImportVideo, &CreateImportVideo::processOutput, this, [this](QString text) {
        appendFfmpegOutput(text + "\n");
    });

    connect(m_createImportVideo, &CreateImportVideo::createWallpaperStateChanged, this, &Create::createWallpaperStateChanged);
    connect(m_createImportVideo, &CreateImportVideo::progressChanged, this, &Create::setProgress);
    connect(m_createImportVideoThread, &QThread::started, m_createImportVideo, &CreateImportVideo::process);
    connect(m_createImportVideo, &CreateImportVideo::abortAndCleanup, this, &Create::abortAndCleanup);

    connect(m_createImportVideo, &CreateImportVideo::finished, m_createImportVideoThread, &QThread::quit);
    connect(m_createImportVideo, &CreateImportVideo::finished, m_createImportVideo, &QObject::deleteLater);
    connect(m_createImportVideoThread, &QThread::finished, m_createImportVideoThread, &QObject::deleteLater);

    m_createImportVideo->moveToThread(m_createImportVideoThread);
    m_createImportVideoThread->start();
}

/*!
    When converting of the wallpaper steps where successful.
*/
void Create::saveWallpaper(QString title, QString description, QString filePath, QString previewImagePath, QString youtube, Create::VideoCodec codec, QVector<QString> tags)
{
    filePath.remove("file:///");
    previewImagePath.remove("file:///");

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
    QFile file(m_workingDir + "/project.json");

    QJsonObject obj;
    obj.insert("description", description);
    obj.insert("title", title);
    obj.insert("youtube", youtube);
    if (codec == Create::VideoCodec::VP8) {
        obj.insert("videoCodec", "vp8");
    } else {
        obj.insert("videoCodec", "vp9");
    }

    QFile audioFile { m_workingDir + "/audio.mp3" };
    if (audioFile.exists() && audioFile.size() > 0) {
        obj.insert("audio", "audio.mp3");
        obj.insert("audioCodec", "mp3");
    }

    // If the input file is a webm we don't need to convert it
    if (filePath.endsWith(".webm")) {
        obj.insert("file", filePathFile.fileName());
    } else {
        obj.insert("file", filePathFile.baseName() + ".webm");
    }
    obj.insert("previewGIF", "preview.gif");
    obj.insert("previewWEBM", "preview.webm");
    if (previewImageFile.exists()) {
        obj.insert("preview", previewImageFile.fileName());
    } else {
        obj.insert("preview", "preview.jpg");
    }
    obj.insert("previewThumbnail", "previewThumbnail.jpg");
    obj.insert("type", "videoWallpaper");

    QJsonArray tagsJsonArray;
    for (const QString &tmp : tags) {
        tagsJsonArray.append(tmp);
    }
    obj.insert("tags", tagsJsonArray);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Could not open /project.json";
        emit createWallpaperStateChanged(CreateImportVideo::ImportVideoState::CreateProjectFileError);
        return;
    }

    QTextStream out(&file);
    out.setCodec("UTF-8");
    QJsonDocument doc(obj);

    out << doc.toJson();

    file.close();
    emit createWallpaperStateChanged(CreateImportVideo::ImportVideoState::CreateProjectFileFinished);
}

void Create::abortAndCleanup()
{
    qWarning() << "Abort and Cleanup!";

    if (m_createImportVideo == nullptr || m_createImportVideoThread == nullptr) {
        qDebug() << m_createImportVideo << m_createImportVideoThread;
        return;
    }

    //    disconnect(m_createImportVideo);
    //    disconnect(m_createImportVideoThread);

    // Save to export path before aborting to be able to cleanup the tmp folder
    QString tmpExportPath = m_createImportVideo->m_exportPath;

    connect(m_createImportVideoThread, &QThread::finished, this, [=]() {
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
        m_createImportVideo = nullptr;
        m_createImportVideoThread = nullptr;
    });
    m_createImportVideoThread->requestInterruption();
}
}
