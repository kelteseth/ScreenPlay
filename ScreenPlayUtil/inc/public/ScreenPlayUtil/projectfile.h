// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#pragma once

#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonObject>
#include <QMetaEnum>
#include <QObject>
#include <QQmlEngine>
#include <QString>
#include <QUrl>
#include <QVariant>
#include <QVariantList>

#include "ScreenPlayUtil/PropertyHelpers.h"
#include "ScreenPlayUtil/util.h"

/*!
    \class ProjectFile
    \brief In ScreenPlay every Wallpaper, Scene or Widget has an project.json to store its configuration.

*/
namespace ScreenPlay {

struct ProjectFile {

    bool init();
    bool isValid();

    QString title;
    QString description;
    // Filenames
    QString file; // myFancyVideo.mp
    QString preview;
    QString previewGIF;
    // Path to project.json
    QFileInfo projectJsonFilePath;
    // Folder name
    QString folderName;
    // Website Wallpaper
    QUrl url;
    // Video Wallpaper
    ScreenPlay::VideoCodec::VideoCodec videoCodec;

    QVariant publishedFileID { 0 };
    QStringList tags;

    InstalledType::InstalledType type = InstalledType::InstalledType::Unknown;
    SearchType::SearchType searchType = SearchType::SearchType::All;
    bool isNew = false;
    bool containsAudio = false;
    QDateTime lastModified;
};
}
