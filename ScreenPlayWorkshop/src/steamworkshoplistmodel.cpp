// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#include "steamworkshoplistmodel.h"

namespace ScreenPlayWorkshop {

/*!
    \class ScreenPlayWorkshop::SteamWorkshopListModel
    \inmodule ScreenPlayWorkshop
    \brief Gets filled and managed by the SteamWorkshop class.
*/

/*!
    \fn SteamWorkshopListModel::SteamWorkshopListModel(AppId_t appID, QObject *parent)
    \brief Constructs a SteamWorkshopListModel for the Steam app identified by
           \a appID with the given \a parent.
*/
SteamWorkshopListModel::SteamWorkshopListModel(AppId_t appID, QObject* parent)
    : QAbstractListModel(parent)
    , m_appID(appID)
{
}

/*!
    \fn QHash<int, QByteArray> SteamWorkshopListModel::roleNames() const
    \brief Returns the role-name map used by QML delegates to access item data
           by name.
*/
QHash<int, QByteArray> SteamWorkshopListModel::roleNames() const
{
    static const QHash<int, QByteArray> roles {
        { TitleRole, "m_workshopTitle" },
        { ImageUrlRole, "m_workshopPreview" },
        { AdditionalPreviewUrlRole, "m_additionalPreviewUrl" },
        { PublishedFileIDRole, "m_publishedFileID" },
        { SubscriptionCountRole, "m_subscriptionCount" },
        { TagsRole, "m_tags" },
        { IsOwnItemRole, "m_isOwnItem" },
    };
    return roles;
}

/*!
    \fn int SteamWorkshopListModel::rowCount(const QModelIndex &parent) const
    \brief Returns the number of workshop items currently in the model.
           Always returns 0 for a valid \a parent (flat list).
*/
int SteamWorkshopListModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;

    return m_workshopItemList.count();
}

/*!
    \fn QVariant SteamWorkshopListModel::data(const QModelIndex &index, int role) const
    \brief Returns the data for the item at \a index under the given \a role.
           Returns an invalid QVariant for out-of-bounds indices or unknown roles.
*/
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
        case TagsRole:
            return m_workshopItemList.at(row).m_tags;
        case IsOwnItemRole:
            return m_workshopItemList.at(row).m_isOwnItem;
        default:
            return QVariant();
        }
    return QVariant();
}

/*!
    \fn void SteamWorkshopListModel::append(WorkshopItem item)
    \brief Appends \a item to the model. Emits \c bannerUrlChanged if this is
           the first item added (so the banner image is updated immediately).
*/
void SteamWorkshopListModel::append(WorkshopItem item)
{
    const bool wasEmpty = m_workshopItemList.isEmpty();
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_workshopItemList.append(std::move(item));
    endInsertRows();
    if (wasEmpty) {
        emit bannerUrlChanged();
    }
}

/*!
    \fn void SteamWorkshopListModel::clear()
    \brief Removes all items from the model and emits \c bannerUrlChanged to
           clear any banner image displayed in the UI.
*/
void SteamWorkshopListModel::clear()
{
    if (m_workshopItemList.isEmpty())
        return;

    beginResetModel();
    m_workshopItemList.clear();
    m_workshopItemList.squeeze();
    endResetModel();
    emit bannerUrlChanged();
}

}

#include "moc_steamworkshoplistmodel.cpp"
