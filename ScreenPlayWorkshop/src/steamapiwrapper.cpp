// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#include <QByteArray>
#include <QJsonArray>
#include <QString>
#include <QStringList>

#include "cstring"
#include "stdlib.h"
#include "steam/steam_api.h"
#include "steam/steamtypes.h"
#include "steamapiwrapper.h"

namespace ScreenPlayWorkshop {
bool setItemTags(const QVariant& updateHandle, const QStringList& tags)
{
    auto pTags = std::make_unique<SteamParamStringArray_t>();
    const uint32_t numTags = tags.size();
    pTags->m_nNumStrings = numTags;
    auto tagStrings = std::make_unique<const char*[]>(numTags);
    for (uint32_t i = 0; i < numTags; ++i) {
        tagStrings[i] = tags.at(i).toUtf8().constData();
    }
    pTags->m_ppStrings = tagStrings.get();
    return SteamUGC()->SetItemTags(updateHandle.toULongLong(), pTags.get());
}
}
