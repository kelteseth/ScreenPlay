#include "create.h"

Create::Create(Settings* st, QMLUtilities* util, QObject* parent)
    : QObject(parent)
{

    m_settings = st;
    m_utils = util;
    qmlRegisterType<Create>("net.aimber.create", 1, 0, "Create");
}

Create::Create()
{
}

Create::~Create()
{
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

    if(m_importState != Create::State::Idle) {
        return;
    }
    QtConcurrent::run([=]() {
        emit localWorkshopCreationStatusChanged(State::Started);

        QString fromVideoPath = QString(videoPath.toString()).replace("file:///", "");
        QString fromImagePath = QString(previewPath.toString()).replace("file:///", "");
        QString toPath = m_settings->localStoragePath().toString() + "/" + title;
        QString toPathWithVideoFile = toPath + "/" + videoPath.fileName();
        QString toPathWithImageFile = toPath + "/" + previewPath.fileName();

        if (QDir(toPath).exists()) {
            if (!QDir(toPath).isEmpty()) {
                emit localWorkshopCreationStatusChanged(State::ErrorFolder);
                return;
            } else {
                //if(!QDir(toPath + ))
            }

        } else {
            //TODO: Display Error
            if (!QDir().mkdir(toPath)) {
                emit localWorkshopCreationStatusChanged(State::ErrorFolderCreation);
                return;
            }
        }

        //Copy Video File
        if (QFile::copy(fromVideoPath, toPathWithVideoFile)) {
            emit localWorkshopCreationStatusChanged(State::CopyVideoFinished);
        } else {
            emit localWorkshopCreationStatusChanged(State::ErrorCopyVideo);
        }

        //Copy Image File
        if (QFile::copy(fromImagePath, toPathWithImageFile)) {
            emit localWorkshopCreationStatusChanged(State::CopyImageFinished);
        } else {
            emit localWorkshopCreationStatusChanged(State::ErrorCopyImage);
        }

        //Copy Project File
        QFile configFile(toPath + "/" + "project.json");

        if (!configFile.open(QIODevice::ReadWrite | QIODevice::Text)) {
            emit localWorkshopCreationStatusChanged(State::ErrorCopyConfig);
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

        emit localWorkshopCreationStatusChanged(State::Finished);
    });
}

void Create::importVideo(QString title, QUrl videoPath, int timeStamp, int videoDuration, int videoFrameRate)
{

    if(m_importState != Create::State::Idle) {
        return;
    }
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
        emit localWorkshopCreationStatusChanged(State::ConvertingPreviewImage);
        pro.data()->start();
        pro.data()->waitForFinished(-1);
        qDebug() << "Done converting video to thumbnail" << pro.data()->readAllStandardOutput();
        emit localWorkshopCreationStatusChanged(State::ConvertingPreviewImageFinished);
        pro.data()->close();
    });

    //createVideoPreview(tmpPath, ((videoDurationInSeconds * videoFrameRate) / 120));
}

void Create::createVideoPreview(QString path, int frames, int frameRate)
{

    if(m_importState != Create::State::Idle) {
        return;
    }
    qDebug() << frames << frameRate;
    QtConcurrent::run([=]() {
        QStringList args;
        args.append("-y");
        args.append("-stats");
        args.append("-i");
        args.append(path);
        args.append("-speed");
        args.append("ultrafast");
        args.append("-vf");
        // We allways want to have a 5 second clip via 24fps -> 120 frames
        // Divided by the number of frames we can skip (timeInSeconds * Framrate)
        // scale & crop parameter: https://unix.stackexchange.com/a/284731
        args.append("select='not(mod(n," + QString::number((frames / 5 )) + "))',setpts=N/FRAME_RATE/TB,crop=in_h*16/9:in_h,scale=-2:400");
        // Disable audio
        args.append("-an");
        args.append("preview.mp4");
        QScopedPointer<QProcess> pro(new QProcess());

        pro.data()->setArguments(args);
        qDebug() << "Start converting video to preview";
#ifdef Q_OS_WIN
        pro.data()->setProgram(QApplication::applicationDirPath() + "/ffmpeg.exe");
#endif
        pro.data()->start();
        pro.data()->waitForFinished(-1);
        qDebug() << pro.data()->program() << pro.data()->arguments();
        qDebug() << "Done converting video to preview" << pro.data()->readAll() << "\n"
                 << pro.data()->readAllStandardError();
        pro.data()->close();
    });
}

void Create::createWallpaper(QString videoPath)
{

    if(m_importState != Create::State::Idle) {
        return;
    }
    videoPath.remove("file:///");

    QtConcurrent::run([=]() {
        // Get video info
        QStringList args;
        args.append("-print_format");
        args.append("json");
        args.append("-show_format");
        args.append("-show_streams");
        args.append(videoPath);
        QScopedPointer<QProcess> pro(new QProcess());
        pro.data()->setArguments(args);

#ifdef Q_OS_WIN
        pro.data()->setProgram(QApplication::applicationDirPath() + "/ffprobe.exe");
#endif

        pro.data()->start();
        pro.data()->waitForFinished(-1);
        QJsonObject obj;
        QJsonParseError err;
        QJsonDocument doc = QJsonDocument::fromJson(pro.data()->readAll(), &err);
        if (err.error == QJsonParseError::NoError) {
            obj = doc.object();
        }

        pro.data()->close();

        // Get video length
        QJsonObject objFormat = obj.value("format").toObject();
        bool okParseDuration = false;
        auto tmpLength = objFormat.value("duration").toVariant().toFloat(&okParseDuration);

        if (!okParseDuration) {
            qDebug() << "Error parsing video length";
            return;
        }

        int length = 0;
        length = static_cast<int>(tmpLength);

        // Get framerate
        QJsonArray arrSteams = obj.value("streams").toArray();
        if (arrSteams.size() < 1) {
            qDebug() << "Error container does not have any video streams";
            return;
        }

        QJsonObject tmpObjStreams = arrSteams.at(0).toObject();

        // The paramter gets us the exact framerate
        // "avg_frame_rate":"47850000/797509"
        // so we need no calc the value by dividing the two numbers
        QString avgFrameRate = tmpObjStreams.value("avg_frame_rate").toVariant().toString();

        QStringList avgFrameRateList = avgFrameRate.split('/', QString::SkipEmptyParts);
        if (avgFrameRateList.length() != 2) {
            qDebug() << "Error could not parse streams with length: " << avgFrameRateList.length();
            return;
        }

        int framerate = 0;
        float value1 = static_cast<float>(avgFrameRateList.at(0).toInt());
        float value2 = static_cast<float>(avgFrameRateList.at(1).toInt());

        framerate = qCeil(value1 / value2);

        this->createVideoPreview(videoPath, length, framerate);
    });
}
