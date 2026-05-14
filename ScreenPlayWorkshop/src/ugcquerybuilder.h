// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#pragma once

#include <QString>
#include <QStringList>

#include "steam/steam_api.h"

namespace ScreenPlayWorkshop {

/*!
    \class ScreenPlayWorkshop::UGCQueryBuilder
    \inmodule ScreenPlayWorkshop
    \brief Fluent builder for Steam UGC queries, replacing the repetitive
           Create / Set* / Send pattern scattered across SteamWorkshop methods.

    Usage:
    \code
    auto apiCall = UGCQueryBuilder::allItems(appID, queryType, page)
        .withSearchText("space")
        .send();
    \endcode
*/
class UGCQueryBuilder {
public:
    /// Default constructor (invalid handle). Assign from a factory method before use.
    UGCQueryBuilder() = default;

    static UGCQueryBuilder allItems(AppId_t appID, EUGCQuery queryType, uint32 page);
    static UGCQueryBuilder userItems(AccountID_t accountID, AppId_t appID,
        EUserUGCList list,
        EUGCMatchingUGCType matchingType,
        EUserUGCListSortOrder sortOrder,
        uint32 page);
    static UGCQueryBuilder details(PublishedFileId_t* ids, uint32 count);

    UGCQueryBuilder& withPreviews();
    UGCQueryBuilder& withKeyValueTags();
    UGCQueryBuilder& withLongDescription();
    UGCQueryBuilder& withChildren();
    UGCQueryBuilder& withSearchText(const QString& text);
    UGCQueryBuilder& withRequiredTags(const QStringList& tags);

    UGCQueryHandle_t handle() const { return m_handle; }

    /// Sends the query and returns the SteamAPICall_t handle.
    SteamAPICall_t send();

private:
    UGCQueryHandle_t m_handle = 0;

    /// Applied automatically by every factory method.
    void applyDefaults();
};

} // namespace ScreenPlayWorkshop
