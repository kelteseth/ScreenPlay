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

    QVariant description() const;
    void setDescription(const QVariant& description);
    QVariant file() const;
    void setFile(const QVariant& file);
    QVariant preview() const;
    void setPreview(const QVariant& preview);
    QVariant title() const;
    void setTitle(const QVariant& title);
    QString folderId() const;
    void setFolderId(const QString& folderId);
    QUrl absoluteStoragePath() const;
    void setAbsoluteStoragePath(const QUrl& absoluteStoragePath);
};
