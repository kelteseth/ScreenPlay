#include "createimportvideo.h"

namespace ScreenPlay {

/*!
    \class ScreenPlay::CreateImportVideo
    \inmodule ScreenPlay
    \brief This class imports (copies) and creates wallaper previews.

    This lass only exsits as long as the user creates a wallpaper and gets
    destroyed if the creation was successful or not.
    The state get propagated via createWallpaperStateChanged(ImportVideoState state);

*/

CreateImportVideo::CreateImportVideo(QObject* parent)
    : QObject(parent)
{
}

CreateImportVideo::CreateImportVideo(const QString& videoPath, const QString& exportPath, QObject* parent)
    : QObject(parent)
{
    m_videoPath = videoPath;
    m_exportPath = exportPath;
}

void CreateImportVideo::process()
{

    if (!createWallpaperInfo() || QThread::currentThread()->isInterruptionRequested()) {
        emit abortAndCleanup();
        return;
    }

    if (!createWallpaperImagePreview() || QThread::currentThread()->isInterruptionRequested()) {
        emit abortAndCleanup();
        return;
    }

    if (!createWallpaperVideoPreview() || QThread::currentThread()->isInterruptionRequested()) {
        emit abortAndCleanup();
        return;
    }

    if (!createWallpaperGifPreview() || QThread::currentThread()->isInterruptionRequested()) {
        emit abortAndCleanup();
        return;
    }

    if (!createWallpaperVideo() || QThread::currentThread()->isInterruptionRequested()) {
        emit abortAndCleanup();
        return;
    }

    // If the video has no audio we can skip the extraction
    if (!m_skipAudio) {
        if (!extractWallpaperAudio() || QThread::currentThread()->isInterruptionRequested()) {
            emit abortAndCleanup();
            return;
        }
    }

    emit createWallpaperStateChanged(ImportVideoState::Finished);
}

bool CreateImportVideo::createWallpaperInfo()
{
    // Get video info
    QStringList args;
    args.append("-print_format");
    args.append("json");
    args.append("-show_format");
    args.append("-show_streams");
    args.append(m_videoPath);
    emit processOutput("ffprobe " + Util::toString(args));
    QScopedPointer<QProcess> pro(new QProcess());
    pro->setArguments(args);

#ifdef Q_OS_WIN
    pro->setProgram(QApplication::applicationDirPath() + "/ffprobe.exe");
#endif
#ifdef Q_OS_MACOS
    pro->setProgram(QApplication::applicationDirPath() + "/ffprobe");
#endif

    pro->start();
    emit createWallpaperStateChanged(ImportVideoState::AnalyseVideo);
    while (!pro->waitForFinished(100)) {
        if (QThread::currentThread()->isInterruptionRequested()) {
            qDebug() << "Interrupt thread";
            pro->terminate();
            if (!pro->waitForFinished(1000)) {
                pro->kill();
            }
            break;
        }
        QCoreApplication::processEvents();
    }
    emit createWallpaperStateChanged(ImportVideoState::AnalyseVideoFinished);
    QJsonObject obj;
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(pro->readAll(), &err);
    if (err.error != QJsonParseError::NoError) {
        qDebug() << "Error parsing ffmpeg json output";
        emit processOutput(pro->readAll());
        emit processOutput("Error parsing ffmpeg json output");
        emit createWallpaperStateChanged(ImportVideoState::AnalyseVideoError);
        return false;
    }

    obj = doc.object();

    if (obj.empty()) {
        qDebug() << "Error! File could not be parsed.";
        emit processOutput("Error! File could not be parsed.");

        emit createWallpaperStateChanged(ImportVideoState::AnalyseVideoError);
        pro->close();
        return false;
    }

    pro->close();

    // Check for audio and video streams
    QJsonArray arrayStream = obj.value("streams").toArray();

    bool hasAudioStream { false };
    bool hasVideoStream { false };

    QJsonObject videoStream;

    for (const auto stream : arrayStream) {
        QString codec_type = stream.toObject().value("codec_type").toString();
        if (codec_type == "video") {
            videoStream = stream.toObject();
            hasVideoStream = true;
        } else if (codec_type == "audio") {
            hasAudioStream = true;
        }
    }

    // Display error if wallpaper has no video
    if (!hasVideoStream) {
        qDebug() << "Error! File has no video Stream!";
        emit processOutput("Error! File has no video Stream!");
        emit createWallpaperStateChanged(ImportVideoState::AnalyseVideoHasNoVideoStreamError);
        return false;
    }

    if (!hasAudioStream)
        m_skipAudio = true;

    // Number of frames is a string for some reason...
    bool okParseNumberOfFrames { false };
    m_numberOfFrames = videoStream.value("nb_frames").toString().toInt(&okParseNumberOfFrames);

    if (!okParseNumberOfFrames) {
        qDebug() << "Error parsing number of frames. Is this really a valid video File?";
        emit processOutput("Error parsing number of frames. Is this really a valid video File?");
        QJsonDocument tmpVideoStreamDoc(videoStream);
        emit processOutput(tmpVideoStreamDoc.toJson());
        emit createWallpaperStateChanged(ImportVideoState::AnalyseVideoError);
        return false;
    }

    QJsonObject objFormat = obj.value("format").toObject();

    // Get video length
    bool okParseDuration = false;
    float tmpLength = objFormat.value("duration").toVariant().toFloat(&okParseDuration);

    if (!okParseDuration) {
        qDebug() << "Error parsing video length. Is this really a valid video File?";
        emit processOutput("Error parsing video length. Is this really a valid video File?");
        emit createWallpaperStateChanged(ImportVideoState::AnalyseVideoError);
        return false;
    }

    int length = static_cast<int>(tmpLength);
    m_length = length;

    // Get framerate
    QJsonArray arrSteams = obj.value("streams").toArray();
    if (arrSteams.empty()) {
        qDebug() << "Error container does not have any video streams";
        emit processOutput("Error container does not have any video streams");
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
        emit processOutput("Error could not parse streams with length: " + QString::number(avgFrameRateList.length()));
        return false;
    }

    int framerate = 0;
    double value1 = avgFrameRateList.at(0).toDouble();
    double value2 = avgFrameRateList.at(1).toDouble();

    framerate = qCeil(value1 / value2);
    m_framerate = framerate;

    return true;
}

bool CreateImportVideo::createWallpaperVideoPreview()
{
    emit createWallpaperStateChanged(ImportVideoState::ConvertingPreviewVideo);

    QStringList args;
    args.append("-y");
    args.append("-stats");
    args.append("-i");
    args.append(m_videoPath);

    args.append("-vf");
    // We allways want to have a 5 second clip via 24fps -> 120 frames
    // Divided by the number of frames we can skip (timeInSeconds * Framrate)
    // scale & crop parameter: https://unix.stackexchange.com/a/284731
    args.append("select='not(mod(n," + QString::number((m_length / 5)) + "))',setpts=N/FRAME_RATE/TB,crop=in_h*16/9:in_h,scale=-2:400");
    // Disable audio
    args.append("-an");
    args.append(m_exportPath + "/preview.webm");
    emit processOutput("ffmpeg " + Util::toString(args));
    QScopedPointer<QProcess> proConvertPreviewWebM(new QProcess());

    proConvertPreviewWebM->setArguments(args);
#ifdef Q_OS_WIN
    proConvertPreviewWebM->setProgram(QApplication::applicationDirPath() + "/ffmpeg.exe");
#endif
#ifdef Q_OS_MACOS
    proConvertPreviewWebM->setProgram(QApplication::applicationDirPath() + "/ffmpeg");
#endif

    proConvertPreviewWebM->setProcessChannelMode(QProcess::MergedChannels);

    connect(proConvertPreviewWebM.data(), &QProcess::readyReadStandardOutput, this, [&]() {
        QString tmpOut = proConvertPreviewWebM->readAllStandardOutput();
        auto tmpList = tmpOut.split(QRegExp("\\s+"), QString::SkipEmptyParts);

        if (tmpList.length() > 2) {
            bool ok = false;
            float currentFrame = QString(tmpList.at(1)).toFloat(&ok);

            if (!ok)
                return;

            qDebug() << currentFrame << static_cast<float>((m_framerate * 5));
            float progress = currentFrame / static_cast<float>((m_framerate * 5));

            this->setProgress(progress);
        }
        emit processOutput(tmpOut);
    });

    proConvertPreviewWebM->start();
    while (!proConvertPreviewWebM->waitForFinished(100)) //Wake up every 100ms and check if we must exit
    {
        if (QThread::currentThread()->isInterruptionRequested()) {
            qDebug() << "Interrupt thread";
            proConvertPreviewWebM->terminate();
            if (!proConvertPreviewWebM->waitForFinished(1000)) {
                proConvertPreviewWebM->kill();
            }
            break;
        }
        QCoreApplication::processEvents();
    }

    QFile previewVideo(m_exportPath + "/preview.webm");
    if (!previewVideo.exists() || !(previewVideo.size() > 0)) {
        emit createWallpaperStateChanged(ImportVideoState::ConvertingPreviewVideoError);
        return false;
    }

    emit processOutput(proConvertPreviewWebM->readAll());
    proConvertPreviewWebM->close();

    emit createWallpaperStateChanged(ImportVideoState::ConvertingPreviewVideoFinished);

    return true;
}

bool CreateImportVideo::createWallpaperGifPreview()
{

    emit createWallpaperStateChanged(ImportVideoState::ConvertingPreviewGif);

    QStringList args;
    args.append("-y");
    args.append("-stats");
    args.append("-i");
    args.append(m_exportPath + "/preview.webm");
    args.append("-filter_complex");
    args.append("[0:v] fps=12,scale=w=480:h=-1,split [a][b];[a] palettegen=stats_mode=single [p];[b][p] paletteuse=new=1");
    args.append(m_exportPath + "/preview.gif");
    emit processOutput("ffmpeg " + Util::toString(args));
    QScopedPointer<QProcess> proConvertGif(new QProcess());
    proConvertGif->setArguments(args);
#ifdef Q_OS_WIN
    proConvertGif->setProgram(QApplication::applicationDirPath() + "/ffmpeg.exe");
#endif
#ifdef Q_OS_MACOS
    proConvertGif->setProgram(QApplication::applicationDirPath() + "/ffmpeg");
#endif

    proConvertGif->start();
    while (!proConvertGif->waitForFinished(100)) //Wake up every 100ms and check if we must exit
    {
        if (QThread::currentThread()->isInterruptionRequested()) {
            qDebug() << "Interrupt thread";
            proConvertGif->terminate();
            if (!proConvertGif->waitForFinished(1000)) {
                proConvertGif->kill();
            }
            break;
        }
        QCoreApplication::processEvents();
    }

    QString tmpErrGif = proConvertGif->readAllStandardError();
    if (!tmpErrGif.isEmpty()) {
        QFile previewGif(m_exportPath + "/preview.gif");
        if (!previewGif.exists() || !(previewGif.size() > 0)) {
            emit createWallpaperStateChanged(ImportVideoState::ConvertingPreviewGifError);
            return false;
        }
    }

    emit processOutput(proConvertGif->readAll());
    proConvertGif->close();
    emit createWallpaperStateChanged(ImportVideoState::ConvertingPreviewGifFinished);

    return true;
}

bool CreateImportVideo::createWallpaperImagePreview()
{

    emit createWallpaperStateChanged(ImportVideoState::ConvertingPreviewImage);

    QStringList args;
    args.clear();
    args.append("-y");
    args.append("-stats");
    args.append("-ss");
    args.append("00:00:02");
    args.append("-i");
    args.append(m_videoPath);
    args.append("-vframes");
    args.append("1");
    args.append("-q:v");
    args.append("2");
    args.append(m_exportPath + "/preview.jpg");

    emit processOutput("ffmpeg " + Util::toString(args));
    QScopedPointer<QProcess> proConvertImage(new QProcess());
    proConvertImage->setArguments(args);
#ifdef Q_OS_WIN
    proConvertImage->setProgram(QApplication::applicationDirPath() + "/ffmpeg.exe");
#endif

#ifdef Q_OS_MACOS
    proConvertImage->setProgram(QApplication::applicationDirPath() + "/ffmpeg");
#endif
    proConvertImage->start();

    while (!proConvertImage->waitForFinished(100)) //Wake up every 100ms and check if we must exit
    {
        if (QThread::currentThread()->isInterruptionRequested()) {
            qDebug() << "Interrupt thread";
            proConvertImage->terminate();
            if (!proConvertImage->waitForFinished(1000)) {
                proConvertImage->kill();
            }
            break;
        }
        QCoreApplication::processEvents();
    }
    QString tmpErrImg = proConvertImage->readAllStandardError();
    if (!tmpErrImg.isEmpty()) {
        QFile previewImg(m_exportPath + "/preview.jpg");
        if (!previewImg.exists() || !(previewImg.size() > 0)) {
            emit createWallpaperStateChanged(ImportVideoState::ConvertingPreviewImageError);
            return false;
        }
    }

    emit processOutput(proConvertImage->readAll());
    proConvertImage->close();
    emit createWallpaperStateChanged(ImportVideoState::ConvertingPreviewImageFinished);

    return true;
}

bool CreateImportVideo::createWallpaperVideo()
{

    if (m_videoPath.endsWith(".webm")) {
        return true;
    }

    emit createWallpaperStateChanged(ImportVideoState::ConvertingVideo);

    QStringList args;
    args.clear();
    args.append("-hide_banner");
    args.append("-y");
    args.append("-stats");
    args.append("-i");
    args.append(m_videoPath);
    args.append("-c:v");
    args.append("libvpx");
    args.append("-qmin");
    args.append("0");
    args.append("-qmax");
    args.append("50");
    args.append("-crf");
    args.append("5");
    args.append("-b:v");
    args.append("5M");

    QFileInfo file(m_videoPath);
    QString convertedFileAbsolutePath { m_exportPath + "/" + file.baseName() + ".webm" };
    args.append(convertedFileAbsolutePath);

    QScopedPointer<QProcess> proConvertVideo(new QProcess());
    proConvertVideo->setArguments(args);
    emit processOutput("ffmpeg " + Util::toString(args));

#ifdef Q_OS_WIN
    proConvertVideo->setProgram(QApplication::applicationDirPath() + "/ffmpeg.exe");
#endif

#ifdef Q_OS_MACOS
    proConvertVideo->setProgram(QApplication::applicationDirPath() + "/ffmpeg");
#endif
    proConvertVideo->setProcessChannelMode(QProcess::MergedChannels);
    connect(proConvertVideo.data(), &QProcess::errorOccurred, this, [&](QProcess::ProcessError error) {
        qDebug() << error;
    });

    connect(proConvertVideo.data(), &QProcess::readyReadStandardOutput, this, [&]() {
        QString tmpOut = proConvertVideo->readAllStandardOutput();
        if (tmpOut.contains("Conversion failed!")) {
            emit createWallpaperStateChanged(ImportVideoState::ConvertingVideoError);
        }
        auto tmpList = tmpOut.split(QRegExp("\\s+"), QString::SkipEmptyParts);

        if (tmpList.length() > 2) {
            bool ok = false;
            float currentFrame = QString(tmpList.at(1)).toFloat(&ok);

            if (!ok)
                return;

            float progress = currentFrame / m_numberOfFrames;

            this->setProgress(progress);
        }
        emit processOutput(tmpOut);
    });

    proConvertVideo->start();

    while (!proConvertVideo->waitForFinished(100)) //Wake up every 100ms and check if we must exit
    {
        if (QThread::currentThread()->isInterruptionRequested()) {
            qDebug() << "Interrupt thread";
            proConvertVideo->terminate();
            if (!proConvertVideo->waitForFinished(1000)) {
                proConvertVideo->kill();
            }
            break;
        }
        QCoreApplication::processEvents();
    }

    QFile video(convertedFileAbsolutePath);
    if (!video.exists() || !(video.size() > 0)) {
        qDebug() << convertedFileAbsolutePath << proConvertVideo->readAll() << video.exists() << video.size();
        emit createWallpaperStateChanged(ImportVideoState::ConvertingVideoError);
        return false;
    }

    proConvertVideo->close();
    emit createWallpaperStateChanged(ImportVideoState::ConvertingVideoFinished);

    return true;
}

bool CreateImportVideo::extractWallpaperAudio()
{

    emit createWallpaperStateChanged(ImportVideoState::ConvertingAudio);

    QStringList args;
    args.clear();
    args.append("-y");
    args.append("-stats");
    args.append("-i");
    args.append(m_videoPath);
    args.append("-f");
    args.append("mp3");
    args.append("-ab");
    args.append("192000");
    args.append("-vn");
    args.append(m_exportPath + "/audio.mp3");

    QScopedPointer<QProcess> proConvertAudio(new QProcess());
    proConvertAudio->setArguments(args);
#ifdef Q_OS_WIN
    proConvertAudio->setProgram(QApplication::applicationDirPath() + "/ffmpeg.exe");
#endif
#ifdef Q_OS_MACOS
    proConvertAudio->setProgram(QApplication::applicationDirPath() + "/ffmpeg");
#endif

    proConvertAudio->start(QIODevice::ReadOnly);
    while (!proConvertAudio->waitForFinished(100)) //Wake up every 100ms and check if we must exit
    {
        if (QThread::currentThread()->isInterruptionRequested()) {
            qDebug() << "Interrupt thread";
            proConvertAudio->terminate();
            if (!proConvertAudio->waitForFinished(1000)) {
                proConvertAudio->kill();
            }
            break;
        }
        QCoreApplication::processEvents();
    }

    QString tmpErrImg = proConvertAudio->readAllStandardError();
    if (!tmpErrImg.isEmpty()) {
        QFile previewImg(m_exportPath + "/audio.mp3");
        if (!previewImg.exists() || !(previewImg.size() > 0)) {
            qDebug() << args;
            qDebug() << proConvertAudio->readAll();
            emit createWallpaperStateChanged(ImportVideoState::ConvertingAudioError);
            return false;
        }
    }

    emit processOutput(proConvertAudio->readAll());
    proConvertAudio->close();
    emit createWallpaperStateChanged(ImportVideoState::ConvertingAudioFinished);

    return true;
}

}
