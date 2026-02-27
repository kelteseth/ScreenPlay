// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#include "steamworkshopitemops.h"
#include "steamworkshop.h"

#include <QLoggingCategory>

Q_LOGGING_CATEGORY(workshopItemOps, "screenplay.workshop.itemops")

namespace ScreenPlayWorkshop {

/*!
    \class ScreenPlayWorkshop::SteamWorkshopItemOps
    \inmodule ScreenPlayWorkshop
    \brief Handles single-item operations: details, metadata/content update,
           delete, vote, subscribe/unsubscribe, install info and file listing.

    Exposed to QML via \c SteamWorkshop::itemOps.
*/

/*!
    \fn SteamWorkshopItemOps::SteamWorkshopItemOps(SteamWorkshop &facade, quint64 appID)
    \brief Constructs a SteamWorkshopItemOps that operates on behalf of \a facade
           using the Steam App ID \a appID.
*/
SteamWorkshopItemOps::SteamWorkshopItemOps(SteamWorkshop& facade, quint64 appID)
    : m_facade(facade)
    , m_appID(appID)
{
}

/*!
    \fn void SteamWorkshopItemOps::requestWorkshopItemDetails(const QVariant publishedFileID)
    \brief Fetches the abbreviated details (title, tags, description, votes, owner)
           for the item identified by \a publishedFileID. Emits
           \c requestItemDetailReturned and triggers a creator name lookup when
           the Steam callback returns.
*/
void SteamWorkshopItemOps::requestWorkshopItemDetails(const QVariant publishedFileID)
{
    if (!m_facade.checkAndSetQueryActive())
        return;

    if (!m_facade.checkOnline())
        return;

    if (!SteamUGC()) {
        qCWarning(workshopItemOps) << "SteamUGC() is null in requestWorkshopItemDetails";
        m_facade.setQueryActive(false);
        return;
    }

    auto id = publishedFileID.toULongLong();
    auto apiCall = UGCQueryBuilder::details(&id, 1).send();

    SteamAsyncCall<SteamUGCQueryCompleted_t>::create(apiCall, [this](SteamUGCQueryCompleted_t* pCallback, bool bIOFailure) {
        m_facade.setQueryActive(false);
        if (bIOFailure) {
            qCWarning(workshopItemOps) << "requestWorkshopItemDetails IO failure";
            return;
        }

        SteamUGCDetails_t details;
        for (uint32 i = 0; i < pCallback->m_unTotalMatchingResults; ++i) {
            if (SteamUGC()->GetQueryUGCResult(pCallback->m_handle, i, &details)) {
                WorkshopItemDetail detail;
                detail.title = QString::fromUtf8(details.m_rgchTitle);
                detail.tags = QString::fromUtf8(details.m_rgchTags).split(",", Qt::SkipEmptyParts);
                detail.steamIDOwner = details.m_ulSteamIDOwner;
                detail.description = QString::fromUtf8(details.m_rgchDescription);
                detail.votesUp = details.m_unVotesUp;
                detail.votesDown = details.m_unVotesDown;
                detail.url = QString::fromUtf8(details.m_rgchURL);
                detail.fileSize = QVariant::fromValue<int32>(details.m_nFileSize);
                detail.publishedFileId = QVariant::fromValue<uint64>(details.m_nPublishedFileId);
                emit requestItemDetailReturned(detail);
                m_facade.requestCreatorName(QString::number(details.m_ulSteamIDOwner));
            } else {
                qCWarning(workshopItemOps) << "GetQueryUGCResult failed!";
            }
        }
        SteamUGC()->ReleaseQueryUGCRequest(pCallback->m_handle); }, this);
}

/*!
    \fn void SteamWorkshopItemOps::requestProfileItemDetails(const QVariant publishedFileID)
    \brief Fetches the full details (including statistics, children, preview URL
           and visibility) for the item identified by \a publishedFileID.
           Emits \c requestProfileItemDetailReturned when done.
*/
void SteamWorkshopItemOps::requestProfileItemDetails(const QVariant publishedFileID)
{
    if (!m_facade.checkAndSetQueryActive())
        return;

    if (!m_facade.checkOnline())
        return;

    auto id = publishedFileID.toULongLong();
    auto apiCall = UGCQueryBuilder::details(&id, 1)
                       .withLongDescription()
                       .withKeyValueTags()
                       .withChildren()
                       .send();

    SteamAsyncCall<SteamUGCQueryCompleted_t>::create(apiCall, [this](SteamUGCQueryCompleted_t* pCallback, bool bIOFailure) {
        m_facade.setQueryActive(false);
        if (bIOFailure) {
            qCWarning(workshopItemOps) << "requestProfileItemDetails IO failure";
            return;
        }

        SteamUGCDetails_t details;
        for (uint32 i = 0; i < pCallback->m_unTotalMatchingResults; ++i) {
            if (SteamUGC()->GetQueryUGCResult(pCallback->m_handle, i, &details)) {
                std::array<char, 512> previewUrl {};
                SteamUGC()->GetQueryUGCPreviewURL(pCallback->m_handle, i, previewUrl.data(), 512);

                quint64 subscriptionCount = 0;
                quint64 favoriteCount = 0;
                quint64 followerCount = 0;
                quint64 uniqueWebsiteViews = 0;
                SteamUGC()->GetQueryUGCStatistic(pCallback->m_handle, i, EItemStatistic::k_EItemStatistic_NumSubscriptions, &subscriptionCount);
                SteamUGC()->GetQueryUGCStatistic(pCallback->m_handle, i, EItemStatistic::k_EItemStatistic_NumFavorites, &favoriteCount);
                SteamUGC()->GetQueryUGCStatistic(pCallback->m_handle, i, EItemStatistic::k_EItemStatistic_NumFollowers, &followerCount);
                SteamUGC()->GetQueryUGCStatistic(pCallback->m_handle, i, EItemStatistic::k_EItemStatistic_NumUniqueWebsiteViews, &uniqueWebsiteViews);

                WorkshopProfileItemDetail detail;
                detail.publishedFileId = QVariant::fromValue<uint64>(details.m_nPublishedFileId);
                detail.title = QString::fromUtf8(details.m_rgchTitle);
                detail.description = QString::fromUtf8(details.m_rgchDescription);
                detail.tags = QString::fromUtf8(details.m_rgchTags).split(",", Qt::SkipEmptyParts);
                detail.steamIDOwner = details.m_ulSteamIDOwner;
                detail.votesUp = details.m_unVotesUp;
                detail.votesDown = details.m_unVotesDown;
                detail.score = details.m_flScore;
                detail.url = QString::fromUtf8(details.m_rgchURL);
                detail.fileSize = QVariant::fromValue<int32>(details.m_nFileSize);
                detail.totalFileSize = QVariant::fromValue<uint64>(details.m_ulTotalFilesSize);
                detail.previewUrl = QString::fromUtf8(previewUrl.data());
                detail.timeCreated = details.m_rtimeCreated;
                detail.timeUpdated = details.m_rtimeUpdated;
                detail.visibility = static_cast<int>(details.m_eVisibility);
                detail.banned = details.m_bBanned;
                detail.acceptedForUse = details.m_bAcceptedForUse;
                detail.subscriptionCount = subscriptionCount;
                detail.favoriteCount = favoriteCount;
                detail.followerCount = followerCount;
                detail.uniqueWebsiteViews = uniqueWebsiteViews;
                detail.numChildren = details.m_unNumChildren;
                emit requestProfileItemDetailReturned(detail);
            } else {
                qCWarning(workshopItemOps) << "GetQueryUGCResult failed for profile item!";
            }
        }
        SteamUGC()->ReleaseQueryUGCRequest(pCallback->m_handle); }, this);
}

/*!
    \fn void SteamWorkshopItemOps::updateItemMetadata(const QVariant publishedFileID, const QString &title, const QString &description, const QStringList &tags, int visibility)
    \brief Submits a metadata update for \a publishedFileID. Only non-empty
           values overwrite the existing field; pass \c{visibility < 0} to leave
           visibility unchanged. Emits \c workshopItemMetadataUpdated when done.
*/
void SteamWorkshopItemOps::updateItemMetadata(const QVariant publishedFileID, const QString& title, const QString& description, const QStringList& tags, const int visibility)
{
    if (!m_facade.checkOnline())
        return;

    const auto fileId = publishedFileID.toULongLong();
    qCInfo(workshopItemOps) << "updateItemMetadata: fileId=" << fileId
            << "title=" << title
            << "tags=" << tags
            << "visibility=" << visibility;

    auto updateHandle = SteamUGC()->StartItemUpdate(m_appID, fileId);

    if (!title.isEmpty())
        SteamUGC()->SetItemTitle(updateHandle, title.toUtf8().constData());

    if (!description.isEmpty())
        SteamUGC()->SetItemDescription(updateHandle, description.toUtf8().constData());

    SteamTagArray tagArray(tags);
    if (!tagArray.isEmpty()) {
        qCInfo(workshopItemOps) << "updateItemMetadata: setting" << tagArray.count() << "tags";
        SteamUGC()->SetItemTags(updateHandle, tagArray.get());
    }

    if (visibility >= 0)
        SteamUGC()->SetItemVisibility(updateHandle, static_cast<ERemoteStoragePublishedFileVisibility>(visibility));

    auto apiCall = SteamUGC()->SubmitItemUpdate(updateHandle, nullptr);
    if (apiCall == k_uAPICallInvalid) {
        qCWarning(workshopItemOps) << "updateItemMetadata: SubmitItemUpdate returned invalid API call!";
        emit workshopItemMetadataUpdated(false, QVariant::fromValue<quint64>(fileId), 0);
        return;
    }

    SteamAsyncCall<SubmitItemUpdateResult_t>::create(apiCall, [this, fileId](SubmitItemUpdateResult_t* pCallback, bool bIOFailure) {
        if (bIOFailure) {
            qCWarning(workshopItemOps) << "updateItemMetadata IO Failure";
            emit workshopItemMetadataUpdated(false, QVariant::fromValue<quint64>(fileId), 0);
            return;
        }

        const bool success = (pCallback->m_eResult == k_EResultOK);
        if (success) {
            qCInfo(workshopItemOps) << "Successfully updated item metadata:" << pCallback->m_nPublishedFileId;
        } else {
            qCWarning(workshopItemOps) << "Failed to update item metadata:" << pCallback->m_nPublishedFileId
                       << "EResult:" << pCallback->m_eResult
                       << "NeedsWorkshopAgreement:" << pCallback->m_bUserNeedsToAcceptWorkshopLegalAgreement;
        }
        emit workshopItemMetadataUpdated(success, QVariant::fromValue<quint64>(pCallback->m_nPublishedFileId), static_cast<int>(pCallback->m_eResult)); }, this);
}

/*!
    \fn QVariantMap SteamWorkshopItemOps::getItemInstallInfo(const QVariant publishedFileID) const
    \brief Returns a map with \c{"path"} (QString), \c{"sizeOnDisk"} (quint64)
           and \c{"timestamp"} (uint32) for the installed item identified by
           \a publishedFileID, or an empty map if the item is not installed.
*/
QVariantMap SteamWorkshopItemOps::getItemInstallInfo(const QVariant publishedFileID) const
{
    QVariantMap result;
    if (!SteamUGC())
        return result;

    const auto id = publishedFileID.toULongLong();
    uint64 punSizeOnDisk = 0;
    std::array<char, 4096> pchFolder {};
    uint32 punTimeStamp = 0;

    if (!SteamUGC()->GetItemInstallInfo(id, &punSizeOnDisk, pchFolder.data(), pchFolder.size(), &punTimeStamp)) {
        return result;
    }

    result["path"] = QString::fromUtf8(pchFolder.data());
    result["sizeOnDisk"] = QVariant::fromValue<quint64>(punSizeOnDisk);
    result["timestamp"] = punTimeStamp;
    return result;
}

/*!
    \fn QVariantList SteamWorkshopItemOps::getItemFileList(const QVariant publishedFileID) const
    \brief Returns a list of maps (\c{"name"}, \c{"size"}) for every file found
           recursively under the install directory of \a publishedFileID.
           Returns an empty list if the item is not installed.
*/
QVariantList SteamWorkshopItemOps::getItemFileList(const QVariant publishedFileID) const
{
    QVariantList files;
    const auto installInfo = getItemInstallInfo(publishedFileID);
    if (installInfo.isEmpty())
        return files;

    const auto path = installInfo["path"].toString();
    QDirIterator it(path, QDir::Files | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);

    const QDir baseDir(path);
    while (it.hasNext()) {
        it.next();
        QVariantMap entry;
        entry["name"] = baseDir.relativeFilePath(it.filePath());
        entry["size"] = it.fileInfo().size();
        files.append(entry);
    }

    return files;
}

/*!
    \fn void SteamWorkshopItemOps::updateItemContent(const QVariant publishedFileID, const QString &absoluteContentPath, const QString &changeNote)
    \brief Uploads the folder at \a absoluteContentPath as the new content for
           \a publishedFileID with the optional \a changeNote. Progress can be
           polled via getContentUpdateProgress(). Emits \c workshopItemContentUpdated
           when the Steam callback returns.
*/
void SteamWorkshopItemOps::updateItemContent(const QVariant publishedFileID, const QString& absoluteContentPath, const QString& changeNote)
{
    if (!m_facade.checkOnline())
        return;

    const auto fileId = publishedFileID.toULongLong();
    m_contentUpdateHandle = SteamUGC()->StartItemUpdate(m_appID, fileId);

    SteamUGC()->SetItemContent(m_contentUpdateHandle, absoluteContentPath.toUtf8().constData());

    const auto apiCall = SteamUGC()->SubmitItemUpdate(
        m_contentUpdateHandle,
        changeNote.isEmpty() ? nullptr : changeNote.toUtf8().constData());

    SteamAsyncCall<SubmitItemUpdateResult_t>::create(apiCall, [this, fileId](SubmitItemUpdateResult_t* pCallback, bool bIOFailure) {
        m_contentUpdateHandle = k_UGCUpdateHandleInvalid;

        if (bIOFailure) {
            qCWarning(workshopItemOps) << "updateItemContent IO Failure";
            emit workshopItemContentUpdated(false, QVariant::fromValue<quint64>(fileId));
            return;
        }

        const bool success = (pCallback->m_eResult == k_EResultOK);
        if (success) {
            qCInfo(workshopItemOps) << "Successfully updated item content:" << pCallback->m_nPublishedFileId;
        } else {
            qCWarning(workshopItemOps) << "Failed to update item content:" << pCallback->m_nPublishedFileId
                       << "Result:" << pCallback->m_eResult;
        }
        emit workshopItemContentUpdated(success, QVariant::fromValue<quint64>(pCallback->m_nPublishedFileId)); }, this);
}

/*!
    \fn QVariantMap SteamWorkshopItemOps::getContentUpdateProgress() const
    \brief Returns a map with \c{"progress"} (double 0–1), \c{"status"} (int),
           \c{"bytesProcessed"} and \c{"bytesTotal"} for the in-progress content
           upload. Returns \c{progress=0} and \c{status=0} when no upload is active.
*/
QVariantMap SteamWorkshopItemOps::getContentUpdateProgress() const
{
    QVariantMap result;
    if (m_contentUpdateHandle == k_UGCUpdateHandleInvalid) {
        result["progress"] = 0.0;
        result["status"] = 0;
        return result;
    }

    uint64 bytesProcessed = 0;
    uint64 bytesTotal = 0;
    const auto status = SteamUGC()->GetItemUpdateProgress(m_contentUpdateHandle, &bytesProcessed, &bytesTotal);

    auto progress = 0.0;
    if (bytesTotal > 0) {
        progress = static_cast<double>(bytesProcessed) / static_cast<double>(bytesTotal);
    }

    result["progress"] = progress;
    result["status"] = static_cast<int>(status);
    result["bytesProcessed"] = QVariant::fromValue<quint64>(bytesProcessed);
    result["bytesTotal"] = QVariant::fromValue<quint64>(bytesTotal);
    return result;
}

/*!
    \fn void SteamWorkshopItemOps::deleteItem(const QVariant publishedFileID)
    \brief Permanently deletes the Workshop item identified by \a publishedFileID
           from Steam. Emits \c workshopItemDeleted when the Steam callback
           returns. The facade wires this signal to remove the item from the
           profile list model.
*/
void SteamWorkshopItemOps::deleteItem(const QVariant publishedFileID)
{
    if (!m_facade.checkOnline())
        return;

    const auto fileId = publishedFileID.toULongLong();
    qCInfo(workshopItemOps) << "Deleting workshop item:" << fileId;

    SteamAsyncCall<DeleteItemResult_t>::create(SteamUGC()->DeleteItem(fileId), [this, fileId](DeleteItemResult_t* pCallback, bool bIOFailure) {
            if (bIOFailure) {
                qCWarning(workshopItemOps) << "deleteItem IO Failure";
                emit workshopItemDeleted(false, QVariant::fromValue<quint64>(fileId));
                return;
            }

            const bool success = (pCallback->m_eResult == k_EResultOK);
            if (success) {
                qCInfo(workshopItemOps) << "Successfully deleted workshop item:" << fileId;
            } else {
                qCWarning(workshopItemOps) << "Failed to delete workshop item:" << fileId
                           << "Result:" << pCallback->m_eResult;
            }
            emit workshopItemDeleted(success, QVariant::fromValue<quint64>(fileId)); }, this);
}

/*!
    \fn void SteamWorkshopItemOps::vote(const QVariant publishedFileID, bool voteUp)
    \brief Submits a thumbs-up (\a voteUp \c true) or thumbs-down
           (\a voteUp \c false) vote for the item identified by \a publishedFileID.
*/
void SteamWorkshopItemOps::vote(const QVariant publishedFileID, const bool voteUp)
{
    if (!m_facade.checkOnline())
        return;

    SteamUGC()->SetUserItemVote(publishedFileID.toULongLong(), voteUp);
}

/*!
    \fn bool SteamWorkshopItemOps::isSubscribed(const QVariant publishedFileID) const
    \brief Returns \c true when the current user is subscribed to the Workshop
           item identified by \a publishedFileID.
*/
bool SteamWorkshopItemOps::isSubscribed(const QVariant publishedFileID) const
{
    if (!SteamUGC())
        return false;

    const auto state = SteamUGC()->GetItemState(publishedFileID.toULongLong());
    return (state & k_EItemStateSubscribed) != 0;
}

/*!
    \fn void SteamWorkshopItemOps::subscribeItem(const QVariant publishedFileID)
    \brief Subscribes the current user to the item identified by
           \a publishedFileID and refreshes the subscription count on
           \c SteamAccount.
*/
void SteamWorkshopItemOps::subscribeItem(const QVariant publishedFileID)
{
    if (!m_facade.checkOnline())
        return;

    SteamUGC()->SubscribeItem(publishedFileID.toULongLong());
    m_facade.steamAccount()->loadAmountSubscribedItems();
}

/*!
    \fn void SteamWorkshopItemOps::unsubscribeItem(const QVariant publishedFileID)
    \brief Unsubscribes the current user from the item identified by
           \a publishedFileID and refreshes the subscription count on
           \c SteamAccount.
*/
void SteamWorkshopItemOps::unsubscribeItem(const QVariant publishedFileID)
{
    if (!m_facade.checkOnline())
        return;

    SteamUGC()->UnsubscribeItem(publishedFileID.toULongLong());
    m_facade.steamAccount()->loadAmountSubscribedItems();
}

} // namespace ScreenPlayWorkshop

#include "moc_steamworkshopitemops.cpp"
