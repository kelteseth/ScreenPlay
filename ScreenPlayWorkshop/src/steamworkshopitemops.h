// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#pragma once

#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QObject>
#include <QQmlEngine>
#include <QVariant>
#include <QVariantList>
#include <QVariantMap>
#include <array>

#include "ScreenPlayCore/steamenumsgenerated.h"
#include "steam/steam_api.h"

#include "steamasynccall.h"
#include "steamtagarray.h"
#include "ugcquerybuilder.h"
#include "workshopitemdetail.h"

namespace ScreenPlayWorkshop {

class SteamWorkshop;

class SteamWorkshopItemOps : public QObject {
    Q_OBJECT
    QML_UNCREATABLE("Created by SteamWorkshop")

public:
    explicit SteamWorkshopItemOps(SteamWorkshop& facade, quint64 appID);

    Q_INVOKABLE void requestWorkshopItemDetails(const QVariant publishedFileID);
    Q_INVOKABLE void requestProfileItemDetails(const QVariant publishedFileID);

    Q_INVOKABLE void updateItemMetadata(const QVariant publishedFileID, const QString& title,
        const QString& description, const QStringList& tags, int visibility = -1);
    Q_INVOKABLE void updateItemContent(const QVariant publishedFileID,
        const QString& absoluteContentPath, const QString& changeNote);
    Q_INVOKABLE QVariantMap getContentUpdateProgress() const;

    Q_INVOKABLE void deleteItem(const QVariant publishedFileID);
    Q_INVOKABLE void vote(const QVariant publishedFileID, bool voteUp);
    Q_INVOKABLE bool isSubscribed(const QVariant publishedFileID) const;
    Q_INVOKABLE void subscribeItem(const QVariant publishedFileID);
    Q_INVOKABLE void unsubscribeItem(const QVariant publishedFileID);

    Q_INVOKABLE QVariantMap getItemInstallInfo(const QVariant publishedFileID) const;
    Q_INVOKABLE QVariantList getItemFileList(const QVariant publishedFileID) const;

signals:
    void requestItemDetailReturned(const WorkshopItemDetail& detail);
    void requestProfileItemDetailReturned(const WorkshopProfileItemDetail& detail);
    void workshopItemMetadataUpdated(bool success, QVariant publishedFileID, int eResult);
    void workshopItemContentUpdated(bool success, QVariant publishedFileID);
    void workshopItemContentUpdateProgress(float progress, int status);
    void workshopItemDeleted(bool success, QVariant publishedFileID);

private:
    SteamWorkshop& m_facade;
    quint64 m_appID;
    UGCUpdateHandle_t m_contentUpdateHandle = k_UGCUpdateHandleInvalid;
};

} // namespace ScreenPlayWorkshop
