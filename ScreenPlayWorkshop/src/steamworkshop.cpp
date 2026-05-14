// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#include "steamworkshop.h"

#include <QLoggingCategory>

Q_LOGGING_CATEGORY(workshopSteam, "screenplay.workshop.steam")

namespace ScreenPlayWorkshop {

/*!
    \class ScreenPlayWorkshop::SteamWorkshop
    \inmodule ScreenPlayWorkshop
    \brief Thin QML-facing class that composes SteamWorkshopSearch,
           SteamWorkshopProfile and SteamWorkshopItemOps.

    Sub-objects are exposed directly to QML via the \c search, \c profile
    and \c itemOps properties. QML calls methods on them directly.
*/

/*!
    \fn bool SteamWorkshop::init()
    \brief Initialises the Steam API, creates all sub-objects and starts the poll
           timer.

    Must be called once after construction. Returns \c false and sets the
    appropriate error property if Steam is not running or the app ID does not
    match.
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
        qCWarning(workshopSteam) << "SteamAPI_Init failed";
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
        qCWarning(workshopSteam) << "SteamAPI_RestartAppIfNecessary failed";
        m_steamErrorRestart = true;
        return false;
    }

    m_steamAccount = std::make_unique<SteamAccount>();
    m_uploadListModel = std::make_unique<UploadListModel>();

    m_search = std::make_unique<SteamWorkshopSearch>(*this, m_appID);
    m_search->init();

    m_profile = std::make_unique<SteamWorkshopProfile>(*this, *m_search, m_appID);
    m_profile->init();

    m_itemOps = std::make_unique<SteamWorkshopItemOps>(*this, m_appID);

    // Wire cross-cutting concern: a successful delete removes the item from
    // the profile list model so the UI stays in sync.
    connect(m_itemOps.get(), &SteamWorkshopItemOps::workshopItemDeleted,
        this, [this](bool success, QVariant publishedFileID) {
            if (success && m_profile) {
                m_profile->workshopProfileListModel()
                    ->removeByPublishedFileID(publishedFileID.toULongLong());
            }
        });

    QObject::connect(&m_pollTimer, &QTimer::timeout, this, []() { SteamAPI_RunCallbacks(); });
    m_pollTimer.start(100);

    setOnline(true);

    return true;
}

/*!
    \fn bool SteamWorkshop::checkOnline()
    \brief Returns \c true when the Steam API is initialised and the user is
           considered online. Logs a warning and returns \c false otherwise.
*/
bool SteamWorkshop::checkOnline()
{
    if (!m_online || m_steamErrorAPIInit) {
        qCWarning(workshopSteam) << "Trying to call steam api while offline or not initialized";
        qCWarning(workshopSteam) << "steamErrorAPIInit: " << m_steamErrorAPIInit;
        qCWarning(workshopSteam) << "steamErrorRestart: " << m_steamErrorRestart;
        return false;
    }
    return true;
}

/*!
    \fn bool SteamWorkshop::checkAndSetQueryActive()
    \brief Atomically checks whether a UGC query is already in flight and, if
           not, marks one as active. Returns \c false (and logs a warning) when
           a concurrent query is detected so callers can bail out early.
*/
bool SteamWorkshop::checkAndSetQueryActive()
{
    if (m_queryActive) {
        qCWarning(workshopSteam) << "Query already active! Abort";
        return false;
    }

    m_queryActive = true;

    return m_queryActive;
}

/*!
    \fn void SteamWorkshop::bulkUploadToWorkshop(QStringList absoluteStoragePaths)
    \brief Queues all paths in \a absoluteStoragePaths for sequential Steam
           Workshop upload via the \c uploadListModel.
*/
void SteamWorkshop::bulkUploadToWorkshop(QStringList absoluteStoragePaths)
{
    // Clear any leftover items from previous uploads
    uploadListModel()->clearWhenFinished();

    qCInfo(workshopSteam) << "bulkUploadToWorkshop called with" << absoluteStoragePaths.size()
                          << "paths:" << absoluteStoragePaths;

    for (const QString& path : absoluteStoragePaths) {
        qCInfo(workshopSteam) << "Append " << path;
        uploadListModel()->append("", path, m_appID);
    }

    qCInfo(workshopSteam) << "Model now has" << uploadListModel()->rowCount() << "items";
}

/*!
    \fn void SteamWorkshop::onWorkshopItemInstalled(ItemInstalled_t* itemInstalled)
    \brief Steam callback fired when a subscribed item has finished installing
           or updating on disk. Forwards the event as the \c workshopItemInstalled
           signal.
*/
void SteamWorkshop::onWorkshopItemInstalled(ItemInstalled_t* itemInstalled)
{
    emit workshopItemInstalled(itemInstalled->m_unAppID, itemInstalled->m_nPublishedFileId);
}

/*!
    \fn void SteamWorkshop::onPersonaStateChange(PersonaStateChange_t* pCallback)
    \brief Steam callback fired when a friend's persona state changes. Used to
           resolve pending creator name requests initiated by requestCreatorName().
*/
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

/*!
    \fn void SteamWorkshop::requestCreatorName(const QString &steamID64)
    \brief Asynchronously resolves the Steam persona name for \a steamID64.

    If the name is already in the local friend cache it is emitted immediately
    via \c creatorNameReady; otherwise the request is queued and the result is
    delivered once the \c PersonaStateChange_t callback fires.
*/
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

/*!
    \fn bool SteamWorkshop::steamErrorAPIInit() const
    \brief Returns \c true when \c SteamAPI_Init() failed during init().
*/
bool SteamWorkshop::steamErrorAPIInit() const
{
    return m_steamErrorAPIInit;
}

/*!
    \fn void SteamWorkshop::setSteamErrorAPIInit(bool newSteamErrorAPIInit)
    \brief Sets the \c steamErrorAPIInit property to \a newSteamErrorAPIInit
           and emits \c steamErrorAPIInitChanged if the value changed.
*/
void SteamWorkshop::setSteamErrorAPIInit(bool newSteamErrorAPIInit)
{
    if (m_steamErrorAPIInit == newSteamErrorAPIInit)
        return;
    m_steamErrorAPIInit = newSteamErrorAPIInit;
    emit steamErrorAPIInitChanged();
}

/*!
    \fn void SteamWorkshop::resetSteamErrorAPIInit()
    \brief Resets the \c steamErrorAPIInit property to \c false.
*/
void SteamWorkshop::resetSteamErrorAPIInit()
{
    setSteamErrorAPIInit(false);
}

/*!
    \fn bool SteamWorkshop::steamErrorRestart() const
    \brief Returns \c true when \c SteamAPI_RestartAppIfNecessary() indicated
           that the process was not launched through Steam.
*/
bool SteamWorkshop::steamErrorRestart() const
{
    return m_steamErrorRestart;
}

/*!
    \fn void SteamWorkshop::setSteamErrorRestart(bool newSteamErrorRestart)
    \brief Sets the \c steamErrorRestart property to \a newSteamErrorRestart
           and emits \c steamErrorRestartChanged if the value changed.
*/
void SteamWorkshop::setSteamErrorRestart(bool newSteamErrorRestart)
{
    if (m_steamErrorRestart == newSteamErrorRestart)
        return;
    m_steamErrorRestart = newSteamErrorRestart;
    emit steamErrorRestartChanged();
}

/*!
    \fn void SteamWorkshop::resetSteamErrorRestart()
    \brief Resets the \c steamErrorRestart property to \c false.
*/
void SteamWorkshop::resetSteamErrorRestart()
{
    setSteamErrorRestart(false);
}

} // namespace ScreenPlayWorkshop

#include "moc_steamworkshop.cpp"
