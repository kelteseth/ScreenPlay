// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#include "steamqmlimageprovider.h"

namespace ScreenPlayWorkshop {
SteamQMLImageProvider::SteamQMLImageProvider(QQuickItem* parent)
    : QQuickItem(parent)
{
    setFlag(QQuickItem::ItemHasContents);
}
}
