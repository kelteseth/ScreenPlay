#include "projectfile.h"

ProjectFile::ProjectFile()
{

}
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

    m_absoluteStoragePath = QUrl(absolutePath.toString() +"/"+ folderName);

    m_folderId = folderName;
}
