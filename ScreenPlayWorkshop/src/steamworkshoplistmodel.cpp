// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#include "steamworkshoplistmodel.h"

namespace ScreenPlayWorkshop {

SteamWorkshopListModel::SteamWorkshopListModel(AppId_t appID, QObject* parent)
    : QAbstractListModel(parent)
    , m_appID(appID)
{
}

QHash<int, QByteArray> SteamWorkshopListModel::roleNames() const
{
    static const QHash<int, QByteArray> roles {
        { TitleRole, "m_workshopTitle" },
        { ImageUrlRole, "m_workshopPreview" },
        { AdditionalPreviewUrlRole, "m_additionalPreviewUrl" },
        { PublishedFileIDRole, "m_publishedFileID" },
        { SubscriptionCountRole, "m_subscriptionCount" },
    };
    return roles;
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

    const int row = index.row();
    if (row < 0 || row >= m_workshopItemList.count()) {
        return QVariant();
    }

    if (index.row() < rowCount())
        switch (role) {
        case TitleRole:
            return m_workshopItemList.at(row).m_title;
        case ImageUrlRole:
            return m_workshopItemList.at(row).m_previewImageUrl;
        case AdditionalPreviewUrlRole:
            return m_workshopItemList.at(row).m_additionalPreviewUrl;
        case PublishedFileIDRole:
            return m_workshopItemList.at(row).m_publishedFileID;
        case SubscriptionCountRole:
            return m_workshopItemList.at(row).m_subscriptionCount;
        default:
            return QVariant();
        }
    return QVariant();
}

void SteamWorkshopListModel::append(WorkshopItem item)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_workshopItemList.append(std::move(item));
    endInsertRows();
}

void SteamWorkshopListModel::clear()
{
    if (m_workshopItemList.isEmpty())
        return;

    beginResetModel();
    m_workshopItemList.clear();
    m_workshopItemList.squeeze();
    endResetModel();
}

}

#include "moc_steamworkshoplistmodel.cpp"
