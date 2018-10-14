#include "create.h"

Create::Create(Settings* st, QMLUtilities* util, QObject* parent)
    : QObject(parent)
{

    m_settings = st;
    m_utils = util;

    qRegisterMetaType<Create::State>();
    qmlRegisterType<Create>("net.aimber.create", 1, 0, "Create");
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

void Create::createWallpaperStart(QString videoPath)
{
    videoPath.remove("file:///");

    QtConcurrent::run([=]() {
        QDir dir;
        dir.cd(this->m_settings->localStoragePath().toString());

        CreateWallpaperData createWallpaperData;
        createWallpaperData.videoPath = videoPath;

        // Create a temp dir so we can later alter it to the workshop id
        auto folderName = QString("_tmp_" + QTime::currentTime().toString()).replace(":", "");

        if (!dir.mkdir(folderName))
            return;

        createWallpaperData.exportPath = dir.path() + "/" + folderName;
        m_workingDir = createWallpaperData.exportPath;

        // If we return early/false this means the creation
        // process did not work
        // Todo: cleanup!

        if (!this->createWallpaperInfo(createWallpaperData))
            return;

        if (!this->createWallpaperVideoPreview(createWallpaperData))
            return;

        if (!this->createWallpaperVideo(createWallpaperData))
            return;

        if (!this->createWallpaperProjectFile(createWallpaperData))
            return;
    });
}

bool Create::createWallpaperInfo(CreateWallpaperData& createWallpaperData)
{
    // Get video info
    QStringList args;
    args.append("-print_format");
    args.append("json");
    args.append("-show_format");
    args.append("-show_streams");
    args.append(createWallpaperData.videoPath);
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
        return false;
    }

    int length = 0;
    length = static_cast<int>(tmpLength);
    createWallpaperData.length = length;

    // Get framerate
    QJsonArray arrSteams = obj.value("streams").toArray();
    if (arrSteams.size() < 1) {
        qDebug() << "Error container does not have any video streams";
        return false;
    }

    QJsonObject tmpObjStreams = arrSteams.at(0).toObject();

    // The paramter gets us the exact framerate
    // "avg_frame_rate":"47850000/797509"
    // so we need no calc the value by dividing the two numbers
    QString avgFrameRate = tmpObjStreams.value("avg_frame_rate").toVariant().toString();

    QStringList avgFrameRateList = avgFrameRate.split('/', QString::SkipEmptyParts);
    if (avgFrameRateList.length() != 2) {
        qDebug() << "Error could not parse streams with length: " << avgFrameRateList.length();
        return false;
    }

    int framerate = 0;
    float value1 = static_cast<float>(avgFrameRateList.at(0).toInt());
    float value2 = static_cast<float>(avgFrameRateList.at(1).toInt());

    framerate = qCeil(value1 / value2);
    createWallpaperData.framerate = framerate;

    return true;
}

bool Create::createWallpaperVideoPreview(CreateWallpaperData& createWallpaperData)
{

    qDebug() << createWallpaperData.length << createWallpaperData.framerate;
    QStringList args;
    args.append("-y");
    args.append("-stats");
    args.append("-i");
    args.append(createWallpaperData.videoPath);
    args.append("-speed");
    args.append("ultrafast");
    args.append("-vf");
    // We allways want to have a 5 second clip via 24fps -> 120 frames
    // Divided by the number of frames we can skip (timeInSeconds * Framrate)
    // scale & crop parameter: https://unix.stackexchange.com/a/284731
    args.append("select='not(mod(n," + QString::number((createWallpaperData.length / 5)) + "))',setpts=N/FRAME_RATE/TB,crop=in_h*16/9:in_h,scale=-2:400");
    // Disable audio
    args.append("-an");
    args.append(createWallpaperData.exportPath + "/preview.mp4");
    QScopedPointer<QProcess> proConvertPreviewMP4(new QProcess());

    proConvertPreviewMP4.data()->setArguments(args);
    qDebug() << "Start converting video to preview mp4";
#ifdef Q_OS_WIN
    proConvertPreviewMP4.data()->setProgram(QApplication::applicationDirPath() + "/ffmpeg.exe");
#endif
    emit createWallpaperStateChanged(Create::State::ConvertingPreviewVideo);

    proConvertPreviewMP4.data()->start();

    QScopedPointer<QTimer> processOutputTimer(new QTimer());

    //    connect(processOutputTimer.data(), &QTimer::timeout, [&]() {
    //        qDebug() << "### " << proConvertPreviewMP4.data()->readAll();
    //        this->processOutput(proConvertPreviewMP4.data()->readAll());
    //    });
    processOutputTimer.data()->start(100);
    proConvertPreviewMP4.data()->waitForFinished(-1);
    if (!proConvertPreviewMP4.data()->readAllStandardError().isEmpty()) {
        emit createWallpaperStateChanged(Create::State::ConvertingPreviewVideoError);
        return false;
    }
    //        qDebug() << proConvertPreviewMP4.data()->program() << proConvertPreviewMP4.data()->arguments();
    //        qDebug() << "Done converting video to preview" << proConvertPreviewMP4.data()->readAll() << "\n"
    //                 << proConvertPreviewMP4.data()->readAllStandardError();
    this->processOutput(proConvertPreviewMP4.data()->readAll());
    proConvertPreviewMP4.data()->close();
    processOutputTimer.data()->stop();
    emit createWallpaperStateChanged(Create::State::ConvertingPreviewVideoFinished);

    /*
     *
     * Create gif
     *
     */

    emit createWallpaperStateChanged(Create::State::ConvertingPreviewGif);
    args.clear();
    args.append("-y");
    args.append("-stats");
    args.append("-i");
    args.append(createWallpaperData.exportPath + "/preview.mp4");
    args.append("-filter_complex");
    args.append("[0:v] fps=12,scale=w=480:h=-1,split [a][b];[a] palettegen=stats_mode=single [p];[b][p] paletteuse=new=1");
    args.append(createWallpaperData.exportPath + "/preview.gif");

    QScopedPointer<QProcess> proConvertGif(new QProcess());
    proConvertGif.data()->setArguments(args);
    qDebug() << "Start converting video to preview gif";
#ifdef Q_OS_WIN
    proConvertGif.data()->setProgram(QApplication::applicationDirPath() + "/ffmpeg.exe");
#endif
    proConvertGif.data()->start();
    proConvertGif.data()->waitForFinished(-1);
    if (!proConvertGif.data()->readAllStandardError().isEmpty()) {
        emit createWallpaperStateChanged(Create::State::ConvertingPreviewGifError);
        return false;
    }

    //        qDebug() << proConvertGif.data()->program() << proConvertGif.data()->arguments();
    //        qDebug() << "Done converting video to preview" << proConvertGif.data()->readAll() << "\n"
    //                 << proConvertGif.data()->readAllStandardError();
    this->processOutput(proConvertPreviewMP4.data()->readAll());
    proConvertGif.data()->close();
    emit createWallpaperStateChanged(Create::State::ConvertingPreviewGifFinished);

    return true;
}

bool Create::createWallpaperVideo(CreateWallpaperData& createWallpaperData)
{
    return true;
}

bool Create::createWallpaperProjectFile(CreateWallpaperData& createWallpaperData)
{
    //Copy Project File
    QFile configFile(createWallpaperData.exportPath + "/project.json");

    if (!configFile.open(QIODevice::ReadWrite | QIODevice::Text)) {

        return false;
    }

    //    QTextStream out(&configFile);
    //    QJsonObject configObj;

    //    configObj.insert("file", videoPath.fileName());
    //    configObj.insert("preview", previewPath.fileName());
    //    //TODO
    //    configObj.insert("description", "");
    //    configObj.insert("title", title);

    //    QJsonDocument configJsonDocument(configObj);
    //    out << configJsonDocument.toJson();
    configFile.close();
    return true;
}
