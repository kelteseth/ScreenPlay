#ifndef PROJECTFILE_H
#define PROJECTFILE_H

#include <QJsonObject>
#include <QUrl>
#include <QVariant>
#include <QVariantList>
#include <QDebug>

class ProjectFile
{
public:
    ProjectFile();
    ProjectFile(QJsonObject obj, QString folderName, QUrl absolutePath);

    QVariant m_description;
    QVariant m_file;
    QVariant m_preview;
    QVariant m_title;
    QString m_folderId;
    QUrl m_absoluteStoragePath;

    QVariantList m_tags; //TODO: Implement me!
};

#endif // PROJECTFILE_H
