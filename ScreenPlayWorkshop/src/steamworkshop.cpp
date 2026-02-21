// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#include "steamworkshop.h"

namespace ScreenPlayWorkshop {
/*!
    \class Steam Workshop
    \brief A wrapper class between the C Steam API and ScreenPlay

*/

bool SteamWorkshop::init()
{
    // https://partner.steamgames.com/doc/sdk/api#SteamAPI_Init
    // A return of false indicates one of the following conditions:
    // - The Steam client isn't running. A running Steam client is required to provide implementations
    //   of the various Steamworks interfaces.
    // - The Steam client couldn't determine the App ID of game. If you're running your application from the executable or debugger directly
    //   then you must have a steam_appid.txt in your game directory next to the executable, with your app ID in it and nothing else.
    //   Steam will look for this file in the current working directory. If you are running your executable from a different directory
    //   you may need to relocate the steam_appid.txt file.
    // - Your application is not running under the same OS user context as the Steam client, such as a different user or administration access level.
    // - Ensure that you own a license for the App ID on the currently active Steam account. Your game must show up in your Steam library.
    // - Your App ID is not completely set up, i.e. in Release State: Unavailable, or it's missing default packages.
    // If you're running into initialization issues then see the Debugging the Steamworks API documentation to learn about the various methods of debugging the Steamworks API.
    // IF THE FAMILY SHARING IS ENABLED THIS WILL FAIL ! #13
    if (!SteamAPI_Init()) {
        qWarning() << "SteamAPI_Init failed";
        m_steamErrorAPIInit = true;
        return false;
    }

    // https://partner.steamgames.com/doc/sdk/api#SteamAPI_RestartAppIfNecessary
    // checks if your executable was launched through Steam and relaunches it through Steam if it wasn't.
    // This is optional but highly recommended as the Steam context associated with your application
    // (including your App ID) will not be set up if the user launches the executable directly.
    // If this occurs then SteamAPI_Init will fail and you will be unable to use the Steamworks API.
    // If you choose to use this then it should be the first Steamworks function call you make,
    // right before SteamAPI_Init.
    // If this returns true then it starts the Steam client if required and launches your game again
    // through it, and you should quit your process as soon as possible. This effectively runs
    // steam://run/<AppID> so it may not relaunch the exact executable that called this function
    // (for example, if you were running from your debugger). It will always relaunch from the
    // version installed in your Steam library folder.
    // Otherwise, if it returns false, then your game was launched by the Steam client and no action
    // needs to be taken. One exception is if a steam_appid.txt file is present then this will return
    // false regardless. This allows you to develop and test without launching your game through the
    // Steam client. Make sure to remove the steam_appid.txt file when uploading the game to your Steam depot!
    if (SteamAPI_RestartAppIfNecessary(m_appID)) {
        qWarning() << "SteamAPI_RestartAppIfNecessary failed";
        m_steamErrorRestart = true;
    }

    m_steamAccount = std::make_unique<SteamAccount>();
    m_workshopListModel = std::make_unique<SteamWorkshopListModel>(m_appID);
    m_workshopProfileListModel = std::make_unique<SteamWorkshopListModel>(m_appID);
    m_uploadListModel = std::make_unique<UploadListModel>();
    QObject::connect(&m_pollTimer, &QTimer::timeout, this, []() { SteamAPI_RunCallbacks(); });
    m_pollTimer.start(100);

    setOnline(true);

    return true;
}

bool SteamWorkshop::checkOnline()
{
    if (!m_online || m_steamErrorAPIInit) {
        qWarning() << "Trying to call steam api while offline or not initialized";
        qWarning() << "steamErrorAPIInit: " << m_steamErrorAPIInit;
        qWarning() << "steamErrorRestart: " << m_steamErrorRestart;
        return false;
    }
    return true;
}

void SteamWorkshop::bulkUploadToWorkshop(QStringList absoluteStoragePaths)
{
    // Clear any leftover items from previous uploads
    uploadListModel()->clearWhenFinished();

    qInfo() << "bulkUploadToWorkshop called with" << absoluteStoragePaths.size() << "paths:" << absoluteStoragePaths;

    for (const QString& path : absoluteStoragePaths) {
        qInfo() << "Append " << path;
        uploadListModel()->append("", path, m_appID);
    }

    qInfo() << "Model now has" << uploadListModel()->rowCount() << "items";
}

void SteamWorkshop::onWorkshopItemInstalled(ItemInstalled_t* itemInstalled)
{
    // GetItemInstallInfo(itemInstalled->m_nPublishedFileId, uint64 *punSizeOnDisk, char *pchFolder, uint32 cchFolderSize, uint32 *punTimeStamp );
    emit workshopItemInstalled(itemInstalled->m_unAppID, itemInstalled->m_nPublishedFileId);
}

void SteamWorkshop::onPersonaStateChange(PersonaStateChange_t* pCallback)
{
    if (!(pCallback->m_nChangeFlags & k_EPersonaChangeName))
        return;
    if (!m_pendingCreatorRequests.contains(pCallback->m_ulSteamID))
        return;

    m_pendingCreatorRequests.remove(pCallback->m_ulSteamID);
    const CSteamID steamID(pCallback->m_ulSteamID);
    const auto name = QString(SteamFriends()->GetFriendPersonaName(steamID));
    const auto steamID64 = QString::number(pCallback->m_ulSteamID);
    emit creatorNameReady(name, steamID64);
}

/*! \brief Requests the Steam persona name for the given steamID64. Emits creatorNameReady when available. */
void SteamWorkshop::requestCreatorName(const QString& steamID64)
{
    const CSteamID creatorID(steamID64.toULongLong());
    if (!SteamFriends()->RequestUserInformation(creatorID, true)) {
        // Already cached
        const auto name = QString(SteamFriends()->GetFriendPersonaName(creatorID));
        emit creatorNameReady(name, steamID64);
    } else {
        // Will arrive via onPersonaStateChange
        m_pendingCreatorRequests.insert(creatorID.ConvertToUint64());
    }
}

void SteamWorkshop::requestWorkshopItemDetails(const QVariant publishedFileID)
{
    if (!checkAndSetQueryActive())
        return;

    if (!checkOnline())
        return;

    if (!SteamUGC()) {
        qWarning() << "SteamUGC() is null in requestWorkshopItemDetails";
        m_queryActive = false;
        return;
    }

    auto id = publishedFileID.toULongLong();
    auto uGCRegquestItemDetailHandle = SteamUGC()->CreateQueryUGCDetailsRequest(&id, 1);
    auto uGCRegquestItemDetailCall = SteamUGC()->SendQueryUGCRequest(uGCRegquestItemDetailHandle);
    m_steamUGCItemDetails.Set(uGCRegquestItemDetailCall, this, &SteamWorkshop::onRequestItemDetailReturned);
}

void SteamWorkshop::onRequestItemDetailReturned(SteamUGCQueryCompleted_t* pCallback, bool bIOFailure)
{
    m_queryActive = false;
    if (bIOFailure) {
        qWarning() << "onRequestItemDetailReturned bIOFailure" << bIOFailure;
        return;
    }

    SteamUGCDetails_t details;
    for (uint32 i = 0; i < pCallback->m_unTotalMatchingResults; ++i) {
        if (SteamUGC()->GetQueryUGCResult(pCallback->m_handle, i, &details)) {

            emit requestItemDetailReturned(
                QString::fromUtf8(details.m_rgchTitle),
                QString::fromUtf8(details.m_rgchTags).split(",", Qt::SkipEmptyParts),
                details.m_ulSteamIDOwner,
                QString::fromUtf8(details.m_rgchDescription),
                details.m_unVotesUp,
                details.m_unVotesDown,
                QString::fromUtf8(details.m_rgchURL),
                QVariant::fromValue<int32>(details.m_nFileSize),
                QVariant::fromValue<uint64>(details.m_nPublishedFileId));
            // Call after emit so QML has already stored creatorSteamID when creatorNameReady fires
            requestCreatorName(QString::number(details.m_ulSteamIDOwner));
        } else {
            qWarning() << "GetQueryUGCResult failed!";
        }
    }
    SteamUGC()->ReleaseQueryUGCRequest(pCallback->m_handle);
}

void SteamWorkshop::requestProfileItemDetails(const QVariant publishedFileID)
{
    if (!checkAndSetQueryActive())
        return;

    if (!checkOnline())
        return;

    auto id = publishedFileID.toULongLong();
    auto handle = SteamUGC()->CreateQueryUGCDetailsRequest(&id, 1);
    SteamUGC()->SetReturnLongDescription(handle, true);
    SteamUGC()->SetReturnKeyValueTags(handle, true);
    SteamUGC()->SetReturnChildren(handle, true);
    auto apiCall = SteamUGC()->SendQueryUGCRequest(handle);
    m_steamUGCProfileItemDetails.Set(apiCall, this, &SteamWorkshop::onRequestProfileItemDetailReturned);
}

void SteamWorkshop::onRequestProfileItemDetailReturned(SteamUGCQueryCompleted_t* pCallback, bool bIOFailure)
{
    m_queryActive = false;
    if (bIOFailure) {
        qWarning() << "onRequestProfileItemDetailReturned bIOFailure" << bIOFailure;
        return;
    }

    SteamUGCDetails_t details;
    for (uint32 i = 0; i < pCallback->m_unTotalMatchingResults; ++i) {
        if (SteamUGC()->GetQueryUGCResult(pCallback->m_handle, i, &details)) {
            const int urlLength = 512;
            std::array<char, 512> previewUrl {};
            SteamUGC()->GetQueryUGCPreviewURL(pCallback->m_handle, i, previewUrl.data(), urlLength);

            quint64 subscriptionCount = 0;
            quint64 favoriteCount = 0;
            quint64 followerCount = 0;
            quint64 uniqueWebsiteViews = 0;
            SteamUGC()->GetQueryUGCStatistic(pCallback->m_handle, i, EItemStatistic::k_EItemStatistic_NumSubscriptions, &subscriptionCount);
            SteamUGC()->GetQueryUGCStatistic(pCallback->m_handle, i, EItemStatistic::k_EItemStatistic_NumFavorites, &favoriteCount);
            SteamUGC()->GetQueryUGCStatistic(pCallback->m_handle, i, EItemStatistic::k_EItemStatistic_NumFollowers, &followerCount);
            SteamUGC()->GetQueryUGCStatistic(pCallback->m_handle, i, EItemStatistic::k_EItemStatistic_NumUniqueWebsiteViews, &uniqueWebsiteViews);

            emit requestProfileItemDetailReturned(
                QVariant::fromValue<uint64>(details.m_nPublishedFileId),
                QString::fromUtf8(details.m_rgchTitle),
                QString::fromUtf8(details.m_rgchDescription),
                QString::fromUtf8(details.m_rgchTags).split(",", Qt::SkipEmptyParts),
                details.m_ulSteamIDOwner,
                details.m_unVotesUp,
                details.m_unVotesDown,
                details.m_flScore,
                QString::fromUtf8(details.m_rgchURL),
                QVariant::fromValue<int32>(details.m_nFileSize),
                QVariant::fromValue<uint64>(details.m_ulTotalFilesSize),
                QString::fromUtf8(previewUrl.data()),
                details.m_rtimeCreated,
                details.m_rtimeUpdated,
                static_cast<int>(details.m_eVisibility),
                details.m_bBanned,
                details.m_bAcceptedForUse,
                subscriptionCount,
                favoriteCount,
                followerCount,
                uniqueWebsiteViews,
                details.m_unNumChildren);
        } else {
            qWarning() << "GetQueryUGCResult failed for profile item!";
        }
    }
    SteamUGC()->ReleaseQueryUGCRequest(pCallback->m_handle);
}

void SteamWorkshop::updateItemMetadata(const QVariant publishedFileID, const QString& title, const QString& description, const QStringList& tags, const int visibility)
{
    if (!checkOnline())
        return;

    m_updateMetadataPublishedFileId = publishedFileID.toULongLong();
    qInfo() << "updateItemMetadata: fileId=" << m_updateMetadataPublishedFileId
            << "title=" << title
            << "tags=" << tags
            << "visibility=" << visibility;

    auto updateHandle = SteamUGC()->StartItemUpdate(m_appID, m_updateMetadataPublishedFileId);

    if (!title.isEmpty()) {
        SteamUGC()->SetItemTitle(updateHandle, title.toUtf8().constData());
    }

    if (!description.isEmpty()) {
        SteamUGC()->SetItemDescription(updateHandle, description.toUtf8().constData());
    }

    if (!tags.isEmpty()) {
        QVector<QByteArray> tagBytes;
        QVector<const char*> tagPointers;
        for (const auto& tag : tags) {
            if (!tag.isEmpty() && tag.length() <= 255) {
                tagBytes.append(tag.toUtf8());
                tagPointers.append(tagBytes.last().constData());
            } else {
                qWarning() << "updateItemMetadata: skipping invalid tag:" << tag << "length:" << tag.length();
            }
        }

        qInfo() << "updateItemMetadata: setting" << tagPointers.size() << "tags";

        if (!tagPointers.isEmpty()) {
            SteamParamStringArray_t steamTags;
            steamTags.m_ppStrings = tagPointers.data();
            steamTags.m_nNumStrings = tagPointers.size();
            SteamUGC()->SetItemTags(updateHandle, &steamTags);
        }
    }

    if (visibility >= 0) {
        SteamUGC()->SetItemVisibility(updateHandle, static_cast<ERemoteStoragePublishedFileVisibility>(visibility));
    }

    auto apiCall = SteamUGC()->SubmitItemUpdate(updateHandle, nullptr);
    if (apiCall == k_uAPICallInvalid) {
        qWarning() << "updateItemMetadata: SubmitItemUpdate returned invalid API call!";
        emit workshopItemMetadataUpdated(false, QVariant::fromValue<quint64>(m_updateMetadataPublishedFileId), 0);
        return;
    }
    m_steamUGCUpdateMetadata.Set(apiCall, this, &SteamWorkshop::onUpdateItemMetadataReturned);
}

void SteamWorkshop::onUpdateItemMetadataReturned(SubmitItemUpdateResult_t* pCallback, bool bIOFailure)
{
    if (bIOFailure) {
        qWarning() << "onUpdateItemMetadataReturned IO Failure";
        emit workshopItemMetadataUpdated(false, QVariant::fromValue<quint64>(m_updateMetadataPublishedFileId), 0);
        return;
    }

    const bool success = (pCallback->m_eResult == k_EResultOK);
    if (success) {
        qInfo() << "Successfully updated item metadata:" << pCallback->m_nPublishedFileId;
    } else {
        qWarning() << "Failed to update item metadata:" << pCallback->m_nPublishedFileId
                   << "EResult:" << pCallback->m_eResult
                   << "NeedsWorkshopAgreement:" << pCallback->m_bUserNeedsToAcceptWorkshopLegalAgreement;
    }
    emit workshopItemMetadataUpdated(success, QVariant::fromValue<quint64>(pCallback->m_nPublishedFileId), static_cast<int>(pCallback->m_eResult));
}

/*! \brief Returns install info for a subscribed workshop item: path, sizeOnDisk, timestamp. */
QVariantMap SteamWorkshop::getItemInstallInfo(const QVariant publishedFileID) const
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

/*! \brief Returns a list of files inside an installed workshop item's folder. Each entry is a map with name, size, isDir. */
QVariantList SteamWorkshop::getItemFileList(const QVariant publishedFileID) const
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

/*! \brief Starts a content update for a workshop item from a local folder. Emits progress signals. */
void SteamWorkshop::updateItemContent(const QVariant publishedFileID, const QString& absoluteContentPath, const QString& changeNote)
{
    if (!checkOnline())
        return;

    m_updateContentPublishedFileId = publishedFileID.toULongLong();
    m_contentUpdateHandle = SteamUGC()->StartItemUpdate(m_appID, m_updateContentPublishedFileId);

    SteamUGC()->SetItemContent(m_contentUpdateHandle, absoluteContentPath.toUtf8().constData());

    const auto apiCall = SteamUGC()->SubmitItemUpdate(
        m_contentUpdateHandle,
        changeNote.isEmpty() ? nullptr : changeNote.toUtf8().constData());
    m_steamUGCUpdateContent.Set(apiCall, this, &SteamWorkshop::onUpdateItemContentReturned);
}

void SteamWorkshop::onUpdateItemContentReturned(SubmitItemUpdateResult_t* pCallback, bool bIOFailure)
{
    m_contentUpdateHandle = k_UGCUpdateHandleInvalid;

    if (bIOFailure) {
        qWarning() << "onUpdateItemContentReturned IO Failure";
        emit workshopItemContentUpdated(false, QVariant::fromValue<quint64>(m_updateContentPublishedFileId));
        return;
    }

    const bool success = (pCallback->m_eResult == k_EResultOK);
    if (success) {
        qInfo() << "Successfully updated item content:" << pCallback->m_nPublishedFileId;
    } else {
        qWarning() << "Failed to update item content:" << pCallback->m_nPublishedFileId
                   << "Result:" << pCallback->m_eResult;
    }
    emit workshopItemContentUpdated(success, QVariant::fromValue<quint64>(pCallback->m_nPublishedFileId));
}

/*! \brief Returns the current content update progress as a map with progress (0..1) and status. */
QVariantMap SteamWorkshop::getContentUpdateProgress() const
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

bool SteamWorkshop::steamErrorAPIInit() const
{
    return m_steamErrorAPIInit;
}

void SteamWorkshop::setSteamErrorAPIInit(bool newSteamErrorAPIInit)
{
    if (m_steamErrorAPIInit == newSteamErrorAPIInit)
        return;
    m_steamErrorAPIInit = newSteamErrorAPIInit;
    emit steamErrorAPIInitChanged();
}

void SteamWorkshop::resetSteamErrorAPIInit()
{
    setSteamErrorAPIInit(false);
}

bool SteamWorkshop::steamErrorRestart() const
{
    return m_steamErrorRestart;
}

void SteamWorkshop::setSteamErrorRestart(bool newSteamErrorRestart)
{
    if (m_steamErrorRestart == newSteamErrorRestart)
        return;
    m_steamErrorRestart = newSteamErrorRestart;
    emit steamErrorRestartChanged();
}

void SteamWorkshop::resetSteamErrorRestart()
{
    setSteamErrorRestart(false);
}

void SteamWorkshop::requestUserItems(
    const ScreenPlayCore::Steam::EUserUGCList listType,
    const ScreenPlayCore::Steam::EUserUGCListSortOrder sortOrder)
{
    if (!checkAndSetQueryActive())
        return;

    if (!checkOnline())
        return;

    m_currentProfileListType = listType;
    m_currentProfileSortOrder = sortOrder;
    m_workshopProfileListModel->clear();
    m_workshopProfileListModel->setIsLoading(true);

    m_UGCListUserItemsHandle = SteamUGC()->CreateQueryUserUGCRequest(
        m_steamAccount->accountID(),
        static_cast<EUserUGCList>(listType),
        EUGCMatchingUGCType::k_EUGCMatchingUGCType_Items,
        static_cast<EUserUGCListSortOrder>(sortOrder),
        m_appID,
        m_appID,
        1);

    m_UGCListUserItemsCall = SteamUGC()->SendQueryUGCRequest(m_UGCListUserItemsHandle);
    m_steamUGCListUserItems.Set(m_UGCListUserItemsCall, this, &SteamWorkshop::onRequestUserItemsReturned);
    bool failed = false;

    if (!SteamUtils()->IsAPICallCompleted(m_UGCListUserItemsCall, &failed)) {
        qInfo() << "CreateQueryUserUGCRequest failed " << failed;
    }
}

/*!
    \brief Loads the next page of user items for the profile view.

    Appends results to the existing profile list instead of replacing them,
    mirroring the endless-scrolling behaviour of the main workshop page.
*/
bool SteamWorkshop::loadNextProfilePage()
{
    qInfo() << "loadNextProfilePage";

    if (!m_workshopProfileListModel->hasMore()) {
        qInfo() << "No more profile pages to load";
        return false;
    }

    if (!checkAndSetQueryActive())
        return false;

    if (!checkOnline())
        return false;

    m_workshopProfileListModel->incrementPage();
    m_workshopProfileListModel->setIsLoading(true);

    m_UGCListUserItemsHandle = SteamUGC()->CreateQueryUserUGCRequest(
        m_steamAccount->accountID(),
        static_cast<EUserUGCList>(m_currentProfileListType),
        EUGCMatchingUGCType::k_EUGCMatchingUGCType_Items,
        static_cast<EUserUGCListSortOrder>(m_currentProfileSortOrder),
        m_appID,
        m_appID,
        m_workshopProfileListModel->currentPage());

    m_UGCListUserItemsCall = SteamUGC()->SendQueryUGCRequest(m_UGCListUserItemsHandle);
    m_steamUGCListUserItems.Set(m_UGCListUserItemsCall, this, &SteamWorkshop::onRequestUserItemsReturned);
    return true;
}

void SteamWorkshop::vote(const QVariant publishedFileID, const bool voteUp)
{
    if (!checkOnline())
        return;

    SteamUGC()->SetUserItemVote(publishedFileID.toULongLong(), voteUp);
}

/*! \brief Returns true if the current user is subscribed to the given workshop item. */
bool SteamWorkshop::isSubscribed(const QVariant publishedFileID) const
{
    if (!SteamUGC())
        return false;

    const auto state = SteamUGC()->GetItemState(publishedFileID.toULongLong());
    return (state & k_EItemStateSubscribed) != 0;
}

/*! \brief Subscribes to the given workshop item. */
void SteamWorkshop::subscribeItem(const QVariant publishedFileID)
{
    if (!checkOnline())
        return;

    SteamUGC()->SubscribeItem(publishedFileID.toULongLong());
    m_steamAccount->loadAmountSubscribedItems();
}

/*! \brief Unsubscribes from the given workshop item. */
void SteamWorkshop::unsubscribeItem(const QVariant publishedFileID)
{
    if (!checkOnline())
        return;

    SteamUGC()->UnsubscribeItem(publishedFileID.toULongLong());
    m_steamAccount->loadAmountSubscribedItems();
}

void SteamWorkshop::deleteItem(const QVariant publishedFileID)
{
    if (!checkOnline())
        return;

    m_deleteItemPublishedFileId = publishedFileID.toULongLong();
    qInfo() << "Deleting workshop item:" << m_deleteItemPublishedFileId;

    SteamAPICall_t hSteamAPICall = SteamUGC()->DeleteItem(m_deleteItemPublishedFileId);
    m_steamUGCDeleteItem.Set(hSteamAPICall, this, &SteamWorkshop::onDeleteItemReturned);
}

void SteamWorkshop::onDeleteItemReturned(DeleteItemResult_t* pCallback, bool bIOFailure)
{
    if (bIOFailure) {
        qWarning() << "onDeleteItemReturned IO Failure";
        emit workshopItemDeleted(false, QVariant::fromValue<quint64>(m_deleteItemPublishedFileId));
        return;
    }

    const bool success = (pCallback->m_eResult == k_EResultOK);
    if (success) {
        qInfo() << "Successfully deleted workshop item:" << m_deleteItemPublishedFileId;
        m_workshopProfileListModel->removeByPublishedFileID(m_deleteItemPublishedFileId);
    } else {
        qWarning() << "Failed to delete workshop item:" << m_deleteItemPublishedFileId
                   << "Result:" << pCallback->m_eResult;
    }

    emit workshopItemDeleted(success, QVariant::fromValue<quint64>(m_deleteItemPublishedFileId));
}

bool SteamWorkshop::searchWorkshop(const ScreenPlayCore::Steam::EUGCQuery enumEUGCQuery)
{
    qInfo() << "searchWorkshop";

    if (!checkAndSetQueryActive())
        return false;

    if (!checkOnline())
        return false;

    if (m_searchHandle != 0) {
        qInfo() << "Invalid m_searchHandle";
        return false;
    }

    if (!SteamUGC()) {
        qWarning() << "SteamUGC() returned null in searchWorkshop - Steam API not properly initialized";
        m_queryActive = false;
        return false;
    }

    // Store current query type for loadNextPage
    m_currentQueryType = enumEUGCQuery;
    m_currentSearchText.clear();
    m_currentUserAccountID = 0;

    // Reset model for new search
    m_workshopListModel->reset();
    m_workshopListModel->setIsLoading(true);

    m_searchHandle = SteamUGC()->CreateQueryAllUGCRequest(
        static_cast<EUGCQuery>(enumEUGCQuery),
        EUGCMatchingUGCType::k_EUGCMatchingUGCType_Items,
        m_appID,
        m_appID,
        m_workshopListModel->currentPage());

    qInfo() << m_searchHandle;

    // Important: First send the request to get the Steam API Call then set the handler
    SteamUGC()->SetReturnAdditionalPreviews(m_searchHandle, true);
    SteamUGC()->SetReturnKeyValueTags(m_searchHandle, true);
    SteamUGC()->SetReturnLongDescription(m_searchHandle, true);
    m_steamUGCQuerySearchWorkshopResult.Set(SteamUGC()->SendQueryUGCRequest(m_searchHandle), this, &SteamWorkshop::onWorkshopSearched);
    return true;
}

/*!
    \brief Loads the next page of workshop items for endless scrolling.
    Appends results to the existing list instead of replacing them.
*/
bool SteamWorkshop::loadNextPage()
{
    qInfo() << "loadNextPage";

    if (!m_workshopListModel->hasMore()) {
        qInfo() << "No more pages to load";
        return false;
    }

    if (!checkAndSetQueryActive())
        return false;

    if (!checkOnline())
        return false;

    if (m_searchHandle != 0) {
        qInfo() << "Invalid m_searchHandle";
        return false;
    }

    if (!SteamUGC()) {
        qWarning() << "SteamUGC() returned null - Steam API not properly initialized";
        return false;
    }

    m_workshopListModel->incrementPage();
    m_workshopListModel->setIsLoading(true);

    UGCQueryHandle_t searchHandle;

    if (m_currentUserAccountID != 0) {
        searchHandle = SteamUGC()->CreateQueryUserUGCRequest(
            m_currentUserAccountID,
            EUserUGCList::k_EUserUGCList_Published,
            EUGCMatchingUGCType::k_EUGCMatchingUGCType_All,
            EUserUGCListSortOrder::k_EUserUGCListSortOrder_LastUpdatedDesc,
            m_appID,
            m_appID,
            m_workshopListModel->currentPage());
    } else if (m_currentSearchText.isEmpty()) {
        searchHandle = SteamUGC()->CreateQueryAllUGCRequest(
            static_cast<EUGCQuery>(m_currentQueryType),
            EUGCMatchingUGCType::k_EUGCMatchingUGCType_Items,
            m_appID,
            m_appID,
            m_workshopListModel->currentPage());
    } else {
        searchHandle = SteamUGC()->CreateQueryAllUGCRequest(
            static_cast<EUGCQuery>(m_currentQueryType),
            EUGCMatchingUGCType::k_EUGCMatchingUGCType_Items,
            m_appID,
            m_appID,
            m_workshopListModel->currentPage());

        const ParsedSearch parsed { m_currentSearchText, m_currentSearchTags };
        if (!applySearchFilters(searchHandle, parsed)) {
            m_workshopListModel->setIsLoading(false);
            m_queryActive = false;
            return false;
        }
    }

    SteamUGC()->SetReturnAdditionalPreviews(searchHandle, true);
    SteamUGC()->SetReturnKeyValueTags(searchHandle, true);
    SteamUGC()->SetReturnLongDescription(searchHandle, true);
    m_steamUGCQuerySearchWorkshopResult.Set(SteamUGC()->SendQueryUGCRequest(searchHandle), this, &SteamWorkshop::onWorkshopSearched);
    return true;
}

/*! \brief Searches the workshop for items published by a specific user. */
void SteamWorkshop::searchWorkshopByUser(const QString& steamID64)
{
    qInfo() << "searchWorkshopByUser" << steamID64;

    if (!checkAndSetQueryActive())
        return;

    if (!checkOnline())
        return;

    if (!SteamUGC()) {
        qWarning() << "SteamUGC() returned null in searchWorkshopByUser";
        m_queryActive = false;
        return;
    }

    const CSteamID creatorID(steamID64.toULongLong());
    const AccountID_t accountID = creatorID.GetAccountID();

    m_currentUserAccountID = accountID;
    m_currentSearchText.clear();

    m_workshopListModel->reset();
    m_workshopListModel->setIsLoading(true);

    const auto searchHandle = SteamUGC()->CreateQueryUserUGCRequest(
        accountID,
        EUserUGCList::k_EUserUGCList_Published,
        EUGCMatchingUGCType::k_EUGCMatchingUGCType_All,
        EUserUGCListSortOrder::k_EUserUGCListSortOrder_LastUpdatedDesc,
        m_appID,
        m_appID,
        m_workshopListModel->currentPage());

    SteamUGC()->SetReturnAdditionalPreviews(searchHandle, true);
    SteamUGC()->SetReturnKeyValueTags(searchHandle, true);
    SteamUGC()->SetReturnLongDescription(searchHandle, true);
    m_steamUGCQuerySearchWorkshopResult.Set(SteamUGC()->SendQueryUGCRequest(searchHandle), this, &SteamWorkshop::onWorkshopSearched);
}

void SteamWorkshop::onWorkshopSearched(SteamUGCQueryCompleted_t* pCallback, bool bIOFailure)
{
    m_queryActive = false;
    m_searchHandle = 0;
    m_workshopListModel->setIsLoading(false);
    if (bIOFailure) {
        qWarning() << "onWorkshopSearched ioFailure";
        return;
    }

    qInfo() << "onWorkshopSearched";
    queryWorkshopItemFromHandle(m_workshopListModel.get(), pCallback);
}

bool SteamWorkshop::queryWorkshopItemFromHandle(SteamWorkshopListModel* listModel, SteamUGCQueryCompleted_t* pCallback)
{
    qInfo() << "queryWorkshopItemFromHandle";

    SteamUGCDetails_t details;
    constexpr int urlLength = 200;
    std::array<char, 200> url {};

    const uint32 totalResults = pCallback->m_unTotalMatchingResults;
    const uint32 results = pCallback->m_unNumResultsReturned;

    if (totalResults <= 0 || results <= 0) {
        qWarning() << "Invalid result count. Aborting! totalResults:" << totalResults << "results " << results;
        SteamUGC()->ReleaseQueryUGCRequest(pCallback->m_handle);
        emit workshopSearchCompleted(0);
        return false;
    }

    const float maxResultsPerPage = 50;
    const int pages = std::ceil(static_cast<double>(totalResults) / maxResultsPerPage);
    listModel->setPages(pages);

    for (uint32 i = 0; i < results; i++) {

        if (SteamUGC()->GetQueryUGCResult(pCallback->m_handle, i, &details)) {

            if (SteamUGC()->GetQueryUGCPreviewURL(pCallback->m_handle, i, url.data(), static_cast<uint32>(urlLength))) {
                QByteArray urlData(url.data());

                // Todo use multiple preview for gif hover effect
                quint64 subscriptionCount = 0;
                SteamUGC()->GetQueryUGCStatistic(pCallback->m_handle, i, EItemStatistic::k_EItemStatistic_NumSubscriptions, &subscriptionCount);

                int addPreviewCount = SteamUGC()->GetQueryUGCNumAdditionalPreviews(pCallback->m_handle, i);
                QUrl additionalPreviewUrl;

                for (int j = 0; j < addPreviewCount; ++j) {
                    std::array<char, 2000> pchURLOrVideoID {};
                    std::array<char, 2000> pchOriginalFileName {};
                    EItemPreviewType previewType;
                    SteamUGC()->GetQueryUGCAdditionalPreview(pCallback->m_handle, i, j, pchURLOrVideoID.data(), pchURLOrVideoID.size(), pchOriginalFileName.data(), pchOriginalFileName.size(), &previewType);
                    additionalPreviewUrl = QByteArray(pchURLOrVideoID.data());
                }

                WorkshopItem item {
                    QVariant::fromValue<uint64>(details.m_nPublishedFileId),
                    subscriptionCount,
                    QString(details.m_rgchTitle),
                    QUrl(urlData),
                    additionalPreviewUrl,
                    QString(details.m_rgchTags).split(",", Qt::SkipEmptyParts),
                    details.m_ulSteamIDOwner == m_steamAccount->steamID64(),
                    details.m_ulSteamIDOwner
                };

                listModel->append(std::move(item));

                // Do not change the background image on every page
                if (i == 0 && listModel->currentPage() == 1) {
                    emit workshopBannerCompleted();
                    requestCreatorName(QString::number(details.m_ulSteamIDOwner));
                }
            }
        } else {
            qWarning() << "Loading error! Index: " << i;
        }
    }

    qInfo() << m_searchHandle << pCallback->m_handle;
    SteamUGC()->ReleaseQueryUGCRequest(pCallback->m_handle);

    emit workshopSearchCompleted(results);
    return true;
}

/*! \brief Parses search input, extracting quoted strings as tags. */
SteamWorkshop::ParsedSearch SteamWorkshop::parseSearchInput(const QString& input) const
{
    ParsedSearch result;
    QStringList tags;
    QString remaining = input;

    // Extract all "quoted" strings as tags
    static const QRegularExpression tagRegex("\"([^\"]+)\"");
    auto it = tagRegex.globalMatch(input);
    while (it.hasNext()) {
        const auto match = it.next();
        tags.append(match.captured(1).trimmed());
        remaining.replace(match.captured(0), "");
    }

    result.text = remaining.simplified();
    result.tags = tags;

    if (!tags.isEmpty()) {
        qInfo() << "parseSearchInput: text=" << result.text << "tags=" << result.tags;
    }

    return result;
}

/*! \brief Applies search text and required tags to a UGC query handle. */
bool SteamWorkshop::applySearchFilters(UGCQueryHandle_t handle, const ParsedSearch& parsed)
{
    if (!parsed.text.isEmpty()) {
        if (!SteamUGC()->SetSearchText(handle, parsed.text.toUtf8().constData())) {
            qWarning() << "SetSearchText failed:" << parsed.text;
            return false;
        }
    }

    for (const auto& tag : parsed.tags) {
        if (!SteamUGC()->AddRequiredTag(handle, tag.toUtf8().constData())) {
            qWarning() << "AddRequiredTag failed:" << tag;
        }
    }

    return true;
}

void SteamWorkshop::searchWorkshopByText(const QString text, const ScreenPlayCore::Steam::EUGCQuery rankedBy)
{
    qInfo() << "searchWorkshopByText" << text;

    if (!checkAndSetQueryActive())
        return;

    if (!checkOnline())
        return;

    if (!SteamUGC()) {
        qWarning() << "SteamUGC() returned null in searchWorkshopByText - Steam API not properly initialized";
        m_queryActive = false;
        return;
    }

    // Store current query for loadNextPage
    m_currentQueryType = rankedBy;
    m_currentSearchText = text;
    m_currentUserAccountID = 0;

    const auto parsed = parseSearchInput(text);
    m_currentSearchTags = parsed.tags;

    // Reset model for new search
    m_workshopListModel->reset();
    m_workshopListModel->setIsLoading(true);

    auto searchHandle = SteamUGC()->CreateQueryAllUGCRequest(
        static_cast<EUGCQuery>(rankedBy),
        EUGCMatchingUGCType::k_EUGCMatchingUGCType_Items,
        m_appID,
        m_appID,
        m_workshopListModel->currentPage());

    if (!applySearchFilters(searchHandle, parsed)) {
        m_workshopListModel->setIsLoading(false);
        m_queryActive = false;
        return;
    }

    // Important: First send the request to get the Steam API Call then set the handler
    SteamUGC()->SetReturnAdditionalPreviews(searchHandle, true);
    SteamUGC()->SetReturnKeyValueTags(searchHandle, true);
    SteamUGC()->SetReturnLongDescription(searchHandle, true);
    m_steamUGCQuerySearchWorkshopResult.Set(SteamUGC()->SendQueryUGCRequest(searchHandle), this, &SteamWorkshop::onWorkshopSearched);
}

void SteamWorkshop::onRequestUserItemsReturned(SteamUGCQueryCompleted_t* pCallback, bool bIOFailure)
{
    m_queryActive = false;
    m_workshopProfileListModel->setIsLoading(false);
    if (bIOFailure) {
        qWarning() << "onRequestUserItemsReturned IO Failure";
        return;
    }

    queryWorkshopItemFromHandle(m_workshopProfileListModel.get(), pCallback);

    // Calculate aggregate user statistics from loaded profile items
    updateUserProfileStatistics();
}

void SteamWorkshop::updateUserProfileStatistics()
{
    const int itemCount = m_workshopProfileListModel->rowCount();
    quint64 totalSubscriptions = 0;

    for (int i = 0; i < itemCount; ++i) {
        const auto index = m_workshopProfileListModel->index(i, 0);
        const auto subscriptions = m_workshopProfileListModel->data(index, SteamWorkshopListModel::SubscriptionCountRole).toULongLong();
        totalSubscriptions += subscriptions;
    }

    if (m_userPublishedItemCount != itemCount) {
        m_userPublishedItemCount = itemCount;
        emit userPublishedItemCountChanged(m_userPublishedItemCount);
    }

    if (m_userTotalSubscriptions != totalSubscriptions) {
        m_userTotalSubscriptions = totalSubscriptions;
        emit userTotalSubscriptionsChanged(m_userTotalSubscriptions);
    }
}
}

#include "moc_steamworkshop.cpp"
