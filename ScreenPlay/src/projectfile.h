#pragma once

#include <QDebug>
#include <QJsonObject>
#include <QUrl>
#include <QVariant>
#include <QVariantList>

class ProjectFile {


public:
    explicit ProjectFile(QJsonObject obj, QString folderName, QUrl absolutePath);
    ProjectFile();
    ~ProjectFile();

    QVariant m_description;
    QVariant m_file;
    QVariant m_preview;
    QVariant m_title;
    QString m_folderId;
    QUrl m_absoluteStoragePath;
    QString m_type;

    QVariantList m_tags; //TODO: Implement me!

};
