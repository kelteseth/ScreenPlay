#pragma once

#include "settings.h"
#include "steam/steam_api.h"
#include "steamworkshoplistmodel.h"
#include <QByteArray>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFuture>
#include <QFutureWatcher>
#include <QObject>
#include <QUrl>

/*  FIXME:
 *  Workaround because QML enums are like c++98 with a
 *  global namespace.
 *  https://www.kdab.com/new-qt-5-8-meta-object-support-namespaces/
 */
namespace LocalWorkshopCreationStatus {
Q_NAMESPACE
enum Value {
    Idle,
    Started,
    CopyVideoFinished,
    CopyImageFinished,
    CopyConfigFinished,
    Finished,
    ErrorFolder,
    ErrorFolderCreation,
    ErrorDiskSpace,
    ErrorCopyVideo,
    ErrorCopyImage,
    ErrorCopyConfig,
    ErrorUnknown,
};
Q_ENUM_NS(Value)
}
namespace RemoteWorkshopCreationStatus {
Q_NAMESPACE
enum Value {
    Idle,
    Started,
    Importing,
    Finished,
    ErrorUpload,
    ErrorWorkshopItemCreation,
    ErrorUnknown,
};
Q_ENUM_NS(Value)
}

class SteamWorkshop : public QObject {
    Q_OBJECT
public:
    explicit SteamWorkshop(QObject* parent = nullptr);
    SteamWorkshop(AppId_t nConsumerAppId, SteamWorkshopListModel* wlm, Settings* s);


    Q_PROPERTY(unsigned int itemProcessed READ itemProcessed WRITE setItemProcessed NOTIFY itemProcessedChanged)
    Q_PROPERTY(unsigned int bytesTotal READ bytesTotal WRITE setBytesTotal NOTIFY bytesTotalChanged)

    // Properties
    unsigned int itemProcessed() const { return m_itemProcessed; }
    unsigned int bytesTotal() const { return m_bytesTotal; }

public slots:

    int getItemUpdateProcess();
    bool contentFolderExist(QString folder);
    void searchWorkshop();
    void createWorkshopItem();
    void submitWorkshopItem(QString title, QString description, QString language, int remoteStoragePublishedFileVisibility, const QUrl projectFile, const QUrl videoFile, int publishedFileId);
    void getAPICallInfo();
    void createLocalWorkshopItem(QString title, QUrl videoPath, QUrl previewPath);
    void subscribeItem(unsigned int id);

    // Properties
    void setItemProcessed(unsigned int itemProcessed)
    {
        if (m_itemProcessed == itemProcessed)
            return;

        m_itemProcessed = itemProcessed;
        emit itemProcessedChanged(m_itemProcessed);
    }

    void setBytesTotal(unsigned int bytesTotal)
    {
        if (m_bytesTotal == bytesTotal)
            return;

        m_bytesTotal = bytesTotal;
        emit bytesTotalChanged(m_bytesTotal);
    }

signals:
    void workshopItemCreated(bool userNeedsToAcceptWorkshopLegalAgreement, int eResult, int publishedFileId);
    void workshopSearched();
    void localWorkshopCreationStatusChanged(LocalWorkshopCreationStatus::Value status);
    void remoteWorkshopCreationStatusChanged(RemoteWorkshopCreationStatus::Value status);
    void workshopSearchResult(unsigned int id, QString title, QUrl imgUrl,int subscriber);

    // Properties
    void itemProcessedChanged(unsigned int itemProcessed);
    void bytesTotalChanged(unsigned int bytesTotal);

private:
    void workshopItemCreated(CreateItemResult_t* pCallback, bool bIOFailure);
    CCallResult<SteamWorkshop, CreateItemResult_t> m_createWorkshopItemCallResult;

    void onWorkshopSearched(SteamUGCQueryCompleted_t* pCallback, bool bIOFailure);
    CCallResult<SteamWorkshop, SteamUGCQueryCompleted_t> m_steamUGCQueryResult;

    AppId_t m_AppId;
    UGCUpdateHandle_t m_UGCUpdateHandle = 0;
    UGCQueryHandle_t m_UGCSearchHandle = 0;
    SteamAPICall_t m_searchCall;
    SteamWorkshopListModel* m_workshopListModel;
    Settings* m_settings;
    unsigned int m_itemProcessed = 0;
    unsigned int m_bytesTotal = 0;
};
