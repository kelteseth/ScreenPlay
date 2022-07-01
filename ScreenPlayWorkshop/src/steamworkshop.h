#pragma once

#include <QByteArray>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFuture>
#include <QFutureWatcher>
#include <QObject>
#include <QQmlEngine>
#include <QTimer>
#include <QUrl>
#include <QtConcurrent/QtConcurrent>
#include <QtQml>

// Steam
#include "steam/steam_api.h"
#include "steam/steam_qt_enums_generated.h"

#include "steamaccount.h"
#include "steamworkshoplistmodel.h"
#include "uploadlistmodel.h"

namespace ScreenPlayWorkshop {

struct SteamItemUpdate {
    SteamItemUpdate(QString absoluteStoragePath)
    {
        this->absoluteStoragePath = absoluteStoragePath;
    }
    QString absoluteStoragePath;
    UGCUpdateHandle_t steamUGCUpdateHandle = 0;
    unsigned long long itemProcessed = 0;
    unsigned long long bytesTotal = 0;
};

class SteamWorkshop : public QObject {
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(bool online READ online WRITE setOnline NOTIFY onlineChanged)
    Q_PROPERTY(bool queryActive READ queryActive WRITE setQueryActive NOTIFY queryActiveChanged)
    Q_PROPERTY(unsigned long long itemProcessed READ itemProcessed WRITE setItemProcessed NOTIFY itemProcessedChanged)
    Q_PROPERTY(unsigned long long bytesTotal READ bytesTotal WRITE setBytesTotal NOTIFY bytesTotalChanged)
    Q_PROPERTY(unsigned int appID READ appID)
    Q_PROPERTY(SteamWorkshopListModel* workshopListModel READ workshopListModel WRITE setWorkshopListModel NOTIFY workshopListModelChanged)
    Q_PROPERTY(SteamWorkshopListModel* workshopProfileListModel READ workshopProfileListModel WRITE setWorkshopProfileListModel NOTIFY workshopProfileListModelChanged)
    Q_PROPERTY(UploadListModel* uploadListModel READ uploadListModel NOTIFY uploadListModelChanged)
    Q_PROPERTY(SteamAccount* steamAccount READ steamAccount WRITE setSteamAccount NOTIFY steamAccountChanged)
    Q_PROPERTY(bool steamErrorRestart READ steamErrorRestart WRITE setSteamErrorRestart RESET resetSteamErrorRestart NOTIFY steamErrorRestartChanged)
    Q_PROPERTY(bool steamErrorAPIInit READ steamErrorAPIInit WRITE setSteamErrorAPIInit RESET resetSteamErrorAPIInit NOTIFY steamErrorAPIInitChanged)

public:
    ~SteamWorkshop()
    {
        qDebug() << "Steam workshop destructor";
        m_pollTimer.stop();
        SteamAPI_Shutdown();
    }
    bool init();

    bool online() const { return m_online; }
    bool queryActive() const { return m_queryActive; }
    unsigned long long itemProcessed() const { return m_itemProcessed; }
    unsigned int appID() const { return m_appID; }
    unsigned long long bytesTotal() const { return m_bytesTotal; }
    UploadListModel* uploadListModel() const { return m_uploadListModel.get(); }
    SteamAccount* steamAccount() const { return m_steamAccount.get(); }
    SteamWorkshopListModel* workshopListModel() const { return m_workshopListModel.get(); }
    SteamWorkshopListModel* workshopProfileListModel() const { return m_workshopProfileListModel.get(); }

    bool steamErrorRestart() const;
    void setSteamErrorRestart(bool newSteamErrorRestart);
    void resetSteamErrorRestart();

    bool steamErrorAPIInit() const;
    void setSteamErrorAPIInit(bool newSteamErrorAPIInit);
    void resetSteamErrorAPIInit();

public slots:
    bool checkOnline();
    void bulkUploadToWorkshop(QStringList absoluteStoragePaths);
    void requestUserItems();
    void requestWorkshopItemDetails(const QVariant publishedFileID);
    void vote(const QVariant publishedFileID, const bool voteUp);
    void subscribeItem(const QVariant publishedFileID);
    bool searchWorkshop(const ScreenPlayWorkshopSteamEnums::EUGCQuery enumEUGCQuery);
    void searchWorkshopByText(const QString text,
        const ScreenPlayWorkshopSteamEnums::EUGCQuery rankedBy = ScreenPlayWorkshopSteamEnums::EUGCQuery::K_EUGCQuery_RankedByTrend);

    bool checkAndSetQueryActive(){
        if (m_queryActive) {
            qWarning() << "Query already active! Abort";
            return false;
        }

        m_queryActive = true;

        return m_queryActive;
    }

    void setItemProcessed(unsigned long long itemProcessed)
    {
        if (m_itemProcessed == itemProcessed)
            return;

        m_itemProcessed = itemProcessed;
        emit itemProcessedChanged(m_itemProcessed);
    }

    void setBytesTotal(unsigned long long bytesTotal)
    {
        if (m_bytesTotal == bytesTotal)
            return;

        m_bytesTotal = bytesTotal;
        emit bytesTotalChanged(m_bytesTotal);
    }

    void setOnline(bool online)
    {
        if (m_online == online)
            return;

        m_online = online;
        emit onlineChanged(m_online);
    }

    void setUploadListModel(UploadListModel* uploadListModel)
    {
        if (m_uploadListModel.get() == uploadListModel)
            return;

        m_uploadListModel.reset(uploadListModel);
        emit uploadListModelChanged(m_uploadListModel.get());
    }

    void setWorkshopListModel(SteamWorkshopListModel* workshopListModel)
    {
        if (m_workshopListModel.get() == workshopListModel)
            return;

        m_workshopListModel.reset(workshopListModel);
        emit workshopListModelChanged(m_workshopListModel.get());
    }

    void setSteamAccount(SteamAccount* steamAccount)
    {
        if (m_steamAccount.get() == steamAccount)
            return;

        m_steamAccount.reset(steamAccount);
        emit steamAccountChanged(m_steamAccount.get());
    }

    void setWorkshopProfileListModel(SteamWorkshopListModel* workshopProfileListModel)
    {
        if (m_workshopProfileListModel.get() == workshopProfileListModel)
            return;
        m_workshopProfileListModel.reset(workshopProfileListModel);
        emit workshopProfileListModelChanged(m_workshopProfileListModel.get());
    }

    void setQueryActive(bool queryActive)
    {
        if (m_queryActive == queryActive)
            return;
        m_queryActive = queryActive;
        emit queryActiveChanged(m_queryActive);
    }

signals:
    void workshopSearchCompleted(const int itemCount);
    void workshopBannerCompleted();
    void workshopItemCreatedSuccessful(
        bool userNeedsToAcceptWorkshopLegalAgreement,
        int eResult,
        QVariant publishedFileId);

    void workshopItemInstalled(int appID, QVariant publishedFileID);

    void itemProcessedChanged(QVariant itemProcessed);
    void bytesTotalChanged(QVariant bytesTotal);
    void onlineChanged(bool online);

    void uploadListModelChanged(UploadListModel* uploadListModel);
    void workshopListModelChanged(SteamWorkshopListModel* workshopListModel);
    void steamAccountChanged(SteamAccount* steamAccount);

    void requestItemDetailReturned(
        const QString& title,
        const QStringList& tags,
        const qulonglong steamIDOwner,
        const QString& description,
        const quint64 votesUp,
        const quint64 votesDown,
        const QString& url,
        const QVariant fileSize,
        const QVariant publishedFileId);

    void workshopProfileListModelChanged(SteamWorkshopListModel*);

    void steamErrorRestartChanged();

    void steamErrorAPIInitChanged();

    void queryActiveChanged(bool queryActive);

private:
    void onWorkshopSearched(SteamUGCQueryCompleted_t* pCallback, bool bIOFailure);
    bool queryWorkshopItemFromHandle(SteamWorkshopListModel* listModel, SteamUGCQueryCompleted_t* pCallback);

    STEAM_CALLBACK(SteamWorkshop, onWorkshopItemInstalled, ItemInstalled_t);

    CCallResult<SteamWorkshop, SteamUGCQueryCompleted_t> m_steamUGCQuerySearchWorkshopResult;

    // List user items
    void onRequestUserItemsReturned(SteamUGCQueryCompleted_t* pCallback, bool bIOFailure);
    CCallResult<SteamWorkshop, SteamUGCQueryCompleted_t> m_steamUGCListUserItems;
    UGCQueryHandle_t m_UGCListUserItemsHandle = 0;
    SteamAPICall_t m_UGCListUserItemsCall = 0;

    // General Item detail
    void onRequestItemDetailReturned(SteamUGCQueryCompleted_t* pCallback, bool bIOFailure);
    CCallResult<SteamWorkshop, SteamUGCQueryCompleted_t> m_steamUGCItemDetails;
    UGCQueryHandle_t m_UGCRegquestItemDetailHandle = 0;
    SteamAPICall_t m_UGCRegquestItemDetailCall = 0;

    UGCQueryHandle_t m_searchHandle = 0;

    QTimer m_pollTimer;
    QQueue<SteamItemUpdate> m_bulkUploadqueue;

    bool m_steamErrorRestart = false;
    bool m_steamErrorAPIInit = false;
    unsigned long long m_itemProcessed = 0;
    unsigned long long m_bytesTotal = 0;
    const unsigned int m_appID = 672870;
    bool m_online = false;
    bool m_queryActive = false;

    std::unique_ptr<SteamWorkshopListModel> m_workshopListModel;
    std::unique_ptr<SteamWorkshopListModel> m_workshopProfileListModel;
    std::unique_ptr<UploadListModel> m_uploadListModel;
    std::unique_ptr<SteamAccount> m_steamAccount;
};
}
