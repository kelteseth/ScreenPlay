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

Create::Create(const shared_ptr<Settings>& settings, QObject* parent)
    : QObject(parent)
    , m_settings(settings)

{
    qRegisterMetaType<CreateImportVideo::State>();
    qmlRegisterType<Create>("ScreenPlay.Create", 1, 0, "Create");
    qmlRegisterType<CreateImportVideo>("ScreenPlay.Create", 1, 0, "CreateImportVideo");
}

// Constructor for the QMLEngine
Create::Create()
    : QObject(nullptr)
    , m_settings(nullptr)
{
    qRegisterMetaType<CreateImportVideo::State>();
    qmlRegisterType<Create>("ScreenPlay.Create", 1, 0, "Create");
    qmlRegisterType<CreateImportVideo>("ScreenPlay.Create", 1, 0, "CreateImportVideo");
}

void Create::createWallpaperStart(QString videoPath)
{
    videoPath.remove("file:///");

    QDir dir;
    dir.cd(m_settings->localStoragePath().toLocalFile());

    // Create a temp dir so we can later alter it to the workshop id
    auto folderName = QString("_tmp_" + QTime::currentTime().toString()).replace(":", "");

    if (!dir.mkdir(folderName)) {
        emit createWallpaperStateChanged(CreateImportVideo::State::CreateTmpFolderError);
        emit abortCreateWallpaper();
        return;
    }
    setWorkingDir(dir.path() + "/" + folderName);

    m_createImportVideoThread = new QThread();
    m_createImportVideo = new CreateImportVideo(videoPath, workingDir());

    connect(m_createImportVideo, &CreateImportVideo::createWallpaperStateChanged, this, &Create::createWallpaperStateChanged);
    connect(m_createImportVideoThread, &QThread::started, m_createImportVideo, &CreateImportVideo::process);
    connect(m_createImportVideo, &CreateImportVideo::canceled, this, &Create::abortAndCleanup);
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

    emit createWallpaperStateChanged(CreateImportVideo::State::CopyFiles);

    QFileInfo previewImageFile(previewImagePath);
    if (!QFile::copy(previewImagePath, m_workingDir + "/" + previewImageFile.fileName())) {
        qDebug() << "Could not copy" << previewImagePath << " to " << m_workingDir + "/" + previewImageFile.fileName();
        emit createWallpaperStateChanged(CreateImportVideo::State::CopyFilesError);
        return;
    }

    QFileInfo filePathFile(filePath);
    if (!QFile::copy(filePath, m_workingDir + "/" + filePathFile.fileName())) {
        qDebug() << "Could not copy" << filePath << " to " << m_workingDir + "/" + filePathFile.fileName();
        emit createWallpaperStateChanged(CreateImportVideo::State::CopyFilesError);
        return;
    }
    emit createWallpaperStateChanged(CreateImportVideo::State::CopyFilesFinished);
    emit createWallpaperStateChanged(CreateImportVideo::State::CreateProjectFile);
    QFile file(m_workingDir + "/project.json");

    QJsonObject obj;
    obj.insert("description", description);
    obj.insert("title", title);
    obj.insert("youtube", youtube);
    obj.insert("file", "video.webm");
    obj.insert("previewGIF", "preview.gif");
    obj.insert("previewWEBM", filePathFile.fileName());
    obj.insert("preview", previewImageFile.fileName());
    obj.insert("type", "video");

    QJsonArray arr;
    for (QString tmp : tags) {
        arr.append(tmp);
    }
    obj.insert("tags", arr);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Could not open /project.json";
        emit createWallpaperStateChanged(CreateImportVideo::State::CreateProjectFileError);
        return;
    }

    QTextStream out(&file);
    QJsonDocument doc(obj);

    out << doc.toJson();

    file.close();
    emit createWallpaperStateChanged(CreateImportVideo::State::CreateProjectFileFinished);
}

void Create::abortAndCleanup()
{
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
                emit createWallpaperStateChanged(CreateImportVideo::State::AbortCleanupError);
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
