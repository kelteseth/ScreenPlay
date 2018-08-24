#include "create.h"

Create::Create(Settings* st, QMLUtilities* util, QObject* parent)
    : QObject(parent)
{
    m_settings = st;
    m_utils = util;
    qRegisterMetaType<LocalWorkshopCreationStatus::Value>();
    qmlRegisterUncreatableMetaObject(LocalWorkshopCreationStatus::staticMetaObject, "LocalWorkshopCreationStatus", 1, 0, "LocalWorkshopCreationStatus", "Error: only enums");
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
        foreach (const QString& fileName, fileNames) {
            const QString newSrcFilePath
                = srcFilePath + QLatin1Char('/') + fileName;
            const QString newTgtFilePath
                = tgtFilePath + QLatin1Char('/') + fileName;
            if (!copyRecursively(newSrcFilePath, newTgtFilePath))
                return false;
        }
    } else {
        if (!QFile::copy(srcFilePath, tgtFilePath))
            return false;
    }
    return true;
}

void Create::importVideo(QString title, QUrl videoPath, QUrl previewPath, int videoDuration)
{
    QtConcurrent::run([=]() {

        emit localWorkshopCreationStatusChanged(LocalWorkshopCreationStatus::Value::Started);

        QString fromVideoPath = QString(videoPath.toString()).replace("file:///", "");
        QString fromImagePath = QString(previewPath.toString()).replace("file:///", "");
        QString toPath = m_settings->localStoragePath().toString() + "/" + title;
        QString toPathWithVideoFile = toPath + "/" + videoPath.fileName();
        QString toPathWithImageFile = toPath + "/" + previewPath.fileName();

        if (QDir(toPath).exists()) {
            if (!QDir(toPath).isEmpty()) {
                emit localWorkshopCreationStatusChanged(LocalWorkshopCreationStatus::Value::ErrorFolder);
                return;
            } else {
                //if(!QDir(toPath + ))
            }

        } else {
            //TODO: Display Error
            if (!QDir().mkdir(toPath)) {
                emit localWorkshopCreationStatusChanged(LocalWorkshopCreationStatus::Value::ErrorFolderCreation);
                return;
            }
        }

        //Copy Video File
        if (QFile::copy(fromVideoPath, toPathWithVideoFile)) {
            emit localWorkshopCreationStatusChanged(LocalWorkshopCreationStatus::Value::CopyVideoFinished);
        } else {
            emit localWorkshopCreationStatusChanged(LocalWorkshopCreationStatus::Value::ErrorCopyVideo);
        }

        //Copy Image File
        if (QFile::copy(fromImagePath, toPathWithImageFile)) {
            emit localWorkshopCreationStatusChanged(LocalWorkshopCreationStatus::Value::CopyImageFinished);
        } else {
            emit localWorkshopCreationStatusChanged(LocalWorkshopCreationStatus::Value::ErrorCopyImage);
        }

        //Copy Project File
        QFile configFile(toPath + "/" + "project.json");

        if (!configFile.open(QIODevice::ReadWrite | QIODevice::Text)) {
            emit localWorkshopCreationStatusChanged(LocalWorkshopCreationStatus::Value::ErrorCopyConfig);
            return;
        }

        QTextStream out(&configFile);
        QJsonObject configObj;

        configObj.insert("file", videoPath.fileName());
        configObj.insert("preview", previewPath.fileName());
        //TODO
        configObj.insert("description", "");
        configObj.insert("title", title);

        QJsonDocument configJsonDocument(configObj);
        out << configJsonDocument.toJson();
        configFile.close();

        emit localWorkshopCreationStatusChanged(LocalWorkshopCreationStatus::Value::Finished);
    });
}

void Create::importVideo(QString title, QUrl videoPath, int timeStamp, int videoDuration, int videoFrameRate)
{

    qDebug() << title << videoPath << timeStamp;

    QString tmpPath = videoPath.toString();
    tmpPath.remove("file:///");

    QString tmpTimeStamp = QString::number(timeStamp);
    QString ffmpegTimeStamp;

    QString sHours;
    QString sMinutes;
    QString sSeconds;
    int intTmpTimeStamp;

    if (tmpTimeStamp.length() > 3) {
        tmpTimeStamp.remove(tmpTimeStamp.length() - 3, 3);
        intTmpTimeStamp = tmpTimeStamp.toInt();

        const int cseconds_in_day = 86400;
        const int cseconds_in_hour = 3600;
        const int cseconds_in_minute = 60;
        const int cseconds = 1;
        int32 hours = (intTmpTimeStamp % cseconds_in_day) / cseconds_in_hour;
        int32 minutes = ((intTmpTimeStamp % cseconds_in_day) % cseconds_in_hour) / cseconds_in_minute;
        int32 seconds = (((intTmpTimeStamp % cseconds_in_day) % cseconds_in_hour) % cseconds_in_minute) / cseconds;

        if (hours < 10) {
            sHours = "0" + QString::number(hours);
        } else {
            sHours = QString::number(hours);
        }
        if (minutes < 10) {
            sMinutes = "0" + QString::number(minutes);
        } else {
            sMinutes = QString::number(minutes);
        }
        if (seconds < 10) {
            sSeconds = "0" + QString::number(seconds);
        } else {
            sSeconds = QString::number(seconds);
        }
    } else {
        ffmpegTimeStamp = "00:00:00";
    }

    QString tmpVideoDuration = QString::number(videoDuration);

    if (tmpVideoDuration.length() > 3)
        tmpVideoDuration.remove(tmpVideoDuration.length() - 3, 3);

    int videoDurationInSeconds = tmpVideoDuration.toInt();

    ffmpegTimeStamp = sHours + ":" + sMinutes + ":" + sSeconds;

    if (intTmpTimeStamp == 0 || videoFrameRate == 0)
        qWarning() << "Cannot be zero!";

    QtConcurrent::run([=]() {
        QStringList args;
        // Seek first because otherwise it will load
        // the complete video first and this will make
        // the conversion 50x slower!
        args.append("-y");
        args.append("-ss");
        args.append(ffmpegTimeStamp);
        args.append("-i");
        args.append(tmpPath);
        args.append("-speed");
        args.append("ultrafast");
        args.append("-vframes");
        args.append("1");
        args.append("preview.png");
        QScopedPointer<QProcess> pro(new QProcess());

        pro.data()->setArguments(args);
        qDebug() << "Start converting video to thumbnail";
#ifdef QT_DEBUG
        pro.data()->setProgram("C:/msys64/mingw64/bin/ffmpeg.exe");
#endif
#ifdef QT_NO_DEBUG
        pro.data()->setProgram("ffmpeg.exe");
#endif
        emit localWorkshopCreationStatusChanged(LocalWorkshopCreationStatus::ConvertingPreviewImage);
        pro.data()->start();
        pro.data()->waitForFinished(-1);
        qDebug() << "Done converting video to thumbnail" << pro.data()->readAllStandardOutput();
        emit localWorkshopCreationStatusChanged(LocalWorkshopCreationStatus::ConvertingPreviewImageFinished);
        pro.data()->close();

    });

    createVideoPreview(tmpPath,((videoDurationInSeconds * videoFrameRate) / 120));

}

void Create::createVideoPreview(QString path, int framesToSkip)
{

    QtConcurrent::run([=]() {
        QStringList args;
        args.append("-y");
        args.append("-i");
        args.append(path);
        args.append("-speed");
        args.append("ultrafast");
        args.append("-vf");
        // We allways want to have a 5 second clip via 24fps -> 120
        // Divided by the number of frames we can skip (timeInSeconds * Framrate)
        args.append("select='not(mod(n\\," + QString::number(framesToSkip) + "))',setpts=N/FRAME_RATE/TB,scale=480:-1");
        args.append("-an");
        args.append("preview.mp4");
        QScopedPointer<QProcess> pro(new QProcess());

        pro.data()->setArguments(args);
        qDebug() << "Start converting video to preview";
#ifdef QT_DEBUG
        pro.data()->setProgram("C:/msys64/mingw64/bin/ffmpeg.exe");
#else
        pro.data()->setProgram("ffmpeg.exe");
#endif
        emit localWorkshopCreationStatusChanged(LocalWorkshopCreationStatus::ConvertingPreviewVideo);
        pro.data()->start();
        pro.data()->waitForFinished(-1);
        qDebug() << "Done converting video to preview" << pro.data()->readAllStandardOutput();
        emit localWorkshopCreationStatusChanged(LocalWorkshopCreationStatus::ConvertingPreviewVideoFinished);
        pro.data()->close();

    });
}
