#include "create.h"

Create::Create(Settings* st, QMLUtilities* util, QObject* parent)
    : QObject(parent)
{

    m_settings = st;
    m_utils = util;

    qRegisterMetaType<CreateImportVideo::State>();
    qmlRegisterType<Create>("net.aimber.create", 1, 0, "Create");
    qmlRegisterType<CreateImportVideo>("net.aimber.create", 1, 0, "CreateImportVideo");
}

void Create::copyProject(QString relativeProjectPath, QString toPath)
{
    if (toPath.contains("file:///")) {
        toPath.remove("file:///");
    }
    QString srcFilePath = m_settings->getScreenPlayBasePath().toString() + relativeProjectPath;

    // Todo: UI Error handling
    if (copyRecursively(srcFilePath, toPath)) {
        emit m_utils->openFolderInExplorer(toPath);
    }
}

bool Create::copyRecursively(const QString& srcFilePath, const QString& tgtFilePath)
{
    QFileInfo srcFileInfo(srcFilePath);
    if (srcFileInfo.isDir()) {
        QDir targetDir(tgtFilePath);
        targetDir.cdUp();
        QDir sourceDir(srcFilePath);
        QStringList fileNames = sourceDir.entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden | QDir::System);
        Q_FOREACH (const QString& fileName, fileNames) {
            const QString newSrcFilePath
                = srcFilePath + QLatin1Char('/') + fileName;
            const QString newTgtFilePath
                = tgtFilePath + QLatin1Char('/') + fileName;
            if (!copyRecursively(newSrcFilePath, newTgtFilePath)) {
                return false;
            }
        }
    } else {
        if (!QFile::copy(srcFilePath, tgtFilePath)) {
            return false;
        }
    }
    return true;
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

    m_createImportVideoThread = new QThread;
    m_createImportVideo = new CreateImportVideo(videoPath, workingDir());

    connect(m_createImportVideo, &CreateImportVideo::createWallpaperStateChanged, this, &Create::createWallpaperStateChanged);
    connect(m_createImportVideoThread, &QThread::started, m_createImportVideo, &CreateImportVideo::process);
    connect(m_createImportVideo, &CreateImportVideo::finished, m_createImportVideoThread, &QThread::quit);
    connect(m_createImportVideo, &CreateImportVideo::finished, m_createImportVideo, &QObject::deleteLater);
    connect(m_createImportVideoThread, &QThread::finished, m_createImportVideoThread, &QObject::deleteLater);

    m_createImportVideo->moveToThread(m_createImportVideoThread);
    m_createImportVideoThread->start();
}

void Create::saveWallpaper(QString title, QString description, QString youtube, QVector<QString> tags)
{
    qDebug() << tags;

    QFile file(m_workingDir + "/project.json");

    QJsonObject obj;
    obj.insert("description", description);
    obj.insert("title", title);
    obj.insert("file", "video.webm");
    obj.insert("previewGIF", "preview.gif");
    obj.insert("previewWEBM", "preview.webm");
    obj.insert("type", "video");

    QJsonArray arr;
    for (QString tmp : tags) {
        arr.append(tmp);
    }
    obj.insert("tags", arr);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Could not open /project.json";
        return;
    }

    QTextStream out(&file);
    QJsonDocument doc(obj);

    out << doc.toJson();

    file.close();
}

void Create::abortAndCleanup()
{
    if (m_createImportVideo == nullptr || m_createImportVideoThread == nullptr) {
        qDebug() << m_createImportVideo << m_createImportVideoThread;
        return;
    }

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
