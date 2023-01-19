// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#include "ScreenPlay/installedlistmodel.h"
#include <QDebug>

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
}

/*!
    \brief Init function that needs to be called after the constructor because we need the GlobalVariables to finish loading.
*/
void InstalledListModel::init()
{
    if (!m_fileSystemWatcher.addPath(m_globalVariables->localStoragePath().toLocalFile())) {
        qWarning() << "Could not setup file system watcher for changed files with path: " << m_globalVariables->localStoragePath().toLocalFile();
    }

    auto reloadLambda = [this]() {
        QTimer::singleShot(500, this, [this]() {
            reset();
        });
    };

    QObject::connect(&m_fileSystemWatcher, &QFileSystemWatcher::directoryChanged, this, reloadLambda);
    QObject::connect(&m_fileSystemWatcher, &QFileSystemWatcher::fileChanged, this, reloadLambda);
}

/*!
    \brief Deleted the item from the local storage and removes it from the
    installed list. We wait for the qml engine to free all resources before
    we proceed. This like the preview.gif will be in use when clicking on an item
*/
void InstalledListModel::deinstallItemAt(const QString& absoluteStoragePath)
{
    QTimer::singleShot(1000, this, [this, absoluteStoragePath]() {
        int index = -1;
        for (int i = 0; i < m_screenPlayFiles.size(); ++i) {
            if (m_screenPlayFiles.at(i).m_absoluteStoragePath.toString() == absoluteStoragePath) {
                index = i;
                break;
            }
        }

        if (index < 0 || index >= m_screenPlayFiles.count()) {
            qWarning() << "Remove folder error, invalid index " << index;
            return;
        }

        beginRemoveRows(QModelIndex(), index, index);
        m_screenPlayFiles.removeAt(index);
        endRemoveRows();

        const QString path = ScreenPlayUtil::toLocal(absoluteStoragePath);

        QDir dir(path);
        bool success = true;
        if (!dir.exists()) {
            qWarning() << "Directory does not exist!" << dir;
            return;
        }

        // We must pause the QFileSystemWatcher to not trigger
        // a reload for every removed file
        m_fileSystemWatcher.blockSignals(true);
        for (auto& item : dir.entryInfoList(QDir::Files)) {
            if (!QFile::remove(item.absoluteFilePath())) {
                qWarning() << "Unable to remove file:" << item;
                success = false;
                break;
            }
        }

        if (!success) {
            qWarning() << "Could not remove folder content at: " << path;
            loadInstalledContent();
        }

        if (!dir.rmdir(path)) {
            qWarning() << "Could not remove folder at: " << path;
            return;
        }

        // Add delay to the watcher, because it was trigger by
        // something when enabling after the removal.
        QTimer::singleShot(3000, this, [this]() {
            m_fileSystemWatcher.blockSignals(false);
        });
    });
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

    const int row = index.row();
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
        case static_cast<int>(ScreenPlayItem::IsNew):
            return m_screenPlayFiles.at(row).m_isNew;
        case static_cast<int>(ScreenPlayItem::LastModified):
            return m_screenPlayFiles.at(row).m_lastModified;
        case static_cast<int>(ScreenPlayItem::SearchType):
            return QVariant::fromValue(m_screenPlayFiles.at(row).m_searchType);
        }

    qWarning() << "Unable to fetch value for row type:" << role;
    return QVariant();
}

/*!
    \brief Returns the variable names for QML.
*/
QHash<int, QByteArray> InstalledListModel::roleNames() const
{
    return {
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
        { static_cast<int>(ScreenPlayItem::IsNew), "m_isNew" },
        { static_cast<int>(ScreenPlayItem::LastModified), "m_lastModified" }
    };
}

/*!
    \brief Append an ProjectFile to the list.
*/
void InstalledListModel::append(const QJsonObject& obj, const QString& folderName, const bool isNew, const QDateTime& lastModified)
{
    beginInsertRows(QModelIndex(), m_screenPlayFiles.size(), m_screenPlayFiles.size());
    m_screenPlayFiles.append(ProjectFile(obj, folderName, m_globalVariables->localStoragePath(), isNew, lastModified));
    endInsertRows();
}

/*!
    \brief Loads all installed content.
           - Skips if the loadContentFuture is already running.
           - Skips projects.json without a "type" field.
*/
void InstalledListModel::loadInstalledContent()
{
    if (m_isLoading) {
        qInfo() << "loadInstalledContent is already running. Skip.";
        return;
    }
    m_isLoading = true;
    auto unused = QtConcurrent::run([this]() {
        QFileInfoList list = QDir(m_globalVariables->localStoragePath().toLocalFile()).entryInfoList(QDir::NoDotAndDotDot | QDir::AllDirs);
        int counter = 0;

        for (const auto& item : list) {
            const QString absoluteFilePath = m_globalVariables->localStoragePath().toLocalFile() + "/" + item.baseName() + "/project.json";

            if (!QFile::exists(absoluteFilePath))
                continue;

            bool isNew = false;

            if (item.birthTime().date() == QDateTime::currentDateTime().date())
                isNew = true;

            if (auto obj = ScreenPlayUtil::openJsonFileToObject(absoluteFilePath)) {

                if (obj->isEmpty())
                    continue;

                if (!obj->contains("type"))
                    continue;

                if (ScreenPlayUtil::getAvailableTypes().contains(obj->value("type").toString())) {
                    if (ScreenPlayUtil::getAvailableTypes().contains(obj->value("type").toString(), Qt::CaseInsensitive)) {
                        append(*obj, item.baseName(), isNew, item.lastModified());
                    }

                    counter += 1;
                }
            }
        }
        setCount(counter);
        emit installedLoadingFinished();
        m_isLoading = false;
    });
}

/*!
    \brief Used for receiving values from qml. One must add all new fields
           when adding new roles to this model.
*/
QVariantMap InstalledListModel::get(const QString& folderName) const
{

    if (m_screenPlayFiles.count() == 0)
        return {};

    const QString localInstalledPath = ScreenPlayUtil::toLocal(m_globalVariables->localStoragePath().toString());

    if (!QDir(localInstalledPath + "/" + folderName).exists()) {
        return {};
    }

    for (const auto& item : m_screenPlayFiles) {
        if (item.m_folderId == folderName) {
            QVariantMap map;
            map.insert("m_title", item.m_title);
            map.insert("m_preview", item.m_preview);
            map.insert("m_previewGIF", item.m_previewGIF);
            map.insert("m_file", item.m_file);
            map.insert("m_type", QVariant::fromValue(item.m_type));
            map.insert("m_absoluteStoragePath", item.m_absoluteStoragePath);
            map.insert("m_publishedFileID", item.m_publishedFileID);
            map.insert("m_isNew", item.m_isNew);
            map.insert("m_lastModified", item.m_lastModified);
            return map;
        }
    }

    return {};
}

/*!
    \brief Removes all entires and loads it again.
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
