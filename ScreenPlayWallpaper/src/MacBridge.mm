/****************************************************************************
**
** Copyright (C) 2020 Elias Steurer (Kelteseth)
** Contact: https://screen-play.app
**
** This file is part of ScreenPlay. ScreenPlay is licensed under a dual license in
** order to ensure its sustainability. When you contribute to ScreenPlay
** you accept that your work will be available under the two following licenses:
**
** $SCREENPLAY_BEGIN_LICENSE$
**
** #### Affero General Public License Usage (AGPLv3)
** Alternatively, this file may be used under the terms of the GNU Affero
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file "ScreenPlay License.md" included in the
** packaging of this App. Please review the following information to
** ensure the GNU Affero Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/agpl-3.0.en.html.
**
** #### Commercial License
** This code is owned by Elias Steurer. By changing/adding to the code you agree to the
** terms written in:
**  * Legal/corporate_contributor_license_agreement.md - For corporate contributors
**  * Legal/individual_contributor_license_agreement.md - For individual contributors
**
** #### Additional Limitations to the AGPLv3 and Commercial Lincese
** This License does not grant any rights in the trademarks,
** service marks, or logos.
**
**
** $SCREENPLAY_END_LICENSE$
**
****************************************************************************/

#include "macbridge.h"
#import <Cocoa/Cocoa.h>
#include <QApplication>
#include <QWindow>
#import <objc/runtime.h>

MacBridge* MacBridge::bridge = NULL;

MacBridge::MacBridge()
{
}

MacBridge* MacBridge::instance()
{
    if (!bridge)
        bridge = new MacBridge;
    return bridge;
}

void MacBridge::SetBackgroundLevel(QWindow* window)
{
    NSView* view = (NSView*)window->winId();
    NSWindow* nsWindow = (NSWindow*)[view window];

    [nsWindow setLevel:CGWindowLevelForKey(kCGDesktopWindowLevelKey) + 1];
    [nsWindow setBackgroundColor:[NSColor blackColor]];
    [nsWindow setAcceptsMouseMovedEvents:false];
    [nsWindow setMovableByWindowBackground:false];
}
