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

/*!
 This constructor is only needed for calling  qRegisterMetaType on CreateImportVideo to register the enums.
 \code
 qRegisterMetaType<CreateImportVideo::ImportVideoState>("CreateImportVideo::ImportVideoState");
 \endcode
 */
CreateImportVideo::CreateImportVideo(QObject* parent)
    : QObject(parent)
{
}

/*!
  Creates a CreateImportVideo object to be used in a different thread. A \a videoPath and a \a exportPath are
  needed for convertion.
*/
CreateImportVideo::CreateImportVideo(const QString& videoPath, const QString& exportPath, QObject* parent)
    : QObject(parent)
{
    m_videoPath = videoPath;
    m_exportPath = exportPath;

#ifdef Q_OS_WIN
    m_ffprobeExecutable = QApplication::applicationDirPath() + "/ffprobe.exe";
#endif
#if defined(Q_OS_MACOS) || defined(Q_OS_UNIX)
    m_ffprobeExecutable = QApplication::applicationDirPath() + "/ffprobe";
#endif

#ifdef Q_OS_WIN
    m_ffmpegExecutable = QApplication::applicationDirPath() + "/ffmpeg.exe";
#endif
#if defined(Q_OS_MACOS) || defined(Q_OS_UNIX)
    m_ffmpegExecutable = QApplication::applicationDirPath() + "/ffmpeg";
#endif

}

/*!
  Processes the multiple steps of creating a wallpaper.
  \list 1
    \li createWallpaperInfo()
    \li createWallpaperImagePreview()
    \li createWallpaperVideoPreview()
    \li createWallpaperGifPreview()
    \li createWallpaperVideo() - skiped if already a webm
    \li extractWallpaperAudio() - skiped if no audio
    \li emit createWallpaperStateChanged(ImportVideoState::Finished);
  \endlist
*/
void CreateImportVideo::process()
{

    if (!createWallpaperInfo() || QThread::currentThread()->isInterruptionRequested()) {
        emit abortAndCleanup();
        return;
    }

    if (!createWallpaperImageThumbnailPreview() || QThread::currentThread()->isInterruptionRequested()) {
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

/*!
  Starts ffprobe and tries to parse the resulting json.
  Returns \c false if :
  \list
    \li Parsing the output json of ffprobe fails.
    \li Has no video.
    \li Cannot parse number of frames.
    \li Is a wrong file format or generally broken.
   \endlist
 */
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

    auto pro = std::make_shared<QProcess>();
    pro->setArguments(args);
    pro->setProgram(m_ffprobeExecutable);
    pro->start();

    emit createWallpaperStateChanged(ImportVideoState::AnalyseVideo);

    waitForFinished(pro);

    emit createWallpaperStateChanged(ImportVideoState::AnalyseVideoFinished);
    auto a = pro->readAll();
    auto objOptional = Util::parseQByteArrayToQJsonObject(a);
    if (!objOptional) {
        qDebug() << "Error parsing ffmpeg json output";
        emit processOutput(pro->readAll());
        emit processOutput("Error parsing ffmpeg json output");
        emit createWallpaperStateChanged(ImportVideoState::AnalyseVideoError);
        return false;
    }

    QJsonObject obj = objOptional.value();
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

/*!
  Starts ffmpeg and tries to covert the given video to a five second preview.
  \code
    //[...]
    args.append("-vf");
    // We allways want to have a 5 second clip via 24fps -> 120 frames
    // Divided by the number of frames we can skip (timeInSeconds * Framrate)
    // scale & crop parameter: https://unix.stackexchange.com/a/284731
    args.append("select='not(mod(n," + QString::number((m_length / 5)) + "))',setpts=N/FRAME_RATE/TB,crop=in_h*16/9:in_h,scale=-2:400");
    // Disable audio
    args.append("-an");
    args.append(m_exportPath + "/preview.webm");
  \endcode
  Returns \c false if :
  \list
    \li Cannot convert the video
    \li Generally broken.
   \endlist
 */
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
    auto proConvertPreviewWebM = std::make_shared<QProcess>();

    proConvertPreviewWebM->setArguments(args);
    proConvertPreviewWebM->setProgram(m_ffmpegExecutable);
    proConvertPreviewWebM->setProcessChannelMode(QProcess::MergedChannels);

    connect(proConvertPreviewWebM.get(), &QProcess::readyReadStandardOutput, this, [&]() {
        QString tmpOut = proConvertPreviewWebM->readAllStandardOutput();
        auto tmpList = tmpOut.split(QRegExp("\\s+"), QString::SkipEmptyParts);

        if (tmpList.length() > 2) {
            bool ok = false;
            float currentFrame = QString(tmpList.at(1)).toFloat(&ok);

            if (!ok)
                return;

            float progress = currentFrame / static_cast<float>((m_framerate * 5));

            this->setProgress(progress);
        }
        emit processOutput(tmpOut);
    });

    proConvertPreviewWebM->start();
    waitForFinished(proConvertPreviewWebM);

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

/*!
  Starts ffmpeg and tries to covert the given video to a 5 second preview gif.
  \code
    //[...]
    args.append("-filter_complex");
    args.append("[0:v] fps=12,scale=w=480:h=-1,split [a][b];[a] palettegen=stats_mode=single [p];[b][p] paletteuse=new=1");
    args.append(m_exportPath + "/preview.gif");
  \endcode
  Returns \c false if :
  \list
    \li Cannot convert the video
    \li Generally broken.
   \endlist
 */
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
    auto proConvertGif = std::make_shared<QProcess>();

    proConvertGif->setArguments(args);
    proConvertGif->setProgram(m_ffmpegExecutable);
    proConvertGif->start();
    waitForFinished(proConvertGif);

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

/*!
  Starts ffmpeg and tries to covert the given video to a image preview.
  Returns \c false if :
  \list
    \li Cannot convert the video
    \li Generally broken.
   \endlist
 */
bool CreateImportVideo::createWallpaperImageThumbnailPreview()
{

    emit createWallpaperStateChanged(ImportVideoState::ConvertingPreviewImageThumbnail);

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
    args.append("-vf");
    args.append("scale=320:-1");
    args.append(m_exportPath + "/previewThumbnail.jpg");

    emit processOutput("ffmpeg " + Util::toString(args));
    auto proConvertImage = std::make_shared<QProcess>();
    proConvertImage->setArguments(args);
    proConvertImage->setProgram(m_ffmpegExecutable);
    proConvertImage->start();

    waitForFinished(proConvertImage);

    QString tmpErrImg = proConvertImage->readAllStandardError();
    if (!tmpErrImg.isEmpty()) {
        QFile previewImg(m_exportPath + "/previewThumbnail.jpg");
        if (!previewImg.exists() || !(previewImg.size() > 0)) {
            emit createWallpaperStateChanged(ImportVideoState::ConvertingPreviewImageThumbnailError);
            return false;
        }
    }

    emit processOutput(proConvertImage->readAll());
    proConvertImage->close();
    emit createWallpaperStateChanged(ImportVideoState::ConvertingPreviewImageThumbnailFinished);

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
    auto proConvertImage = std::make_shared<QProcess>();
    proConvertImage->setArguments(args);
#ifdef Q_OS_WIN
    proConvertImage->setProgram(QApplication::applicationDirPath() + "/ffmpeg.exe");
#endif
#ifdef defined(Q_OS_MACOS) || defined(Q_OS_UNIX)
    proConvertImage->setProgram(QApplication::applicationDirPath() + "/ffmpeg");
#endif
    proConvertImage->start();
    waitForFinished(proConvertImage);

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

/*!
  Starts ffmpeg and tries to covert the given video to a webm video.
  \code
    //[...]
    args.append("-c:v");
    args.append("libvpx-vp8");
    args.append("-crf");
    args.append("30");
    args.append("-pix_fmt");
    args.append("yuv420p");
    args.append("-b:v");
    args.append("0");
  \endcode
  Returns \c false if :
  \list
    \li Cannot convert the video
    \li Generally broken.
   \endlist
 */
bool CreateImportVideo::createWallpaperVideo()
{

    if (m_videoPath.endsWith(".webm")) {
        return true;
    }

    emit createWallpaperStateChanged(ImportVideoState::ConvertingVideo);

    QStringList args;
    args.append("-hide_banner");
    args.append("-y");
    args.append("-stats");
    args.append("-i");
    args.append(m_videoPath);
    args.append("-c:v");
    args.append("libvpx");
    args.append("-b:v");
    args.append("0");
    args.append("-crf");
    args.append("10");
    args.append("-pass");
    args.append("1");

#ifdef Q_OS_WIN
    args.append("-an");
    args.append("-f");
    args.append("webm");
    args.append("NULL");
#endif
#ifdef Q_OS_MACOS
    args.append("-an");
    args.append("-f");
    args.append("webm");
    args.append("/dev/null");
#endif

    auto proConvertVideoPass1 = std::make_shared<QProcess>();
    proConvertVideoPass1->setArguments(args);
    proConvertVideoPass1->setProgram(m_ffmpegExecutable);
    proConvertVideoPass1->setProcessChannelMode(QProcess::MergedChannels);


    connect(proConvertVideoPass1.get(), &QProcess::readyReadStandardOutput, this, [&]() {
        QString tmpOut = proConvertVideoPass1->readAllStandardOutput();
        if (tmpOut.contains("Conversion failed!")) {
            emit createWallpaperStateChanged(ImportVideoState::ConvertingVideoError);
        }
        auto tmpList = tmpOut.split(QRegExp("\\s+"), QString::SkipEmptyParts);

        if (tmpList.length() > 2) {
            bool ok = false;
            float currentFrame = QString(tmpList.at(1)).toFloat(&ok);

            if (!ok)
                return;

            float progress = (currentFrame / m_numberOfFrames) / 2;

            this->setProgress(progress);
        }
        emit processOutput(tmpOut);
    });

    proConvertVideoPass1->start();
    waitForFinished(proConvertVideoPass1);
    proConvertVideoPass1->close();


    args.clear();
    args.append("-hide_banner");
    args.append("-y");
    args.append("-stats");
    args.append("-i");
    args.append(m_videoPath);
    args.append("-c:v");
    args.append("libvpx");
    args.append("-b:v");
    args.append("0");
    args.append("-crf");
    args.append("10");
    args.append("-pass");
    args.append("2");
    QFileInfo file(m_videoPath);
    QString convertedFileAbsolutePath { m_exportPath + "/" + file.baseName() + ".webm" };
    args.append(convertedFileAbsolutePath);

    auto proConvertVideoPass2 = std::make_shared<QProcess>();
    proConvertVideoPass2->setArguments(args);
    proConvertVideoPass2->setProgram(m_ffmpegExecutable);
    proConvertVideoPass2->setProcessChannelMode(QProcess::MergedChannels);

    connect(proConvertVideoPass2.get(), &QProcess::readyReadStandardOutput, this, [&]() {
        QString tmpOut = proConvertVideoPass2->readAllStandardOutput();
        if (tmpOut.contains("Conversion failed!")) {
            emit createWallpaperStateChanged(ImportVideoState::ConvertingVideoError);
        }
        auto tmpList = tmpOut.split(QRegExp("\\s+"), QString::SkipEmptyParts);

        if (tmpList.length() > 2) {
            bool ok = false;
            float currentFrame = QString(tmpList.at(1)).toFloat(&ok);

            if (!ok)
                return;

            float progress = 0.5 + ((currentFrame / m_numberOfFrames) / 2);

            this->setProgress(progress);
        }
        emit processOutput(tmpOut);
    });

    proConvertVideoPass2->start();
    waitForFinished(proConvertVideoPass2);
    proConvertVideoPass2->close();

    QFile video(convertedFileAbsolutePath);
    if (!video.exists() || !(video.size() > 0)) {
        qDebug() << convertedFileAbsolutePath << proConvertVideoPass1->readAll() << video.exists() << video.size();
        emit createWallpaperStateChanged(ImportVideoState::ConvertingVideoError);
        return false;
    }

    emit createWallpaperStateChanged(ImportVideoState::ConvertingVideoFinished);

    return true;
}
/*!
  Starts ffmpeg and tries to covert the given audio into a seperate mp3.
  \code
    //[...]
    args.append("mp3");
    args.append("-ab");
    args.append("192000");
    args.append("-vn");
    args.append(m_exportPath + "/audio.mp3");
  \endcode
  Returns \c false if :
  \list
    \li Cannot convert the audio
    \li Generally broken.
   \endlist
 */
bool CreateImportVideo::extractWallpaperAudio()
{

    emit createWallpaperStateChanged(ImportVideoState::ConvertingAudio);

    QStringList args;
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

    auto proConvertAudio = std::make_shared<QProcess>();
    proConvertAudio->setArguments(args);
    proConvertAudio->setProgram(m_ffmpegExecutable);
    proConvertAudio->start(QIODevice::ReadOnly);

    waitForFinished(proConvertAudio);

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

void CreateImportVideo::waitForFinished(std::shared_ptr<QProcess>& process)
{
    while (!process->waitForFinished(100)) //Wake up every 100ms and check if we must exit
    {
        if (QThread::currentThread()->isInterruptionRequested()) {
            qDebug() << "Interrupt thread";
            process->terminate();
            if (!process->waitForFinished(1000)) {
                process->kill();
            }
            break;
        }
        QCoreApplication::processEvents();
    }
}

}
