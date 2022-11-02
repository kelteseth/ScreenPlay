/****************************************************************************
**
** Copyright (C) 2020 Elias Steurer (Kelteseth)
** Contact: https://screen-play.app
**
** This file is part of ScreenPlay. ScreenPlay is licensed under a dual license in
** order to ensure its sustainability. When you contribute to ScreenPlay
** you accept that your work will be available under the two following licenses:
**
** $SCREENPLAY_BEGIN_LICENSE$
**
** #### Affero General Public License Usage (AGPLv3)
** Alternatively, this file may be used under the terms of the GNU Affero
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file "ScreenPlay License.md" included in the
** packaging of this App. Please review the following information to
** ensure the GNU Affero Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/agpl-3.0.en.html.
**
** #### Commercial License
** This code is owned by Elias Steurer. By changing/adding to the code you agree to the
** terms written in:
**  * Legal/corporate_contributor_license_agreement.md - For corporate contributors
**  * Legal/individual_contributor_license_agreement.md - For individual contributors
**
** #### Additional Limitations to the AGPLv3 and Commercial Lincese
** This License does not grant any rights in the trademarks,
** service marks, or logos.
**
**
** $SCREENPLAY_END_LICENSE$
**
****************************************************************************/

#pragma once

#include <QDateTime>
#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <QMetaEnum>
#include <QString>
#include <QUrl>
#include <QVariant>
#include <QVariantList>

#include "ScreenPlayUtil/util.h"

/*!
    \class ProjectFile
    \brief In ScreenPlay every Wallpaper, Scene or Widget has an project.json to store its configuration

*/
namespace ScreenPlay {

struct ProjectFile {

    ProjectFile(
        const QJsonObject& obj,
        const QString& folderName,
        const QUrl& absolutePath,
        const bool isNew,
        const QDateTime& lastModified)
    {

        if (obj.contains("description"))
            m_description = obj.value("description").toString();

        if (obj.contains("file"))
            m_file = obj.value("file").toString();

        if (obj.contains("previewThumbnail")) {
            m_preview = obj.value("previewThumbnail").toString();
        } else {
            if (obj.contains("preview")) {
                m_preview = obj.value("preview").toString();
            }
        }

        if (obj.contains("previewGIF"))
            m_previewGIF = obj.value("previewGIF").toString();

        if (obj.contains("title"))
            m_title = obj.value("title").toString();

        if (obj.contains("workshopid")) {
            m_publishedFileID = obj.value("workshopid").toInt(0);
        }

        if (obj.contains("tags")) {
            if (obj.value("tags").isArray()) {
                auto tagArray = obj.value("tags").toArray();
                if (tagArray.size() > 0) {
                    for (const auto& tag : tagArray) {
                        m_tags.append(tag.toString());
                    }
                }
            }
        }

        m_absoluteStoragePath = QUrl(absolutePath.toString() + "/" + folderName);

        m_folderId = folderName;

        if (!obj.contains("type"))
            return;

        auto type = ScreenPlayUtil::getInstalledTypeFromString(obj.value("type").toString());
        if (!type) {
            qWarning() << "Type could not parsed from: " << *type << folderName;
            return;
        }

        m_type = *type;
        if (m_type == InstalledType::InstalledType::GifWallpaper) {
            m_preview = m_previewGIF;
        }
        m_searchType = ScreenPlayUtil::getSearchTypeFromInstalledType(m_type);
        m_isNew = isNew;
        m_lastModified = lastModified;
    }

    ProjectFile() { }

    QString m_title;
    QString m_description;
    // Filenames
    QString m_file;
    QString m_preview;
    QString m_previewGIF;
    // Path
    QUrl m_absoluteStoragePath;
    // Folder name
    QString m_folderId;

    QVariant m_publishedFileID { 0 };
    QStringList m_tags;

    InstalledType::InstalledType m_type = InstalledType::InstalledType::Unknown;
    SearchType::SearchType m_searchType = SearchType::SearchType::All;
    bool m_isNew = false;
    QDateTime m_lastModified;
};
}
