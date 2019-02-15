#include "create.h"

Create::Create(Settings* st, QMLUtilities* util, QObject* parent)
    : QObject(parent)
{

    m_settings = st;
    m_utils = util;
    m_futureWatcher.setFuture(m_future);

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

    m_future = QtConcurrent::run([=]() {
        QDir dir;
        dir.cd(this->m_settings->localStoragePath().toLocalFile());

        m_createWallpaperData.videoPath = videoPath;

        // Create a temp dir so we can later alter it to the workshop id
        auto folderName = QString("_tmp_" + QTime::currentTime().toString()).replace(":", "");

        if (!dir.mkdir(folderName))
            return;

        m_createWallpaperData.exportPath = dir.path() + "/" + folderName;
        this->setWorkingDir(m_createWallpaperData.exportPath);

        // If we return early/false this means the creation
        // process did not work
        bool ok = true;

        if (!this->createWallpaperInfo())
            ok = false;

        if (ok && !this->createWallpaperVideoPreview())
            ok = false;

        if (ok && !this->createWallpaperGifPreview())
            ok = false;

        if (ok && !this->extractWallpaperAudio())
            ok = false;

        if (!ok)
            abortAndCleanup();
    });
}

bool Create::createWallpaperInfo()
{
    // Get video info
    QStringList args;
    args.append("-print_format");
    args.append("json");
    args.append("-show_format");
    args.append("-show_streams");
    args.append(m_createWallpaperData.videoPath);
    QScopedPointer<QProcess> pro(new QProcess());
    pro.data()->setArguments(args);

#ifdef Q_OS_WIN
    pro.data()->setProgram(QApplication::applicationDirPath() + "/ffprobe.exe");
#endif
#ifdef Q_OS_MACOS
    pro.data()->setProgram(QApplication::applicationDirPath() + "/ffprobe");
#endif

    pro.data()->start();
    emit createWallpaperStateChanged(Create::State::AnalyseVideo);
    pro.data()->waitForFinished(-1);
    emit createWallpaperStateChanged(Create::State::AnalyseVideoFinished);
    QJsonObject obj;
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(pro.data()->readAll(), &err);
    if (err.error != QJsonParseError::NoError) {
        emit processOutput("Error parsing ffmpeg json output");
        emit createWallpaperStateChanged(Create::State::AnalyseVideoError);
        return false;
    }

    obj = doc.object();
    pro.data()->close();

    // Get video length
    QJsonObject objFormat = obj.value("format").toObject();
    bool okParseDuration = false;
    auto tmpLength = objFormat.value("duration").toVariant().toFloat(&okParseDuration);

    if (!okParseDuration) {
        qDebug() << "Error parsing video length";
        emit processOutput("Error parsing video length");
        emit createWallpaperStateChanged(Create::State::AnalyseVideoError);
        return false;
    }

    int length = 0;
    length = static_cast<int>(tmpLength);
    m_createWallpaperData.length = length;

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
    m_createWallpaperData.framerate = framerate;

    return true;
}

bool Create::createWallpaperVideoPreview()
{

    QStringList args;
    args.append("-loglevel");
    args.append("error");
    args.append("-y");
    args.append("-stats");
    args.append("-i");
    args.append(m_createWallpaperData.videoPath);
    args.append("-speed");
    args.append("ultrafast");
    args.append("-vf");
    // We allways want to have a 5 second clip via 24fps -> 120 frames
    // Divided by the number of frames we can skip (timeInSeconds * Framrate)
    // scale & crop parameter: https://unix.stackexchange.com/a/284731
    args.append("select='not(mod(n," + QString::number((m_createWallpaperData.length / 5)) + "))',setpts=N/FRAME_RATE/TB,crop=in_h*16/9:in_h,scale=-2:400");
    // Disable audio
    args.append("-an");
    args.append(m_createWallpaperData.exportPath + "/preview.webm");
    QScopedPointer<QProcess> proConvertPreviewWebM(new QProcess());

    proConvertPreviewWebM.data()->setArguments(args);
#ifdef Q_OS_WIN
    proConvertPreviewWebM.data()->setProgram(QApplication::applicationDirPath() + "/ffmpeg.exe");
#endif
#ifdef Q_OS_MACOS
    proConvertPreviewMP4.data()->setProgram(QApplication::applicationDirPath() + "/ffmpeg");
#endif
    emit createWallpaperStateChanged(Create::State::ConvertingPreviewVideo);

    connect(this, &Create::abortCreateWallpaper, proConvertPreviewWebM.data(), &QProcess::kill);
    proConvertPreviewWebM.data()->start();
    while (!proConvertPreviewWebM.data()->waitForFinished(100)) //Wake up every 100ms and check if we must exit
    {
        QCoreApplication::processEvents();
    }
    disconnect(this, &Create::abortCreateWallpaper, proConvertPreviewWebM.data(), &QProcess::kill);
    QString tmpErr = proConvertPreviewWebM.data()->readAllStandardError();
    if (!tmpErr.isEmpty()) {
        QFile previewVideo(m_createWallpaperData.exportPath + "/preview.webm");
        if (!previewVideo.exists() && !(previewVideo.size() > 0)) {
            emit processOutput(tmpErr);
            emit createWallpaperStateChanged(Create::State::ConvertingPreviewVideoError);
            return false;
        }
    }

    this->processOutput(proConvertPreviewWebM.data()->readAll());
    proConvertPreviewWebM.data()->close();

    emit createWallpaperStateChanged(Create::State::ConvertingPreviewVideoFinished);

    return true;
}

bool Create::createWallpaperGifPreview()
{

    emit createWallpaperStateChanged(Create::State::ConvertingPreviewGif);

    QStringList args;
    args.append("-y");
    args.append("-stats");
    args.append("-i");
    args.append(m_createWallpaperData.exportPath + "/preview.webm");
    args.append("-filter_complex");
    args.append("[0:v] fps=12,scale=w=480:h=-1,split [a][b];[a] palettegen=stats_mode=single [p];[b][p] paletteuse=new=1");
    args.append(m_createWallpaperData.exportPath + "/preview.gif");

    QScopedPointer<QProcess> proConvertGif(new QProcess());
    proConvertGif.data()->setArguments(args);
#ifdef Q_OS_WIN
    proConvertGif.data()->setProgram(QApplication::applicationDirPath() + "/ffmpeg.exe");
#endif
#ifdef Q_OS_MACOS
    proConvertGif.data()->setProgram(QApplication::applicationDirPath() + "/ffmpeg");
#endif
    connect(this, &Create::abortCreateWallpaper, proConvertGif.data(), &QProcess::kill);
    proConvertGif.data()->start();
    while (!proConvertGif.data()->waitForFinished(100)) //Wake up every 100ms and check if we must exit
    {
        QCoreApplication::processEvents();
    }
    disconnect(this, &Create::abortCreateWallpaper, proConvertGif.data(), &QProcess::kill);
    QString tmpErrGif = proConvertGif.data()->readAllStandardError();
    if (!tmpErrGif.isEmpty()) {
        QFile previewGif(m_createWallpaperData.exportPath + "/preview.gif");
        if (!previewGif.exists() && !(previewGif.size() > 0)) {
            emit createWallpaperStateChanged(Create::State::ConvertingPreviewGifError);
            return false;
        }
    }

    this->processOutput(proConvertGif.data()->readAll());
    proConvertGif.data()->close();
    emit createWallpaperStateChanged(Create::State::ConvertingPreviewGifFinished);

    return true;
}

bool Create::createWallpaperImagePreview()
{

    emit createWallpaperStateChanged(Create::State::ConvertingPreviewImage);

    QStringList args;
    args.clear();
    args.append("-y");
    args.append("-stats");
    args.append("-ss");
    args.append("00:00:02");
    args.append("-i");
    args.append(m_createWallpaperData.videoPath);
    args.append("-vframes");
    args.append("1");
    args.append("-q:v");
    args.append("2");
    args.append(m_createWallpaperData.exportPath + "/preview.png");

    QScopedPointer<QProcess> proConvertImage(new QProcess());
    proConvertImage.data()->setArguments(args);
#ifdef Q_OS_WIN
    proConvertImage.data()->setProgram(QApplication::applicationDirPath() + "/ffmpeg.exe");
#endif

#ifdef Q_OS_MACOS
    proConvertImage.data()->setProgram(QApplication::applicationDirPath() + "/ffmpeg");
#endif
    proConvertImage.data()->start();

    while (!proConvertImage.data()->waitForFinished(100)) //Wake up every 100ms and check if we must exit
    {
        QCoreApplication::processEvents();
    }
    QString tmpErrImg = proConvertImage.data()->readAllStandardError();
    if (!tmpErrImg.isEmpty()) {
        QFile previewImg(m_createWallpaperData.exportPath + "/preview.png");
        if (!previewImg.exists() && !(previewImg.size() > 0)) {
            emit createWallpaperStateChanged(Create::State::ConvertingPreviewImageError);
            return false;
        }
    }

    this->processOutput(proConvertImage.data()->readAll());
    proConvertImage.data()->close();
    emit createWallpaperStateChanged(Create::State::ConvertingPreviewImageFinished);

    return true;
}

bool Create::extractWallpaperAudio()
{

    emit createWallpaperStateChanged(Create::State::ConvertingAudio);

    QStringList args;
    args.clear();
    args.append("-y");
    args.append("-stats");
    args.append("-i");
    args.append(m_createWallpaperData.videoPath);
    args.append("-f");
    args.append("mp3");
    args.append("-ab");
    args.append("192000");
    args.append("-vn");
    args.append(m_createWallpaperData.exportPath + "/audio.mp3");

    QScopedPointer<QProcess> proConvertAudio(new QProcess());
    proConvertAudio.data()->setArguments(args);
#ifdef Q_OS_WIN
    proConvertAudio.data()->setProgram(QApplication::applicationDirPath() + "/ffmpeg.exe");
#endif
#ifdef Q_OS_MACOS
    pro.data()->setProgram(QApplication::applicationDirPath() + "/ffmpeg");
#endif

    connect(this, &Create::abortCreateWallpaper, proConvertAudio.data(), &QProcess::kill);
    proConvertAudio.data()->start(QIODevice::ReadOnly);
    while (!proConvertAudio.data()->waitForFinished(100)) //Wake up every 100ms and check if we must exit
    {
        QCoreApplication::processEvents();
    }
    disconnect(this, &Create::abortCreateWallpaper, proConvertAudio.data(), &QProcess::kill);

    QString tmpErrImg = proConvertAudio.data()->readAllStandardError();
    if (!tmpErrImg.isEmpty()) {
        QFile previewImg(m_createWallpaperData.exportPath + "/audio.mp3");
        if (!previewImg.exists() && !(previewImg.size() > 0)) {
            emit createWallpaperStateChanged(Create::State::ConvertingAudioError);
            return false;
        }
    }

    this->processOutput(proConvertAudio.data()->readAll());
    proConvertAudio.data()->close();
    emit createWallpaperStateChanged(Create::State::ConvertingAudioFinished);

    return true;
}

bool Create::createWallpaperProjectFile(const QString title, const QString description)
{
    //Copy Project File
    QFile configFile(m_createWallpaperData.exportPath + "/project.json");

    if (!configFile.open(QIODevice::ReadWrite | QIODevice::Text)) {
        return false;
    }

    QTextStream out(&configFile);
    QJsonObject configObj;

    configObj.insert("description", description);
    configObj.insert("title", title);

    configObj.insert("file", "video.webm");
    configObj.insert("preview", "preview.png");
    configObj.insert("previewGIF", "preview.gif");
    configObj.insert("previewWebM", "preview.webm");
    configObj.insert("type", "video");

    QJsonDocument configJsonDocument(configObj);
    out << configJsonDocument.toJson();
    configFile.close();

    emit createWallpaperStateChanged(Create::State::Finished);

    return true;
}

void Create::abortAndCleanup()
{
    emit abortCreateWallpaper();
    m_futureWatcher.cancel();

    QDir exportPath(m_createWallpaperData.exportPath);

    if (exportPath.exists()) {
        if (!exportPath.removeRecursively()) {
            emit createWallpaperStateChanged(Create::State::AbortCleanupError);
            qWarning() << "Could not delete temp exportPath: " << exportPath;
        } else {
            qDebug() << "cleanup " << m_createWallpaperData.exportPath;
        }
    }
}
