// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#include "steamworkshopsearch.h"
#include "steamworkshop.h"

#include <QLoggingCategory>

Q_LOGGING_CATEGORY(workshopSearch, "screenplay.workshop.search")

namespace ScreenPlayWorkshop {

/*!
    \class ScreenPlayWorkshop::SteamWorkshopSearch
    \inmodule ScreenPlayWorkshop
    \brief Handles workshop browse, text search, user search and pagination.

    Owns the main \c workshopListModel and all search-related state.
    Exposed to QML via \c SteamWorkshop::search.
*/

/*!
    \fn SteamWorkshopSearch::SteamWorkshopSearch(SteamWorkshop &facade, quint64 appID)
    \brief Constructs a SteamWorkshopSearch that operates on behalf of \a facade
           using the Steam App ID \a appID.
*/
SteamWorkshopSearch::SteamWorkshopSearch(SteamWorkshop& facade, quint64 appID)
    : m_facade(facade)
    , m_appID(appID)
{
}

/*!
    \fn void SteamWorkshopSearch::init()
    \brief Creates the \c workshopListModel. Must be called once by
           SteamWorkshop::init() before any search methods are used.
*/
void SteamWorkshopSearch::init()
{
    m_workshopListModel = std::make_unique<SteamWorkshopListModel>(m_appID);
}

/*!
    \fn bool SteamWorkshopSearch::searchWorkshop(ScreenPlayCore::Steam::EUGCQuery enumEUGCQuery)
    \brief Starts a fresh ranked browse query using the sort order specified by
           \a enumEUGCQuery. Resets the list model and emits \c workshopSearchCompleted
           when the Steam callback returns.
    \return \c false if a query is already active, Steam is offline, or
            \c SteamUGC() is null.
*/
bool SteamWorkshopSearch::searchWorkshop(const ScreenPlayCore::Steam::EUGCQuery enumEUGCQuery)
{
    qCInfo(workshopSearch) << "searchWorkshop";

    if (!m_facade.checkAndSetQueryActive())
        return false;

    if (!m_facade.checkOnline())
        return false;

    if (m_searchHandle != 0) {
        qCInfo(workshopSearch) << "Invalid m_searchHandle";
        return false;
    }

    if (!SteamUGC()) {
        qCWarning(workshopSearch) << "SteamUGC() returned null in searchWorkshop - Steam API not properly initialized";
        m_facade.setQueryActive(false);
        return false;
    }

    m_currentQueryType = enumEUGCQuery;
    m_currentSearchText.clear();
    m_currentUserAccountID = 0;

    m_workshopListModel->reset();
    m_workshopListModel->setIsLoading(true);

    auto query = UGCQueryBuilder::allItems(m_appID, static_cast<EUGCQuery>(enumEUGCQuery), m_workshopListModel->currentPage());

    m_searchHandle = query.handle();
    qCInfo(workshopSearch) << m_searchHandle;

    SteamAsyncCall<SteamUGCQueryCompleted_t>::create(query.send(), [this](auto* cb, bool io) { onWorkshopSearched(cb, io); }, this);
    return true;
}

/*!
    \fn bool SteamWorkshopSearch::loadNextPage()
    \brief Fetches the next page of results for the current query (browse, text
           search, or user search). Does nothing and returns \c false when
           there are no more pages or a query is already active.
*/
bool SteamWorkshopSearch::loadNextPage()
{
    qCInfo(workshopSearch) << "loadNextPage";

    if (!m_workshopListModel->hasMore()) {
        qCInfo(workshopSearch) << "No more pages to load";
        return false;
    }

    if (!m_facade.checkAndSetQueryActive())
        return false;

    if (!m_facade.checkOnline())
        return false;

    if (m_searchHandle != 0) {
        qCInfo(workshopSearch) << "Invalid m_searchHandle";
        return false;
    }

    if (!SteamUGC()) {
        qCWarning(workshopSearch) << "SteamUGC() returned null - Steam API not properly initialized";
        return false;
    }

    m_workshopListModel->incrementPage();
    m_workshopListModel->setIsLoading(true);

    const auto page = m_workshopListModel->currentPage();
    UGCQueryBuilder query;

    if (m_currentUserAccountID != 0) {
        query = UGCQueryBuilder::userItems(
            m_currentUserAccountID, m_appID,
            EUserUGCList::k_EUserUGCList_Published,
            EUGCMatchingUGCType::k_EUGCMatchingUGCType_All,
            EUserUGCListSortOrder::k_EUserUGCListSortOrder_LastUpdatedDesc,
            page);
    } else {
        query = UGCQueryBuilder::allItems(m_appID, static_cast<EUGCQuery>(m_currentQueryType), page);
        if (!m_currentSearchText.isEmpty()) {
            const ParsedSearch parsed { m_currentSearchText, m_currentSearchTags };
            query.withSearchText(parsed.text).withRequiredTags(parsed.tags);
        }
    }

    SteamAsyncCall<SteamUGCQueryCompleted_t>::create(query.send(), [this](auto* cb, bool io) { onWorkshopSearched(cb, io); }, this);
    return true;
}

/*!
    \fn void SteamWorkshopSearch::searchWorkshopByText(const QString &text, ScreenPlayCore::Steam::EUGCQuery rankedBy)
    \brief Searches the Workshop for \a text, ranked by \a rankedBy. Quoted
           substrings in \a text are automatically extracted as tag filters via
           parseSearchInput(). Resets the list model before issuing the query.
*/
void SteamWorkshopSearch::searchWorkshopByText(const QString& text, const ScreenPlayCore::Steam::EUGCQuery rankedBy)
{
    qCInfo(workshopSearch) << "searchWorkshopByText" << text;

    if (!m_facade.checkAndSetQueryActive())
        return;

    if (!m_facade.checkOnline())
        return;

    if (!SteamUGC()) {
        qCWarning(workshopSearch) << "SteamUGC() returned null in searchWorkshopByText - Steam API not properly initialized";
        m_facade.setQueryActive(false);
        return;
    }

    m_currentQueryType = rankedBy;
    m_currentSearchText = text;
    m_currentUserAccountID = 0;

    const auto parsed = parseSearchInput(text);
    m_currentSearchTags = parsed.tags;

    m_workshopListModel->reset();
    m_workshopListModel->setIsLoading(true);

    auto apiCall = UGCQueryBuilder::allItems(m_appID, static_cast<EUGCQuery>(rankedBy), m_workshopListModel->currentPage())
                       .withSearchText(parsed.text)
                       .withRequiredTags(parsed.tags)
                       .send();

    SteamAsyncCall<SteamUGCQueryCompleted_t>::create(apiCall, [this](auto* cb, bool io) { onWorkshopSearched(cb, io); }, this);
}

/*!
    \fn void SteamWorkshopSearch::searchWorkshopByUser(const QString &steamID64)
    \brief Filters the Workshop to show only items created by the user identified
           by \a steamID64. Resets the list model before issuing the query.
*/
void SteamWorkshopSearch::searchWorkshopByUser(const QString& steamID64)
{
    qCInfo(workshopSearch) << "searchWorkshopByUser" << steamID64;

    if (!m_facade.checkAndSetQueryActive())
        return;

    if (!m_facade.checkOnline())
        return;

    if (!SteamUGC()) {
        qCWarning(workshopSearch) << "SteamUGC() returned null in searchWorkshopByUser";
        m_facade.setQueryActive(false);
        return;
    }

    const CSteamID creatorID(steamID64.toULongLong());
    m_currentUserAccountID = creatorID.GetAccountID();
    m_currentSearchText.clear();

    m_workshopListModel->reset();
    m_workshopListModel->setIsLoading(true);

    auto apiCall = UGCQueryBuilder::userItems(
        m_currentUserAccountID, m_appID,
        EUserUGCList::k_EUserUGCList_Published,
        EUGCMatchingUGCType::k_EUGCMatchingUGCType_All,
        EUserUGCListSortOrder::k_EUserUGCListSortOrder_LastUpdatedDesc,
        m_workshopListModel->currentPage())
                       .send();

    SteamAsyncCall<SteamUGCQueryCompleted_t>::create(apiCall, [this](auto* cb, bool io) { onWorkshopSearched(cb, io); }, this);
}

/*!
    \fn void SteamWorkshopSearch::onWorkshopSearched(SteamUGCQueryCompleted_t *pCallback, bool bIOFailure)
    \brief Internal Steam async callback. Clears the active-query flag,
           delegates model population to queryWorkshopItemFromHandle(), and
           resets the list-model loading state.
*/
void SteamWorkshopSearch::onWorkshopSearched(SteamUGCQueryCompleted_t* pCallback, bool bIOFailure)
{
    m_facade.setQueryActive(false);
    m_searchHandle = 0;
    m_workshopListModel->setIsLoading(false);
    if (bIOFailure) {
        qCWarning(workshopSearch) << "onWorkshopSearched ioFailure";
        return;
    }

    qCInfo(workshopSearch) << "onWorkshopSearched";
    queryWorkshopItemFromHandle(m_workshopListModel.get(), pCallback);
}

/*!
    \fn bool SteamWorkshopSearch::queryWorkshopItemFromHandle(SteamWorkshopListModel *listModel, SteamUGCQueryCompleted_t *pCallback)
    \brief Shared helper that iterates the results in \a pCallback, constructs
           \c WorkshopItem values and appends them to \a listModel. Emits
           \c workshopBannerCompleted for the first result on the first page and
           \c workshopSearchCompleted when done.
    \return \c false when the callback reports zero matching results.
*/
bool SteamWorkshopSearch::queryWorkshopItemFromHandle(SteamWorkshopListModel* listModel, SteamUGCQueryCompleted_t* pCallback)
{
    qCInfo(workshopSearch) << "queryWorkshopItemFromHandle";

    SteamUGCDetails_t details;
    constexpr int urlLength = 200;
    std::array<char, 200> url {};

    const uint32 totalResults = pCallback->m_unTotalMatchingResults;
    const uint32 results = pCallback->m_unNumResultsReturned;

    if (totalResults <= 0 || results <= 0) {
        qCWarning(workshopSearch) << "Invalid result count. Aborting! totalResults:" << totalResults << "results " << results;
        SteamUGC()->ReleaseQueryUGCRequest(pCallback->m_handle);
        emit workshopSearchCompleted(0);
        return false;
    }

    const float maxResultsPerPage = 50;
    const int pages = std::ceil(static_cast<double>(totalResults) / maxResultsPerPage);
    listModel->setPages(pages);

    for (uint32 i = 0; i < results; i++) {

        if (SteamUGC()->GetQueryUGCResult(pCallback->m_handle, i, &details)) {

            if (SteamUGC()->GetQueryUGCPreviewURL(pCallback->m_handle, i, url.data(), static_cast<uint32>(urlLength))) {
                QByteArray urlData(url.data());

                quint64 subscriptionCount = 0;
                SteamUGC()->GetQueryUGCStatistic(pCallback->m_handle, i, EItemStatistic::k_EItemStatistic_NumSubscriptions, &subscriptionCount);

                int addPreviewCount = SteamUGC()->GetQueryUGCNumAdditionalPreviews(pCallback->m_handle, i);
                QUrl additionalPreviewUrl; // animated preview (GIF or Steam-converted WebP→GIF)
                qInfo() << "Item" << details.m_rgchTitle
                        << "mainPreview:" << urlData
                        << "addPreviewCount:" << addPreviewCount;

                for (int j = 0; j < addPreviewCount; ++j) {
                    std::array<char, 2000> pchURLOrVideoID {};
                    std::array<char, 2000> pchOriginalFileName {};
                    EItemPreviewType previewType;
                    SteamUGC()->GetQueryUGCAdditionalPreview(
                        pCallback->m_handle,
                        i,
                        j,
                        pchURLOrVideoID.data(),
                        pchURLOrVideoID.size(),
                        pchOriginalFileName.data(),
                        pchOriginalFileName.size(),
                        &previewType);
                    const QString previewUrl = QByteArray(pchURLOrVideoID.data());
                    const QString originalFileName = QByteArray(pchOriginalFileName.data());
                    qInfo() << "Additional preview" << j << "of" << addPreviewCount
                            << "url:" << previewUrl
                            << "originalFileName:" << originalFileName
                            << "previewType:" << static_cast<int>(previewType);

                    // Steam CDN URLs have no file extension (ugc/{id}/{hash}/?imw=...),
                    // so we must classify by originalFileName.
                    // Animated previews: .gif, .webp (Steam converts WebP→GIF on serve).
                    // Static previews: .jpg, .png — skip, we already have the main preview.
                    if (originalFileName.endsWith(".gif", Qt::CaseInsensitive)
                        || originalFileName.endsWith(".webp", Qt::CaseInsensitive)) {
                        // Strip query params (imw, imh, impolicy, etc.) — they cause
                        // Steam's CDN to re-encode/resize, which can break animation.
                        QUrl rawUrl(previewUrl);
                        rawUrl.setQuery(QString());
                        additionalPreviewUrl = rawUrl;
                    }
                    // .jpg/.png additional previews are intentionally ignored
                }
                qInfo() << "Resolved animated preview for" << details.m_rgchTitle
                        << "url:" << additionalPreviewUrl;

                WorkshopItem item {
                    QVariant::fromValue<uint64>(details.m_nPublishedFileId),
                    subscriptionCount,
                    QString(details.m_rgchTitle),
                    QUrl(urlData),
                    additionalPreviewUrl,
                    QUrl(), // WebP URL unused — Steam converts WebP to GIF
                    QString(details.m_rgchTags).split(",", Qt::SkipEmptyParts),
                    details.m_ulSteamIDOwner == m_facade.steamAccount()->steamID64(),
                    details.m_ulSteamIDOwner
                };

                listModel->append(std::move(item));

                if (i == 0 && listModel->currentPage() == 1) {
                    emit workshopBannerCompleted();
                    m_facade.requestCreatorName(QString::number(details.m_ulSteamIDOwner));
                }
            }
        } else {
            qCWarning(workshopSearch) << "Loading error! Index: " << i;
        }
    }

    qCInfo(workshopSearch) << m_searchHandle << pCallback->m_handle;
    SteamUGC()->ReleaseQueryUGCRequest(pCallback->m_handle);

    emit workshopSearchCompleted(results);
    return true;
}

/*!
    \fn SteamWorkshopSearch::ParsedSearch SteamWorkshopSearch::parseSearchInput(const QString &input) const
    \brief Splits \a input into a plain search \c text and a list of \c tags.
           Any double-quoted substrings (e.g. \c{"Relaxing"}) are removed from
           the text and treated as required tags.
*/
SteamWorkshopSearch::ParsedSearch SteamWorkshopSearch::parseSearchInput(const QString& input) const
{
    ParsedSearch result;
    QStringList tags;
    QString remaining = input;

    static const QRegularExpression tagRegex("\"([^\"]+)\"");
    auto it = tagRegex.globalMatch(input);
    while (it.hasNext()) {
        const auto match = it.next();
        tags.append(match.captured(1).trimmed());
        remaining.replace(match.captured(0), "");
    }

    result.text = remaining.simplified();
    result.tags = tags;

    if (!tags.isEmpty()) {
        qCInfo(workshopSearch) << "parseSearchInput: text=" << result.text << "tags=" << result.tags;
    }

    return result;
}

} // namespace ScreenPlayWorkshop

#include "moc_steamworkshopsearch.cpp"
