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

#define WIDTH 512
#define HEIGHT 384
#define OPAQUE 0xffffffff
#define NAME "xwinwrap"
#define ATOM(a) XInternAtom(display, #a, False)

namespace ScreenPlay {
WallpaperExit::Code LinuxX11Window::start()
{

    if (!debugMode()) {
        connect(m_sdk.get(), &ScreenPlaySDK::sdkDisconnected, this, &LinuxX11Window::destroyThis);
    }

    qmlRegisterSingletonInstance<LinuxX11Window>("ScreenPlayWallpaper", 1, 0, "Wallpaper", this);

    auto* screen = QGuiApplication::screens().at(0);
    m_window.setGeometry(screen->geometry());
    Window window = m_window.winId();

    Display* display = XOpenDisplay("");

    XSetWindowAttributes attrs = { ParentRelative, 0L, 0, 0L, 0, 0,
        Always, 0L, 0L, False, StructureNotifyMask | ExposureMask | ButtonPressMask | ButtonReleaseMask, 0L, False, 0, 0 };

    XWMHints wmHint;
    Atom xa;
    int flags;
    flags |= CWBorderPixel | CWColormap;

    XChangeWindowAttributes(display, window, flags, &attrs);

    wmHint.flags = InputHint | StateHint;
    wmHint.input = true;
    wmHint.initial_state = WithdrawnState;

    XSetWMProperties(display, window, NULL, NULL, NULL,
        0, NULL, &wmHint, NULL);

    xa = ATOM(_NET_WM_WINDOW_TYPE);

    Atom prop;
    xa = ATOM(_MOTIF_WM_HINTS);
    if (xa != None) {
        long prop[5] = { 2, 0, 0, 0, 0 };
        XChangeProperty(display, window, xa, xa, 32,
            PropModeReplace, (unsigned char*)prop, 5);
    }
    xa = ATOM(_NET_WM_STATE);
    if (xa != None) {
        Atom xa_prop = ATOM(_NET_WM_STATE_BELOW);

        XChangeProperty(display, window, xa, XA_ATOM, 32,
            PropModeAppend, (unsigned char*)&xa_prop, 1);
    }
    xa = ATOM(_NET_WM_DESKTOP);
    if (xa != None) {
        CARD32 xa_prop = 0xFFFFFFFF;

        XChangeProperty(display, window, xa, XA_CARDINAL, 32,
            PropModeAppend, (unsigned char*)&xa_prop, 1);
    }

    xa = ATOM(_NET_WM_STATE);
    if (xa != None) {
        Atom xa_prop = ATOM(_NET_WM_STATE_STICKY);

        XChangeProperty(display, window, xa, XA_ATOM, 32,
            PropModeAppend, (unsigned char*)&xa_prop, 1);
    }

    xa = ATOM(_NET_WM_STATE);
    if (xa != None) {
        Atom xa_prop = ATOM(_NET_WM_STATE_SKIP_TASKBAR);

        XChangeProperty(display, window, xa, XA_ATOM, 32,
            PropModeAppend, (unsigned char*)&xa_prop, 1);
    }
    xa = ATOM(_NET_WM_STATE);
    if (xa != None) {
        Atom xa_prop = ATOM(_NET_WM_STATE_SKIP_PAGER);

        XChangeProperty(display, window, xa, XA_ATOM, 32,
            PropModeAppend, (unsigned char*)&xa_prop, 1);
    }

    XMapWindow(display, window);

    XSync(display, window);
    QDir workingDir(QGuiApplication::instance()->applicationDirPath());
    m_window.engine()->addImportPath(workingDir.path() + "/qml");
    m_window.setResizeMode(QQuickView::ResizeMode::SizeRootObjectToView);
    m_window.setSource(QUrl("qrc:/qml/ScreenPlayWallpaper/qml/Wallpaper.qml"));
    m_window.show();
    return WallpaperExit::Code::Ok;
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
    m_window.setVisible(show);
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