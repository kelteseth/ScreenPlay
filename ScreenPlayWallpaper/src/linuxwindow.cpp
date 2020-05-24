#include "linuxwindow.h"
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xproto.h>
#include <X11/Xutil.h>

#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define WIDTH 512
#define HEIGHT 384

#define OPAQUE 0xffffffff

#define NAME "xwinwrap"

#define ATOM(a) XInternAtom(display, #a, False)

/*
 * 
 * 
 * 
 * This code is non functional only here for testing! 
 * 
 * 
 * 
 */

LinuxWindow::LinuxWindow(QVector<int> activeScreensList, QString projectPath, QString id, QString volume, const QString fillmode, const bool checkWallpaperVisible, QObject* parent)
    : BaseWindow(projectPath, activeScreensList, checkWallpaperVisible)
{
    m_window.setWidth(1920);
    m_window.setHeight(1080);

    m_window.show();

    setAppID(id);

    bool ok = false;
    float volumeParsed = volume.toFloat(&ok);
    if (!ok) {
        qFatal("Could not parse volume");
    }

    setVolume(volumeParsed);
    setFillMode(fillmode);

    // Ether for one Screen or for all
    if ((QApplication::screens().length() == activeScreensList.length()) && (activeScreensList.length() != 1)) {
        setupWallpaperForAllScreens();
    } else if (activeScreensList.length() == 1) {
        setupWallpaperForOneScreen(activeScreensList.at(0));
        setCanFade(true);
    } else if (activeScreensList.length() > 1) {
        setupWallpaperForMultipleScreens(activeScreensList);
    }

    setWidth(m_window.width());
    setHeight(m_window.height());

    m_window.setResizeMode(QQuickView::ResizeMode::SizeRootObjectToView);
    m_window.rootContext()->setContextProperty("window", this);
    //m_window.rootContext()->setContextProperty("desktopProperties", &m_windowsDesktopProperties);
    // Instead of setting "renderType: Text.NativeRendering" every time
    // we can set it here once :)

    m_window.setTextRenderType(QQuickWindow::TextRenderType::NativeTextRendering);
    m_window.setSource(QUrl("qrc:/mainWindow.qml"));

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

    /*QObject::connect(&m_checkForFullScreenWindowTimer, &QTimer::timeout, this, &WinWindow::checkForFullScreenWindow);

    if (checkWallpaperVisible) {
        m_checkForFullScreenWindowTimer.start(10);
    }

    QTimer::singleShot(1000, [this]() {
        setupWindowMouseHook();
    });*/

    /*********/

    /*bool ok = false;
    float volumeParsed = volume.toFloat(&ok);
    if (!ok) {
        qFatal("Could not parse volume");
    }
    setVolume(volumeParsed);

    // WARNING: Setting Window flags must be called *here*!
    Qt::WindowFlags flags = m_window.flags();
    m_window.setFlags(flags | Qt::FramelessWindowHint | Qt::Desktop);

    m_window.setResizeMode(QQuickView::ResizeMode::SizeRootObjectToView);
    m_window.rootContext()->setContextProperty("window", this);
    // Instead of setting "renderType: Text.NativeRendering" every time
    // we can set it here once :)
    m_window.setTextRenderType(QQuickWindow::TextRenderType::NativeTextRendering);
    m_window.setSource(QUrl("qrc:/Wallpaper.qml"));*/
}

void LinuxWindow::setupWallpaperForOneScreen(int activeScreen)
{
}

void LinuxWindow::setupWallpaperForAllScreens()
{
}

void LinuxWindow::setupWallpaperForMultipleScreens(const QVector<int>& activeScreensList)
{
}

void LinuxWindow::setVisible(bool show)
{
    m_window.setVisible(show);
}

void LinuxWindow::destroyThis()
{
    QCoreApplication::quit();
}
