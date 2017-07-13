#ifndef STEAMWORKSHOP_H
#define STEAMWORKSHOP_H

#include <QObject>

#include "steam/steam_api.h"
#include <QDebug>

class SteamWorkshop : public QObject {
    Q_OBJECT
public:
    explicit SteamWorkshop(QObject* parent = nullptr);
    SteamWorkshop(AppId_t nConsumerAppId);

public slots:
    void searchWorkshop();
    void createWorkshopItem();

signals:
    void workshopItemCreated(bool userNeedsToAcceptWorkshopLegalAgreement, int eResult, int publishedFileId);
    void workshopSearched();

private:
    void onWorkshopItemCreated(CreateItemResult_t* pCallback, bool bIOFailure);
    CCallResult<SteamWorkshop, CreateItemResult_t> m_createWorkshopItemCallResult;

    void onWorkshopSearched(SteamUGCQueryCompleted_t* pCallback, bool bIOFailure);
    CCallResult<SteamWorkshop, SteamUGCQueryCompleted_t> m_steamUGCQueryResult;

    AppId_t m_AppId;
};

#endif // STEAMWORKSHOP_H
#
