#include "wallpaperdata.h"
#include "ScreenPlayUtil/util.h"

namespace ScreenPlay {


/*!
  \brief Loads the wallpaper object in the wallpaper array:

"timelineWallpaper": [
    {
        "endTime": "08:32:00",
        "startTime": "00:00:00",
        "wallpaper": [
            {
                "absolutePath": "file:///C:/Code/Cpp/ScreenPlay/672870/1234567",
                "file": "AAA.webm",
                "fillMode": "Cover",
                "isLooping": false,
                "monitors": [
                    0
                ],
                "playbackRate": 1,
                "previewImage": "previewThumbnail.jpg",
                "properties": {
                },
                "type": "VideoWallpaper",
                "volume": 1
            }
        ]
    },
*/
QJsonObject WallpaperData::serialize() const
{
    QJsonObject data;
    data.insert("isLooping", isLooping);
    data.insert("absolutePath", absolutePath);
    data.insert("previewImage", previewImage);
    data.insert("playbackRate", playbackRate);
    data.insert("volume", volume);
    data.insert("file", file);
    data.insert("properties", properties);
    data.insert("type", QVariant::fromValue(type).toString());
    data.insert("fillMode", QVariant::fromValue(fillMode).toString());

    // Serialize QVector<int> monitors
    QJsonArray monitorArray;
    for (int monitor : monitors) {
        monitorArray.append(monitor);
    }
    data.insert("monitors", monitorArray);

    return data;
}

std::optional<ScreenPlay::WallpaperData> ScreenPlay::WallpaperData::loadWallpaperConfig(const QJsonObject &wallpaperObj)
{
    if (wallpaperObj.empty())
        return std::nullopt;

    QJsonArray monitorsArray = wallpaperObj.value("monitors").toArray();

    QVector<int> monitors;
    for (const QJsonValueRef monitorNumber : monitorsArray) {
        int value = monitorNumber.toInt(-1);
        if (value == -1) {
            qWarning() << "Could not parse monitor number to display content at";
            return std::nullopt;
        }

        if (monitors.contains(value)) {
            qWarning() << "The monitor: " << value << " is sharing the config multiple times. ";
            return std::nullopt;
        }
        monitors.append(value);
    }

    float volume = static_cast<float>(wallpaperObj.value("volume").toDouble(-1.0));

    if (volume == -1.0F)
        volume = 1.0f;

    WallpaperData wallpaperData;
    wallpaperData.monitors = monitors;
    wallpaperData.volume = volume;
    wallpaperData.absolutePath = wallpaperObj.value("absolutePath").toString();
    wallpaperData.previewImage = wallpaperObj.value("previewImage").toString();
    wallpaperData.playbackRate = wallpaperObj.value("playbackRate").toDouble(1.0);
    wallpaperData.file = wallpaperObj.value("file").toString();
    wallpaperData.properties = wallpaperObj.value("properties").toObject();

    const QString fillModeString = wallpaperObj.value("fillMode").toString();
    const QString typeString = wallpaperObj.value("type").toString();
    wallpaperData.type = QStringToEnum<ContentTypes::InstalledType>(typeString, ContentTypes::InstalledType::VideoWallpaper);
    wallpaperData.fillMode = QStringToEnum<Video::FillMode>(fillModeString, Video::FillMode::Cover);
    return wallpaperData;
}
}
