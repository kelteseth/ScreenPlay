// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#include "screenplayworkshop.h"

namespace ScreenPlayWorkshop {
ScreenPlayWorkshop::ScreenPlayWorkshop()
    : QObject(nullptr)
{
    qInfo() << "ScreenPlayWorkshop";
    m_installedListModel = std::make_unique<InstalledListModel>();
    m_installedListFilter = std::make_unique<InstalledListFilter>();
    m_steamWorkshop = std::make_unique<SteamWorkshop>();
    // Note: init() must be called explicitly to load content
    // Use init() for default path from QSettings
    // Use init(contentPath) for custom path (e.g., testing)
}

}

#include "moc_screenplayworkshop.cpp"
