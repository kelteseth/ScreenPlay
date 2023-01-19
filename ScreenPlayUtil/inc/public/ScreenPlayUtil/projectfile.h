// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#pragma once

#include <QDateTime>
#include <QDebug>
#include <QObject>
#include <QJsonArray>
#include <QJsonObject>
#include <QMetaEnum>
#include <QString>
#include <QUrl>
#include <QDir>
#include <QVariant>
#include <QVariantList>
#include <QQmlEngine>
#include <QFileInfo>

#include "ScreenPlayUtil/util.h"
#include "ScreenPlayUtil/PropertyHelpers.h"

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
    QString file;
    QString preview;
    QString previewGIF;
    // Path to project.json
    QFileInfo projectJsonFilePath;
    // Folder name
    QString folderName;

    QVariant publishedFileID { 0 };
    QStringList tags;

    InstalledType::InstalledType type = InstalledType::InstalledType::Unknown;
    SearchType::SearchType searchType = SearchType::SearchType::All;
    bool isNew = false;
    QDateTime lastModified;
};
}
