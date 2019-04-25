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

struct ProjectFile {

    explicit ProjectFile(QJsonObject obj, QString folderName, QUrl absolutePath);
    ProjectFile();
    ~ProjectFile();

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
