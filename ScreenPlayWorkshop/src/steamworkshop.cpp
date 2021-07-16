#include "steamworkshop.h"

namespace ScreenPlayWorkshop {
/*!
    \class Steam Workshop
    \brief A wrapper class between the C Steam API and ScreenPlay

*/

SteamWorkshop::SteamWorkshop(AppId_t appID, QObject* parent)
    : QObject(parent)
    , m_appID(appID)
{
}

bool SteamWorkshop::init()
{
    if (SteamAPI_RestartAppIfNecessary(m_appID)) {
        qWarning() << "SteamAPI_RestartAppIfNecessary with appID" << m_appID;
        m_steamErrorRestart = true;
        return false;
    }

    //IF THE FAMILY SHARING IS ENABLED THIS WILL FAIL ! #13
    if (!SteamAPI_Init()) {
        m_steamErrorAPIInit = true;
        return false;
    }

    QObject::connect(&m_pollTimer, &QTimer::timeout, this, []() { SteamAPI_RunCallbacks(); });
    m_pollTimer.start(100);

    m_steamAccount = std::make_unique<SteamAccount>();
    m_workshopListModel = std::make_unique<SteamWorkshopListModel>(m_appID);
    m_workshopProfileListModel = std::make_unique<SteamWorkshopListModel>(m_appID);
    m_uploadListModel = std::make_unique<UploadListModel>();
    setOnline(true);
    return true;
}

bool SteamWorkshop::checkOnline()
{
    if (!m_online) {
        qWarning() << "Trying to call steam api while offline";
        qWarning() << "steamErrorAPIInit: " << m_steamErrorAPIInit;
        qWarning() << "steamErrorRestart: " << m_steamErrorRestart;
        return false;
    }
    return true;
}

void SteamWorkshop::bulkUploadToWorkshop(QStringList absoluteStoragePaths)
{
    for (const QString& path : absoluteStoragePaths) {
        qInfo() << "Append " << absoluteStoragePaths;
        uploadListModel()->append("", path, m_appID);
    }
}

void SteamWorkshop::onWorkshopItemInstalled(ItemInstalled_t* itemInstalled)
{
    //GetItemInstallInfo(itemInstalled->m_nPublishedFileId, uint64 *punSizeOnDisk, char *pchFolder, uint32 cchFolderSize, uint32 *punTimeStamp );
    emit workshopItemInstalled(itemInstalled->m_unAppID, itemInstalled->m_nPublishedFileId);
}

void SteamWorkshop::requestWorkshopItemDetails(const QVariant publishedFileID)
{
    if (!checkOnline())
        return;

    auto id = publishedFileID.toULongLong();
    auto uGCRegquestItemDetailHandle = SteamUGC()->CreateQueryUGCDetailsRequest(&id, 1);
    auto uGCRegquestItemDetailCall = SteamUGC()->SendQueryUGCRequest(uGCRegquestItemDetailHandle);
    m_steamUGCItemDetails.Set(uGCRegquestItemDetailCall, this, &SteamWorkshop::onRequestItemDetailReturned);
}

void SteamWorkshop::onRequestItemDetailReturned(SteamUGCQueryCompleted_t* pCallback, bool bIOFailure)
{

    if (bIOFailure) {
        qWarning() << "onRequestItemDetailReturned bIOFailure" << bIOFailure;
        return;
    }

    SteamUGCDetails_t details;
    for (uint32 i = 0; i < pCallback->m_unTotalMatchingResults; ++i) {
        if (SteamUGC()->GetQueryUGCResult(pCallback->m_handle, i, &details)) {

            emit requestItemDetailReturned(
                QString::fromUtf8(details.m_rgchTitle),
                QString::fromUtf8(details.m_rgchTags).split(","),
                details.m_ulSteamIDOwner,
                QString::fromUtf8(details.m_rgchDescription),
                details.m_unVotesUp,
                details.m_unVotesDown,
                QString::fromUtf8(details.m_rgchURL),
                QVariant::fromValue<int32>(details.m_nFileSize),
                QVariant::fromValue<uint64>(details.m_nPublishedFileId));
        } else {
            qWarning() << "GetQueryUGCResult failed!";
        }
    }
}

void SteamWorkshop::requestUserItems()
{
    if (!checkOnline())
        return;

    m_UGCListUserItemsHandle = SteamUGC()->CreateQueryUserUGCRequest(
        m_steamAccount->accountID(),
        EUserUGCList::k_EUserUGCList_Published,
        EUGCMatchingUGCType::k_EUGCMatchingUGCType_Items,
        EUserUGCListSortOrder::k_EUserUGCListSortOrder_LastUpdatedDesc,
        m_appID,
        m_appID,
        1);

    m_UGCListUserItemsCall = SteamUGC()->SendQueryUGCRequest(m_UGCListUserItemsHandle);
    m_steamUGCListUserItems.Set(m_UGCListUserItemsCall, this, &SteamWorkshop::onRequestUserItemsReturned);
    bool failed = false;

    if (!SteamUtils()->IsAPICallCompleted(m_UGCListUserItemsCall, &failed)) {
        qInfo() << "CreateQueryUserUGCRequest failed " << failed;
    }
}

void SteamWorkshop::vote(const QVariant publishedFileID, const bool voteUp)
{
    if (!checkOnline())
        return;

    SteamUGC()->SetUserItemVote(publishedFileID.toULongLong(), voteUp);
}

void SteamWorkshop::subscribeItem(const QVariant publishedFileID)
{
    if (!checkOnline())
        return;

    SteamUGC()->SubscribeItem(publishedFileID.toULongLong());
    m_steamAccount->loadAmountSubscribedItems();
}

void SteamWorkshop::searchWorkshop(const int enumEUGCQuery)
{
    if (!checkOnline())
        return;

    auto searchHandle = SteamUGC()->CreateQueryAllUGCRequest(
        static_cast<EUGCQuery>(enumEUGCQuery),
        EUGCMatchingUGCType::k_EUGCMatchingUGCType_Items,
        m_appID,
        m_appID,
        m_workshopListModel->currentPage());

    m_workshopListModel->clear();

    //Important: First send the request to get the Steam API Call then set the handler
    SteamUGC()->SetReturnAdditionalPreviews(searchHandle, true);
    SteamUGC()->SetReturnKeyValueTags(searchHandle, true);
    SteamUGC()->SetReturnLongDescription(searchHandle, true);
    m_steamUGCQuerySearchWorkshopResult.Set(SteamUGC()->SendQueryUGCRequest(searchHandle), this, &SteamWorkshop::onWorkshopSearched);
}

void SteamWorkshop::onWorkshopSearched(SteamUGCQueryCompleted_t* pCallback, bool bIOFailure)
{
    if (bIOFailure) {
        qWarning() << "onWorkshopSearched ioFailure";
        return;
    }
    queryWorkshopItemFromHandle(m_workshopListModel.get(), pCallback);
}

bool SteamWorkshop::queryWorkshopItemFromHandle(SteamWorkshopListModel* listModel, SteamUGCQueryCompleted_t* pCallback)
{

    qDebug() << "queryWorkshopItemFromHandle " << pCallback->m_unNumResultsReturned << pCallback->m_unTotalMatchingResults;

    SteamUGCDetails_t details;
    const int urlLength = 200;
    char url[urlLength];
    uint32 previews = 0;
    uint32 subscriber = 0;

    // Tags
    uint32 keyValueTags = 0;
    const int cchKeySize = 2000;
    char* cchKey[cchKeySize];
    const int cchValueSize = 2000;
    char* pchValue[cchValueSize];

    const uint32 totalResults = pCallback->m_unTotalMatchingResults;
    const uint32 results = pCallback->m_unNumResultsReturned;

    if (totalResults <= 0 || results <= 0) {
        qWarning() << "Invalid result count. Aborting! totalResults:" << totalResults << "results " << results;
        return {};
    }

    const float maxResultsPerPage = 50;
    const int pages = std::ceil(static_cast<double>(totalResults) / maxResultsPerPage);
    listModel->setPages(pages);

    for (uint32 i = 0; i < results; i++) {

        if (SteamUGC()->GetQueryUGCResult(pCallback->m_handle, i, &details)) {

            if (SteamUGC()->GetQueryUGCPreviewURL(pCallback->m_handle, i, url, static_cast<uint32>(urlLength))) {
                QByteArray urlData(url);

                //Todo use multiple preview for gif hover effect
                quint64 subscriptionCount = 0;
                SteamUGC()->GetQueryUGCStatistic(pCallback->m_handle, i, EItemStatistic::k_EItemStatistic_NumSubscriptions, &subscriptionCount);

                int addPreviewCount = SteamUGC()->GetQueryUGCNumAdditionalPreviews(pCallback->m_handle, i);
                QUrl additionalPreviewUrl;

                for (int j = 0; j < addPreviewCount; ++j) {
                    const int cchURLSize = 2000;
                    char pchURLOrVideoID[cchURLSize];
                    const int pchOriginalFileNameSize = 2000;
                    char pchOriginalFileName[pchOriginalFileNameSize];
                    EItemPreviewType previewType;
                    SteamUGC()->GetQueryUGCAdditionalPreview(pCallback->m_handle, i, j, pchURLOrVideoID, cchURLSize, pchOriginalFileName, pchOriginalFileNameSize, &previewType);
                    additionalPreviewUrl = QByteArray(pchURLOrVideoID);
                }

                WorkshopItem item { QVariant::fromValue<uint64>(details.m_nPublishedFileId), subscriptionCount, QString(details.m_rgchTitle), QUrl(urlData), additionalPreviewUrl };

                listModel->append(std::move(item));

                // Do not change the background image on every page
                if (i == 0 && listModel->currentPage() == 1) {
                    emit workshopSearched();
                }

                //                const int keyValueTagsCount = SteamUGC()->GetQueryUGCNumKeyValueTags(pCallback->m_handle, i);
                //                for (int j = 0; j < keyValueTagsCount; ++j) {
                //                    const int keySize = 2000;
                //                    char key[keySize];
                //                    const int valueSize = 2000;
                //                    char value[valueSize];
                //                    SteamUGC()->GetQueryUGCKeyValueTag(pCallback->m_handle, i, j, key, keySize, value, valueSize);
                //                }
            }
        } else {
            qWarning() << "Loading error! Index: " << i;
        }
    }

    SteamUGC()->ReleaseQueryUGCRequest(pCallback->m_handle);
    return true;
}

void SteamWorkshop::searchWorkshopByText(const QString& text, const ScreenPlayWorkshopSteamEnums::EUGCQuery rankedBy)
{
    if (!checkOnline())
        return;

    auto searchHandle = SteamUGC()->CreateQueryAllUGCRequest(
        static_cast<EUGCQuery>(rankedBy),
        EUGCMatchingUGCType::k_EUGCMatchingUGCType_Items,
        m_appID,
        m_appID,
        m_workshopListModel->currentPage());

    m_workshopListModel->clear();

    if (!SteamUGC()->SetSearchText(searchHandle, QByteArray(text.toUtf8()).data())) {
        qWarning() << "Search Failed with query: " << text;
        return;
    }

    //Important: First send the request to get the Steam API Call then set the handler
    m_steamUGCQuerySearchWorkshopResult.Set(SteamUGC()->SendQueryUGCRequest(searchHandle), this, &SteamWorkshop::onWorkshopSearched);
}

void SteamWorkshop::onRequestUserItemsReturned(SteamUGCQueryCompleted_t* pCallback, bool bIOFailure)
{
    if (bIOFailure) {
        qDebug() << bIOFailure;
        return;
    }

    queryWorkshopItemFromHandle(m_workshopProfileListModel.get(), pCallback);

    //    SteamUGCDetails_t details;
    //    for (uint32 i = 0; i < pCallback->m_unTotalMatchingResults; ++i) {
    //        if (SteamUGC()->GetQueryUGCResult(pCallback->m_handle, i, &details)) {

    //            WorkshopItem item { QVariant::fromValue<uint64>(details.m_nPublishedFileId), subscriptionCount, QString(details.m_rgchTitle), QUrl(urlData), additionalPreviewUrl };

    //            m_workshopListModel->append(item);
    //            qInfo()
    //                << details.m_rgchTitle
    //                << details.m_unVotesDown
    //                << details.m_unVotesDown
    //                << details.m_rgchURL
    //                << details.m_nFileSize
    //                << details.m_nPublishedFileId;
    //        }
    //    }
}
}
