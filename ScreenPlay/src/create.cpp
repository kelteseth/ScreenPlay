#include "create.h"

/*!
    \class Create
    \since 1.0
    \brief  Baseclass for creating wallapers, widgets and the corresponding
            wizards. As for this writing (April 2019) it is solely used to
            import webm wallpaper and create the gif/web 5 second previews.
    \todo
            - This class would need to be refactored to be used in more creation types.
*/
namespace ScreenPlay {
Create::Create(const shared_ptr<Settings>& settings, QObject* parent)
    : QObject(parent)
    , m_settings(settings)

{
    qRegisterMetaType<CreateImportVideo::ImportVideoState>("CreateImportVideo::ImportVideoState");
    qmlRegisterUncreatableType<CreateImportVideo>("ScreenPlay.Create", 1, 0, "CreateImportVideo", "Error only for enums");
    qmlRegisterType<Create>("ScreenPlay.Create", 1, 0, "Create");
}

// Constructor for the QMLEngine
Create::Create()
    : QObject(nullptr)
    , m_settings(nullptr)
{
    qRegisterMetaType<CreateImportVideo::ImportVideoState>("CreateImportVideo::ImportVideoState");
    qmlRegisterUncreatableType<ScreenPlay::CreateImportVideo>("ScreenPlay.Create", 1, 0, "CreateImportVideo", "Error only for enums");
    qmlRegisterType<Create>("ScreenPlay.Create", 1, 0, "Create");
}

void Create::createWallpaperStart(QString videoPath)
{
    videoPath.remove("file:///");

    QDir dir;
    dir.cd(m_settings->localStoragePath().toLocalFile());

    // Create a temp dir so we can later alter it to the workshop id
    auto folderName = QString("_tmp_" + QTime::currentTime().toString()).replace(":", "");

    if (!dir.mkdir(folderName)) {
        emit createWallpaperStateChanged(CreateImportVideo::ImportVideoState::CreateTmpFolderError);
        emit abortCreateWallpaper();
        return;
    }
    setWorkingDir(dir.path() + "/" + folderName);

    m_createImportVideoThread = new QThread();
    m_createImportVideo = new CreateImportVideo(videoPath, workingDir());

    connect(m_createImportVideo, &CreateImportVideo::createWallpaperStateChanged, this, &Create::createWallpaperStateChanged);
    connect(m_createImportVideoThread, &QThread::started, m_createImportVideo, &CreateImportVideo::process);
    connect(m_createImportVideo, &CreateImportVideo::canceled, this, &Create::abortAndCleanup);
    connect(m_createImportVideo, &CreateImportVideo::createWallpaperStateChanged, this, [](CreateImportVideo::ImportVideoState state) {
        qDebug() << state;
    });
    connect(m_createImportVideo, &CreateImportVideo::finished, m_createImportVideoThread, &QThread::quit);
    connect(m_createImportVideo, &CreateImportVideo::finished, m_createImportVideo, &QObject::deleteLater);
    connect(m_createImportVideoThread, &QThread::finished, m_createImportVideoThread, &QObject::deleteLater);

    m_createImportVideo->moveToThread(m_createImportVideoThread);
    m_createImportVideoThread->start();
}

void Create::saveWallpaper(QString title, QString description, QString filePath, QString previewImagePath, QString youtube, QVector<QString> tags)
{
    filePath.remove("file:///");
    previewImagePath.remove("file:///");

    emit createWallpaperStateChanged(CreateImportVideo::ImportVideoState::CopyFiles);

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
    obj.insert("type", "videoWallpaper");

    QJsonArray tagsJsonArray;
    for (QString tmp : tags) {
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

    disconnect(m_createImportVideo);
    disconnect(m_createImportVideoThread);

    // Save to export path before aborting to be able to cleanup the tmp folder
    QString tmpExportPath = m_createImportVideo->m_exportPath;

    connect(m_createImportVideoThread, &QThread::destroyed, this, [=]() {
        QDir exportPath(tmpExportPath);

        if (exportPath.exists()) {
            if (!exportPath.removeRecursively()) {
                emit createWallpaperStateChanged(CreateImportVideo::ImportVideoState::AbortCleanupError);
                qWarning() << "Could not delete temp exportPath: " << exportPath;
            } else {
                qDebug() << "cleanup " << tmpExportPath;
            }
        }
        m_createImportVideo = nullptr;
        m_createImportVideoThread = nullptr;
    });
    m_createImportVideoThread->requestInterruption();
}
}
