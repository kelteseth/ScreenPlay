#include "ScreenPlayCore/projectfile.h"
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(coreProject, "screenplay.core.project")

namespace ScreenPlay {

bool ProjectFile::init()
{
    if (!isValid())
        return false;

    Util util;
    const auto jsonObjOpt = util.openJsonFileToObject(projectJsonFilePath.absoluteFilePath());
    QDir folder = projectJsonFilePath.dir();
    folderName = folder.dirName();
    QFileInfo folderInfo(folder.path());
    lastModified = folderInfo.birthTime();
    if (folderInfo.birthTime().date() == QDateTime::currentDateTime().date())
        isNew = true;

    if (!jsonObjOpt.has_value())
        return false;

    const QJsonObject& obj = jsonObjOpt.value();
    if (obj.isEmpty())
        return false;

    if (!obj.contains("title"))
        return false;
    title = obj.value("title").toString();

    if (!obj.contains("type"))
        return false;

    auto typeParsed = util.getInstalledTypeFromString(obj.value("type").toString());
    if (!typeParsed.has_value()) {
        qCWarning(coreProject) << "Type could not parsed from string: " << obj.value("type").toString();
        return false;
    }
    type = typeParsed.value();

    // File is required. Website Wallpaper doe not have a file, but a url
    if (!obj.contains("file") && type != ScreenPlay::ContentTypes::InstalledType::WebsiteWallpaper)
        return false;

    if (type != ScreenPlay::ContentTypes::InstalledType::WebsiteWallpaper) {
        file = obj.value("file").toString();

        if (type == ScreenPlay::ContentTypes::InstalledType::GodotWallpaper) {
            QFileInfo fileInfo(folder.path() + "/wallpaper.tscn");
            if (!fileInfo.exists()) {
                qCCritical(coreProject) << "Requested file:" << fileInfo.absoluteFilePath() << "does not exist!";
                return false;
            }
        } else {
            QFileInfo fileInfo(folder.path() + "/" + file);
            if (!fileInfo.exists()) {
                qCInfo(coreProject) << "Requested file:" << fileInfo.absoluteFilePath() << "does not exist!";
                return false;
            }
        }
    }

    // Optional:
    if (!obj.contains("description"))
        description = obj.value("description").toString();

    if (obj.contains("previewGIF"))
        previewGIF = obj.value("previewGIF").toString();

    if (obj.contains("previewWEBP"))
        previewWebP = obj.value("previewWEBP").toString();

    if (obj.contains("url"))
        url = QUrl(obj.value("url").toString());

    // Steam Workshop IDs (PublishedFileId_t) are uint64 values that
    // regularly exceed INT_MAX. The JSON field is often serialised as a
    // string (e.g. "2968713114"). We store it as a QVariant via
    // toVariant() so the original representation is preserved and can
    // later be converted with toULongLong().
    if (obj.contains("workshopid"))
        publishedFileID = obj.value("workshopid").toVariant();

    if (obj.contains("previewThumbnail")) {
        preview = obj.value("previewThumbnail").toString();
    } else {
        if (obj.contains("preview"))
            preview = obj.value("preview").toString();
    }

    if (obj.contains("tags")) {
        if (obj.value("tags").isArray()) {
            auto tagArray = obj.value("tags").toArray();
            if (tagArray.size() > 0) {
                for (const auto& tag : tagArray) {
                    tags.append(tag.toString());
                }
            }
        }
    }

    if (type == ContentTypes::InstalledType::GifWallpaper) {
        // Prefer static preview images over the GIF itself.
        // Only fall back to the GIF if no preview/previewThumbnail was set.
        if (preview.isEmpty())
            preview = previewGIF;
    }
    if (type == ContentTypes::InstalledType::WebsiteWallpaper) {
        if (url.isEmpty()) {
            qWarning() << "No url was specified for a websiteWallpaper!";
            return false;
        }
    }

    searchType = util.getSearchTypeFromInstalledType(type);

    if (obj.contains("codec")) {
        if (auto videoCodecOpt = util.getVideoCodecFromString(obj.value("codec").toString())) {
            videoCodec = videoCodecOpt.value();
        } else {
            qWarning("Invalid videoCodec was specified inside the json object!");
        }
    } else if (type == ScreenPlay::ContentTypes::InstalledType::VideoWallpaper) {
        // qWarning("No videoCodec was specified inside the json object!");
        if (file.endsWith(".mp4")) {
            videoCodec = ScreenPlay::Video::VideoCodec::H264;
            // qWarning("Eyeball to h264 because of .mp4");
        } else if (file.endsWith(".webm")) {
            videoCodec = ScreenPlay::Video::VideoCodec::VP8;
            // qWarning("Eyeball to VP8 because of .webm");
        }
    }

    if (type == ScreenPlay::ContentTypes::InstalledType::VideoWallpaper) {
        QFileInfo audioFile(folder.absolutePath() + "/audio.mp3");
        containsAudio = audioFile.exists();
    }

    return true;
}
bool ProjectFile::isValid()
{
    if (!projectJsonFilePath.isFile())
        return false;

    return true;
}
}
