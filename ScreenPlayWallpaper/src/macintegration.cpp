// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#include "macintegration.h"

namespace ScreenPlay::MacIntegration {
void setBackgroundLevelImpl(void* cocoaWindowOrView);
void enableSpaceChangeReapplyImpl(bool enable);

void setBackgroundLevel(void* cocoaWindowOrView)
{
    setBackgroundLevelImpl(cocoaWindowOrView);
}

void enableSpaceChangeReapply(bool enable)
{
    enableSpaceChangeReapplyImpl(enable);
}
}
