// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#include "ScreenPlay/globalvariables.h"

namespace ScreenPlay {

/*!
    \class ScreenPlay::GlobalVariables
    \inmodule ScreenPlay
    \brief  Contains all variables that are globally needed.
*/

/*!
    \brief Constructs the global variabls.
*/
ScreenPlay::GlobalVariables::GlobalVariables(QObject* parent)
    : QObject(parent)
{
    setLocalSettingsPath(QUrl { QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) });
}

}

#include "moc_globalvariables.cpp"
