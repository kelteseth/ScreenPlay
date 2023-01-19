#include "ScreenPlayUtil/projectfile.h"

namespace ScreenPlay {

    bool ProjectFile::init()
    {
        if(!isValid())
            return false;

        const auto jsonObjOpt = ScreenPlayUtil::openJsonFileToObject(projectJsonFilePath.absoluteFilePath());
        QDir folder = projectJsonFilePath.dir();
        folderName = folder.dirName();
        QFileInfo folderInfo(folder.path());
        lastModified = folderInfo.birthTime();
        if (folderInfo.birthTime().date() == QDateTime::currentDateTime().date())
            isNew = true;

        if(!jsonObjOpt.has_value())
            return false;

        const QJsonObject& obj =jsonObjOpt.value();
        if (obj.isEmpty())
            return false;
        
        //Required:
        if (!obj.contains("description"))
            return false;
        description = obj.value("description").toString();

        if (!obj.contains("file"))
            return false;
        file = obj.value("file").toString();


        if (!obj.contains("title"))
            return false;
        title = obj.value("title").toString();

        if (!obj.contains("type"))
            return false;

        // Optional:
        if (obj.contains("previewGIF"))
            previewGIF = obj.value("previewGIF").toString();

        if (obj.contains("workshopid"))
            publishedFileID = obj.value("workshopid").toInt(0);
        
        if (obj.contains("previewThumbnail")) {
            preview = obj.value("previewThumbnail").toString();
        } else {
            if (!obj.contains("preview")) {
                return false;
            }
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

        auto typeParsed = ScreenPlayUtil::getInstalledTypeFromString(obj.value("type").toString());
        if (!typeParsed) {
            qWarning() << "Type could not parsed from: " << *typeParsed;
            return false;
        }

        type = *typeParsed;
        if (type == InstalledType::InstalledType::GifWallpaper) {
            preview = previewGIF;
        }
        searchType = ScreenPlayUtil::getSearchTypeFromInstalledType(type);

        return true;
    }
    bool ProjectFile::isValid()
    {
        if(!projectJsonFilePath.isFile())
            return false;

        return true;
    }
}
