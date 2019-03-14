#include "create.h"

Create::Create(Settings* st, QMLUtilities* util, QObject* parent)
    : QObject(parent)
{

    m_settings = st;
    m_utils = util;

    qRegisterMetaType<CreateImportVideo::State>();
    qmlRegisterType<Create>("net.aimber.create", 1, 0, "Create");
    qmlRegisterType<CreateImportVideo>("net.aimber.create", 1, 0, "Create");
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
    dir.cd(this->m_settings->localStoragePath().toLocalFile());

    m_createWallpaperData.videoPath = videoPath;

    // Create a temp dir so we can later alter it to the workshop id
    auto folderName = QString("_tmp_" + QTime::currentTime().toString()).replace(":", "");

    if (!dir.mkdir(folderName)) {
        emit abortCreateWallpaper();
        return;
    }

    m_createWallpaperData.exportPath = dir.path() + "/" + folderName;
    setWorkingDir(m_createWallpaperData.exportPath);

    m_createImportVideoThread = new QThread;
    m_createImportVideo = new CreateImportVideo(videoPath, m_createWallpaperData.exportPath);
    //connect(m_createImportVideo, SIGNAL(error(QString)), this, SLOT(errorString(QString)));
    connect(m_createImportVideoThread, &QThread::started, m_createImportVideo, &CreateImportVideo::process);
    connect(m_createImportVideo, &CreateImportVideo::finished, m_createImportVideoThread, &QThread::quit);
    connect(m_createImportVideo, &CreateImportVideo::finished, m_createImportVideo, &QObject::deleteLater);
    connect(m_createImportVideoThread, &QThread::finished, m_createImportVideoThread, &QObject::deleteLater);
    connect(m_createImportVideoThread, &QThread::destroyed, this, [this]() {
        QDir exportPath(m_createWallpaperData.exportPath);

        if (exportPath.exists()) {
            if (!exportPath.removeRecursively()) {
                emit createWallpaperStateChanged(CreateImportVideo::State::AbortCleanupError);
                qWarning() << "Could not delete temp exportPath: " << exportPath;
            } else {
                qDebug() << "cleanup " << m_createWallpaperData.exportPath;
            }
        }

    });
    m_createImportVideo->moveToThread(m_createImportVideoThread);
    m_createImportVideoThread->start();
}

void Create::abortAndCleanup()
{
    m_createImportVideoThread->requestInterruption();
}
