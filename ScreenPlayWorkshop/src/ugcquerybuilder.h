// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#pragma once

#include <QDebug>
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
        .withDefaults()
        .withSearchText("space")
        .send();
    \endcode
*/
class UGCQueryBuilder {
public:
    /// Default constructor (invalid handle). Assign from a factory method before use.
    UGCQueryBuilder() = default;

    // --- Factory methods ---

    static UGCQueryBuilder allItems(AppId_t appID, EUGCQuery queryType, uint32 page)
    {
        UGCQueryBuilder b;
        b.m_handle = SteamUGC()->CreateQueryAllUGCRequest(
            queryType,
            k_EUGCMatchingUGCType_Items,
            appID, appID, page);
        return b;
    }

    static UGCQueryBuilder userItems(AccountID_t accountID, AppId_t appID,
        EUserUGCList list,
        EUGCMatchingUGCType matchingType,
        EUserUGCListSortOrder sortOrder,
        uint32 page)
    {
        UGCQueryBuilder b;
        b.m_handle = SteamUGC()->CreateQueryUserUGCRequest(
            accountID, list, matchingType, sortOrder,
            appID, appID, page);
        return b;
    }

    static UGCQueryBuilder details(PublishedFileId_t* ids, uint32 count)
    {
        UGCQueryBuilder b;
        b.m_handle = SteamUGC()->CreateQueryUGCDetailsRequest(ids, count);
        return b;
    }

    // --- Option setters (fluent) ---

    UGCQueryBuilder& withPreviews()
    {
        SteamUGC()->SetReturnAdditionalPreviews(m_handle, true);
        return *this;
    }

    UGCQueryBuilder& withKeyValueTags()
    {
        SteamUGC()->SetReturnKeyValueTags(m_handle, true);
        return *this;
    }

    UGCQueryBuilder& withLongDescription()
    {
        SteamUGC()->SetReturnLongDescription(m_handle, true);
        return *this;
    }

    UGCQueryBuilder& withChildren()
    {
        SteamUGC()->SetReturnChildren(m_handle, true);
        return *this;
    }

    /// Convenience: sets additional previews, key-value tags, and long description
    /// (the three flags every workshop search query needs).
    UGCQueryBuilder& withDefaults()
    {
        return withPreviews().withKeyValueTags().withLongDescription();
    }

    UGCQueryBuilder& withSearchText(const QString& text)
    {
        if (!text.isEmpty()) {
            if (!SteamUGC()->SetSearchText(m_handle, text.toUtf8().constData()))
                qWarning() << "UGCQueryBuilder: SetSearchText failed for:" << text;
        }
        return *this;
    }

    UGCQueryBuilder& withRequiredTags(const QStringList& tags)
    {
        for (const auto& tag : tags) {
            if (!SteamUGC()->AddRequiredTag(m_handle, tag.toUtf8().constData()))
                qWarning() << "UGCQueryBuilder: AddRequiredTag failed for:" << tag;
        }
        return *this;
    }

    // --- Accessors ---

    UGCQueryHandle_t handle() const { return m_handle; }

    /// Sends the query and returns the SteamAPICall_t handle.
    SteamAPICall_t send() { return SteamUGC()->SendQueryUGCRequest(m_handle); }

private:
    UGCQueryHandle_t m_handle = 0;
};

} // namespace ScreenPlayWorkshop
