#pragma once

#include <QByteArray>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QObject>
#include <QUrl>
#include <QFuture>
#include <QtConcurrent/QtConcurrent>

#include "steam/steam_api.h"
#include "steamworkshoplistmodel.h"
#include "settings.h"

class SteamWorkshop : public QObject {
    Q_OBJECT
public:
    explicit SteamWorkshop(QObject* parent = nullptr);
    SteamWorkshop(AppId_t nConsumerAppId, SteamWorkshopListModel* wlm, Settings* s);


public slots:
    void searchWorkshop();
    void createWorkshopItem();
    void submitWorkshopItem(QString title, QString description, QString language, int remoteStoragePublishedFileVisibility, QUrl absoluteContentPath, QUrl absolutePreviewPath);
    int getItemUpdateProcess();
    void getAPICallInfo();
    void createLocalWorkshopItem(QString title, QUrl videoPath, QUrl previewPath);

signals:
    void workshopItemCreated(bool userNeedsToAcceptWorkshopLegalAgreement, int eResult, int publishedFileId);
    void workshopSearched();
    void localFileCopyCompleted(bool successful);

private:
    void onWorkshopItemCreated(CreateItemResult_t* pCallback, bool bIOFailure);
    CCallResult<SteamWorkshop, CreateItemResult_t> m_createWorkshopItemCallResult;

    void onWorkshopSearched(SteamUGCQueryCompleted_t* pCallback, bool bIOFailure);
    CCallResult<SteamWorkshop, SteamUGCQueryCompleted_t> m_steamUGCQueryResult;

    AppId_t m_AppId;
    UGCUpdateHandle_t m_UGCUpdateHandle;
    UGCQueryHandle_t m_UGCSearchHandle;
    SteamAPICall_t m_searchCall;
    SteamWorkshopListModel* m_workshopListModel;
    Settings* m_settings;
};
