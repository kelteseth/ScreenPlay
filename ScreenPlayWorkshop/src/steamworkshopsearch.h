// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#pragma once

#include <QDebug>
#include <QObject>
#include <QQmlEngine>
#include <QRegularExpression>
#include <QString>
#include <QStringList>
#include <array>
#include <memory>

#include "ScreenPlayCore/steamenumsgenerated.h"
#include "steam/steam_api.h"

#include "steamasynccall.h"
#include "steamworkshoplistmodel.h"
#include "ugcquerybuilder.h"
#include "workshopitem.h"

namespace ScreenPlayWorkshop {

class SteamWorkshop;

class SteamWorkshopSearch : public QObject {
    Q_OBJECT
    QML_UNCREATABLE("Created by SteamWorkshop")

    Q_PROPERTY(ScreenPlayWorkshop::SteamWorkshopListModel* workshopListModel READ workshopListModel CONSTANT)

public:
    explicit SteamWorkshopSearch(SteamWorkshop& facade, quint64 appID);

    void init();

    Q_INVOKABLE bool searchWorkshop(ScreenPlayCore::Steam::EUGCQuery enumEUGCQuery);
    Q_INVOKABLE bool loadNextPage();
    Q_INVOKABLE void searchWorkshopByText(const QString& text,
        ScreenPlayCore::Steam::EUGCQuery rankedBy = ScreenPlayCore::Steam::EUGCQuery::K_EUGCQuery_RankedByTrend);
    Q_INVOKABLE void searchWorkshopByUser(const QString& steamID64);

    SteamWorkshopListModel* workshopListModel() const { return m_workshopListModel.get(); }

    bool queryWorkshopItemFromHandle(SteamWorkshopListModel* listModel, SteamUGCQueryCompleted_t* pCallback);

signals:
    void workshopSearchCompleted(int itemCount);
    void workshopBannerCompleted();

private:
    struct ParsedSearch {
        QString text;
        QStringList tags;
    };

    void onWorkshopSearched(SteamUGCQueryCompleted_t* pCallback, bool bIOFailure);
    ParsedSearch parseSearchInput(const QString& input) const;

    SteamWorkshop& m_facade;
    quint64 m_appID;

    UGCQueryHandle_t m_searchHandle = 0;
    AccountID_t m_currentUserAccountID = 0;
    ScreenPlayCore::Steam::EUGCQuery m_currentQueryType = ScreenPlayCore::Steam::EUGCQuery::K_EUGCQuery_RankedByTrend;
    QString m_currentSearchText;
    QStringList m_currentSearchTags;

    std::unique_ptr<SteamWorkshopListModel> m_workshopListModel;
};

} // namespace ScreenPlayWorkshop
