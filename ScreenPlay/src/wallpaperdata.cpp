// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#include "wallpaperdata.h"
#include "ScreenPlay/monitorlistmodel.h"
#include "ScreenPlayCore/util.h"

namespace ScreenPlay {

/*!
  \brief Loads the wallpaper object in the wallpaper array:

"timelineWallpaper": [
    {
        "endTime": "08:32:00",
        "startTime": "00:00:00",
        "wallpaper": [
            {
                "absolutePath": "C:/Code/Cpp/ScreenPlay/672870/1234567",
                "file": "AAA.webm",
                "fillMode": "Cover",
                "isLooping": false,
                "title": "title",
                "monitors": [
                    0
                ],
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
    QJsonObject data = serializeBase();
    data.insert("isLooping", isLooping());
    data.insert("volume", QString::number(volume(), 'f', 2).toDouble());
    data.insert("fillMode", QVariant::fromValue(fillMode()).toString());

    // Serialize QVector<int> monitors
    QJsonArray monitorArray;
    for (int monitor : monitors()) {
        monitorArray.append(monitor);
    }
    data.insert("monitors", monitorArray);

    return data;
}

QString WallpaperData::toString() const
{
    QString monitorList = [this]() {
        QStringList list;
        for (int monitor : monitors()) {
            list << QString::number(monitor);
        }
        return "[" + list.join(", ") + "]";
    }();

    return QStringLiteral(R"(WallpaperData {
    isLooping: %1
    absolutePath: %2
    previewImage: %3
    volume: %4
    file: %5
    properties: %6
    type: %7
    fillMode: %8
    monitors: %9
    title: %10
})")
        .arg(isLooping() ? "true" : "false")
        .arg(absolutePath())
        .arg(previewImage())
        .arg(QString::number(volume()))
        .arg(file())
        .arg(QString(QJsonDocument(properties()).toJson(QJsonDocument::Compact)))
        .arg(QString::number(static_cast<int>(type())))
        .arg(QString::number(static_cast<int>(fillMode())))
        .arg(monitorList)
        .arg(title());
}

bool WallpaperData::hasContent()
{
    return InstalledContentData::hasContent();
}

QString WallpaperData::loadErrorToString(LoadError error)
{
    switch (error) {
    case LoadError::None:
        return QObject::tr("No error");
    case LoadError::EmptyConfiguration:
        return QObject::tr("Empty wallpaper configuration provided");
    case LoadError::InvalidMonitorNumber:
        return QObject::tr("Invalid monitor number found in configuration");
    case LoadError::DuplicateMonitor:
        return QObject::tr("Duplicate monitor configuration detected");
    case LoadError::MonitorDoesNotExist:
        return QObject::tr("Monitor specified in configuration no longer exists");
    case LoadError::WallpaperFileDoesNotExist:
        return QObject::tr("Wallpaper file specified in configuration does not exist");
    }
    return QObject::tr("Unknown error");
}

std::expected<ScreenPlay::WallpaperData, ScreenPlay::WallpaperData::LoadError> ScreenPlay::WallpaperData::loadTimelineWallpaperConfig(const QJsonObject& wallpaperObj)
{
    if (wallpaperObj.empty())
        return std::unexpected(LoadError::EmptyConfiguration);

    const auto systemMonitors = MonitorListModel::getSystemMonitors();
    QJsonArray monitorsArray = wallpaperObj.value("monitors").toArray();

    QVector<int> monitors;
    for (const QJsonValueRef monitorNumber : monitorsArray) {
        int value = monitorNumber.toInt(-1);
        if (value == -1) {
            qWarning() << "Could not parse monitor number to display content at";
            return std::unexpected(LoadError::InvalidMonitorNumber);
        }

        if (monitors.contains(value)) {
            qWarning() << "The monitor: " << value << " is sharing the config multiple times. ";
            return std::unexpected(LoadError::DuplicateMonitor);
        }

        bool match = false;
        for (const auto systemMonitor : systemMonitors) {
            if (systemMonitor.m_monitorIndex == value) {
                match = true;
                break;
            }
        }
        if (!match) {
            qWarning() << "The monitor: " << value << " does not longer exists. Removing timeline.";
            return std::unexpected(LoadError::MonitorDoesNotExist);
        }

        monitors.append(value);
    }

    float volume = static_cast<float>(wallpaperObj.value("volume").toDouble(-1.0));

    if (volume == -1.0F)
        volume = 1.0f;

    WallpaperData wallpaperData;
    wallpaperData.loadBaseFromJson(wallpaperObj);
    wallpaperData.setMonitors(monitors);
    wallpaperData.setVolume(volume);

    const QString fillModeString = wallpaperObj.value("fillMode").toString();
    wallpaperData.setFillMode(
        QStringToEnum<Video::FillMode>(fillModeString, Video::FillMode::Cover));

    if (!QFileInfo(wallpaperData.absolutePath() + "/" + wallpaperData.file()).exists()) {
        qWarning() << "Requested wallpaper file does not exists";
        return std::unexpected(LoadError::WallpaperFileDoesNotExist);
    }

    return wallpaperData;
}
}
