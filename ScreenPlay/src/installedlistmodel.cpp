#include "installedlistmodel.h"

namespace ScreenPlay {

/*!
    \class ScreenPlay::InstalledListModel
    \inmodule ScreenPlay
    \brief Lists all installed wallpapers, widgets etc. from a given Path

*/

InstalledListModel::InstalledListModel(
    const shared_ptr<GlobalVariables>& globalVariables,
    QObject* parent)
    : QAbstractListModel(parent)
    , m_globalVariables { globalVariables }
{
    QObject::connect(this, &InstalledListModel::addInstalledItem,
        this, &InstalledListModel::append, Qt::QueuedConnection);
}
void InstalledListModel::init()
{
    if (!m_fileSystemWatcher.addPath(m_globalVariables->localStoragePath().toLocalFile())) {
        qWarning() << "Could not setup file system watcher for changed files with path: " << m_globalVariables->localStoragePath().toLocalFile();
    }

    loadInstalledContent();

    QObject::connect(&m_fileSystemWatcher, &QFileSystemWatcher::directoryChanged, this, &InstalledListModel::reset);
}
int InstalledListModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;

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
        case PreviewGIFRole:
            return m_screenPlayFiles.at(index.row()).m_previewGIF;
        case TypeRole:
            return m_screenPlayFiles.at(index.row()).m_type;
        case FolderIdRole:
            return m_screenPlayFiles.at(index.row()).m_folderId;
        case FileIdRole:
            return m_screenPlayFiles.at(index.row()).m_file;
        case AbsoluteStoragePathRole:
            return m_screenPlayFiles.at(index.row()).m_absoluteStoragePath;
        case WorkshopIDRole:
            return m_screenPlayFiles.at(index.row()).m_workshopID;
        default:
            return QVariant();
        }
    return QVariant();
}

QHash<int, QByteArray> InstalledListModel::roleNames() const
{
    static const QHash<int, QByteArray> roles {
        { TitleRole, "screenTitle" },
        { TypeRole, "screenType" },
        { PreviewRole, "screenPreview" },
        { PreviewGIFRole, "screenPreviewGIF" },
        { FolderIdRole, "screenFolderId" },
        { FileIdRole, "screenFile" },
        { AbsoluteStoragePathRole, "screenAbsoluteStoragePath" },
        { WorkshopIDRole, "screenWorkshopID" },
    };
    return roles;
}

void InstalledListModel::append(const QJsonObject& obj, const QString& folderName)
{
    beginInsertRows(QModelIndex(), m_screenPlayFiles.size(), m_screenPlayFiles.size());
    m_screenPlayFiles.append(ProjectFile(obj, folderName, m_globalVariables->localStoragePath()));
    endInsertRows();
}

void InstalledListModel::loadInstalledContent()
{
    QtConcurrent::run([this]() {
        QFileInfoList list = QDir(m_globalVariables->localStoragePath().toLocalFile()).entryInfoList(QDir::NoDotAndDotDot | QDir::AllDirs);
        QString projectItemPath;
        int counter {};

        for (auto&& item : list) {
            projectItemPath = m_globalVariables->localStoragePath().toLocalFile() + "/" + item.baseName() + "/project.json";

            if (auto obj = Util::openJsonFileToObject(projectItemPath)) {

                if (obj.value().isEmpty())
                    continue;

                if (!obj.value().contains("file") || !obj.value().contains("type"))
                    continue;

                QStringList availableTypes {
                    "qmlWallpaper",
                    "htmlWallpaper",
                    "videoWallpaper",
                    "godotWallpaper",

                    "qmlWidget",
                    "htmlWidget",
                    "standaloneWidget"
                };

                if (availableTypes.contains(obj.value().value("type").toString())) {
                    emit addInstalledItem(obj.value(), item.baseName());
                }

                counter += 1;
            }
        }

        setCount(counter);
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
            map.insert("screenPreviewGIF", m_screenPlayFiles[i].m_previewGIF);
            map.insert("screenFile", m_screenPlayFiles[i].m_file);
            map.insert("screenType", m_screenPlayFiles[i].m_type);
            map.insert("screenAbsoluteStoragePath", m_screenPlayFiles[i].m_absoluteStoragePath);
            map.insert("screenWorkshopID", m_screenPlayFiles[i].m_workshopID);
        }
    }

    return map;
}

void InstalledListModel::reset()
{
    beginResetModel();
    m_screenPlayFiles.clear();
    m_screenPlayFiles.squeeze();
    endResetModel();
    loadInstalledContent();
}

}
