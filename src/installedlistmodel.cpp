#include "installedlistmodel.h"

InstalledListModel::InstalledListModel(QObject* parent)
    : QAbstractListModel(parent)
{
}

int InstalledListModel::rowCount(const QModelIndex& parent) const
{
    return m_screenPlayFiles.count();
}

QVariant InstalledListModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() < rowCount())
        switch (role) {
        case TitleRole:
            return m_screenPlayFiles.at(index.row()).m_title;
        case PreviewRole:
            return m_screenPlayFiles.at(index.row()).m_preview;
        case FolderIdRole:
            return m_screenPlayFiles.at(index.row()).m_folderId;
        case FileIdRole:
            return m_screenPlayFiles.at(index.row()).m_file;
        default:
            return QVariant();
        }
    return QVariant();
}

QHash<int, QByteArray> InstalledListModel::roleNames() const
{
    static const QHash<int, QByteArray> roles{
        { TitleRole, "screenTitle" },
        { PreviewRole, "screenPreview" },
        { FolderIdRole, "screenFolderId" },
        { FileIdRole, "screenFile" },
    };
    return roles;
}

bool InstalledListModel::getProjectByName(QString profileName, ProjectFile *spf)
{
    for (int i = 0; i < m_screenPlayFiles.size(); i++) {
        if (m_screenPlayFiles.at(i).m_folderId == profileName) {
            *spf = m_screenPlayFiles.at(i);
            return true;
        }
    }
    return false;
}

void InstalledListModel::append(const QJsonObject obj, const QString folderName)
{
    int row = 0;

    beginInsertRows(QModelIndex(), row, row);

    ProjectFile tmpFile(obj, folderName);
    m_screenPlayFiles.append(tmpFile);

    endInsertRows();
}

void InstalledListModel::loadScreens()
{
    QJsonDocument jsonProject;
    QJsonParseError parseError;

    QFileInfoList list = QDir(m_absoluteStoragePath.toString()).entryInfoList(QDir::NoDotAndDotDot | QDir::AllDirs);
    QString tmpPath;

    for (auto&& item : list) {
        tmpPath = m_absoluteStoragePath.toString() +"/" +item.baseName() + "/project.json";
        qDebug() << tmpPath;
        if (!QFile(tmpPath).exists())
            continue;

        QFile projectConfig;
        projectConfig.setFileName(tmpPath);
        projectConfig.open(QIODevice::ReadOnly | QIODevice::Text);
        QString projectConfigData = projectConfig.readAll();
        jsonProject = QJsonDocument::fromJson(projectConfigData.toUtf8(), &parseError);

        if (!(parseError.error == QJsonParseError::NoError))
            continue;

        append(jsonProject.object(), item.baseName());
    }
}

QVariantMap InstalledListModel::get(QString folderId)
{

    QVariantMap map;
    if (m_screenPlayFiles.count() == 0)
        return map;

    for (int i = 0; i < m_screenPlayFiles.count(); i++) {

        if (m_screenPlayFiles[i].m_folderId == folderId) {
            map.insert("screenTitle", m_screenPlayFiles[i].m_title);
            map.insert("screenPreview", m_screenPlayFiles[i].m_preview);
            map.insert("screenFile", m_screenPlayFiles[i].m_file);
        }
    }

    return map;
}


