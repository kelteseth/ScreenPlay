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

void SteamWorkshop::submitWorkshopItem(QString title, QString description, QString language, int remoteStoragePublishedFileVisibility, QUrl absoluteVideoPath, QUrl absolutePreviewPath)
{
    qDebug() << "Submit";
    SteamUGC()->SetItemTitle(m_UGCUpdateHandle, QByteArray(title.toLatin1()).data());
    SteamUGC()->SetItemDescription(m_UGCUpdateHandle, QByteArray(description.toLatin1()).data());
    SteamUGC()->SetItemUpdateLanguage(m_UGCUpdateHandle,QByteArray(language.toLatin1()).data());
    auto visibility = static_cast<ERemoteStoragePublishedFileVisibility>(remoteStoragePublishedFileVisibility);
    SteamUGC()->SetItemVisibility(m_UGCUpdateHandle,visibility);
    SteamUGC()->SubmitItemUpdate(m_UGCUpdateHandle,nullptr);
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
        EUGCMatchingUGCType::k_EUGCMatchingUGCType_Videos,
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
