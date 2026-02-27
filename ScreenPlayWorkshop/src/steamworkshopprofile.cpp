// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#include "steamworkshopprofile.h"
#include "steamworkshop.h"
#include "steamworkshopsearch.h"

#include <QLoggingCategory>

Q_LOGGING_CATEGORY(workshopProfile, "screenplay.workshop.profile")

namespace ScreenPlayWorkshop {

/*!
    \class ScreenPlayWorkshop::SteamWorkshopProfile
    \inmodule ScreenPlayWorkshop
    \brief Handles the user's own workshop items: fetching, pagination and stats.

    Owns the \c workshopProfileListModel and profile-related state.
    Exposed to QML via \c SteamWorkshop::profile.
*/

/*!
    \fn SteamWorkshopProfile::SteamWorkshopProfile(SteamWorkshop &facade, SteamWorkshopSearch &search, quint64 appID)
    \brief Constructs a SteamWorkshopProfile. The \a facade is used for online
           checks and account access; \a search provides the shared
           queryWorkshopItemFromHandle() helper; \a appID identifies the Steam app.
*/
SteamWorkshopProfile::SteamWorkshopProfile(SteamWorkshop& facade, SteamWorkshopSearch& search, quint64 appID)
    : m_facade(facade)
    , m_search(search)
    , m_appID(appID)
{
}

/*!
    \fn void SteamWorkshopProfile::init()
    \brief Creates the \c workshopProfileListModel. Must be called once by
           SteamWorkshop::init() before any profile methods are used.
*/
void SteamWorkshopProfile::init()
{
    m_workshopProfileListModel = std::make_unique<SteamWorkshopListModel>(m_appID);
}

/*!
    \fn void SteamWorkshopProfile::requestUserItems(ScreenPlayCore::Steam::EUserUGCList listType, ScreenPlayCore::Steam::EUserUGCListSortOrder sortOrder)
    \brief Fetches the current user's Workshop items of type \a listType,
           sorted by \a sortOrder. Clears the profile list model before
           issuing the query.
*/
void SteamWorkshopProfile::requestUserItems(
    const ScreenPlayCore::Steam::EUserUGCList listType,
    const ScreenPlayCore::Steam::EUserUGCListSortOrder sortOrder)
{
    if (!m_facade.checkAndSetQueryActive())
        return;

    if (!m_facade.checkOnline())
        return;

    m_currentProfileListType = listType;
    m_currentProfileSortOrder = sortOrder;
    m_workshopProfileListModel->clear();
    m_workshopProfileListModel->setIsLoading(true);

    auto apiCall = UGCQueryBuilder::userItems(
        m_facade.steamAccount()->accountID(), m_appID,
        static_cast<EUserUGCList>(listType),
        EUGCMatchingUGCType::k_EUGCMatchingUGCType_Items,
        static_cast<EUserUGCListSortOrder>(sortOrder),
        1)
                       .send();

    SteamAsyncCall<SteamUGCQueryCompleted_t>::create(apiCall, [this](auto* cb, bool io) { onRequestUserItemsReturned(cb, io); }, this);
}

/*!
    \fn bool SteamWorkshopProfile::loadNextProfilePage()
    \brief Fetches the next page of the user's Workshop items using the same
           list type and sort order as the most recent requestUserItems() call.
    \return \c false when there are no more pages or a query is already active.
*/
bool SteamWorkshopProfile::loadNextProfilePage()
{
    qCInfo(workshopProfile) << "loadNextProfilePage";

    if (!m_workshopProfileListModel->hasMore()) {
        qCInfo(workshopProfile) << "No more profile pages to load";
        return false;
    }

    if (!m_facade.checkAndSetQueryActive())
        return false;

    if (!m_facade.checkOnline())
        return false;

    m_workshopProfileListModel->incrementPage();
    m_workshopProfileListModel->setIsLoading(true);

    auto apiCall = UGCQueryBuilder::userItems(
        m_facade.steamAccount()->accountID(), m_appID,
        static_cast<EUserUGCList>(m_currentProfileListType),
        EUGCMatchingUGCType::k_EUGCMatchingUGCType_Items,
        static_cast<EUserUGCListSortOrder>(m_currentProfileSortOrder),
        m_workshopProfileListModel->currentPage())
                       .send();

    SteamAsyncCall<SteamUGCQueryCompleted_t>::create(apiCall, [this](auto* cb, bool io) { onRequestUserItemsReturned(cb, io); }, this);
    return true;
}

/*!
    \fn void SteamWorkshopProfile::onRequestUserItemsReturned(SteamUGCQueryCompleted_t *pCallback, bool bIOFailure)
    \brief Internal Steam async callback. Clears the active-query flag,
           delegates model population to SteamWorkshopSearch::queryWorkshopItemFromHandle(),
           then refreshes aggregate statistics via updateUserProfileStatistics().
*/
void SteamWorkshopProfile::onRequestUserItemsReturned(SteamUGCQueryCompleted_t* pCallback, bool bIOFailure)
{
    m_facade.setQueryActive(false);
    m_workshopProfileListModel->setIsLoading(false);
    if (bIOFailure) {
        qCWarning(workshopProfile) << "onRequestUserItemsReturned IO Failure";
        return;
    }

    m_search.queryWorkshopItemFromHandle(m_workshopProfileListModel.get(), pCallback);
    updateUserProfileStatistics();
}

/*!
    \fn void SteamWorkshopProfile::updateUserProfileStatistics()
    \brief Recalculates \c userPublishedItemCount and \c userTotalSubscriptions
           from the current contents of the profile list model and emits the
           relevant change signals when values differ.
*/
void SteamWorkshopProfile::updateUserProfileStatistics()
{
    const int itemCount = m_workshopProfileListModel->rowCount();
    quint64 totalSubscriptions = 0;

    for (int i = 0; i < itemCount; ++i) {
        const auto index = m_workshopProfileListModel->index(i, 0);
        const auto subscriptions = m_workshopProfileListModel->data(index, SteamWorkshopListModel::SubscriptionCountRole).toULongLong();
        totalSubscriptions += subscriptions;
    }

    if (m_userPublishedItemCount != itemCount) {
        m_userPublishedItemCount = itemCount;
        emit userPublishedItemCountChanged(m_userPublishedItemCount);
    }

    if (m_userTotalSubscriptions != totalSubscriptions) {
        m_userTotalSubscriptions = totalSubscriptions;
        emit userTotalSubscriptionsChanged(m_userTotalSubscriptions);
    }
}

} // namespace ScreenPlayWorkshop

#include "moc_steamworkshopprofile.cpp"
