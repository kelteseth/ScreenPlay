#include "projectfile.h"

ProjectFile::ProjectFile(QJsonObject obj, QString folderName, QUrl absolutePath)
{


    if (obj.contains("description"))
        m_description = obj.value("description");

    if (obj.contains("file"))
        m_file = obj.value("file");

    if (obj.contains("preview"))
        m_preview = obj.value("preview");

    if (obj.contains("title"))
        m_title = obj.value("title");

    if (obj.contains("type")) {
        QString tmp = obj.value("type").toString();
        tmp = tmp.toLower();
        if (tmp == "video") {
            m_type = "video";
        } else if (tmp == "widget") {
            m_type = "widget";
        }else if (tmp == "qmlscene") {
            m_type = "qmlScene";
        }
    }

    m_absoluteStoragePath = QUrl(absolutePath.toString() + "/" + folderName);

    m_folderId = folderName;
}

ProjectFile::ProjectFile()
{

}

ProjectFile::~ProjectFile()
{
}



QUrl ProjectFile::absoluteStoragePath() const
{
    return m_absoluteStoragePath;
}

void ProjectFile::setAbsoluteStoragePath(const QUrl &absoluteStoragePath)
{
    m_absoluteStoragePath = absoluteStoragePath;
}

QString ProjectFile::folderId() const
{
    return m_folderId;
}

void ProjectFile::setFolderId(const QString &folderId)
{
    m_folderId = folderId;
}

QVariant ProjectFile::title() const
{
    return m_title;
}

void ProjectFile::setTitle(const QVariant &title)
{
    m_title = title;
}

QVariant ProjectFile::preview() const
{
    return m_preview;
}

void ProjectFile::setPreview(const QVariant &preview)
{
    m_preview = preview;
}

QVariant ProjectFile::file() const
{
    return m_file;
}

void ProjectFile::setFile(const QVariant &file)
{
    m_file = file;
}

QVariant ProjectFile::description() const
{
    return m_description;
}

void ProjectFile::setDescription(const QVariant &description)
{
    m_description = description;
}
