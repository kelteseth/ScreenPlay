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

#include "globalvariables.h"
#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <QMetaEnum>
#include <QString>
#include <QUrl>
#include <QVariant>
#include <QVariantList>

/*!
    \class ProjectFile
    \brief In ScreenPlay every Wallpaper, Scene or Widget has an project.json to store its configuration

*/
namespace ScreenPlay {

struct ProjectFile {

    ProjectFile(const QJsonObject& obj,
        const QString& folderName,
        const QUrl& absolutePath)
    {

        if (obj.contains("description"))
            m_description = obj.value("description");

        if (obj.contains("file"))
            m_file = obj.value("file");

        if (obj.contains("previewThumbnail")) {
            m_preview = obj.value("previewThumbnail");
        } else {
            if (obj.contains("preview"))
                m_preview = obj.value("preview");
        }

        if (obj.contains("previewGIF"))
            m_previewGIF = obj.value("previewGIF");

        if (obj.contains("title"))
            m_title = obj.value("title");

        if (obj.contains("workshopid")) {
            m_workshopID = obj.value("workshopid").toInt(0);
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

        QString type = obj.value("type").toString();
        if (type.endsWith("Wallpaper")) {
            if (type.startsWith("video")) {
                m_type = InstalledType::InstalledType::VideoWallpaper;
                return;
            }
            if (type.startsWith("qml")) {
                m_type = InstalledType::InstalledType::QMLWallpaper;
                return;
            }
            if (type.startsWith("html")) {
                m_type = InstalledType::InstalledType::HTMLWallpaper;
                return;
            }
            if (type.startsWith("godot")) {
                m_type = InstalledType::InstalledType::GodotWallpaper;
                return;
            }
        }

        if (type.endsWith("Widget")) {
            if (type.startsWith("qml")) {
                m_type = InstalledType::InstalledType::QMLWidget;
                return;
            }
            if (type.startsWith("html")) {
                m_type = InstalledType::InstalledType::HTMLWidget;
                return;
            }
        }


        qWarning() << "Type could not parsed from: " << type << folderName;
    }

    ProjectFile() { }

    QVariant m_description;
    QVariant m_file;
    QVariant m_preview;
    QVariant m_previewGIF;
    QVariant m_title;
    QString m_folderId;
    QUrl m_absoluteStoragePath;
    InstalledType::InstalledType m_type = InstalledType::InstalledType::Unknown;
    int m_workshopID { 0 };
    QVariantList m_tags;
};
}
