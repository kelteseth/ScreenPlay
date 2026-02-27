// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#include "steamworkshop.h"

#include <QLoggingCategory>

Q_LOGGING_CATEGORY(workshopSteam, "screenplay.workshop.steam")

namespace ScreenPlayWorkshop {

/*!
    \class ScreenPlayWorkshop::SteamWorkshop
    \inmodule ScreenPlayWorkshop
    \brief Thin QML-facing facade that composes SteamWorkshopSearch,
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

    if (!SteamAPI_Init()) {
        qCWarning(workshopSteam) << "SteamAPI_Init failed";
        m_steamErrorAPIInit = true;
        return false;
    }

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
