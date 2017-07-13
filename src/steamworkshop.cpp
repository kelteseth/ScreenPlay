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

void SteamWorkshop::onWorkshopItemCreated(CreateItemResult_t* pCallback, bool bIOFailure)
{
    emit workshopItemCreated(pCallback->m_bUserNeedsToAcceptWorkshopLegalAgreement, pCallback->m_eResult, pCallback->m_nPublishedFileId);
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
