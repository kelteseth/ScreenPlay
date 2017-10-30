#include "steamworkshoplistmodel.h"

SteamWorkshopListModel::SteamWorkshopListModel(QObject *parent)
    : QAbstractListModel(parent)
{

}

QHash<int, QByteArray> SteamWorkshopListModel::roleNames() const
{
    static const QHash<int, QByteArray> roles{
        { TitleRole, "workshopTitle" },
        { ImageUrlRole, "workshopPreview" },
        { IDRole, "workshopID" },
    };
    return roles;
}

QVariant SteamWorkshopListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    // FIXME: Implement me!
}

bool SteamWorkshopListModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role)
{
    if (value != headerData(section, orientation, role)) {
        // FIXME: Implement me!
        emit headerDataChanged(orientation, section, section);
        return true;
    }
    return false;
}

int SteamWorkshopListModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;

    return m_workshopItemList.count();
}

QVariant SteamWorkshopListModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() < rowCount())
        switch (role) {
        case TitleRole:
            return m_workshopItemList.at(index.row())->m_title;
        case ImageUrlRole:
            return m_workshopItemList.at(index.row())->m_previewImageUrl;
        case IDRole:
            return m_workshopItemList.at(index.row())->m_id;
        default:
            return QVariant();
        }
    return QVariant();
}

void SteamWorkshopListModel::append(unsigned int id, QString title, QUrl imgUrl)
{
    int row = 0;
    beginInsertRows(QModelIndex(), row, row);
    m_workshopItemList.append(QSharedPointer<WorkshopItem>(new WorkshopItem(id, title, imgUrl)));
    endInsertRows();
}

QUrl SteamWorkshopListModel::getBannerUrl()
{
    return m_workshopItemList.at(0)->m_previewImageUrl;
}

QString SteamWorkshopListModel::getBannerText()
{
    return m_workshopItemList.at(0)->m_title;
}

unsigned int SteamWorkshopListModel::getBannerID()
{
    return m_workshopItemList.at(0)->m_id;
}

bool SteamWorkshopListModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (data(index, role) != value) {
        // FIXME: Implement me!
        emit dataChanged(index, index, QVector<int>() << role);
        return true;
    }
    return false;
}

Qt::ItemFlags SteamWorkshopListModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsEditable; // FIXME: Implement me!
}

bool SteamWorkshopListModel::insertRows(int row, int count, const QModelIndex& parent)
{
    beginInsertRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    endInsertRows();
}

bool SteamWorkshopListModel::removeRows(int row, int count, const QModelIndex& parent)
{
    beginRemoveRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    endRemoveRows();
}
