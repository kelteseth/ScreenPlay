// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#include "ScreenPlay/createimportvideo.h"
#include "ScreenPlay/gifoptimizer.h"
#include "ScreenPlayCore/util.h"
#include <QGuiApplication>
#include <QLoggingCategory>
#include <cmath>

Q_LOGGING_CATEGORY(createImportVideo, "screenplay.createimportvideo")

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
    m_ffprobeExecutable = Util::ffprobeExecutable();
    m_ffmpegExecutable = Util::ffmpegExecutable();

    if (!Util::isFFmpegBundled())
        return; // system PATH is used; QProcess will surface a startup error if missing

    if (!QFileInfo::exists(m_ffprobeExecutable))
        qFatal("FFPROBE executable not found!");
    if (!QFileInfo::exists(m_ffmpegExecutable))
        qFatal("FFMPEG executable not found!");
}

/*!
  \brief Computes an ffmpeg crop=W:H filter string that extracts the largest centered 16:9 region
         from the source video. Handles wider-than-16:9 (crop sides) and taller-than-16:9
         (square, portrait — crop top/bottom) sources. Returns an empty string if dimensions
         are not yet known.
 */
QString CreateImportVideo::build16x9CropFilter() const
{
    if (m_videoWidth <= 0 || m_videoHeight <= 0)
        return {};

    int cropW, cropH;
    if (m_videoWidth * 9 > m_videoHeight * 16) {
        // Source is wider than 16:9 → keep full height, crop width
        cropH = m_videoHeight;
        cropW = (m_videoHeight * 16) / 9;
    } else {
        // Source is taller than 16:9 (portrait / square) → keep full width, crop height
        cropW = m_videoWidth;
        cropH = (m_videoWidth * 9) / 16;
    }
    // Codec requires even dimensions
    cropW = (cropW / 2) * 2;
    cropH = (cropH / 2) * 2;
    qCInfo(createImportVideo) << "16:9 crop filter: crop=" << cropW << ":" << cropH
                              << "from source" << m_videoWidth << "x" << m_videoHeight;
    return QString("crop=%1:%2").arg(cropW).arg(cropH);
}

/*!
  \brief Detects the container format (WebM/Matroska vs others) by doing a quick FFprobe query.
         This allows us to use the correct detailed analysis method.
 */
bool CreateImportVideo::detectContainerFormat()
{
    QStringList args;
    args.append("-print_format");
    args.append("json");
    args.append("-show_format");
    args.append(m_videoPath);

    Util util;
    const QString ffmpegOut = waitForFinished(args, QProcess::SeparateChannels, Executable::FFPROBE);

    auto obj = util.parseQByteArrayToQJsonObject(QByteArray::fromStdString(ffmpegOut.toStdString()));
    if (!obj) {
        qCWarning(createImportVideo) << "Error parsing FFprobe format detection output";
        return false;
    }

    if (obj->contains("format")) {
        const QJsonObject formatObj = obj->value("format").toObject();
        const QString formatName = formatObj.value("format_name").toString();

        // WebM and Matroska containers both need frame counting
        m_isWebm = formatName.contains("webm", Qt::CaseInsensitive) || formatName.contains("matroska", Qt::CaseInsensitive);

        qCInfo(createImportVideo) << "Container format detected:" << formatName << "-> isWebM/Matroska:" << m_isWebm;
    }

    return true;
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
    // First, do a quick format detection to determine container type
    if (!detectContainerFormat()) {
        return false;
    }

    // Get video info with appropriate command based on container format
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
        args.append("stream=nb_read_frames,r_frame_rate,width,height");
    } else {
        args.append("-show_format");
        args.append("-show_streams");
    }

    args.append(m_videoPath);

    Util util;
    emit processOutput("ffprobe " + util.toString(args));

    emit createWallpaperStateChanged(Import::State::AnalyseVideo);

    const QString ffmpegOut = waitForFinished(args, QProcess::SeparateChannels, Executable::FFPROBE);
    qCInfo(createImportVideo) << ffmpegOut;

    emit createWallpaperStateChanged(Import::State::AnalyseVideoFinished);

    auto obj = util.parseQByteArrayToQJsonObject(QByteArray::fromStdString(ffmpegOut.toStdString()));

    if (!obj) {
        QString error = ffmpegOut;
        qCWarning(createImportVideo) << "Error parsing FFPROBE json output:" << error << "\n Args: " << args;

        emit processOutput(ffmpegOut);
        emit processOutput("Error parsing FFPROBE json output");
        emit createWallpaperStateChanged(Import::State::AnalyseVideoError);
        return false;
    }

    if (obj->empty()) {
        qCCritical(createImportVideo) << "Error! File could not be parsed.";
        emit processOutput("Error! File could not be parsed.");
        emit createWallpaperStateChanged(Import::State::AnalyseVideoError);
        return false;
    }

    // For WebM/Matroska containers, we already have the frame count data
    // so go directly to the specialized analysis method
    if (m_isWebm) {
        return analyzeWebmReadFrames(obj.value());
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
        qCWarning(createImportVideo) << "Error not valid json strucutre!" << obj;
        return false;
    }

    const QJsonArray streams = obj.value("streams").toArray();
    const QJsonObject firstStream = streams.first().toObject();
    qCInfo(createImportVideo) << "streams:" << streams;

    m_skipAudio = true;
    for (const auto& stream : streams) {
        if (stream.toObject().value("codec_type").toString() == "audio") {
            m_skipAudio = false;
            break;
        }
    }

    bool okParseNumberOfFrames { false };
    int numberOfFrames = firstStream.value("nb_read_frames").toString().toInt(&okParseNumberOfFrames);
    if (!okParseNumberOfFrames) {
        qCWarning(createImportVideo) << "Error could not parse nb_read_frames";
        return false;
    }
    m_numberOfFrames = numberOfFrames;
    m_videoWidth = firstStream.value("width").toInt();
    m_videoHeight = firstStream.value("height").toInt();
    qCInfo(createImportVideo) << "Video dimensions (WebM):" << m_videoWidth << "x" << m_videoHeight;

    // "r_frame_rate": "25/1"
    QString frameRate = firstStream.value("r_frame_rate").toString();
    QStringList frameRateList = frameRate.split('/', Qt::SplitBehaviorFlags::SkipEmptyParts);
    if (frameRateList.length() != 2) {
        qCWarning(createImportVideo) << "Error could not parse streams with length: " << frameRateList.length();
        emit processOutput("Error could not parse streams with length: " + QString::number(frameRateList.length()));
        return false;
    }

    const double first = frameRateList.at(0).toDouble();
    const double second = frameRateList.at(1).toDouble();

    if (second == 0.0) {
        qCWarning(createImportVideo) << "Error: frame rate denominator is zero";
        return false;
    }

    m_framerate = std::ceil(first / second);

    // If the video is to short
    m_smallVideo = m_numberOfFrames < (m_framerate * 5);
    m_length = std::ceil(m_numberOfFrames / m_framerate);

    qCInfo(createImportVideo) << m_numberOfFrames << m_framerate << m_smallVideo << m_length;
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
        qCDebug(createImportVideo) << "Error container does not have any video streams";
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
    } else if (codecName == "mjpeg") {
        m_sourceCodec = Video::VideoCodec::MJPEG;
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
        qCDebug(createImportVideo) << "Error! File has no video Stream!";
        emit processOutput("Error! File has no video Stream!");
        emit createWallpaperStateChanged(Import::State::AnalyseVideoHasNoVideoStreamError);
        return false;
    }

    if (!hasAudioStream)
        m_skipAudio = true;

    m_videoWidth = videoStream.value("width").toInt();
    m_videoHeight = videoStream.value("height").toInt();
    qCInfo(createImportVideo) << "Video dimensions:" << m_videoWidth << "x" << m_videoHeight;

    // Number of frames is a string for some reason...
    bool okParseNumberOfFrames { false };
    m_numberOfFrames = videoStream.value("nb_frames").toString().toInt(&okParseNumberOfFrames);

    // FFmpeg 8+ doesn't always provide nb_frames for MKV files, so calculate from duration and frame rate
    if (!okParseNumberOfFrames) {
        qCInfo(createImportVideo) << "nb_frames not available, calculating from duration and frame rate";

        QJsonObject objFormat = obj.value("format").toObject();

        // Get video length
        bool okParseDuration = false;
        const float tmpLength = objFormat.value("duration").toVariant().toFloat(&okParseDuration);

        if (!okParseDuration) {
            qCDebug(createImportVideo) << "Error parsing video length. Is this really a valid video File?";
            emit processOutput("Error parsing video length. Is this really a valid video File?");
            emit createWallpaperStateChanged(Import::State::AnalyseVideoError);
            return false;
        }

        m_length = static_cast<int>(tmpLength);

        // Get frame rate to calculate number of frames
        const QString avgFrameRate = videoStream.value("avg_frame_rate").toVariant().toString();
        const QStringList avgFrameRateList = avgFrameRate.split('/', Qt::SplitBehaviorFlags::SkipEmptyParts);

        if (avgFrameRateList.length() != 2) {
            qCDebug(createImportVideo) << "Error could not parse frame rate with length: " << avgFrameRateList.length();
            emit processOutput("Error could not parse frame rate with length: " + QString::number(avgFrameRateList.length()));
            return false;
        }

        const double first = avgFrameRateList.at(0).toDouble();
        const double second = avgFrameRateList.at(1).toDouble();

        if (second == 0.0) {
            qCWarning(createImportVideo) << "Error: frame rate denominator is zero";
            return false;
        }

        m_framerate = std::ceil(first / second);

        // Calculate number of frames from duration and frame rate
        m_numberOfFrames = static_cast<int>(std::ceil(tmpLength * m_framerate));

        qCInfo(createImportVideo) << "Calculated values - Duration:" << m_length << "Frame rate:" << m_framerate << "Number of frames:" << m_numberOfFrames;

        // If the video is too short
        m_smallVideo = m_numberOfFrames < (m_framerate * 5);

        return true;
    }

    QJsonObject objFormat = obj.value("format").toObject();

    // Get video length
    bool okParseDuration = false;
    const float tmpLength = objFormat.value("duration").toVariant().toFloat(&okParseDuration);

    if (!okParseDuration) {
        qCDebug(createImportVideo) << "Error parsing video length. Is this really a valid video File?";
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
        qCDebug(createImportVideo) << "Error could not parse streams with length: " << avgFrameRateList.length();
        emit processOutput("Error could not parse streams with length: " + QString::number(avgFrameRateList.length()));
        return false;
    }

    const double first = avgFrameRateList.at(0).toDouble();
    const double second = avgFrameRateList.at(1).toDouble();

    if (second == 0.0) {
        qCWarning(createImportVideo) << "Error: frame rate denominator is zero";
        return false;
    }

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
        qCInfo(createImportVideo) << "Regular video length detected!";
        args.append("-vf");
        {
            const QString crop = build16x9CropFilter();
            const QString cropPart = crop.isEmpty() ? QString() : ("," + crop);
            args.append("select='not(mod(n," + QString::number((m_length / 5)) + "))',setpts=N/FRAME_RATE/TB" + cropPart + ",scale=854:480");
        }
    }
    // Disable audio
    args.append("-an");
    args.append(m_exportPath + "/preview.webm");
    emit processOutput("ffmpeg " + Util().toString(args));

    // Use MergedChannels so ffmpeg stderr (stats, errors) is captured and visible in QML logs
    const QString ffmpegOut = waitForFinished(args, QProcess::MergedChannels);
    emit processOutput(ffmpegOut);

    const QFile previewVideo(m_exportPath + "/preview.webm");
    if (!previewVideo.exists() || !(previewVideo.size() > 0)) {
        emit createWallpaperStateChanged(Import::State::ConvertingPreviewVideoError);
        return false;
    }

    emit createWallpaperStateChanged(Import::State::ConvertingPreviewVideoFinished);

    return true;
}

/*!
  \brief Starts ffmpeg and tries to convert the given video to a WebP preview.
  Returns \c false if :
  \list
    \li Cannot convert the video
    \li Generally broken.
   \endlist
 */
bool CreateImportVideo::createWallpaperWebpPreview()
{
    emit createWallpaperStateChanged(Import::State::ConvertingPreviewWebp);

    QStringList args;
    args.append("-y");
    args.append("-stats");
    // Limit to 3 seconds to stay under Steam's 1MB additional preview limit.
    if (!m_smallVideo) {
        args.append("-t");
        args.append("3");
    }
    args.append("-i");
    if (m_isWebm) {
        args.append(m_videoPath);
    } else {
        args.append(m_exportPath + "/preview.webm");
    }

    // Convert to WebP animated image, cropped to 16:9 at 636x358.
    // 636 width matches the Steam web preview display size.
    // The input (preview.webm) is already ~5s, but we add -t 5 above as a
    // safety net for the m_isWebm path where we read from the original.
    // Lower quality (40) and higher compression (6) to stay under Steam's
    // 1MB AddItemPreviewFile limit.
    // IMPORTANT: when the input is preview.webm it is already 16:9 (854x480),
    // so do NOT apply crop again - only scale. Crop is only needed when reading
    // directly from the original source video (m_isWebm path).
    args.append("-vf");
    if (m_isWebm) {
        const QString crop = build16x9CropFilter();
        const QString cropPart = crop.isEmpty() ? QString() : (crop + ",");
        args.append(cropPart + "fps=12,scale=636:358");
    } else {
        // preview.webm is already cropped - just resample fps and scale down
        args.append("fps=12,scale=636:358");
    }
    args.append("-c:v");
    args.append("libwebp");
    args.append("-lossless");
    args.append("0");
    args.append("-compression_level");
    args.append("6");
    args.append("-quality");
    args.append("40");
    args.append("-preset");
    args.append("default");
    args.append("-loop");
    args.append("0");
    args.append(m_exportPath + "/preview.webp");

    emit processOutput("ffmpeg " + Util().toString(args));

    // Use MergedChannels so ffmpeg stderr (errors, stats) appears in QML logs
    const QString ffmpegOut = waitForFinished(args, QProcess::MergedChannels);
    emit processOutput(ffmpegOut);

    const QFile previewWebp(m_exportPath + "/preview.webp");
    if (!previewWebp.exists() || !(previewWebp.size() > 0)) {
        emit createWallpaperStateChanged(Import::State::ConvertingPreviewWebpError);
        return false;
    }

    emit createWallpaperStateChanged(Import::State::ConvertingPreviewWebpFinished);

    return true;
}

/*!
  \brief Creates an optimised animated GIF preview via \c GifOptimizer.
         Uses the already-generated preview.webm (5 s, 854x480, 16:9) as
         input and adaptively reduces quality until the output fits under
         Steam's 1 MB AddItemPreviewFile limit.
  Returns \c false if the optimisation fails or no tier fits.
 */
bool CreateImportVideo::createWallpaperGifPreview()
{
    emit createWallpaperStateChanged(Import::State::ConvertingPreviewGif);

    GifOptimizer optimizer(m_ffmpegExecutable, m_interrupt);
    connect(&optimizer, &GifOptimizer::processOutput,
        this, &CreateImportVideo::processOutput);

    const QString inputPath = m_exportPath + "/preview.webm";
    const QString outputPath = m_exportPath + "/preview.gif";

    auto result = optimizer.optimize(inputPath, outputPath);

    if (!result) {
        emit processOutput(result.error().message);
        emit createWallpaperStateChanged(Import::State::ConvertingPreviewGifError);
        return false;
    }

    emit processOutput(QString("GIF created: %1 bytes, %2x%3 @ %4fps, %5 colors, pass %6")
            .arg(result->fileSize)
            .arg(result->width)
            .arg(result->height)
            .arg(result->fps)
            .arg(result->maxColors)
            .arg(result->passUsed));

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
    args.append("-vf");
    {
        const QString crop = build16x9CropFilter();
        const QString cropPart = crop.isEmpty() ? QString() : (crop + ",");
        if (m_smallVideo) {
            // Select first frame https://stackoverflow.com/a/44073745/12619313
            args.append("select=eq(n\\,0)," + cropPart + "scale=320:180");
        } else {
            args.append(cropPart + "scale=320:180");
        }
    }
    args.append(m_exportPath + "/previewThumbnail.jpg");

    emit processOutput("ffmpeg " + Util().toString(args));

    const QString ffmpegOut = waitForFinished(args);
    emit processOutput(ffmpegOut);

    const QFile previewImg(m_exportPath + "/previewThumbnail.jpg");
    if (!previewImg.exists() || !(previewImg.size() > 0)) {
        emit createWallpaperStateChanged(Import::State::ConvertingPreviewImageThumbnailError);
        return false;
    }

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
    // Order of arguments is important
    if (!m_smallVideo) {
        args.append("-vframes");
        args.append("1");
    }
    args.append("-q:v");
    args.append("2");
    args.append("-vf");
    {
        // Crop to 16:9 and scale to 854x480 for a consistent preview size.
        const QString crop = build16x9CropFilter();
        const QString cropPart = crop.isEmpty() ? QString() : (crop + ",");
        if (m_smallVideo) {
            // Select first frame https://stackoverflow.com/a/44073745/12619313
            args.append("select=eq(n\\,0)," + cropPart + "scale=854:480");
        } else {
            args.append(cropPart + "scale=854:480");
        }
    }
    args.append(m_exportPath + "/preview.jpg");

    emit processOutput("ffmpeg " + Util().toString(args));
    const QString ffmpegOut = waitForFinished(args);
    emit processOutput(ffmpegOut);

    const QFile previewImg(m_exportPath + "/preview.jpg");
    if (!previewImg.exists() || !(previewImg.size() > 0)) {
        emit createWallpaperStateChanged(Import::State::ConvertingPreviewImageError);
        return false;
    }

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

    // If target codec is NoConversion, it means no conversion needed - just copy the original
    if (m_targetCodec == Video::VideoCodec::NoConversion) {
        qCInfo(createImportVideo) << "No conversion needed, copying original file";

        const QString targetFilePath = m_exportPath + "/" + sourceFile.fileName();

        if (!QFile::copy(sourceFile.absoluteFilePath(), targetFilePath)) {
            qCDebug(createImportVideo) << "Could not copy" << sourceFile.absoluteFilePath() << " to " << targetFilePath;
            return false;
        }
        emit createWallpaperStateChanged(Import::State::Finished);
        return true;
    }

    if (m_sourceCodec == m_targetCodec) {
        qCInfo(createImportVideo) << "Skip video convert because they are the same";

        // Determine the target file extension based on the target codec
        QString targetFileEnding;
        if (m_targetCodec == Video::VideoCodec::VP8) {
            targetFileEnding = ".webm";
        } else if (m_targetCodec == Video::VideoCodec::VP9) {
            targetFileEnding = ".webm";
        } else if (m_targetCodec == Video::VideoCodec::AV1) {
            targetFileEnding = ".mkv";
        } else if (m_targetCodec == Video::VideoCodec::H264) {
            targetFileEnding = ".mp4";
        } else if (m_targetCodec == Video::VideoCodec::H265) {
            targetFileEnding = ".mp4";
        } else {
            // Default to original extension if unknown codec
            targetFileEnding = "." + sourceFile.suffix();
        }

        const QString targetFilePath = m_exportPath + "/" + sourceFile.completeBaseName() + targetFileEnding;

        if (!QFile::copy(sourceFile.absoluteFilePath(), targetFilePath)) {
            qCDebug(createImportVideo) << "Could not copy" << sourceFile.absoluteFilePath() << " to " << targetFilePath;
            return false;
        }
        emit createWallpaperStateChanged(Import::State::Finished);
        return true;
    }

    emit createWallpaperStateChanged(Import::State::ConvertingVideo);

    connect(m_process.get(), &QProcess::readyReadStandardOutput, this, [&]() {
        QString tmpOut = m_process->readAllStandardOutput();
        qCInfo(createImportVideo) << tmpOut;
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
    QString targetFormat;

    if (m_targetCodec == Video::VideoCodec::VP8) {
        targetCodec = "libvpx";
        targetFileEnding = ".webm";
        targetFormat = "webm";
    } else if (m_targetCodec == Video::VideoCodec::VP9) {
        targetCodec = "libvpx-vp9";
        targetFileEnding = ".webm";
        targetFormat = "webm";
    } else if (m_targetCodec == Video::VideoCodec::AV1) {
        if (QOperatingSystemVersion::currentType() == QOperatingSystemVersion::Windows) {
            targetCodec = "libsvtav1";
        } else {
            targetCodec = "libaom-av1";
        }
        targetFileEnding = ".mkv";
        targetFormat = "matroska";
    } else if (m_targetCodec == Video::VideoCodec::H264) {
        targetFileEnding = ".mp4";
        targetFormat = "mp4";
        // if (QOperatingSystemVersion::currentType() == QOperatingSystemVersion::Windows) {
        //     targetCodec = "h264_mf";
        // } else {
        targetCodec = "libx264";
        // }
    }

    const QString convertedFileAbsolutePath { m_exportPath + "/" + sourceFile.completeBaseName() + targetFileEnding };
    const int threads = QThread::idealThreadCount();

    QStringList args;
    args.append("-hide_banner");
    args.append("-y");
    args.append("-stats");
    args.append("-i");
    args.append(m_videoPath);
    args.append("-c:v");
    args.append(targetCodec);

    if (m_targetCodec == Video::VideoCodec::VP8
        || m_targetCodec == Video::VideoCodec::VP9
        || m_targetCodec == Video::VideoCodec::AV1) {
        // Single-pass CRF: constant quality, no bitrate cap needed for local wallpapers.
        // -b:v 0 tells libvpx to use pure CRF mode (no bitrate ceiling).
        args.append("-b:v");
        args.append("0");
        args.append("-crf");
        args.append(QString::number(m_quality));
        args.append("-threads");
        args.append(QString::number(threads));

        if (m_targetCodec == Video::VideoCodec::VP9) {
            // -speed 2: good quality/speed tradeoff (0=slowest/best, 5=fastest)
            args.append("-speed");
            args.append("2");
            // Enable tile-column parallelism so threads are actually used.
            // tile-columns=2 → 4 tile columns for ≥1080p, good for 4-16 threads.
            args.append("-tile-columns");
            args.append("2");
            args.append("-row-mt");
            args.append("1");
            args.append("-auto-alt-ref");
            args.append("1");
            args.append("-lag-in-frames");
            args.append("25");
        } else if (m_targetCodec == Video::VideoCodec::VP8) {
            args.append("-speed");
            args.append("2");
        }

        qCInfo(createImportVideo) << "threads" << threads << "m_quality" << m_quality;

        if (!m_skipAudio) {
            args.append("-c:a");
            args.append("libopus");
            args.append("-b:a");
            args.append("128k");
        } else {
            args.append("-an");
        }

        args.append(convertedFileAbsolutePath);
    } else {
        // Single-pass CRF encoding for H.264.
        // m_quality uses the VP9-style scale (0 = best, 63 = worst).
        // Map linearly to the useful H.264 CRF range: 17 (visually lossless) – 28 (acceptable).
        // No bitrate cap needed for local wallpapers (same rationale as VP9 path).
        int h264Crf = 17 + static_cast<int>(std::round(m_quality * 11.0 / 63.0));
        args.append("-crf");
        args.append(QString::number(h264Crf));

        args.append("-preset");
        args.append("slow");

        args.append("-pix_fmt");
        args.append("yuv420p");

        args.append("-threads");
        args.append(QString::number(threads));
        qCInfo(createImportVideo) << "threads" << threads << "m_quality" << m_quality << "h264Crf" << h264Crf;

        if (!m_skipAudio) {
            args.append("-c:a");
            args.append("aac");
            args.append("-b:a");
            args.append("192k");
        } else {
            args.append("-an");
        }

        args.append(convertedFileAbsolutePath);
    }

    const QString ffmpegOutput = waitForFinished(args);

    QFile video(convertedFileAbsolutePath);
    if (!video.exists() || !(video.size() > 0)) {
        qCDebug(createImportVideo) << convertedFileAbsolutePath << ffmpegOutput << video.exists() << video.size();
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
    emit processOutput(tmpErrImg);

    const QFile audioFile(m_exportPath + "/audio.mp3");
    if (!audioFile.exists() || !(audioFile.size() > 0)) {
        qCDebug(createImportVideo) << args;
        qCDebug(createImportVideo) << tmpErrImg;
        emit createWallpaperStateChanged(Import::State::ConvertingAudioError);
        return false;
    }

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
        qCDebug(createImportVideo) << "error enum val = " << error << m_process->errorString();
        emit createWallpaperStateChanged(Import::State::AnalyseVideoError);
        m_process->terminate();
        if (!m_process->waitForFinished(1000)) {
            m_process->kill();
        }
    });

    QObject::connect(m_process.get(), QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
        [=](int exitCode, QProcess::ExitStatus exitStatus) {
            qCInfo(createImportVideo) << "Process finished with exit code: " << exitCode;
            if (exitCode != 0)
                qCWarning(createImportVideo) << "Process finished with exit code: " << exitCode << " exitStatus:" << exitStatus;
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
        qCCritical(createImportVideo) << "Unable to change permission " << m_process->program() << " to be exectuable";
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
