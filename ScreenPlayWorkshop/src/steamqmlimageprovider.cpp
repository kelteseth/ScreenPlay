#include "steamqmlimageprovider.h"

namespace ScreenPlayWorkshop {
SteamQMLImageProvider::SteamQMLImageProvider(QQuickItem* parent)
    : QQuickItem(parent)
{
    setFlag(QQuickItem::ItemHasContents);
}
}
