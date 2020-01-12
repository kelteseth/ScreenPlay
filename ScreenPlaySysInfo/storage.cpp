#include "storage.h"

Storage::Storage(QObject* parent)
    : QAbstractListModel(parent)
{
    loadStorageDevices();
}

QHash<int, QByteArray> Storage::roleNames() const
{
    static const QHash<int, QByteArray> roles {
        { static_cast<int>(StorageRole::Name), "name" },
        { static_cast<int>(StorageRole::DisplayName), "displayName" },
        { static_cast<int>(StorageRole::IsReadOnly), "isReadOnly" },
        { static_cast<int>(StorageRole::IsReady), "isReady" },
        { static_cast<int>(StorageRole::IsRoot), "isRoot" },
        { static_cast<int>(StorageRole::IsValid), "isValid" },
        { static_cast<int>(StorageRole::BytesAvailable), "bytesAvailable" },
        { static_cast<int>(StorageRole::BytesTotal), "bytesTotal" },
        { static_cast<int>(StorageRole::BytesFree), "bytesFree" },
        { static_cast<int>(StorageRole::FileSystemType), "fileSystemType" },
    };
    return roles;
}

int Storage::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;

    return m_storageInfoList.count();
}

QVariant Storage::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    int row = index.row();
    if (row < 0 || row >= m_storageInfoList.count()) {
        return QVariant();
    }

    auto roleEnum = static_cast<StorageRole>(role);

    if (index.row() < rowCount()) {
        switch (roleEnum) {
        case StorageRole::Name:
            return m_storageInfoList.at(row).name();
        case StorageRole::DisplayName:
            return m_storageInfoList.at(row).displayName();
        case StorageRole::IsReadOnly:
            return m_storageInfoList.at(row).isReady();
        case StorageRole::IsReady:
            return m_storageInfoList.at(row).isReady();
        case StorageRole::IsRoot:
            return m_storageInfoList.at(row).isRoot();
        case StorageRole::IsValid:
            return m_storageInfoList.at(row).isValid();
        case StorageRole::BytesAvailable:
            return m_storageInfoList.at(row).bytesAvailable();
        case StorageRole::BytesFree:
            return m_storageInfoList.at(row).bytesFree();
        case StorageRole::BytesTotal:
            return m_storageInfoList.at(row).bytesTotal();
        case StorageRole::FileSystemType:
            return m_storageInfoList.at(row).fileSystemType();
        }
    }

    return QVariant();
}

void Storage::refresh()
{
    reset();
    loadStorageDevices();
}

void Storage::reset()
{
    beginResetModel();
    m_storageInfoList.clear();
    m_storageInfoList.squeeze();
    endResetModel();
}

void Storage::loadStorageDevices()
{
    beginInsertRows(QModelIndex(), 0, rowCount());
    for (auto storage : QStorageInfo::mountedVolumes()) {
        m_storageInfoList.append(storage);
    }
    endInsertRows();
}
