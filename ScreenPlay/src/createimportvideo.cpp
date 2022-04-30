#include "ScreenPlay/createimportvideo.h"
#include "ScreenPlayUtil/util.h"

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
     qRegisterMetaType<ImportVideoState::ImportVideoState>("ImportVideoState::ImportVideoState");
     \endcode
 */

/*!
  \brief Creates a CreateImportVideo object to be used in a different thread. A \a videoPath and a \a exportPath are
  needed for convertion.
*/
CreateImportVideo::CreateImportVideo(
    const QString& videoPath,
    const QString& exportPath,
    const QString& codec,
    const int quality,
    std::atomic<bool>& interrupt)
    : QObject(nullptr)
    , m_quality(quality)
    , m_interrupt(interrupt)
{
    m_videoPath = videoPath;
    m_exportPath = exportPath;
    m_codec = codec;
    setupFFMPEG();
}

CreateImportVideo::CreateImportVideo(const QString& videoPath, const QString& exportPath, std::atomic<bool>& interrupt)
    : QObject(nullptr)
    , m_quality(0)
    , m_interrupt(interrupt)
{
    m_videoPath = videoPath;
    m_exportPath = exportPath;
    setupFFMPEG();
}

void CreateImportVideo::setupFFMPEG()
{

#ifdef Q_OS_LINUX
    // Use system ffmpeg
    m_ffprobeExecutable = "ffprobe";
    m_ffmpegExecutable = "ffmpeg";
#else
    m_ffprobeExecutable = QApplication::applicationDirPath() + "/ffprobe" + ScreenPlayUtil::executableBinEnding();
    m_ffmpegExecutable = QApplication::applicationDirPath() + "/ffmpeg" + ScreenPlayUtil::executableBinEnding();
#endif

    if (!QFileInfo::exists(m_ffprobeExecutable)) {
        qFatal("FFPROBE executable not found!");
    }

    if (!QFileInfo::exists(m_ffmpegExecutable)) {
        qFatal("FFMPEG executable not found!");
    }
}

/*!
  \brief Starts ffprobe and tries to parse the resulting json. If the video
         is a container that not contains the video length like webm or mkv
         we need to count the frames ourself. We then call analyzeWebmReadFrames
         or analyzeVideo to parse the output.
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
        args.append("stream=nb_read_frames,r_frame_rate");
    } else {
        args.append("-show_format");
        args.append("-show_streams");
    }

    args.append(m_videoPath);

    emit processOutput("ffprobe " + ScreenPlayUtil::toString(args));

    emit createWallpaperStateChanged(ImportVideoState::ImportVideoState::AnalyseVideo);

    const QString ffmpegOut = waitForFinished(args, QProcess::SeparateChannels, Executable::FFPROBE);
    qInfo() << ffmpegOut;

    emit createWallpaperStateChanged(ImportVideoState::ImportVideoState::AnalyseVideoFinished);

    auto obj = ScreenPlayUtil::parseQByteArrayToQJsonObject(QByteArray::fromStdString(ffmpegOut.toStdString()));

    if (!obj) {
        QString error = ffmpegOut;
        qWarning() << "Error parsing FFPROBE json output:" << error << "\n Args: " << args;

        emit processOutput(ffmpegOut);
        emit processOutput("Error parsing FFPROBE json output");
        emit createWallpaperStateChanged(ImportVideoState::ImportVideoState::AnalyseVideoError);
        return false;
    }

    if (obj->empty()) {
        qCritical() << "Error! File could not be parsed.";
        emit processOutput("Error! File could not be parsed.");
        emit createWallpaperStateChanged(ImportVideoState::ImportVideoState::AnalyseVideoError);
        return false;
    }

    if (m_isWebm) {
        return analyzeWebmReadFrames(obj.value());
    } else {
        return analyzeVideo(obj.value());
    }
}

/*!
    \brief .
    {"programs":[],"streams":[{"nb_read_frames":"480"}]}
*/
bool CreateImportVideo::analyzeWebmReadFrames(const QJsonObject& obj)
{

    emit createWallpaperStateChanged(ImportVideoState::ImportVideoState::AnalyseVideo);

    // Number of frames is a string for some reason...
    if (!obj.value("streams").isArray()) {
        qWarning() << "Error not valid json strucutre!" << obj;
        return false;
    }

    const QJsonArray streams = obj.value("streams").toArray();
    const QJsonObject firstStream = streams.first().toObject();
    qInfo() << "streams:" << streams;

    bool okParseNumberOfFrames { false };
    int numberOfFrames = firstStream.value("nb_read_frames").toString().toInt(&okParseNumberOfFrames);
    if (!okParseNumberOfFrames) {
        qWarning() << "Error could not parse nb_read_frames";
        return false;
    }
    m_numberOfFrames = numberOfFrames;

    // "r_frame_rate": "25/1"
    QString frameRate = firstStream.value("r_frame_rate").toString();
    QStringList frameRateList = frameRate.split('/', Qt::SplitBehaviorFlags::SkipEmptyParts);
    if (frameRateList.length() != 2) {
        qWarning() << "Error could not parse streams with length: " << frameRateList.length();
        emit processOutput("Error could not parse streams with length: " + QString::number(frameRateList.length()));
        return false;
    }

    const double first = frameRateList.at(0).toDouble();
    const double second = frameRateList.at(1).toDouble();

    m_framerate = std::ceil(first / second);

    // If the video is to short
    m_smallVideo = m_numberOfFrames < (m_framerate * 5);
    m_length = std::ceil(m_numberOfFrames / m_framerate);

    qInfo() << m_numberOfFrames << m_framerate << m_smallVideo << m_length;
    return true;
}

/*!
    \brief .
*/
bool CreateImportVideo::analyzeVideo(const QJsonObject& obj)
{
    // Check for audio and video streams
    const QJsonArray arrayStream = obj.value("streams").toArray();

    bool hasAudioStream { false };
    bool hasVideoStream { false };

    QJsonObject videoStream;

    for (const auto& stream : arrayStream) {
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
        emit createWallpaperStateChanged(ImportVideoState::ImportVideoState::AnalyseVideoHasNoVideoStreamError);
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
        emit createWallpaperStateChanged(ImportVideoState::ImportVideoState::AnalyseVideoError);
        return false;
    }

    QJsonObject objFormat = obj.value("format").toObject();

    // Get video length
    bool okParseDuration = false;
    const float tmpLength = objFormat.value("duration").toVariant().toFloat(&okParseDuration);

    if (!okParseDuration) {
        qDebug() << "Error parsing video length. Is this really a valid video File?";
        emit processOutput("Error parsing video length. Is this really a valid video File?");
        emit createWallpaperStateChanged(ImportVideoState::ImportVideoState::AnalyseVideoError);
        return false;
    }

    m_length = static_cast<int>(tmpLength);

    // Get framerate
    const QJsonArray streams = obj.value("streams").toArray();
    if (streams.empty()) {
        qDebug() << "Error container does not have any video streams";
        emit processOutput("Error container does not have any video streams");
        return false;
    }

    const QJsonObject firstStream = streams.first().toObject();

    // The paramter gets us the exact framerate
    // "avg_frame_rate":"47850000/797509"
    // so we need no calc the value by dividing the two numbers
    const QString avgFrameRate = firstStream.value("avg_frame_rate").toVariant().toString();

    const QStringList avgFrameRateList = avgFrameRate.split('/', Qt::SplitBehaviorFlags::SkipEmptyParts);
    if (avgFrameRateList.length() != 2) {
        qDebug() << "Error could not parse streams with length: " << avgFrameRateList.length();
        emit processOutput("Error could not parse streams with length: " + QString::number(avgFrameRateList.length()));
        return false;
    }

    const double first = avgFrameRateList.at(0).toDouble();
    const double second = avgFrameRateList.at(1).toDouble();

    m_framerate = std::ceil(first / second);

    // If the video is to short
    m_smallVideo = m_numberOfFrames < (m_framerate * 5);

    return true;
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

    emit createWallpaperStateChanged(ImportVideoState::ImportVideoState::ConvertingPreviewVideo);

    QStringList args;
    args.append("-y");
    args.append("-stats");
    args.append("-i");
    args.append(m_videoPath);
    // If the video is shorter than 5 seconds we simply convert the original to webm
    if (!m_smallVideo) {
        qInfo() << "Regular video length detected!";
        args.append("-vf");
        // We allways want to have a 5 second clip via 24fps -> 120 frames
        // Divided by the number of frames we can skip (timeInSeconds * Framrate)
        // scale & crop parameter: https://unix.stackexchange.com/a/284731
        args.append("select='not(mod(n," + QString::number((m_length / 5)) + "))',setpts=N/FRAME_RATE/TB,crop=in_h*16/9:in_h,scale=-2:120");
    }
    // Disable audio
    args.append("-an");
    args.append(m_exportPath + "/preview.webm");
    emit processOutput("ffmpeg " + ScreenPlayUtil::toString(args));

    const QString ffmpegOut = waitForFinished(args);
    const QFile previewVideo(m_exportPath + "/preview.webm");
    if (!previewVideo.exists() || !(previewVideo.size() > 0)) {
        emit createWallpaperStateChanged(ImportVideoState::ImportVideoState::ConvertingPreviewVideoError);
        return false;
    }

    emit processOutput(ffmpegOut);

    emit createWallpaperStateChanged(ImportVideoState::ImportVideoState::ConvertingPreviewVideoFinished);

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

    emit createWallpaperStateChanged(ImportVideoState::ImportVideoState::ConvertingPreviewGif);

    QStringList args;
    args.append("-y");
    args.append("-stats");
    args.append("-i");
    if (m_isWebm) {
        args.append(m_videoPath);
    } else {
        args.append(m_exportPath + "/preview.webm");
    }
    args.append("-filter_complex");
    args.append("[0:v] fps=12,scale=w=480:h=-1,split [a][b];[a] palettegen=stats_mode=single [p];[b][p] paletteuse=new=1");
    args.append(m_exportPath + "/preview.gif");
    emit processOutput("ffmpeg " + ScreenPlayUtil::toString(args));

    const QString ffmpegOut = waitForFinished(args);

    if (!ffmpegOut.isEmpty()) {
        const QFile previewGif(m_exportPath + "/preview.gif");
        if (!previewGif.exists() || !(previewGif.size() > 0)) {
            emit createWallpaperStateChanged(ImportVideoState::ImportVideoState::ConvertingPreviewGifError);
            return false;
        }
    }

    emit processOutput(ffmpegOut);
    emit createWallpaperStateChanged(ImportVideoState::ImportVideoState::ConvertingPreviewGifFinished);

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

    emit createWallpaperStateChanged(ImportVideoState::ImportVideoState::ConvertingPreviewImageThumbnail);

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

    emit processOutput("ffmpeg " + ScreenPlayUtil::toString(args));

    const QString ffmpegOut = waitForFinished(args);
    if (!ffmpegOut.isEmpty()) {
        const QFile previewImg(m_exportPath + "/previewThumbnail.jpg");
        if (!previewImg.exists() || !(previewImg.size() > 0)) {
            emit createWallpaperStateChanged(ImportVideoState::ImportVideoState::ConvertingPreviewImageThumbnailError);
            return false;
        }
    }

    emit processOutput(ffmpegOut);
    emit createWallpaperStateChanged(ImportVideoState::ImportVideoState::ConvertingPreviewImageThumbnailFinished);

    return true;
}

/*!
  \brief .
*/
bool CreateImportVideo::createWallpaperImagePreview()
{

    emit createWallpaperStateChanged(ImportVideoState::ImportVideoState::ConvertingPreviewImage);

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

    emit processOutput("ffmpeg " + ScreenPlayUtil::toString(args));
    const QString ffmpegOut = waitForFinished(args);
    if (!ffmpegOut.isEmpty()) {
        const QFile previewImg(m_exportPath + "/preview.jpg");
        if (!previewImg.exists() || !(previewImg.size() > 0)) {
            emit createWallpaperStateChanged(ImportVideoState::ImportVideoState::ConvertingPreviewImageError);
            return false;
        }
    }

    emit processOutput(ffmpegOut);
    emit createWallpaperStateChanged(ImportVideoState::ImportVideoState::ConvertingPreviewImageFinished);

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
bool CreateImportVideo::createWallpaperVideo()
{
    emit createWallpaperStateChanged(ImportVideoState::ImportVideoState::ConvertingVideo);

    connect(m_process.get(), &QProcess::readyReadStandardOutput, this, [&]() {
        QString tmpOut = m_process->readAllStandardOutput();
        qInfo() << tmpOut;
        if (tmpOut.contains("Conversion failed!")) {
            emit createWallpaperStateChanged(ImportVideoState::ImportVideoState::ConvertingVideoError);
        }
        const auto tmpList = tmpOut.split(QRegularExpression("\\s+"), Qt::SplitBehaviorFlags::SkipEmptyParts);

        if (tmpList.length() > 2) {
            bool ok = false;
            const float currentFrame = QString(tmpList.at(1)).toFloat(&ok);

            if (!ok)
                return;

            const float progress = (currentFrame / m_numberOfFrames);

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
    if (m_codec == "vp8")
        args.append("libvpx");
    if (m_codec == "vp9")
        args.append("libvpx-vp9");
    if (m_codec == "av1")
        args.append("libaom-av1");
    args.append("-b:v");
    args.append("13000k");
    args.append("-threads");
    args.append(QString::number(QThread::idealThreadCount()));
    qInfo() << "threads" << QThread::idealThreadCount() << "m_quality" << m_quality;
    args.append("-speed");
    args.append("4");
    args.append("-tile-columns");
    args.append("0");
    args.append("-frame-parallel");
    args.append("0");
    args.append("-crf");
    args.append(QString::number(m_quality));
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
    if (m_codec == "vp8")
        args.append("libvpx");
    if (m_codec == "vp9")
        args.append("libvpx-vp9");
    if (m_codec == "av1")
        args.append("libaom-av1");
    args.append("-b:v");
    args.append("13000k");
    args.append("-threads");
    args.append(QString::number(QThread::idealThreadCount()));
    args.append("-speed");
    args.append("0");
    args.append("-tile-columns");
    args.append("0");
    args.append("-frame-parallel");
    args.append("0");
    args.append("-auto-alt-ref");
    args.append("1");
    args.append("-lag-in-frames");
    args.append("25");
    args.append("-crf");
    args.append(QString::number(m_quality));
    args.append("-pass");
    args.append("2");
    const QFileInfo file(m_videoPath);
    const QString convertedFileAbsolutePath { m_exportPath + "/" + file.completeBaseName() + ".webm" };
    args.append(convertedFileAbsolutePath);

    const QString ffmpegOutput = waitForFinished(args);

    QFile video(convertedFileAbsolutePath);
    if (!video.exists() || !(video.size() > 0)) {
        qDebug() << convertedFileAbsolutePath << ffmpegOutput << video.exists() << video.size();
        emit createWallpaperStateChanged(ImportVideoState::ImportVideoState::ConvertingVideoError);
        return false;
    }

    emit createWallpaperStateChanged(ImportVideoState::ImportVideoState::ConvertingVideoFinished);

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

    emit createWallpaperStateChanged(ImportVideoState::ImportVideoState::ConvertingAudio);

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
        const QFile previewImg(m_exportPath + "/audio.mp3");
        if (!previewImg.exists() || !(previewImg.size() > 0)) {
            qDebug() << args;
            qDebug() << tmpErrImg;
            emit createWallpaperStateChanged(ImportVideoState::ImportVideoState::ConvertingAudioError);
            return false;
        }
    }

    emit processOutput(tmpErrImg);
    emit createWallpaperStateChanged(ImportVideoState::ImportVideoState::ConvertingAudioFinished);

    return true;
}

/*!
  \brief Function to start an external ffmpeg/ffprobe executable and wait (blocking) until it is finished.

*/
QString CreateImportVideo::waitForFinished(
    const QStringList& args,
    const QProcess::ProcessChannelMode processChannelMode,
    const Executable executable)
{

    m_process = std::make_unique<QProcess>();
    QObject::connect(m_process.get(), &QProcess::errorOccurred, [=](QProcess::ProcessError error) {
        qDebug() << "error enum val = " << error << m_process->errorString();
        emit createWallpaperStateChanged(ImportVideoState::ImportVideoState::AnalyseVideoError);
        m_process->terminate();
        if (!m_process->waitForFinished(1000)) {
            m_process->kill();
        }
    });

    QObject::connect(m_process.get(), QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
        [=](int exitCode, QProcess::ExitStatus exitStatus) {
            qInfo() << "Process finished with exit code: " << exitCode;
            if (exitCode != 0)
                qWarning() << "Process finished with exit code: " << exitCode << " exitStatus:" << exitStatus;
        });

    if (executable == Executable::FFMPEG) {
        m_process->setProgram(m_ffmpegExecutable);
    } else {
        m_process->setProgram(m_ffprobeExecutable);
    }

#ifdef Q_OS_OSX
    QProcess changeChmod;
    changeChmod.setProgram("chmod");
    changeChmod.setArguments({ "+x", m_process->program() });
    changeChmod.start();
    if (!changeChmod.waitForFinished()) {
        qCritical() << "Unable to change permission " << m_process->program() << " to be exectuable";
    }
#endif

    m_process->setProcessChannelMode(processChannelMode);
    m_process->setArguments(args);
    m_process->setWorkingDirectory(QApplication::applicationDirPath());
    m_process->start();

    qInfo() << m_process->workingDirectory() << m_process->program() << m_process->arguments();

    while (!m_process->waitForFinished(10)) //Wake up every 10ms and check if we must exit
    {
        if (m_interrupt) {
            qInfo() << "Interrupt thread";
            m_process->terminate();
            if (!m_process->waitForFinished(1000)) {
                m_process->kill();
            }
            break;
        }
        QCoreApplication::processEvents();
    }
    QString processOutput;
    if (processChannelMode == QProcess::SeparateChannels) {
        processOutput = m_process->readAllStandardOutput();
    } else {
        processOutput = m_process->readAll();
    }

    if (!processOutput.isEmpty())
        qInfo() << "ProcessOutput:" << processOutput;

    m_process->close();

    return processOutput;
}

}
