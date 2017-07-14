#include "steamworkshop.h"

SteamWorkshop::SteamWorkshop(QObject* parent)
    : QObject(parent)
{
}

SteamWorkshop::SteamWorkshop(AppId_t nConsumerAppId)
{
    m_AppId = nConsumerAppId;
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
    QString thumb = QString(absolutePreviewPath.toString()).remove(0,8);
    qDebug() << video << thumb;
    SteamUGC()->SetItemTitle(m_UGCUpdateHandle, QByteArray(title.toLatin1()).data());
    SteamUGC()->SetItemDescription(m_UGCUpdateHandle, QByteArray(description.toLatin1()).data());
    SteamUGC()->SetItemUpdateLanguage(m_UGCUpdateHandle, QByteArray(language.toLatin1()).data());
    auto visibility = static_cast<ERemoteStoragePublishedFileVisibility>(remoteStoragePublishedFileVisibility);
    SteamUGC()->SetItemContent(m_UGCUpdateHandle, QByteArray(video.toLatin1()).data());
    //SteamUGC()->SetItemPreview(m_UGCUpdateHandle, QByteArray(thumb.toLatin1()).data());
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

void SteamWorkshop::onWorkshopItemCreated(CreateItemResult_t* pCallback, bool bIOFailure)
{
    emit workshopItemCreated(pCallback->m_bUserNeedsToAcceptWorkshopLegalAgreement, pCallback->m_eResult, pCallback->m_nPublishedFileId);
    m_UGCUpdateHandle = SteamUGC()->StartItemUpdate(m_AppId, pCallback->m_nPublishedFileId);
}

void SteamWorkshop::searchWorkshop()
{
    qDebug() << "search";
    UGCQueryHandle_t hUGCQueryHandle = SteamUGC()->CreateQueryAllUGCRequest(EUGCQuery::k_EUGCQuery_RankedByTrend,
        EUGCMatchingUGCType::k_EUGCMatchingUGCType_All,
        m_AppId,
        m_AppId,
        1);
    m_steamUGCQueryResult.Set(hUGCQueryHandle, this, &SteamWorkshop::onWorkshopSearched);
    SteamUGC()->SendQueryUGCRequest(hUGCQueryHandle);
}

void SteamWorkshop::onWorkshopSearched(SteamUGCQueryCompleted_t* pCallback, bool bIOFailure)
{
    qDebug() << "searchend";
    emit workshopSearched();
}
