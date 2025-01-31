// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#include "ScreenPlayCore/macutils.h"
#import <Cocoa/Cocoa.h>
#import <MacTypes.h>
#import <objc/runtime.h>

MacUtils* MacUtils::macUtils = nullptr;

MacUtils::MacUtils()
{
}

MacUtils* MacUtils::instance()
{
    if (!macUtils)
        macUtils = new MacUtils;
    return macUtils;
}

void MacUtils::showDockIcon(const bool show)
{
    ProcessSerialNumber psn = { 0, kCurrentProcess };

    if (show) {
        TransformProcessType(&psn, kProcessTransformToForegroundApplication);
    } else {
        TransformProcessType(&psn, kProcessTransformToUIElementApplication);
    }
}
