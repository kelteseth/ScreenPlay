// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#include "macintegration.h"

namespace ScreenPlay::MacIntegration {
void setBackgroundLevelImpl(void* cocoaWindowOrView);

void setBackgroundLevel(void* cocoaWindowOrView)
{
    setBackgroundLevelImpl(cocoaWindowOrView);
}
}
