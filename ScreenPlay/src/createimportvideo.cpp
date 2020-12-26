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
 \brief This constructor is only needed for calling  qRegisterMetaType on CreateImportVideo to register the enums.
     \code
     qRegisterMetaType<CreateImportVideo::ImportVideoState>("CreateImportVideo::ImportVideoState");
     \endcode
 */
CreateImportVideo::CreateImportVideo(QObject* parent)
    : QObject(parent)
{
}

/*!
  \brief Creates a CreateImportVideo object to be used in a different thread. A \a videoPath and a \a exportPath are
  needed for convertion.
*/
CreateImportVideo::CreateImportVideo(const QString& videoPath, const QString& exportPath, const QStringList& codecs, QObject* parent)
    : QObject(parent)
{
    m_videoPath = videoPath;
    m_exportPath = exportPath;
    m_codecs = codecs;

#ifdef Q_OS_WIN
    const QString fileEnding = ".exe";
#else
    const QString fileEnding = "";
#endif

    m_ffprobeExecutable = QApplication::applicationDirPath() + "/ffprobe" + fileEnding;
    m_ffmpegExecutable = QApplication::applicationDirPath() + "/ffmpeg" + fileEnding;
}

/*!
  \brief Processes the multiple steps of creating a wallpaper.
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

    // If the video has no audio we can skip the extraction
    if (!m_skipAudio) {
        if (!extractWallpaperAudio() || QThread::currentThread()->isInterruptionRequested()) {
            emit abortAndCleanup();
            return;
        }
    }

    if (m_isWebm) {
        emit createWallpaperStateChanged(ImportVideoState::Finished);
        return;
    }

    for (const auto& codec : qAsConst(m_codecs)) {
        if (!createWallpaperVideo(codec) || QThread::currentThread()->isInterruptionRequested()) {
            emit abortAndCleanup();
            return;
        }
    }

    emit createWallpaperStateChanged(ImportVideoState::Finished);
}

/*!
  \brief Starts ffprobe and tries to parse the resulting json.
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
    if (m_videoPath.endsWith(".webm") || m_videoPath.endsWith(".mkv")) {
        m_isWebm = true;
    }

    // Get video info
    QStringList args;
    args.append("-print_format");
    args.append("json");

    // MKV/Webm a simple container query will result in N/A nb_frames
    // https://stackoverflow.com/questions/2017843/fetch-frame-count-with-ffmpeg
    if (m_isWebm) {
        args.append("-count_frames");
        args.append("-select_streams");
        args.append("v:0");
        args.append("-show_entries");
        args.append("stream=nb_read_frames");
    } else {
        args.append("-show_format");
        args.append("-show_streams");
    }

    args.append(m_videoPath);

    emit processOutput("ffprobe " + Util::toString(args));

    emit createWallpaperStateChanged(ImportVideoState::AnalyseVideo);

    const QString ffmpegOut = waitForFinished(args, QProcess::SeparateChannels, Executable::FFPROBE);
    qInfo() << ffmpegOut;

    emit createWallpaperStateChanged(ImportVideoState::AnalyseVideoFinished);

    auto obj = Util::parseQByteArrayToQJsonObject(QByteArray::fromStdString(ffmpegOut.toStdString()));
    if (!obj) {
        QString error = ffmpegOut;
        qWarning() << "Error parsing FFPROBE json output:" << error << "\n Args: " << args;

        emit processOutput(ffmpegOut);
        emit processOutput("Error parsing FFPROBE json output");
        emit createWallpaperStateChanged(ImportVideoState::AnalyseVideoError);
        return false;
    }

    if (obj->empty()) {
        qWarning() << "Error! File could not be parsed.";
        emit processOutput("Error! File could not be parsed.");
        emit createWallpaperStateChanged(ImportVideoState::AnalyseVideoError);

        return false;
    }

    if (m_isWebm) {
        return analyzeWebmReadFrames(obj.value());
    } else {
        return analyzeVideo(obj.value());
    }
}

/*!
  \brief Starts ffmpeg and tries to covert the given video to a five second preview.
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
    // If the video is shorter than 5 seconds we simply convert the original to webm
    if (!m_smallVideo) {
        args.append("-vf");
        // We allways want to have a 5 second clip via 24fps -> 120 frames
        // Divided by the number of frames we can skip (timeInSeconds * Framrate)
        // scale & crop parameter: https://unix.stackexchange.com/a/284731
        args.append("select='not(mod(n," + QString::number((m_length / 5)) + "))',setpts=N/FRAME_RATE/TB,crop=in_h*16/9:in_h,scale=-2:400");
        // Disable audio
    }
    args.append("-an");
    args.append(m_exportPath + "/preview.webm");
    emit processOutput("ffmpeg " + Util::toString(args));

    connect(this, &CreateImportVideo::ffmpegOutput, this, [&](QString tmpOut) {
        auto tmpList = tmpOut.split(QRegExp("\\s+"), Qt::SplitBehaviorFlags::SkipEmptyParts);

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

    const QString ffmpegOut = waitForFinished(args);
    QFile previewVideo(m_exportPath + "/preview.webm");
    if (!previewVideo.exists() || !(previewVideo.size() > 0)) {
        emit createWallpaperStateChanged(ImportVideoState::ConvertingPreviewVideoError);
        return false;
    }

    emit processOutput(ffmpegOut);

    emit createWallpaperStateChanged(ImportVideoState::ConvertingPreviewVideoFinished);

    return true;
}

/*!
  \brief Starts ffmpeg and tries to covert the given video to a 5 second preview gif.
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

    const QString ffmpegOut = waitForFinished(args);

    if (!ffmpegOut.isEmpty()) {
        QFile previewGif(m_exportPath + "/preview.gif");
        if (!previewGif.exists() || !(previewGif.size() > 0)) {
            emit createWallpaperStateChanged(ImportVideoState::ConvertingPreviewGifError);
            return false;
        }
    }

    emit processOutput(ffmpegOut);
    emit createWallpaperStateChanged(ImportVideoState::ConvertingPreviewGifFinished);

    return true;
}

/*!
  \brief Starts ffmpeg and tries to covert the given video to a image preview.
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
    // If the video is shorter than 3 seconds we use the first frame
    if (!m_smallVideo) {
        args.append("-ss");
        args.append("00:00:02");
    }
    args.append("-i");
    args.append(m_videoPath);
    // Order of arguments is important
    if (!m_smallVideo) {
        args.append("-vframes");
        args.append("1");
    }
    args.append("-q:v");
    args.append("2");
    if (m_smallVideo) {
        args.append("-vf");
        // Select first frame https://stackoverflow.com/a/44073745/12619313
        args.append("select=eq(n\\,0), scale=320:-1");
    } else {
        args.append("-vf");
        args.append("scale=320:-1");
    }
    args.append(m_exportPath + "/previewThumbnail.jpg");

    emit processOutput("ffmpeg " + Util::toString(args));

    const QString ffmpegOut = waitForFinished(args);
    if (!ffmpegOut.isEmpty()) {
        QFile previewImg(m_exportPath + "/previewThumbnail.jpg");
        if (!previewImg.exists() || !(previewImg.size() > 0)) {
            emit createWallpaperStateChanged(ImportVideoState::ConvertingPreviewImageThumbnailError);
            return false;
        }
    }

    emit processOutput(ffmpegOut);
    emit createWallpaperStateChanged(ImportVideoState::ConvertingPreviewImageThumbnailFinished);

    return true;
}

/*!
  \brief .
*/
bool CreateImportVideo::createWallpaperImagePreview()
{

    emit createWallpaperStateChanged(ImportVideoState::ConvertingPreviewImage);

    QStringList args;
    args.clear();
    args.append("-y");
    args.append("-stats");
    // If the video is shorter than 3 seconds we use the first frame
    if (!m_smallVideo) {
        args.append("-ss");
        args.append("00:00:02");
    }
    args.append("-i");
    args.append(m_videoPath);
    args.append("-q:v");
    args.append("2");
    if (m_smallVideo) {
        args.append("-vf");
        // Select first frame https://stackoverflow.com/a/44073745/12619313
        args.append("select=eq(n\\,0)");
    }
    args.append(m_exportPath + "/preview.jpg");

    emit processOutput("ffmpeg " + Util::toString(args));
    const QString ffmpegOut = waitForFinished(args);
    if (!ffmpegOut.isEmpty()) {
        QFile previewImg(m_exportPath + "/preview.jpg");
        if (!previewImg.exists() || !(previewImg.size() > 0)) {
            emit createWallpaperStateChanged(ImportVideoState::ConvertingPreviewImageError);
            return false;
        }
    }

    emit processOutput(ffmpegOut);
    emit createWallpaperStateChanged(ImportVideoState::ConvertingPreviewImageFinished);

    return true;
}

/*!
  \brief Starts ffmpeg and tries to covert the given video to a webm video.
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
bool CreateImportVideo::createWallpaperVideo(const QString& codec)
{
    emit createWallpaperStateChanged(ImportVideoState::ConvertingVideo);

    //
    connect(this, &CreateImportVideo::ffmpegOutput, this, [&](QString tmpOut) {
        if (tmpOut.contains("Conversion failed!")) {
            emit createWallpaperStateChanged(ImportVideoState::ConvertingVideoError);
        }
        auto tmpList = tmpOut.split(QRegExp("\\s+"), Qt::SplitBehaviorFlags::SkipEmptyParts);

        if (tmpList.length() > 2) {
            bool ok = false;
            float currentFrame = QString(tmpList.at(1)).toFloat(&ok);

            if (!ok)
                return;

            float progress = (currentFrame / m_numberOfFrames);

            this->setProgress(progress);
        }
        emit processOutput(tmpOut);
    });

    QStringList args;
    args.append("-hide_banner");
    args.append("-y");
    args.append("-stats");
    args.append("-i");
    args.append(m_videoPath);
    args.append("-c:v");
    if (codec == "vp8") {
        args.append("libvpx");
    }
    if (codec == "vp9") {
        args.append("libvpx-vp9");
    }
    args.append("-b:v");
    args.append("0");
    args.append("-crf");
    args.append("10");
    args.append("-pass");
    args.append("1");

    args.append("-an");
    args.append("-f");
    args.append("webm");

#ifdef Q_OS_WIN
    args.append("NULL");
#else
    args.append("/dev/null");
#endif

    waitForFinished(args);

    // Second pass

    args.clear();
    args.append("-hide_banner");
    args.append("-y");
    args.append("-stats");
    args.append("-i");
    args.append(m_videoPath);
    args.append("-c:v");
    if (codec == "vp8") {
        args.append("libvpx");
    }
    if (codec == "vp9") {
        args.append("libvpx-vp9");
    }
    args.append("-b:v");
    args.append("0");
    args.append("-crf");
    args.append("10");
    args.append("-pass");
    args.append("2");
    QFileInfo file(m_videoPath);
    QString convertedFileAbsolutePath { m_exportPath + "/" + file.baseName() + ".webm" };
    args.append(convertedFileAbsolutePath);

    const QString ffmpegOutput = waitForFinished(args);

    QFile video(convertedFileAbsolutePath);
    if (!video.exists() || !(video.size() > 0)) {
        qDebug() << convertedFileAbsolutePath << ffmpegOutput << video.exists() << video.size();
        emit createWallpaperStateChanged(ImportVideoState::ConvertingVideoError);
        return false;
    }

    emit createWallpaperStateChanged(ImportVideoState::ConvertingVideoFinished);

    return true;
}
/*!
  \brief Starts ffmpeg and tries to covert the given audio into a seperate mp3.
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

    const QString tmpErrImg = waitForFinished(args);

    if (!tmpErrImg.isEmpty()) {
        QFile previewImg(m_exportPath + "/audio.mp3");
        if (!previewImg.exists() || !(previewImg.size() > 0)) {
            qDebug() << args;
            qDebug() << tmpErrImg;
            emit createWallpaperStateChanged(ImportVideoState::ConvertingAudioError);
            return false;
        }
    }

    emit processOutput(tmpErrImg);
    emit createWallpaperStateChanged(ImportVideoState::ConvertingAudioFinished);

    return true;
}

/*!
  \brief .
*/
QString CreateImportVideo::waitForFinished(
    const QStringList& args,
    const QProcess::ProcessChannelMode processChannelMode,
    const Executable executable)
{

    QProcess process;
    if (executable == Executable::FFMPEG) {
        process.setProgram(m_ffmpegExecutable);
    } else {
        process.setProgram(m_ffprobeExecutable);
    }
    process.setProcessChannelMode(processChannelMode);
    process.setArguments(args);
    process.start();

    while (!process.waitForFinished(10)) //Wake up every 10ms and check if we must exit
    {
        if (QThread::currentThread()->isInterruptionRequested()) {
            qDebug() << "Interrupt thread";
            process.terminate();
            if (!process.waitForFinished(1000)) {
                process.kill();
            }
            break;
        }
        QCoreApplication::processEvents();
    }
    QString processOutput;
    if (processChannelMode == QProcess::SeparateChannels) {
        processOutput = process.readAllStandardOutput();
    } else {
        processOutput = process.readAll();
    }
    process.close();

    return processOutput;
}

/*!
    \brief .
*/
bool CreateImportVideo::analyzeWebmReadFrames(const QJsonObject& obj)
{
    // Get video info
    QStringList args;
    args.append("-print_format");
    args.append("json");

    // MKV/Webm a simple container query will result in N/A nb_frames
    // https://stackoverflow.com/questions/2017843/fetch-frame-count-with-ffmpeg
    args.append("-count_frames");
    args.append("-select_streams");
    args.append("v:0");
    args.append("-show_entries");
    args.append("stream=nb_read_frames");
    args.append(m_videoPath);

    emit processOutput("ffprobe " + Util::toString(args));

    emit createWallpaperStateChanged(ImportVideoState::AnalyseVideo);

    const QString ffmpegOut = waitForFinished(args, QProcess::SeparateChannels, Executable::FFPROBE);
    qInfo() << ffmpegOut;
    qInfo() << obj << obj.value("streams");
    // Number of frames is a string for some reason...
    bool okParseNumberOfFrames { false };
    if (!obj.value("streams").isArray()) {
        qWarning() << "Wrong json strucutre!";
        return false;
    }

    QJsonArray streams = obj.value("streams").toArray();
    int numberOfFrames = streams.first().toObject().value("nb_read_frames").toString().toInt(&okParseNumberOfFrames);
    if (okParseNumberOfFrames)
        m_numberOfFrames = numberOfFrames;
    else
        return false;

    qInfo() << m_numberOfFrames;
    return false;
}

/*!
    \brief .
*/
bool CreateImportVideo::analyzeVideo(const QJsonObject& obj)
{
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

    QStringList avgFrameRateList = avgFrameRate.split('/', Qt::SplitBehaviorFlags::SkipEmptyParts);
    if (avgFrameRateList.length() != 2) {
        qDebug() << "Error could not parse streams with length: " << avgFrameRateList.length();
        emit processOutput("Error could not parse streams with length: " + QString::number(avgFrameRateList.length()));
        return false;
    }

    const double value1 = avgFrameRateList.at(0).toDouble();
    const double value2 = avgFrameRateList.at(1).toDouble();

    m_framerate = qCeil(value1 / value2);

    // If the video is to short
    m_smallVideo = m_numberOfFrames < (m_framerate * 5);

    return true;
}

}
