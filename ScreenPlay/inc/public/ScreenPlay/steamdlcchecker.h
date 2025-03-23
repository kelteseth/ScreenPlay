// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#pragma once

#include "ScreenPlayCore/globalenums.h"
#include "ScreenPlayCore/steamenumsgenerated.h"
#include <QObject>
#include <expected>

namespace ScreenPlay {

enum class SteamError {
    None,
    RestartRequired,
    InitializationFailed,
    AppsInterfaceUnavailable
};

class SteamDLCChecker : public QObject {
    Q_OBJECT

public:
    explicit SteamDLCChecker(QObject* parent = nullptr);
    ~SteamDLCChecker();

    // Main method to check DLC status and determine version
    std::expected<ScreenPlayEnums::Version, SteamError> checkDLCStatus();

    // Individual DLC check method
    std::expected<bool, SteamError> isDLCInstalled(quint64 dlcAppID);

    // Helper methods for Steam initialization and shutdown
    std::expected<bool, SteamError> initializeSteam();
    void shutdownSteam();

    // Static app IDs
    static constexpr quint64 MAIN_APP_ID = 672870;
    static constexpr quint64 PRO_DLC_APP_ID = 3340700; // ScreenPlay OpenSource Plus Supporter
    static constexpr quint64 ULTRA_DLC_APP_ID = 0; // Ultimate (not used yet)

private:
    bool m_steamInitialized = false;
    bool m_restartRequired = false;
};

} // namespace ScreenPlay
