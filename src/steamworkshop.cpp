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
