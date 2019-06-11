#pragma once

#include <QDebug>
#include <QJsonObject>
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

        if (obj.contains("preview"))
            m_preview = obj.value("preview");

        if (obj.contains("previewGIF"))
            m_previewGIF = obj.value("previewGIF");

        if (obj.contains("title"))
            m_title = obj.value("title");

        if (obj.contains("type")) {
            QString tmp = obj.value("type").toString().toLower();
            if (tmp == "video") {
                m_type = "video";
            } else if (tmp == "qmlwidget" || tmp == "standalonewidget") {
                m_type = "widget";
            } else if (tmp == "qmlscene") {
                m_type = "qmlScene";
            } else if (tmp == "html") {
                m_type = "html";
            }
        }

        if (obj.contains("workshopid")) {
            m_workshopID = obj.value("workshopid").toInt();
        } else {
            m_workshopID = 0;
        }

        m_absoluteStoragePath = QUrl(absolutePath.toString() + "/" + folderName);

        m_folderId = folderName;
    }

    ProjectFile() {}
    ~ProjectFile() {}

    QVariant m_description;
    QVariant m_file;
    QVariant m_preview;
    QVariant m_previewGIF;
    QVariant m_title;
    QString m_folderId;
    QUrl m_absoluteStoragePath;
    QString m_type;
    QJsonObject m_settings;
    int m_workshopID;

    QVariantList m_tags; //TODO: Implement me!
};
}
