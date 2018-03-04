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
        } else if (tmp == "qmlscene") {
            m_type = "qmlScene";
        }
    }

    if (obj.contains("workshopid")){
        m_workshopID = obj.value("workshopid").toInt();
    } else {
        m_workshopID = 0;
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


