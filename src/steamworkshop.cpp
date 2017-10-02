#include "steamworkshop.h"

SteamWorkshop::SteamWorkshop(QObject* parent)
    : QObject(parent)
{
}

SteamWorkshop::SteamWorkshop(AppId_t nConsumerAppId, SteamWorkshopListModel* wlm)
{
    m_AppId = nConsumerAppId;
    m_workshopListModel = wlm;
}

void SteamWorkshop::createWorkshopItem()
{
    SteamAPICall_t hSteamAPICall = SteamUGC()->CreateItem(m_AppId, EWorkshopFileType::k_EWorkshopFileTypeCommunity);
    m_createWorkshopItemCallResult.Set(hSteamAPICall, this, &SteamWorkshop::onWorkshopItemCreated);
}

void SteamWorkshop::submitWorkshopItem(QString title, QString description, QString language, int remoteStoragePublishedFileVisibility, QUrl absoluteContentPath, QUrl absolutePreviewPath)
{
    // We need to remove the file because steam wants a folder to upload
    QString tmpVideoPath = QFileInfo(absoluteContentPath.toString()).path();
    QString video = QString(tmpVideoPath).remove(0, 8);
    QString thumb = QString(absolutePreviewPath.toString()).remove(0, 8);

    qDebug() << video << thumb;
    SteamUGC()->SetItemTitle(m_UGCUpdateHandle, QByteArray(title.toLatin1()).data());
    SteamUGC()->SetItemDescription(m_UGCUpdateHandle, QByteArray(description.toLatin1()).data());
    SteamUGC()->SetItemUpdateLanguage(m_UGCUpdateHandle, QByteArray(language.toLatin1()).data());
    auto visibility = static_cast<ERemoteStoragePublishedFileVisibility>(remoteStoragePublishedFileVisibility);
    SteamUGC()->SetItemContent(m_UGCUpdateHandle, QByteArray(video.toLatin1()).data());
    SteamUGC()->SetItemPreview(m_UGCUpdateHandle, QByteArray(thumb.toLatin1()).data());
    SteamUGC()->SetItemVisibility(m_UGCUpdateHandle, visibility);
    SteamUGC()->SubmitItemUpdate(m_UGCUpdateHandle, nullptr);
}

int SteamWorkshop::getItemUpdateProcess()
{
    unsigned long long itemProcessed = 0;
    unsigned long long bytesTotoal = 0;
    EItemUpdateStatus status = SteamUGC()->GetItemUpdateProgress(m_UGCUpdateHandle, &itemProcessed, &bytesTotoal);
    qDebug() << itemProcessed << "," << bytesTotoal;
    return status;
}

void SteamWorkshop::getAPICallInfo()
{
    qDebug() << SteamUtils()->GetAPICallFailureReason(m_searchCall);
}

void SteamWorkshop::onWorkshopItemCreated(CreateItemResult_t* pCallback, bool bIOFailure)
{
    if (bIOFailure)
        return;

    emit workshopItemCreated(pCallback->m_bUserNeedsToAcceptWorkshopLegalAgreement, pCallback->m_eResult, pCallback->m_nPublishedFileId);
    m_UGCUpdateHandle = SteamUGC()->StartItemUpdate(m_AppId, pCallback->m_nPublishedFileId);
}

void SteamWorkshop::searchWorkshop()
{
    m_UGCSearchHandle = SteamUGC()->CreateQueryAllUGCRequest(EUGCQuery::k_EUGCQuery_RankedByVote,
        EUGCMatchingUGCType::k_EUGCMatchingUGCType_Items,
        m_AppId,
        m_AppId,
        1);

    //Important: First send the request to get the Steam API Call
    //           then set the handler
    m_searchCall = SteamUGC()->SendQueryUGCRequest(m_UGCSearchHandle);
    m_steamUGCQueryResult.Set(m_searchCall, this, &SteamWorkshop::onWorkshopSearched);
}

void SteamWorkshop::onWorkshopSearched(SteamUGCQueryCompleted_t* pCallback, bool bIOFailure)
{

    if (bIOFailure)
        return;

    SteamUGCDetails_t details;
    const int urlLength = 200;
    char url[urlLength];
    uint32 previews = 0;
    uint32 results = pCallback->m_unTotalMatchingResults;

    for (uint32 i = 0; i < results; i++) {
        if (SteamUGC()->GetQueryUGCResult(pCallback->m_handle, i, &details)) {
            if (SteamUGC()->GetQueryUGCPreviewURL(pCallback->m_handle, i, url, static_cast<uint32>(urlLength))) {
                QByteArray urlData(url);
                previews = SteamUGC()->GetQueryUGCNumAdditionalPreviews(pCallback->m_handle, i);

                m_workshopListModel->append(QString(details.m_rgchTitle),QUrl(urlData));
            }
        }
    }

    SteamUGC()->ReleaseQueryUGCRequest(pCallback->m_handle);

    emit workshopSearched();
}
