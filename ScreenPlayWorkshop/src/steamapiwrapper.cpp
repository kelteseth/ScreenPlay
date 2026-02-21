// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#include <QString>
#include <QStringList>

#include "steam/steam_api.h"
#include "steamapiwrapper.h"
#include "steamtagarray.h"

namespace ScreenPlayWorkshop {
bool setItemTags(const QVariant& updateHandle, const QStringList& tags)
{
    SteamTagArray tagArray(tags);
    return SteamUGC()->SetItemTags(updateHandle.toULongLong(), tagArray.get());
}
}
