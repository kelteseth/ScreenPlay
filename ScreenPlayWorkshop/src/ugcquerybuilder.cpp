// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#include "ugcquerybuilder.h"

#include <QDebug>

/*!
    \file ugcquerybuilder.cpp
    \brief Implementation of the fluent UGCQueryBuilder.

    Steam's UGC query API is a sequence of three steps that has to be
    repeated almost identically every time:

    \list
      \li Create the query handle (CreateQueryAllUGCRequest /
          CreateQueryUserUGCRequest / CreateQueryUGCDetailsRequest).
      \li Apply zero or more SetReturn* / SetSearchText / AddRequiredTag
          options that configure what comes back.
      \li SendQueryUGCRequest, then wait for the SteamAPICall_t to
          complete via a CCallResult.
    \endlist

    UGCQueryBuilder collapses that into a single chained expression and
    centralises the "always-on" options (additional previews, key/value
    tags, long description) inside applyDefaults(), so callers no longer
    have to remember to opt into them at every call site.

    Typical usage pairs this builder with SteamAsyncCall to dispatch and
    await the result:

    \code
    SteamAPICall_t apiCall = UGCQueryBuilder::allItems(appID, queryType, page)
        .withSearchText("space")
        .withRequiredTags({ "Anime", "NSFW" })
        .send();

    SteamAsyncCall<SteamUGCQueryCompleted_t>::create(apiCall,
        [this](auto* cb, bool ioFailure) { onWorkshopSearched(cb, ioFailure); },
        this);
    \endcode

    Errors from individual option setters are logged but do not abort the
    chain — the resulting query is still sent so the caller still gets a
    callback. If you need strict failure semantics, check each setter's
    return value at the call site instead.
*/

namespace ScreenPlayWorkshop {

/*!
    \brief Builds a query that returns every UGC item published for \a appID,
           paginated by \a page (Steam pages are 1-indexed). \a queryType picks
           the ranking/order (e.g. by vote, recency).
*/
UGCQueryBuilder UGCQueryBuilder::allItems(AppId_t appID, EUGCQuery queryType, uint32 page)
{
    UGCQueryBuilder b;
    b.m_handle = SteamUGC()->CreateQueryAllUGCRequest(
        queryType,
        k_EUGCMatchingUGCType_Items,
        appID, appID, page);
    b.applyDefaults();
    return b;
}

/*!
    \brief Builds a query restricted to items belonging to \a accountID — used
           for "my items" / profile views. \a list, \a matchingType and
           \a sortOrder narrow the result, \a page selects the page.
*/
UGCQueryBuilder UGCQueryBuilder::userItems(AccountID_t accountID, AppId_t appID,
    EUserUGCList list,
    EUGCMatchingUGCType matchingType,
    EUserUGCListSortOrder sortOrder,
    uint32 page)
{
    UGCQueryBuilder b;
    b.m_handle = SteamUGC()->CreateQueryUserUGCRequest(
        accountID, list, matchingType, sortOrder,
        appID, appID, page);
    b.applyDefaults();
    return b;
}

/*!
    \brief Builds a query for a specific set of \a count published file ids.
           Used to refresh metadata for items already known to the client.
*/
UGCQueryBuilder UGCQueryBuilder::details(PublishedFileId_t* ids, uint32 count)
{
    UGCQueryBuilder b;
    b.m_handle = SteamUGC()->CreateQueryUGCDetailsRequest(ids, count);
    b.applyDefaults();
    return b;
}

/*!
    \brief Asks Steam to include additional preview images (beyond the main
           thumbnail) in the result.
*/
UGCQueryBuilder& UGCQueryBuilder::withPreviews()
{
    const bool ok = SteamUGC()->SetReturnAdditionalPreviews(m_handle, true);
    return *this;
}

/*!
    \brief Asks Steam to include key/value metadata tags attached to each item.
*/
UGCQueryBuilder& UGCQueryBuilder::withKeyValueTags()
{
    SteamUGC()->SetReturnKeyValueTags(m_handle, true);
    return *this;
}

/*!
    \brief Asks Steam to include the full long-form description rather than
           the short summary.
*/
UGCQueryBuilder& UGCQueryBuilder::withLongDescription()
{
    SteamUGC()->SetReturnLongDescription(m_handle, true);
    return *this;
}

/*!
    \brief Asks Steam to include child item references (collections etc.).
           Not enabled by applyDefaults() because most queries do not need it.
*/
UGCQueryBuilder& UGCQueryBuilder::withChildren()
{
    SteamUGC()->SetReturnChildren(m_handle, true);
    return *this;
}

/*!
    \brief Adds a free-form search filter \a text. Empty input is a no-op.
           Logs a warning if Steam rejects the filter.
*/
UGCQueryBuilder& UGCQueryBuilder::withSearchText(const QString& text)
{
    if (!text.isEmpty()) {
        if (!SteamUGC()->SetSearchText(m_handle, text.toUtf8().constData()))
            qWarning() << "UGCQueryBuilder: SetSearchText failed for:" << text;
    }
    return *this;
}

/*!
    \brief Restricts results to items that carry every entry in \a tags.
           Each tag is added independently; failures are logged per tag rather
           than aborting the chain.
*/
UGCQueryBuilder& UGCQueryBuilder::withRequiredTags(const QStringList& tags)
{
    for (const auto& tag : tags) {
        if (!SteamUGC()->AddRequiredTag(m_handle, tag.toUtf8().constData()))
            qWarning() << "UGCQueryBuilder: AddRequiredTag failed for:" << tag;
    }
    return *this;
}

/*!
    \brief Dispatches the configured query and returns the SteamAPICall_t
           the caller can pass to SteamAsyncCall to await results.
*/
SteamAPICall_t UGCQueryBuilder::send()
{
    return SteamUGC()->SendQueryUGCRequest(m_handle);
}

/*!
    \brief Opt-ins applied automatically by every factory so the caller does
           not have to remember the common previews/key-values/long-description
           trio. withChildren() is intentionally excluded — most queries do
           not need it and it costs extra round-trip data.
*/
void UGCQueryBuilder::applyDefaults()
{
    withPreviews();
    withKeyValueTags();
    withLongDescription();
}

} // namespace ScreenPlayWorkshop
