#include "installedlistmodel.h"

InstalledListModel::InstalledListModel(QObject* parent)
    : QAbstractListModel(parent)
{
    QObject::connect(this, &InstalledListModel::addInstalledItem,
        this, &InstalledListModel::append, Qt::QueuedConnection);
}

int InstalledListModel::rowCount(const QModelIndex& parent) const
{
    return m_screenPlayFiles.count();
}

QVariant InstalledListModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    int row = index.row();
    if (row < 0 || row >= m_screenPlayFiles.count()) {
        return QVariant();
    }

    if (index.row() < rowCount())
        switch (role) {
        case TitleRole:
            return m_screenPlayFiles.at(index.row()).m_title;
        case PreviewRole:
            return m_screenPlayFiles.at(index.row()).m_preview;
        case TypeRole:
            return m_screenPlayFiles.at(index.row()).m_type;
        case FolderIdRole:
            return m_screenPlayFiles.at(index.row()).m_folderId;
        case FileIdRole:
            return m_screenPlayFiles.at(index.row()).m_file;
        case AbsoluteStoragePathRole:
            return m_screenPlayFiles.at(index.row()).m_absoluteStoragePath;
        default:
            return QVariant();
        }
    return QVariant();
}

QHash<int, QByteArray> InstalledListModel::roleNames() const
{
    static const QHash<int, QByteArray> roles{
        { TitleRole, "screenTitle" },
        { TypeRole, "screenType" },
        { PreviewRole, "screenPreview" },
        { FolderIdRole, "screenFolderId" },
        { FileIdRole, "screenFile" },
        { AbsoluteStoragePathRole, "screenAbsoluteStoragePath" },
    };
    return roles;
}

bool InstalledListModel::getProjectByAbsoluteStoragePath(QUrl* path, ProjectFile* spf)
{

    for (int i = 0; i < m_screenPlayFiles.count(); ++i) {
        if (m_screenPlayFiles.at(i).m_absoluteStoragePath == *path) {
            *spf = m_screenPlayFiles.at(i);
            return true;
        }
    }

    return false;
}

void InstalledListModel::append(const QJsonObject obj, const QString folderName)
{
    beginInsertRows(QModelIndex(), m_screenPlayFiles.size(), m_screenPlayFiles.size());

    ProjectFile tmpFile(obj, folderName, m_absoluteStoragePath);
    m_screenPlayFiles.append(tmpFile);

    endInsertRows();
}

void InstalledListModel::loadScreens()
{
    QtConcurrent::run([this]() {
        QJsonDocument jsonProject;
        QJsonParseError parseError;

        QFileInfoList list = QDir(m_absoluteStoragePath.toString()).entryInfoList(QDir::NoDotAndDotDot | QDir::AllDirs);
        QString tmpPath;

        for (auto&& item : list) {
            tmpPath = m_absoluteStoragePath.toString() + "/" + item.baseName() + "/project.json";

            if (!QFile(tmpPath).exists())
                continue;

            QFile projectConfig;
            projectConfig.setFileName(tmpPath);
            projectConfig.open(QIODevice::ReadOnly | QIODevice::Text);
            QString projectConfigData = projectConfig.readAll();
            jsonProject = QJsonDocument::fromJson(projectConfigData.toUtf8(), &parseError);

            if (!(parseError.error == QJsonParseError::NoError))
                continue;

            if (jsonProject.object().value("type").toString() == "scene")
                continue;

            if (jsonProject.isEmpty())
                continue;

            if (jsonProject.object().empty())
                continue;

            //Some settings dont have a file type
            if(!jsonProject.object().contains("type")){
                if(jsonProject.object().contains("file")){
                    QString fileEnding = jsonProject.object().value("file").toString();
                    if(fileEnding.endsWith(".mp4") || fileEnding.endsWith(".vp9")){
                        jsonProject.object().insert("type","video");
                    }
                }
            }


            emit addInstalledItem(jsonProject.object(), item.baseName());
        }
        emit installedLoadingFinished();
    });
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
            map.insert("screenType", m_screenPlayFiles[i].m_type);
            map.insert("screenAbsoluteStoragePath", m_screenPlayFiles[i].m_absoluteStoragePath);
        }
    }

    return map;
}

int InstalledListModel::getAmountItemLoaded()
{
    return m_screenPlayFiles.count();
}

void InstalledListModel::reset()
{
    beginResetModel();
    m_screenPlayFiles.clear();
    m_screenPlayFiles.squeeze();
    endResetModel();

}
