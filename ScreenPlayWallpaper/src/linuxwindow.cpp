#include "linuxwindow.h"
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/Xproto.h>

#include <X11/extensions/shape.h>
//#include <X11/extensions/Xrender.h>


#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>

#define WIDTH  512
#define HEIGHT 384

#define OPAQUE 0xffffffff

#define NAME "xwinwrap"

#define ATOM(a) XInternAtom(display, #a, False)

LinuxWindow::LinuxWindow(QVector<int> activeScreensList, QString projectPath, QString id, QString volume, const QString fillmode,const bool checkWallpaperVisible, QObject* parent)
    : BaseWindow(projectPath,activeScreensList,checkWallpaperVisible)
{
    m_window.setWidth(1920);
    m_window.setHeight(1080);



    /*********/
    m_window.show();
    //m_window.hide();

    setAppID(id);

    bool ok = false;
    float volumeParsed = volume.toFloat(&ok);
    if (!ok) {
        qFatal("Could not parse volume");
    }

    setVolume(volumeParsed);
    setFillMode(fillmode);

    // WARNING: Setting Window flags must be called *here*!
    //SetWindowLongPtr(m_windowHandle, GWL_EXSTYLE, WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT);
    //SetWindowLongPtr(m_windowHandle, GWL_STYLE, WS_POPUPWINDOW);

    //Calculate window coordinates
    calcOffsets();

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

    Display *display = XOpenDisplay("");

    XSetWindowAttributes attrs = { ParentRelative, 0L, 0, 0L, 0, 0,
                                          Always, 0L, 0L, False, StructureNotifyMask | ExposureMask |
                                          ButtonPressMask | ButtonReleaseMask, 0L, False, 0, 0
                                        };



           XWMHints wmHint;
           Atom xa;

           int flags;

           //attrs.colormap = window.colourmap;
           flags |= CWBorderPixel | CWColormap;

           XChangeWindowAttributes(display,window,flags,&attrs);
           /*window.window = XCreateWindow(display, window.root, window.x,
                                         window.y, window.width, window.height, 0, depth, InputOutput, visual,
                                         flags, &attrs);*/

           wmHint.flags = InputHint | StateHint;
           // wmHint.input = undecorated ? False : True;
           wmHint.input = true;
           wmHint.initial_state = WithdrawnState;

           XSetWMProperties(display, window, NULL, NULL, NULL,
                            0, NULL, &wmHint, NULL);

           xa = ATOM(_NET_WM_WINDOW_TYPE);

           Atom prop;

           //prop = ATOM(_NET_WM_WINDOW_TYPE_DESKTOP);


          // XChangeProperty(display, window, xa, XA_ATOM, 32,
          //                 PropModeReplace, (unsigned char *) &prop, 1);

           //if (undecorated) {
               xa = ATOM(_MOTIF_WM_HINTS);
               if (xa != None) {
                   long prop[5] = { 2, 0, 0, 0, 0 };
                   XChangeProperty(display, window, xa, xa, 32,
                                   PropModeReplace, (unsigned char *) prop, 5);
               }
           //}

           /* Below other windows */
           //if (below) {

               /*xa = ATOM(_WIN_LAYER);
               if (xa != None) {
                   long prop = 0;

                   XChangeProperty(display, window, xa, XA_CARDINAL, 32,
                                   PropModeAppend, (unsigned char *) &prop, 1);
               }*/

               xa = ATOM(_NET_WM_STATE);
               if (xa != None) {
                   Atom xa_prop = ATOM(_NET_WM_STATE_BELOW);

                   XChangeProperty(display, window, xa, XA_ATOM, 32,
                                   PropModeAppend, (unsigned char *) &xa_prop, 1);
               }
          //}


           /* Sticky */
           //if (sticky) {

               xa = ATOM(_NET_WM_DESKTOP);
               if (xa != None) {
                   CARD32 xa_prop = 0xFFFFFFFF;

                   XChangeProperty(display, window, xa, XA_CARDINAL, 32,
                                   PropModeAppend, (unsigned char *) &xa_prop, 1);
               }

               xa = ATOM(_NET_WM_STATE);
               if (xa != None) {
                   Atom xa_prop = ATOM(_NET_WM_STATE_STICKY);

                   XChangeProperty(display, window, xa, XA_ATOM, 32,
                                   PropModeAppend, (unsigned char *) &xa_prop, 1);
               }
           //}

           /* Skip taskbar */
           //if (skip_taskbar) {

               xa = ATOM(_NET_WM_STATE);
               if (xa != None) {
                   Atom xa_prop = ATOM(_NET_WM_STATE_SKIP_TASKBAR);

                   XChangeProperty(display, window, xa, XA_ATOM, 32,
                                   PropModeAppend, (unsigned char *) &xa_prop, 1);
               }
           //}

           /* Skip pager */
           //if (skip_pager) {

               xa = ATOM(_NET_WM_STATE);
               if (xa != None) {
                   Atom xa_prop = ATOM(_NET_WM_STATE_SKIP_PAGER);

                   XChangeProperty(display, window, xa, XA_ATOM, 32,
                                   PropModeAppend, (unsigned char *) &xa_prop, 1);
               }
           //}


       //if (opacity != OPAQUE)
       //    setWindowOpacity (opacity);

      /* if (noInput)
       {
           Region region;

           region = XCreateRegion ();
           if (region)
           {
               XShapeCombineRegion (display, window.window, ShapeInput, 0, 0, region, ShapeSet);
               XDestroyRegion (region);
           }
       }*/



      XMapWindow(display, window);

      XSync (display, window);


     /*while (1)
     {
       XEvent event;

       XNextEvent(display, &event);
     }

     XUnmapWindow(display, win);
     XCloseDisplay(display);*/



    //m_window.hide();

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
    m_window.setSource(QUrl("qrc:/mainWindow.qml"));*/
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

void LinuxWindow::calcOffsets()
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

