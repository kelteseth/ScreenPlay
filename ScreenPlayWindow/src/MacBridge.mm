#include "macbridge.h"
#import <Cocoa/Cocoa.h>
#import <objc/runtime.h>
#include <QApplication>
#include <QWindow>

MacBridge * MacBridge::bridge = NULL;

MacBridge::MacBridge()
{

}

MacBridge * MacBridge::instance()
{
    if(!bridge)
        bridge = new MacBridge;
    return bridge;
}

void MacBridge::SetBackgroundLevel(QWindow* window)
{
    NSView* view = (NSView *) window->winId();
    NSWindow* nsWindow = (NSWindow*)[view window];

    [nsWindow setLevel:CGWindowLevelForKey(kCGDesktopWindowLevelKey) + 1];
    [nsWindow setBackgroundColor: [NSColor blackColor]];
    [nsWindow setAcceptsMouseMovedEvents: false];
    [nsWindow setMovableByWindowBackground: false];
}
