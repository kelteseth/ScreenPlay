// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#include "linuxx11window.h"
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xproto.h>
#include <X11/Xutil.h>
#include <X11/extensions/shape.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <QGuiApplication>


#define ATOM(a) XInternAtom(display, #a, False)

namespace ScreenPlay {
    /**
 * @brief Initializes and displays the X11 window for the wallpaper
 * 
 * The order of operations is critical for proper window sizing and display:
 * 1. Platform check (must be X11/xcb)
 * 2. Get X11 display
 * 3. Get screen geometry
 * 4. Set up QQuickView sizing (strict order required):
 *    a. Set resize mode to SizeRootObjectToView first
 *    b. Set geometry to match screen
 *    c. Set minimum size
 *    d. Set maximum size
 * 5. Get window ID
 * 6. Set X11 window properties
 * 7. Sync X11 display to ensure properties are processed
 * 8. Finally show the window
 * 
 * This specific order ensures the QML root item receives proper size
 * signals and the window displays correctly on the desktop.
 * 
 * @return WallpaperExit::Code::Ok on success, WallpaperExit::Code::Invalid_Setup_Error on failure
 */
WallpaperExit::Code LinuxX11Window::start()
{
    if (QGuiApplication::platformName() != "xcb") {
        return WallpaperExit::Code::Invalid_Setup_Error;
    }

    if (auto *x11App = qGuiApp->nativeInterface<QNativeInterface::QX11Application>()) {
        Display* display = x11App->display();
        if (!display) {
            return WallpaperExit::Code::Invalid_Setup_Error;
        }

        auto* screen = QGuiApplication::screens().at(0);
        QRect screenGeometry = screen->geometry();

        // Set initial size and position BEFORE loading QML
        m_quickView->setMinimumSize(QSize(screenGeometry.width(), screenGeometry.height()));
        m_quickView->setMaximumSize(QSize(screenGeometry.width(), screenGeometry.height()));
        m_quickView->setResizeMode(QQuickView::SizeRootObjectToView);  // Important!
        m_quickView->setGeometry(screenGeometry);

        // Get window ID
        Window window = m_quickView->winId();

        // Set window properties
        Atom type = ATOM(_NET_WM_WINDOW_TYPE_DESKTOP);
        XChangeProperty(display, window,
                       ATOM(_NET_WM_WINDOW_TYPE),
                       XA_ATOM, 32, PropModeReplace,
                       (unsigned char*)&type, 1);

        // Make sure properties are processed before showing
        XSync(display, False);
        
        // Now show the window
        m_quickView->show();
        
        return WallpaperExit::Code::Ok;
    }
    return WallpaperExit::Code::Invalid_Setup_Error;
}
void LinuxX11Window::setupWallpaperForOneScreen(int activeScreen)
{
}

void LinuxX11Window::setupWallpaperForAllScreens()
{
}

void LinuxX11Window::setupWallpaperForMultipleScreens(const QVector<int>& activeScreensList)
{
}

void LinuxX11Window::setVisible(bool show)
{
    m_quickView->setVisible(show);
}

void LinuxX11Window::destroyThis()
{
    QCoreApplication::quit();
}

void LinuxX11Window::terminate()
{
    QCoreApplication::quit();
}

}