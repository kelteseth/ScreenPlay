// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#pragma once

#include <QByteArray>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QFuture>
#include <QFutureWatcher>
#include <QObject>
#include <QQmlEngine>
#include <QRegularExpression>
#include <QSet>
#include <QTimer>
#include <QUrl>
#include <QtConcurrent/QtConcurrent>
#include <QtQml>
#include <array>

// Steam
#include "ScreenPlayCore/steamenumsgenerated.h"
#include "steam/steam_api.h"

#include "steamaccount.h"
#include "steamasynccall.h"
#include "steamtagarray.h"
#include "steamworkshoplistmodel.h"
#include "workshopitemdetail.h"
#include "ugcquerybuilder.h"
#include "uploadlistmodel.h"

namespace ScreenPlayWorkshop {

class SteamWorkshop : public QObject {
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(bool online READ online WRITE setOnline NOTIFY onlineChanged)
    Q_PROPERTY(bool queryActive READ queryActive WRITE setQueryActive NOTIFY queryActiveChanged)
    Q_PROPERTY(quint64 itemProcessed READ itemProcessed WRITE setItemProcessed NOTIFY itemProcessedChanged)
    Q_PROPERTY(quint64 bytesTotal READ bytesTotal WRITE setBytesTotal NOTIFY bytesTotalChanged)
    Q_PROPERTY(quint64 appID READ appID)
    Q_PROPERTY(ScreenPlayWorkshop::SteamWorkshopListModel* workshopListModel READ workshopListModel WRITE setWorkshopListModel NOTIFY workshopListModelChanged)
    Q_PROPERTY(ScreenPlayWorkshop::SteamWorkshopListModel* workshopProfileListModel READ workshopProfileListModel WRITE setWorkshopProfileListModel NOTIFY workshopProfileListModelChanged)
    Q_PROPERTY(ScreenPlayWorkshop::UploadListModel* uploadListModel READ uploadListModel NOTIFY uploadListModelChanged)
    Q_PROPERTY(ScreenPlayWorkshop::SteamAccount* steamAccount READ steamAccount WRITE setSteamAccount NOTIFY steamAccountChanged)
    Q_PROPERTY(bool steamErrorRestart READ steamErrorRestart WRITE setSteamErrorRestart RESET resetSteamErrorRestart NOTIFY steamErrorRestartChanged)
    Q_PROPERTY(bool steamErrorAPIInit READ steamErrorAPIInit WRITE setSteamErrorAPIInit RESET resetSteamErrorAPIInit NOTIFY steamErrorAPIInitChanged)
    Q_PROPERTY(int userPublishedItemCount READ userPublishedItemCount NOTIFY userPublishedItemCountChanged)
    Q_PROPERTY(quint64 userTotalSubscriptions READ userTotalSubscriptions NOTIFY userTotalSubscriptionsChanged)

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
    quint64 itemProcessed() const { return m_itemProcessed; }
    quint64 appID() const { return m_appID; }
    quint64 bytesTotal() const { return m_bytesTotal; }
    UploadListModel* uploadListModel() const { return m_uploadListModel.get(); }
    SteamAccount* steamAccount() const { return m_steamAccount.get(); }
    SteamWorkshopListModel* workshopListModel() const { return m_workshopListModel.get(); }
    SteamWorkshopListModel* workshopProfileListModel() const { return m_workshopProfileListModel.get(); }
    int userPublishedItemCount() const { return m_userPublishedItemCount; }
    quint64 userTotalSubscriptions() const { return m_userTotalSubscriptions; }

    bool steamErrorRestart() const;
    void setSteamErrorRestart(bool newSteamErrorRestart);
    void resetSteamErrorRestart();

    bool steamErrorAPIInit() const;
    void setSteamErrorAPIInit(bool newSteamErrorAPIInit);
    void resetSteamErrorAPIInit();

    Q_INVOKABLE bool isSubscribed(const QVariant publishedFileID) const;
    Q_INVOKABLE QVariantMap getItemInstallInfo(const QVariant publishedFileID) const;
    Q_INVOKABLE QVariantList getItemFileList(const QVariant publishedFileID) const;

public slots:
    bool checkOnline();
    void bulkUploadToWorkshop(QStringList absoluteStoragePaths);
    void updateItemContent(const QVariant publishedFileID, const QString& absoluteContentPath, const QString& changeNote);
    QVariantMap getContentUpdateProgress() const;
    void requestUserItems(
        const ScreenPlayCore::Steam::EUserUGCList listType = ScreenPlayCore::Steam::EUserUGCList::K_EUserUGCList_Published,
        const ScreenPlayCore::Steam::EUserUGCListSortOrder sortOrder = ScreenPlayCore::Steam::EUserUGCListSortOrder::K_EUserUGCListSortOrder_LastUpdatedDesc);
    bool loadNextProfilePage();
    void requestWorkshopItemDetails(const QVariant publishedFileID);
    void requestProfileItemDetails(const QVariant publishedFileID);
    void vote(const QVariant publishedFileID, const bool voteUp);
    void subscribeItem(const QVariant publishedFileID);
    void unsubscribeItem(const QVariant publishedFileID);
    void deleteItem(const QVariant publishedFileID);
    void updateItemMetadata(const QVariant publishedFileID, const QString& title, const QString& description, const QStringList& tags, const int visibility = -1);
    bool searchWorkshop(const ScreenPlayCore::Steam::EUGCQuery enumEUGCQuery);
    bool loadNextPage();
    void searchWorkshopByText(const QString text, const ScreenPlayCore::Steam::EUGCQuery rankedBy = ScreenPlayCore::Steam::EUGCQuery::K_EUGCQuery_RankedByTrend);
    Q_INVOKABLE void searchWorkshopByUser(const QString& steamID64);
    Q_INVOKABLE void requestCreatorName(const QString& steamID64);

    bool checkAndSetQueryActive()
    {
        if (m_queryActive) {
            qWarning() << "Query already active! Abort";
            return false;
        }

        m_queryActive = true;

        return m_queryActive;
    }

    void setItemProcessed(quint64 itemProcessed)
    {
        if (m_itemProcessed == itemProcessed)
            return;

        m_itemProcessed = itemProcessed;
        emit itemProcessedChanged(m_itemProcessed);
    }

    void setBytesTotal(quint64 bytesTotal)
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
    void creatorNameReady(const QString& name, const QString& steamID64);
    void workshopItemCreatedSuccessful(bool userNeedsToAcceptWorkshopLegalAgreement, int eResult, QVariant publishedFileId);

    void workshopItemInstalled(int appID, QVariant publishedFileID);
    void workshopItemDeleted(bool success, QVariant publishedFileID);

    void itemProcessedChanged(QVariant itemProcessed);
    void bytesTotalChanged(QVariant bytesTotal);
    void onlineChanged(bool online);

    void uploadListModelChanged(UploadListModel* uploadListModel);
    void workshopListModelChanged(SteamWorkshopListModel* workshopListModel);
    void steamAccountChanged(SteamAccount* steamAccount);

    void userPublishedItemCountChanged(int count);
    void userTotalSubscriptionsChanged(quint64 total);

    void requestItemDetailReturned(const WorkshopItemDetail& detail);
    void requestProfileItemDetailReturned(const WorkshopProfileItemDetail& detail);

    void workshopItemMetadataUpdated(bool success, QVariant publishedFileID, int eResult);
    void workshopItemContentUpdated(bool success, QVariant publishedFileID);
    void workshopItemContentUpdateProgress(float progress, int status);

    void workshopProfileListModelChanged(SteamWorkshopListModel*);

    void steamErrorRestartChanged();

    void steamErrorAPIInitChanged();

    void queryActiveChanged(bool queryActive);

private:
    void onWorkshopSearched(SteamUGCQueryCompleted_t* pCallback, bool bIOFailure);
    bool queryWorkshopItemFromHandle(SteamWorkshopListModel* listModel, SteamUGCQueryCompleted_t* pCallback);

    STEAM_CALLBACK(SteamWorkshop, onWorkshopItemInstalled, ItemInstalled_t);
    STEAM_CALLBACK(SteamWorkshop, onPersonaStateChange, PersonaStateChange_t);

    // User items (profile) — callback kept as method because it's shared by
    // requestUserItems() and loadNextProfilePage().
    void onRequestUserItemsReturned(SteamUGCQueryCompleted_t* pCallback, bool bIOFailure);
    void updateUserProfileStatistics();
    ScreenPlayCore::Steam::EUserUGCList m_currentProfileListType = ScreenPlayCore::Steam::EUserUGCList::K_EUserUGCList_Published;
    ScreenPlayCore::Steam::EUserUGCListSortOrder m_currentProfileSortOrder = ScreenPlayCore::Steam::EUserUGCListSortOrder::K_EUserUGCListSortOrder_LastUpdatedDesc;

    // Content update progress tracking (read by getContentUpdateProgress())
    UGCUpdateHandle_t m_contentUpdateHandle = k_UGCUpdateHandleInvalid;

    UGCQueryHandle_t m_searchHandle = 0;
    QSet<quint64> m_pendingCreatorRequests;
    AccountID_t m_currentUserAccountID = 0;
    ScreenPlayCore::Steam::EUGCQuery m_currentQueryType = ScreenPlayCore::Steam::EUGCQuery::K_EUGCQuery_RankedByTrend;
    QString m_currentSearchText;
    QStringList m_currentSearchTags;

    struct ParsedSearch {
        QString text;
        QStringList tags;
    };
    ParsedSearch parseSearchInput(const QString& input) const;

    QTimer m_pollTimer;

    bool m_steamErrorRestart = false;
    bool m_steamErrorAPIInit = false;
    quint64 m_itemProcessed = 0;
    quint64 m_bytesTotal = 0;
    const quint64 m_appID = 672870;
    bool m_online = false;
    bool m_queryActive = false;
    int m_userPublishedItemCount = 0;
    quint64 m_userTotalSubscriptions = 0;

    std::unique_ptr<SteamWorkshopListModel> m_workshopListModel;
    std::unique_ptr<SteamWorkshopListModel> m_workshopProfileListModel;
    std::unique_ptr<UploadListModel> m_uploadListModel;
    std::unique_ptr<SteamAccount> m_steamAccount;
};
}
