// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#pragma once

#include <QByteArray>
#include <QDebug>
#include <QObject>
#include <QQmlEngine>
#include <QSet>
#include <QTimer>
#include <QUrl>
#include <QtQml>
#include <memory>

#include "ScreenPlayCore/steamenumsgenerated.h"
#include "steam/steam_api.h"

#include "steamaccount.h"
#include "steamworkshopitemops.h"
#include "steamworkshopprofile.h"
#include "steamworkshopsearch.h"
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
    Q_PROPERTY(ScreenPlayWorkshop::UploadListModel* uploadListModel READ uploadListModel NOTIFY uploadListModelChanged)
    Q_PROPERTY(ScreenPlayWorkshop::SteamAccount* steamAccount READ steamAccount NOTIFY steamAccountChanged)
    Q_PROPERTY(bool steamErrorRestart READ steamErrorRestart WRITE setSteamErrorRestart RESET resetSteamErrorRestart NOTIFY steamErrorRestartChanged)
    Q_PROPERTY(bool steamErrorAPIInit READ steamErrorAPIInit WRITE setSteamErrorAPIInit RESET resetSteamErrorAPIInit NOTIFY steamErrorAPIInitChanged)

    Q_PROPERTY(ScreenPlayWorkshop::SteamWorkshopSearch* search READ search CONSTANT)
    Q_PROPERTY(ScreenPlayWorkshop::SteamWorkshopProfile* profile READ profile CONSTANT)
    Q_PROPERTY(ScreenPlayWorkshop::SteamWorkshopItemOps* itemOps READ itemOps CONSTANT)

public:
    ~SteamWorkshop()
    {
        qDebug() << "Steam workshop destructor";
        m_pollTimer.stop();
        m_itemOps.reset();
        m_profile.reset();
        m_search.reset();
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

    SteamWorkshopSearch* search() const { return m_search.get(); }
    SteamWorkshopProfile* profile() const { return m_profile.get(); }
    SteamWorkshopItemOps* itemOps() const { return m_itemOps.get(); }

    bool steamErrorRestart() const;
    void setSteamErrorRestart(bool newSteamErrorRestart);
    void resetSteamErrorRestart();

    bool steamErrorAPIInit() const;
    void setSteamErrorAPIInit(bool newSteamErrorAPIInit);
    void resetSteamErrorAPIInit();

    bool checkOnline();

    /*!
        \brief RAII guard that acquires the query-active flag on construction
               and resets it on destruction unless dismiss() is called.
               Use dismiss() once the async Steam call is in flight
               (the callback takes over responsibility for resetting the flag).
    */
    class QueryGuard {
    public:
        explicit QueryGuard(SteamWorkshop& workshop)
            : m_workshop(workshop)
            , m_acquired(workshop.checkAndSetQueryActive())
        {
        }
        ~QueryGuard()
        {
            if (m_acquired)
                m_workshop.setQueryActive(false);
        }
        explicit operator bool() const { return m_acquired; }
        void dismiss() { m_acquired = false; }
        QueryGuard(const QueryGuard&) = delete;
        QueryGuard& operator=(const QueryGuard&) = delete;

    private:
        SteamWorkshop& m_workshop;
        bool m_acquired;
    };

public slots:
    void bulkUploadToWorkshop(QStringList absoluteStoragePaths);
    void requestCreatorName(const QString& steamID64);

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

    void setQueryActive(bool queryActive)
    {
        if (m_queryActive == queryActive)
            return;
        m_queryActive = queryActive;
        emit queryActiveChanged(m_queryActive);
    }

    void setSteamAccount(SteamAccount* steamAccount)
    {
        if (m_steamAccount.get() == steamAccount)
            return;
        m_steamAccount.reset(steamAccount);
        emit steamAccountChanged(m_steamAccount.get());
    }

signals:
    void creatorNameReady(const QString& name, const QString& steamID64);
    void workshopItemCreatedSuccessful(bool userNeedsToAcceptWorkshopLegalAgreement, int eResult, QVariant publishedFileId);
    void workshopItemInstalled(int appID, QVariant publishedFileID);

    void itemProcessedChanged(QVariant itemProcessed);
    void bytesTotalChanged(QVariant bytesTotal);
    void onlineChanged(bool online);
    void queryActiveChanged(bool queryActive);

    void uploadListModelChanged(UploadListModel* uploadListModel);
    void steamAccountChanged(SteamAccount* steamAccount);

    void steamErrorRestartChanged();
    void steamErrorAPIInitChanged();

private:
    bool checkAndSetQueryActive();

    STEAM_CALLBACK(SteamWorkshop, onWorkshopItemInstalled, ItemInstalled_t);
    STEAM_CALLBACK(SteamWorkshop, onPersonaStateChange, PersonaStateChange_t);

    QTimer m_pollTimer;
    QSet<quint64> m_pendingCreatorRequests;

    bool m_steamErrorRestart = false;
    bool m_steamErrorAPIInit = false;
    quint64 m_itemProcessed = 0;
    quint64 m_bytesTotal = 0;
    const quint64 m_appID = 672870;
    bool m_online = false;
    bool m_queryActive = false;

    std::unique_ptr<SteamAccount> m_steamAccount;
    std::unique_ptr<UploadListModel> m_uploadListModel;

    std::unique_ptr<SteamWorkshopSearch> m_search;
    std::unique_ptr<SteamWorkshopProfile> m_profile;
    std::unique_ptr<SteamWorkshopItemOps> m_itemOps;
};

} // namespace ScreenPlayWorkshop
