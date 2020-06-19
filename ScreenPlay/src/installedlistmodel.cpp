#include "installedlistmodel.h"

namespace ScreenPlay {

/*!
    \class ScreenPlay::InstalledListModel
    \inmodule ScreenPlay
    \brief Lists all installed wallpapers, widgets etc. from a given Path.

    Currently we only support one path for the user content. Via a QFileSystemWatcher we
    automatically reload the list. Dynamic insert and remove is not yet implemented.
    Otherwhise it is a regular QAbstractListModel based list model.
*/

/*!
  \brief Constructor.
*/
InstalledListModel::InstalledListModel(
    const std::shared_ptr<GlobalVariables>& globalVariables,
    QObject* parent)
    : QAbstractListModel(parent)
    , m_globalVariables { globalVariables }
{
    QObject::connect(this, &InstalledListModel::addInstalledItem,
        this, &InstalledListModel::append, Qt::QueuedConnection);
}

/*!
    \brief Init function that needs to be called after the constructor because we need the GlobalVariables to finish loading.
*/
void InstalledListModel::init()
{
    if (!m_fileSystemWatcher.addPath(m_globalVariables->localStoragePath().toLocalFile())) {
        qWarning() << "Could not setup file system watcher for changed files with path: " << m_globalVariables->localStoragePath().toLocalFile();
    }

    loadInstalledContent();

    QObject::connect(&m_fileSystemWatcher, &QFileSystemWatcher::directoryChanged, this, &InstalledListModel::reset);
    QObject::connect(&m_fileSystemWatcher, &QFileSystemWatcher::fileChanged, this, &InstalledListModel::reset);
}

/*!
    \brief Returns the length of the installed items.
*/
int InstalledListModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;

    return m_screenPlayFiles.count();
}

/*!
    \brief Retruns the data member of the ProjectFile.
*/
QVariant InstalledListModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    int row = index.row();
    if (row < 0 || row >= m_screenPlayFiles.count()) {
        return QVariant();
    }

    if (row < rowCount())
        switch (role) {
        case TitleRole:
            return m_screenPlayFiles.at(row).m_title;
        case PreviewRole:
            return m_screenPlayFiles.at(row).m_preview;
        case PreviewGIFRole:
            return m_screenPlayFiles.at(row).m_previewGIF;
        case TypeRole:
            return QVariant::fromValue(m_screenPlayFiles.at(row).m_type);
        case FolderIdRole:
            return m_screenPlayFiles.at(row).m_folderId;
        case FileIdRole:
            return m_screenPlayFiles.at(row).m_file;
        case AbsoluteStoragePathRole:
            return m_screenPlayFiles.at(row).m_absoluteStoragePath;
        case WorkshopIDRole:
            return m_screenPlayFiles.at(row).m_workshopID;
        case TagsRole:
            return m_screenPlayFiles.at(row).m_tags;
        case SearchTypeRole:
            return QVariant::fromValue(m_screenPlayFiles.at(row).m_searchType);
        default:
            return QVariant();
        }
    return QVariant();
}

/*!
    \brief Returns the variable names for QML.
*/
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
        { TagsRole, "screenTags" },
        { SearchTypeRole, "screenSearchType" },
    };
    return roles;
}

/*!

*/
void InstalledListModel::append(const QJsonObject& obj, const QString& folderName)
{
    beginInsertRows(QModelIndex(), m_screenPlayFiles.size(), m_screenPlayFiles.size());
    m_screenPlayFiles.append(ProjectFile(obj, folderName, m_globalVariables->localStoragePath()));
    endInsertRows();
}

/*!

*/
void InstalledListModel::loadInstalledContent()
{
    QtConcurrent::run([this]() {
        QFileInfoList list = QDir(m_globalVariables->localStoragePath().toLocalFile()).entryInfoList(QDir::NoDotAndDotDot | QDir::AllDirs);
        QString projectItemPath;
        int counter {};

        for (auto&& item : list) {
            projectItemPath = m_globalVariables->localStoragePath().toLocalFile() + "/" + item.baseName() + "/project.json";

            if (auto obj = Util::openJsonFileToObject(projectItemPath)) {

                if (obj->isEmpty())
                    continue;

                if (!obj->contains("file") || !obj->contains("type"))
                    continue;

                if (GlobalVariables::getAvailableTypes().contains(obj->value("type").toString())) {
                    emit addInstalledItem(*obj, item.baseName());
                }

                counter += 1;
            }
        }

        setCount(counter);
        emit installedLoadingFinished();
    });
}

QVariantMap InstalledListModel::get(QString folderId) const
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
            map.insert("screenType", QVariant::fromValue(m_screenPlayFiles[i].m_type));
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
