// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#include "macintegration.h"
#include "macbridge.h"

MacIntegration::MacIntegration(QObject* parent)
    : QObject(parent)
{
    MacBridge::instance();
}

void MacIntegration::SetBackgroundLevel(QWindow* window)
{
    MacBridge::SetBackgroundLevel(window);
}

#include "moc_macintegration.cpp"
