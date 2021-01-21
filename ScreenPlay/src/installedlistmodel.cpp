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

    auto reloadLambda = [this]() {
        QTimer::singleShot(500, [this]() {
            reset();
        });
    };

    QObject::connect(&m_fileSystemWatcher, &QFileSystemWatcher::directoryChanged, this, reloadLambda);
    QObject::connect(&m_fileSystemWatcher, &QFileSystemWatcher::fileChanged, this, reloadLambda);
}

/*!
    \brief .
*/
bool InstalledListModel::deinstallItemAt(const int index)
{
    if (index < 0 || index >= m_screenPlayFiles.count()) {
        qWarning() << "remove folder error, invalid index " << index;
        return false;
    }

    beginRemoveRows(QModelIndex(), index, index);
    const QString path = QUrl::fromUserInput(m_screenPlayFiles.at(index).m_absoluteStoragePath.toString()).toLocalFile();

    QDir dir(path);
    const bool success = dir.removeRecursively();

    if (!success)
        qWarning() << "Could not remove folder: " << m_screenPlayFiles.at(index).m_absoluteStoragePath.toString();

    m_screenPlayFiles.removeAt(index);
    endRemoveRows();
    return success;
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
        case static_cast<int>(ScreenPlayItem::Title):
            return m_screenPlayFiles.at(row).m_title;
        case static_cast<int>(ScreenPlayItem::Preview):
            return m_screenPlayFiles.at(row).m_preview;
        case static_cast<int>(ScreenPlayItem::PreviewGIF):
            return m_screenPlayFiles.at(row).m_previewGIF;
        case static_cast<int>(ScreenPlayItem::Type):
            return QVariant::fromValue(m_screenPlayFiles.at(row).m_type);
        case static_cast<int>(ScreenPlayItem::FolderId):
            return m_screenPlayFiles.at(row).m_folderId;
        case static_cast<int>(ScreenPlayItem::FileId):
            return m_screenPlayFiles.at(row).m_file;
        case static_cast<int>(ScreenPlayItem::AbsoluteStoragePath):
            return m_screenPlayFiles.at(row).m_absoluteStoragePath;
        case static_cast<int>(ScreenPlayItem::PublishedFileID):
            return m_screenPlayFiles.at(row).m_publishedFileID;
        case static_cast<int>(ScreenPlayItem::Tags):
            return m_screenPlayFiles.at(row).m_tags;
        case static_cast<int>(ScreenPlayItem::SearchType):
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
        { static_cast<int>(ScreenPlayItem::Title), "m_title" },
        { static_cast<int>(ScreenPlayItem::Type), "m_type" },
        { static_cast<int>(ScreenPlayItem::Preview), "m_preview" },
        { static_cast<int>(ScreenPlayItem::PreviewGIF), "m_previewGIF" },
        { static_cast<int>(ScreenPlayItem::FolderId), "m_folderId" },
        { static_cast<int>(ScreenPlayItem::FileId), "m_file" },
        { static_cast<int>(ScreenPlayItem::AbsoluteStoragePath), "m_absoluteStoragePath" },
        { static_cast<int>(ScreenPlayItem::PublishedFileID), "m_publishedFileID" },
        { static_cast<int>(ScreenPlayItem::Tags), "m_tags" },
        { static_cast<int>(ScreenPlayItem::SearchType), "m_searchType" },
    };
    return roles;
}

/*!
    \brief .
*/
void InstalledListModel::append(const QJsonObject& obj, const QString& folderName)
{
    beginInsertRows(QModelIndex(), m_screenPlayFiles.size(), m_screenPlayFiles.size());
    m_screenPlayFiles.append(ProjectFile(obj, folderName, m_globalVariables->localStoragePath()));
    endInsertRows();
}

/*!
    \brief .
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

                if (!obj->contains("type"))
                    continue;

                if (GlobalVariables::getAvailableTypes().contains(obj->value("type").toString())) {
                    if (GlobalVariables::getAvailableTypes().contains(obj->value("type").toString(), Qt::CaseInsensitive)) {
                        emit addInstalledItem(*obj, item.baseName());
                    }

                    counter += 1;
                }
            }
        }
        setCount(counter);
        emit installedLoadingFinished();
    });
}

/*!
    \brief .
*/
QVariantMap InstalledListModel::get(const QString& folderId) const
{

    if (m_screenPlayFiles.count() == 0)
        return {};

    QVariantMap map;
    for (int i = 0; i < m_screenPlayFiles.count(); i++) {

        if (m_screenPlayFiles[i].m_folderId == folderId) {
            map.insert("m_title", m_screenPlayFiles[i].m_title);
            map.insert("m_preview", m_screenPlayFiles[i].m_preview);
            map.insert("m_previewGIF", m_screenPlayFiles[i].m_previewGIF);
            map.insert("m_file", m_screenPlayFiles[i].m_file);
            map.insert("m_type", QVariant::fromValue(m_screenPlayFiles[i].m_type));
            map.insert("m_absoluteStoragePath", m_screenPlayFiles[i].m_absoluteStoragePath);
            map.insert("m_publishedFileID", m_screenPlayFiles[i].m_publishedFileID);
            return map;
        }
    }

    return {};
}

/*!
    \brief .
*/
void InstalledListModel::reset()
{
    beginResetModel();
    m_screenPlayFiles.clear();
    m_screenPlayFiles.squeeze();
    endResetModel();
    loadInstalledContent();
}

}
