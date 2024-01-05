// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#include "ScreenPlay/createimportvideo.h"
#include "ScreenPlayUtil/util.h"
#include <QGuiApplication>

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
  \brief Creates a CreateImportVideo object to be used in a different thread. A \a videoPath and a \a exportPath are
  needed for convertion.
*/
CreateImportVideo::CreateImportVideo(
    const QString& videoPath,
    const QString& exportPath,
    const ScreenPlay::Video::VideoCodec targetCodec,
    const int quality,
    std::atomic<bool>& interrupt)
    : QObject(nullptr)
    , m_quality(quality)
    , m_interrupt(interrupt)
{
    m_videoPath = videoPath;
    m_exportPath = exportPath;
    m_targetCodec = targetCodec;
    setupFFMPEG();
}

void CreateImportVideo::setupFFMPEG()
{

    Util util;
#ifdef Q_OS_LINUX
    // Use system ffmpeg
    m_ffprobeExecutable = "ffprobe";
    m_ffmpegExecutable = "ffmpeg";
#else
    m_ffprobeExecutable = QGuiApplication::applicationDirPath() + "/ffprobe" + util.executableBinEnding();
    m_ffmpegExecutable = QGuiApplication::applicationDirPath() + "/ffmpeg" + util.executableBinEnding();
#endif
// We use system ffmpeg on linux
#ifndef Q_OS_LINUX
    if (!QFileInfo::exists(m_ffprobeExecutable)) {
        qFatal("FFPROBE executable not found!");
    }

    if (!QFileInfo::exists(m_ffmpegExecutable)) {
        qFatal("FFMPEG executable not found!");
    }
#endif
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

    Util util;
    emit processOutput("ffprobe " + util.toString(args));

    emit createWallpaperStateChanged(Import::State::AnalyseVideo);

    const QString ffmpegOut = waitForFinished(args, QProcess::SeparateChannels, Executable::FFPROBE);
    qInfo() << ffmpegOut;

    emit createWallpaperStateChanged(Import::State::AnalyseVideoFinished);

    auto obj = util.parseQByteArrayToQJsonObject(QByteArray::fromStdString(ffmpegOut.toStdString()));

    if (!obj) {
        QString error = ffmpegOut;
        qWarning() << "Error parsing FFPROBE json output:" << error << "\n Args: " << args;

        emit processOutput(ffmpegOut);
        emit processOutput("Error parsing FFPROBE json output");
        emit createWallpaperStateChanged(Import::State::AnalyseVideoError);
        return false;
    }

    if (obj->empty()) {
        qCritical() << "Error! File could not be parsed.";
        emit processOutput("Error! File could not be parsed.");
        emit createWallpaperStateChanged(Import::State::AnalyseVideoError);
        return false;
    }
    return analyzeVideo(obj.value());
}

/*!
    \brief .
    {"programs":[],"streams":[{"nb_read_frames":"480"}]}
*/
bool CreateImportVideo::analyzeWebmReadFrames(const QJsonObject& obj)
{

    emit createWallpaperStateChanged(Import::State::AnalyseVideo);

    // Number of frames is a string for some reason...
    if (!obj.value("streams").isArray()) {
        qWarning() << "Error not valid json strucutre!" << obj;
        return false;
    }

    const QJsonArray streams = obj.value("streams").toArray();
    const QJsonObject firstStream = streams.first().toObject();
    qInfo() << "streams:" << streams;

    for (const auto& stream : streams) {
        QString codec_type = stream.toObject().value("codec_type").toString();
        if (codec_type == "audio") {
            m_skipAudio = false;
        } else {
            m_skipAudio = true;
        }
    }

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
    // Get framerate
    const QJsonArray streams = obj.value("streams").toArray();
    if (streams.empty()) {
        qDebug() << "Error container does not have any video streams";
        emit processOutput("Error container does not have any video streams");
        return false;
    }

    const QJsonObject firstStream = streams.first().toObject();
    const QString codecName = firstStream.value("codec_name").toVariant().toString();

    // It is not that important to check for all codecs,
    // we just need a check for the important once to skip
    // import convertion if it is the same codec.
    if (codecName == "vp8") {
        m_sourceCodec = Video::VideoCodec::VP8;
    } else if (codecName == "vp9") {
        m_sourceCodec = Video::VideoCodec::VP9;
    } else if (codecName == "av1") {
        m_sourceCodec = Video::VideoCodec::AV1;
    } else if (codecName == "h264") {
        m_sourceCodec = Video::VideoCodec::H264;
    } else if (codecName == "hevc") {
        m_sourceCodec = Video::VideoCodec::H265; // HEVC is H.265
    } else {
        m_sourceCodec = Video::VideoCodec::Unknown;
    }

    if (m_sourceCodec == Video::VideoCodec::VP8 || m_sourceCodec == Video::VideoCodec::VP9) {
        return analyzeWebmReadFrames(obj);
    }

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
        emit createWallpaperStateChanged(Import::State::AnalyseVideoHasNoVideoStreamError);
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
        emit createWallpaperStateChanged(Import::State::AnalyseVideoError);
        return false;
    }

    QJsonObject objFormat = obj.value("format").toObject();

    // Get video length
    bool okParseDuration = false;
    const float tmpLength = objFormat.value("duration").toVariant().toFloat(&okParseDuration);

    if (!okParseDuration) {
        qDebug() << "Error parsing video length. Is this really a valid video File?";
        emit processOutput("Error parsing video length. Is this really a valid video File?");
        emit createWallpaperStateChanged(Import::State::AnalyseVideoError);
        return false;
    }

    m_length = static_cast<int>(tmpLength);

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

    emit createWallpaperStateChanged(Import::State::ConvertingPreviewVideo);

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
    emit processOutput("ffmpeg " + Util().toString(args));

    const QString ffmpegOut = waitForFinished(args);
    const QFile previewVideo(m_exportPath + "/preview.webm");
    if (!previewVideo.exists() || !(previewVideo.size() > 0)) {
        emit createWallpaperStateChanged(Import::State::ConvertingPreviewVideoError);
        return false;
    }

    emit processOutput(ffmpegOut);

    emit createWallpaperStateChanged(Import::State::ConvertingPreviewVideoFinished);

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

    emit createWallpaperStateChanged(Import::State::ConvertingPreviewGif);

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
    emit processOutput("ffmpeg " + Util().toString(args));

    const QString ffmpegOut = waitForFinished(args);

    if (!ffmpegOut.isEmpty()) {
        const QFile previewGif(m_exportPath + "/preview.gif");
        if (!previewGif.exists() || !(previewGif.size() > 0)) {
            emit createWallpaperStateChanged(Import::State::ConvertingPreviewGifError);
            return false;
        }
    }

    emit processOutput(ffmpegOut);
    emit createWallpaperStateChanged(Import::State::ConvertingPreviewGifFinished);

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

    emit createWallpaperStateChanged(Import::State::ConvertingPreviewImageThumbnail);

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

    emit processOutput("ffmpeg " + Util().toString(args));

    const QString ffmpegOut = waitForFinished(args);
    if (!ffmpegOut.isEmpty()) {
        const QFile previewImg(m_exportPath + "/previewThumbnail.jpg");
        if (!previewImg.exists() || !(previewImg.size() > 0)) {
            emit createWallpaperStateChanged(Import::State::ConvertingPreviewImageThumbnailError);
            return false;
        }
    }

    emit processOutput(ffmpegOut);
    emit createWallpaperStateChanged(Import::State::ConvertingPreviewImageThumbnailFinished);

    return true;
}

/*!
  \brief .
*/
bool CreateImportVideo::createWallpaperImagePreview()
{

    emit createWallpaperStateChanged(Import::State::ConvertingPreviewImage);

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

    emit processOutput("ffmpeg " + Util().toString(args));
    const QString ffmpegOut = waitForFinished(args);
    if (!ffmpegOut.isEmpty()) {
        const QFile previewImg(m_exportPath + "/preview.jpg");
        if (!previewImg.exists() || !(previewImg.size() > 0)) {
            emit createWallpaperStateChanged(Import::State::ConvertingPreviewImageError);
            return false;
        }
    }

    emit processOutput(ffmpegOut);
    emit createWallpaperStateChanged(Import::State::ConvertingPreviewImageFinished);

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
    const QFileInfo sourceFile(m_videoPath);

    if (m_sourceCodec == m_targetCodec) {
        qInfo() << "Skip video convert because they are the same";
        if (!QFile::copy(sourceFile.absoluteFilePath(), m_exportPath + "/" + sourceFile.fileName())) {
            qDebug() << "Could not copy" << sourceFile.absoluteFilePath() << " to " << m_exportPath;
            return false;
        }
        emit createWallpaperStateChanged(Import::State::Finished);
        return true;
    }

    emit createWallpaperStateChanged(Import::State::ConvertingVideo);

    connect(m_process.get(), &QProcess::readyReadStandardOutput, this, [&]() {
        QString tmpOut = m_process->readAllStandardOutput();
        qInfo() << tmpOut;
        if (tmpOut.contains("Conversion failed!")) {
            emit createWallpaperStateChanged(Import::State::ConvertingVideoError);
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

    QString targetCodec;
    QString targetFileEnding;
    if (m_targetCodec == Video::VideoCodec::VP8) {
        targetCodec = "libvpx";
        targetFileEnding = ".webm";
    } else if (m_targetCodec == Video::VideoCodec::VP9) {
        targetCodec = "libvpx-vp9";
        targetFileEnding = ".webm";
    } else if (m_targetCodec == Video::VideoCodec::AV1) {
        targetCodec = "libaom-av1";
        targetFileEnding = ".mkv";
    } else if (m_targetCodec == Video::VideoCodec::H264) {
        targetFileEnding = ".mp4";
        if (QOperatingSystemVersion::currentType() == QOperatingSystemVersion::Windows) {
            targetCodec = "h264_mf";
        } else {
            targetCodec = "libx264";
        }
    }

    QStringList args;
    args.append("-hide_banner");
    args.append("-y");
    args.append("-stats");
    args.append("-i");
    args.append(m_videoPath);
    args.append("-c:v");
    args.append(targetCodec);
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
    args.append(targetCodec);
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
    const QString convertedFileAbsolutePath { m_exportPath + "/" + sourceFile.completeBaseName() + targetFileEnding };
    args.append(convertedFileAbsolutePath);

    const QString ffmpegOutput = waitForFinished(args);

    QFile video(convertedFileAbsolutePath);
    if (!video.exists() || !(video.size() > 0)) {
        qDebug() << convertedFileAbsolutePath << ffmpegOutput << video.exists() << video.size();
        emit createWallpaperStateChanged(Import::State::ConvertingVideoError);
        return false;
    }

    emit createWallpaperStateChanged(Import::State::ConvertingVideoFinished);

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

    emit createWallpaperStateChanged(Import::State::ConvertingAudio);

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
            emit createWallpaperStateChanged(Import::State::ConvertingAudioError);
            return false;
        }
    }

    emit processOutput(tmpErrImg);
    emit createWallpaperStateChanged(Import::State::ConvertingAudioFinished);

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
    QObject::connect(m_process.get(), &QProcess::errorOccurred, this, [=, this](QProcess::ProcessError error) {
        qDebug() << "error enum val = " << error << m_process->errorString();
        emit createWallpaperStateChanged(Import::State::AnalyseVideoError);
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

#ifdef Q_OS_MACOS
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
    m_process->setWorkingDirectory(QGuiApplication::applicationDirPath());
    m_process->start();

    qInfo() << m_process->workingDirectory() << m_process->program() << m_process->arguments();

    while (!m_process->waitForFinished(10)) // Wake up every 10ms and check if we must exit
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

#include "moc_createimportvideo.cpp"
