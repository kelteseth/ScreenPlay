// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#include "screenplayworkshop.h"

#include <QLoggingCategory>

Q_LOGGING_CATEGORY(workshopMain, "screenplay.workshop.main")

namespace ScreenPlayWorkshop {

/*!
    \class ScreenPlayWorkshop::ScreenPlayWorkshop
    \inmodule ScreenPlayWorkshop
    \brief Top-level QObject that owns InstalledListModel, InstalledListFilter,
           and SteamWorkshop; exposed as a QML element.

    The constructor creates the three sub-objects. Call \c init() (or
    \c init(const QUrl &)) after construction to load installed content and
    connect to Steam.
*/

/*!
    \fn ScreenPlayWorkshop::ScreenPlayWorkshop()
    \brief Constructs the top-level workshop object and creates
           InstalledListModel, InstalledListFilter, and SteamWorkshop.
           \c init() must be called separately to start loading content.
*/
ScreenPlayWorkshop::ScreenPlayWorkshop()
    : QObject(nullptr)
{
    qCInfo(workshopMain) << "ScreenPlayWorkshop";
    m_installedListModel = std::make_unique<InstalledListModel>();
    m_installedListFilter = std::make_unique<InstalledListFilter>();
    m_steamWorkshop = std::make_unique<SteamWorkshop>();
    // Note: init() must be called explicitly to load content
    // Use init() for default path from QSettings
    // Use init(contentPath) for custom path (e.g., testing)
}

}

#include "moc_screenplayworkshop.cpp"
