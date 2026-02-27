// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#pragma once

#include <QDebug>
#include <QObject>
#include <QQmlEngine>
#include <memory>

#include "ScreenPlayCore/steamenumsgenerated.h"
#include "steam/steam_api.h"

#include "steamasynccall.h"
#include "steamworkshoplistmodel.h"
#include "ugcquerybuilder.h"

namespace ScreenPlayWorkshop {

class SteamWorkshop;
class SteamWorkshopSearch;

class SteamWorkshopProfile : public QObject {
    Q_OBJECT
    QML_UNCREATABLE("Created by SteamWorkshop")

    Q_PROPERTY(ScreenPlayWorkshop::SteamWorkshopListModel* workshopProfileListModel READ workshopProfileListModel CONSTANT)
    Q_PROPERTY(int userPublishedItemCount READ userPublishedItemCount NOTIFY userPublishedItemCountChanged)
    Q_PROPERTY(quint64 userTotalSubscriptions READ userTotalSubscriptions NOTIFY userTotalSubscriptionsChanged)

public:
    explicit SteamWorkshopProfile(SteamWorkshop& facade, SteamWorkshopSearch& search, quint64 appID);

    void init();

    Q_INVOKABLE void requestUserItems(
        ScreenPlayCore::Steam::EUserUGCList listType = ScreenPlayCore::Steam::EUserUGCList::K_EUserUGCList_Published,
        ScreenPlayCore::Steam::EUserUGCListSortOrder sortOrder = ScreenPlayCore::Steam::EUserUGCListSortOrder::K_EUserUGCListSortOrder_LastUpdatedDesc);
    Q_INVOKABLE bool loadNextProfilePage();

    SteamWorkshopListModel* workshopProfileListModel() const { return m_workshopProfileListModel.get(); }
    int userPublishedItemCount() const { return m_userPublishedItemCount; }
    quint64 userTotalSubscriptions() const { return m_userTotalSubscriptions; }

signals:
    void userPublishedItemCountChanged(int count);
    void userTotalSubscriptionsChanged(quint64 total);

private:
    void onRequestUserItemsReturned(SteamUGCQueryCompleted_t* pCallback, bool bIOFailure);
    void updateUserProfileStatistics();

    SteamWorkshop& m_facade;
    SteamWorkshopSearch& m_search;
    quint64 m_appID;

    ScreenPlayCore::Steam::EUserUGCList m_currentProfileListType = ScreenPlayCore::Steam::EUserUGCList::K_EUserUGCList_Published;
    ScreenPlayCore::Steam::EUserUGCListSortOrder m_currentProfileSortOrder = ScreenPlayCore::Steam::EUserUGCListSortOrder::K_EUserUGCListSortOrder_LastUpdatedDesc;

    int m_userPublishedItemCount = 0;
    quint64 m_userTotalSubscriptions = 0;

    std::unique_ptr<SteamWorkshopListModel> m_workshopProfileListModel;
};

} // namespace ScreenPlayWorkshop
