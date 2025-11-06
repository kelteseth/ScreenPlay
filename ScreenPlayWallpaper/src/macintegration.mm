// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#include "macintegration.h"

#import <Cocoa/Cocoa.h>

namespace ScreenPlay::MacIntegration {
void setBackgroundLevelImpl(void* cocoaWindowOrView)
{
    if (!cocoaWindowOrView) {
        return;
    }

    id cocoaObject = (id)cocoaWindowOrView;
    NSWindow* window = nil;

    if ([cocoaObject isKindOfClass:[NSWindow class]]) {
        window = (NSWindow*)cocoaObject;
    } else if ([cocoaObject isKindOfClass:[NSView class]]) {
        window = [(NSView*)cocoaObject window];
    }

    if (!window) {
        return;
    }

    [window setLevel:CGWindowLevelForKey(kCGDesktopWindowLevelKey) + 1];
    [window setBackgroundColor:[NSColor blackColor]];
    [window setAcceptsMouseMovedEvents:false];
    [window setMovableByWindowBackground:false];
}
}
