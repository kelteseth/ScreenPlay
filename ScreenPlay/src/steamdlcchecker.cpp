// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#include "steamdlcchecker.h"
#include <QDebug>
// Include Steam API
#include "steam/steam_api.h"

namespace ScreenPlay {

SteamDLCChecker::SteamDLCChecker(QObject* parent)
    : QObject(parent)
    , m_steamInitialized(false)
    , m_restartRequired(false)
{
}

SteamDLCChecker::~SteamDLCChecker()
{
    if (m_steamInitialized) {
        shutdownSteam();
    }
}

std::expected<bool, SteamError> SteamDLCChecker::initializeSteam()
{
    // Check if Steam needs to restart the app
    // This must be called before SteamAPI_Init
    if (SteamAPI_RestartAppIfNecessary(MAIN_APP_ID)) {
        qWarning() << "SteamAPI_RestartAppIfNecessary returned true. Application should restart through Steam.";
        m_restartRequired = true;
        return std::unexpected(SteamError::RestartRequired);
    }

    // Initialize Steam API
    if (!SteamAPI_Init()) {
        qWarning() << "SteamAPI_Init failed";
        /*
         * Possible failure reasons:
         * - The Steam client isn't running
         * - The Steam client couldn't determine the App ID of game
         * - Your application is not running under the same OS user context as the Steam client
         * - You don't own a license for the App ID
         * - Your App ID is not completely set up
         * - Family sharing enabled can cause failures
         */
        return std::unexpected(SteamError::InitializationFailed);
    }

    m_steamInitialized = true;
    return true;
}

void SteamDLCChecker::shutdownSteam()
{
    if (m_steamInitialized) {
        SteamAPI_Shutdown();
        m_steamInitialized = false;
    }
}

std::expected<bool, SteamError> SteamDLCChecker::isDLCInstalled(quint64 dlcAppID)
{
    // Check if Steam is initialized
    if (!m_steamInitialized) {
        auto initResult = initializeSteam();
        if (!initResult) {
            return std::unexpected(initResult.error());
        }
    }

    // Check if SteamApps interface is available
    if (!SteamApps()) {
        qWarning() << "SteamApps interface not available";
        return std::unexpected(SteamError::AppsInterfaceUnavailable);
    }

    bool dlcOwned = SteamApps()->BIsDlcInstalled(dlcAppID);
    qInfo() << "Steam DLC" << dlcAppID << "ownership status:" << (dlcOwned ? "Owned" : "Not owned");
    return dlcOwned;
}

std::expected<ScreenPlayEnums::Version, SteamError> SteamDLCChecker::checkDLCStatus()
{
    // Initialize Steam if needed
    if (!m_steamInitialized) {
        auto initResult = initializeSteam();
        if (!initResult) {
            return std::unexpected(initResult.error());
        }
    }

    // Check ownership of PRO DLC
    auto proResult = isDLCInstalled(PRO_DLC_APP_ID);
    if (!proResult) {
        return std::unexpected(proResult.error());
    }
    bool hasProDLC = proResult.value();

    // Check ownership of ULTRA DLC (if implemented)
    bool hasUltraDLC = false;
    if (ULTRA_DLC_APP_ID > 0) {
        auto ultraResult = isDLCInstalled(ULTRA_DLC_APP_ID);
        if (!ultraResult) {
            return std::unexpected(ultraResult.error());
        }
        hasUltraDLC = ultraResult.value();
    }

    // Determine version based on DLC ownership
    ScreenPlayEnums::Version version;
    if (hasUltraDLC) {
        version = ScreenPlayEnums::Version::OpenSourceUltraSteam;
    } else if (hasProDLC) {
        version = ScreenPlayEnums::Version::OpenSourceProSteam;
    } else {
        version = ScreenPlayEnums::Version::OpenSourceSteam;
    }

    // Clean shutdown
    shutdownSteam();
    return version;
}


} // namespace ScreenPlay
