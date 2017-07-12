#ifndef STEAMWORKSHOP_H
#define STEAMWORKSHOP_H

#include <QObject>


#include "steam/steam_api.h"
#include <QDebug>

class SteamWorkshop : public QObject {
    Q_OBJECT
public:
    explicit SteamWorkshop(QObject *parent = nullptr);
    SteamWorkshop(AppId_t nConsumerAppId);


private:
    void onWorkshopItemCreated(CreateItemResult_t* pCallback, bool bIOFailure);
    CCallResult<SteamWorkshop, CreateItemResult_t> m_createWorkshopItemCallResult;

    AppId_t m_AppId;
public slots:
    void createWorkshopItem();

signals:
    void workshopItemCreatedQML(bool islegalAgreementAccepted, int eResult, int publishedFileId);
};

#endif // STEAMWORKSHOP_H
#
