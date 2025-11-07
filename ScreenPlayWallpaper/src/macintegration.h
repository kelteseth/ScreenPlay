// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#pragma once

namespace ScreenPlay::MacIntegration {
/*!\
 * Elevate the native window so it behaves like a desktop wallpaper window.
 */
void setBackgroundLevel(void* cocoaWindowOrView);

/*!\
 * Reapply the wallpaper window whenever the active Mission Control Space changes.\
 * Enable this when the wallpaper falls behind after space transitions.
 */
void enableSpaceChangeReapply(bool enable);
}
