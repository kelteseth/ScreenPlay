// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#include "macintegration.h"

#import <Cocoa/Cocoa.h>

namespace {
NSWindow* gTrackedWindow = nil;
id gSpaceObserver = nil;
bool gSpaceReapplyEnabled = false;
constexpr NSUInteger kReapplyBehaviorFlags = NSWindowCollectionBehaviorCanJoinAllSpaces
    | NSWindowCollectionBehaviorStationary
    | NSWindowCollectionBehaviorIgnoresCycle;

void updateCollectionBehavior(NSWindow* window)
{
    if (!window) {
        return;
    }

    NSUInteger behavior = [window collectionBehavior];

    if (gSpaceReapplyEnabled) {
        behavior |= kReapplyBehaviorFlags;
    } else {
        behavior &= ~kReapplyBehaviorFlags;
    }

    [window setCollectionBehavior:behavior];
}

void reapplyForCurrentSpace()
{
    NSWindow* window = gTrackedWindow;
    if (!window) {
        return;
    }

    [window setLevel:CGWindowLevelForKey(kCGDesktopWindowLevelKey) + 1];
    [window orderBack:nil];
}

void installSpaceObserver()
{
    if (!gSpaceReapplyEnabled || gSpaceObserver || !gTrackedWindow) {
        return;
    }

    gSpaceObserver = [[[NSWorkspace sharedWorkspace] notificationCenter] addObserverForName:NSWorkspaceActiveSpaceDidChangeNotification
                                                                                         object:nil
                                                                                          queue:[NSOperationQueue mainQueue]
                                                                                     usingBlock:^(NSNotification*) {
                                                                                         reapplyForCurrentSpace();
                                                                                     }];
}

void uninstallSpaceObserver()
{
    if (!gSpaceObserver) {
        return;
    }

    [[[NSWorkspace sharedWorkspace] notificationCenter] removeObserver:gSpaceObserver];
    gSpaceObserver = nil;
}
}

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
    updateCollectionBehavior(window);

    gTrackedWindow = window;
    if (gSpaceReapplyEnabled) {
        installSpaceObserver();
        reapplyForCurrentSpace();
    }
}

void enableSpaceChangeReapplyImpl(bool enable)
{
    if (gSpaceReapplyEnabled == enable) {
        if (enable) {
            installSpaceObserver();
            reapplyForCurrentSpace();
        } else {
            uninstallSpaceObserver();
        }
        updateCollectionBehavior(gTrackedWindow);
        return;
    }

    gSpaceReapplyEnabled = enable;
    updateCollectionBehavior(gTrackedWindow);

    if (enable) {
        installSpaceObserver();
        reapplyForCurrentSpace();
    } else {
        uninstallSpaceObserver();
    }
}
}
