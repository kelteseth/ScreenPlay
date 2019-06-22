#include "createimportvideo.h"

namespace ScreenPlay {

/*!
    \class CreateImportVideo
    \brief This class imports (copies) and creates wallaper previews. This
           class only exsits as long as the user creates a wallpaper and gets
           destroyed if the creation was successful or not.

           The state get propagated via createWallpaperStateChanged(ImportVideoState state);

    \todo
            - Maybe: Replace with QThread to avoid running QCoreApplication::processEvents();?
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

    auto cleanup = qScopeGuard([this] {
        qDebug() << "ABORT";
        this->requestInterruption();
        emit canceled();
    });

    if (!createWallpaperInfo())
        return;

    if (!createWallpaperVideoPreview())
        return;

    if (!createWallpaperGifPreview())
        return;

    if (!extractWallpaperAudio())
        return;

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
    QScopedPointer<QProcess> pro(new QProcess());
    pro.data()->setArguments(args);

#ifdef Q_OS_WIN
    pro.data()->setProgram(QApplication::applicationDirPath() + "/ffprobe.exe");
#endif
#ifdef Q_OS_MACOS
    pro.data()->setProgram(QApplication::applicationDirPath() + "/ffprobe");
#endif

    pro.data()->start();
    emit createWallpaperStateChanged(ImportVideoState::AnalyseVideo);
    while (!pro.data()->waitForFinished(100)) {
        if (QThread::currentThread()->isInterruptionRequested()) {
            qDebug() << "Interrupt thread";
            pro.data()->terminate();
            if (!pro.data()->waitForFinished(1000)) {
                pro.data()->kill();
            }
            break;
        }
        QCoreApplication::processEvents();
    }
    emit createWallpaperStateChanged(ImportVideoState::AnalyseVideoFinished);
    QJsonObject obj;
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(pro.data()->readAll(), &err);
    if (err.error != QJsonParseError::NoError) {
        emit processOutput("Error parsing ffmpeg json output");
        emit createWallpaperStateChanged(ImportVideoState::AnalyseVideoError);
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
        emit createWallpaperStateChanged(ImportVideoState::AnalyseVideoError);
        return false;
    }

    int length = 0;
    length = static_cast<int>(tmpLength);
    m_length = length;

    // Get framerate
    QJsonArray arrSteams = obj.value("streams").toArray();
    if (arrSteams.empty()) {
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
    double value1 = static_cast<double>(avgFrameRateList.at(0).toInt());
    double value2 = static_cast<double>(avgFrameRateList.at(1).toInt());

    framerate = qCeil(value1 / value2);
    m_framerate = framerate;

    return true;
}

bool CreateImportVideo::createWallpaperVideoPreview()
{

    QStringList args;
    args.append("-loglevel");
    args.append("error");
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
    QScopedPointer<QProcess> proConvertPreviewWebM(new QProcess());

    proConvertPreviewWebM.data()->setArguments(args);
#ifdef Q_OS_WIN
    proConvertPreviewWebM.data()->setProgram(QApplication::applicationDirPath() + "/ffmpeg.exe");
#endif
#ifdef Q_OS_MACOS
    proConvertPreviewMP4.data()->setProgram(QApplication::applicationDirPath() + "/ffmpeg");
#endif
    emit createWallpaperStateChanged(ImportVideoState::ConvertingPreviewVideo);

    proConvertPreviewWebM.data()->start();
    while (!proConvertPreviewWebM.data()->waitForFinished(100)) //Wake up every 100ms and check if we must exit
    {
        if (QThread::currentThread()->isInterruptionRequested()) {
            qDebug() << "Interrupt thread";
            proConvertPreviewWebM.data()->terminate();
            if (!proConvertPreviewWebM.data()->waitForFinished(1000)) {
                proConvertPreviewWebM.data()->kill();
            }
            break;
        }
        QCoreApplication::processEvents();
    }

    QString tmpErr = proConvertPreviewWebM.data()->readAllStandardError();
    if (!tmpErr.isEmpty()) {
        QFile previewVideo(m_exportPath + "/preview.webm");
        if (!previewVideo.exists() && !(previewVideo.size() > 0)) {
            emit processOutput(tmpErr);
            emit createWallpaperStateChanged(ImportVideoState::ConvertingPreviewVideoError);
            return false;
        }
    }

    //this->processOutput(proConvertPreviewWebM.data()->readAll());
    proConvertPreviewWebM.data()->close();

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

    QScopedPointer<QProcess> proConvertGif(new QProcess());
    proConvertGif.data()->setArguments(args);
#ifdef Q_OS_WIN
    proConvertGif.data()->setProgram(QApplication::applicationDirPath() + "/ffmpeg.exe");
#endif
#ifdef Q_OS_MACOS
    proConvertGif.data()->setProgram(QApplication::applicationDirPath() + "/ffmpeg");
#endif

    proConvertGif.data()->start();
    while (!proConvertGif.data()->waitForFinished(100)) //Wake up every 100ms and check if we must exit
    {
        if (QThread::currentThread()->isInterruptionRequested()) {
            qDebug() << "Interrupt thread";
            proConvertGif.data()->terminate();
            if (!proConvertGif.data()->waitForFinished(1000)) {
                proConvertGif.data()->kill();
            }
            break;
        }
        QCoreApplication::processEvents();
    }

    QString tmpErrGif = proConvertGif.data()->readAllStandardError();
    if (!tmpErrGif.isEmpty()) {
        QFile previewGif(m_exportPath + "/preview.gif");
        if (!previewGif.exists() && !(previewGif.size() > 0)) {
            emit createWallpaperStateChanged(ImportVideoState::ConvertingPreviewGifError);
            return false;
        }
    }

    //this->processOutput(proConvertGif.data()->readAll());
    proConvertGif.data()->close();
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
    args.append(m_exportPath + "/preview.png");

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
        if (QThread::currentThread()->isInterruptionRequested()) {
            qDebug() << "Interrupt thread";
            proConvertImage.data()->terminate();
            if (!proConvertImage.data()->waitForFinished(1000)) {
                proConvertImage.data()->kill();
            }
            break;
        }
        QCoreApplication::processEvents();
    }
    QString tmpErrImg = proConvertImage.data()->readAllStandardError();
    if (!tmpErrImg.isEmpty()) {
        QFile previewImg(m_exportPath + "/preview.png");
        if (!previewImg.exists() && !(previewImg.size() > 0)) {
            emit createWallpaperStateChanged(ImportVideoState::ConvertingPreviewImageError);
            return false;
        }
    }

    //this->processOutput(proConvertImage.data()->readAll());
    proConvertImage.data()->close();
    emit createWallpaperStateChanged(ImportVideoState::ConvertingPreviewImageFinished);

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
    proConvertAudio.data()->setArguments(args);
#ifdef Q_OS_WIN
    proConvertAudio.data()->setProgram(QApplication::applicationDirPath() + "/ffmpeg.exe");
#endif
#ifdef Q_OS_MACOS
    pro.data()->setProgram(QApplication::applicationDirPath() + "/ffmpeg");
#endif

    proConvertAudio.data()->start(QIODevice::ReadOnly);
    while (!proConvertAudio.data()->waitForFinished(100)) //Wake up every 100ms and check if we must exit
    {
        if (QThread::currentThread()->isInterruptionRequested()) {
            qDebug() << "Interrupt thread";
            proConvertAudio.data()->terminate();
            if (!proConvertAudio.data()->waitForFinished(1000)) {
                proConvertAudio.data()->kill();
            }
            break;
        }
        QCoreApplication::processEvents();
    }

    QString tmpErrImg = proConvertAudio.data()->readAllStandardError();
    if (!tmpErrImg.isEmpty()) {
        QFile previewImg(m_exportPath + "/audio.mp3");
        if (!previewImg.exists() && !(previewImg.size() > 0)) {
            emit createWallpaperStateChanged(ImportVideoState::ConvertingAudioError);
            return false;
        }
    }

    //this->processOutput(proConvertAudio.data()->readAll());
    proConvertAudio.data()->close();
    emit createWallpaperStateChanged(ImportVideoState::ConvertingAudioFinished);

    return true;
}

}
