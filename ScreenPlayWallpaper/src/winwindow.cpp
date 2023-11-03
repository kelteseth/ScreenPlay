// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#include "winwindow.h"
#include "windowsintegration.h"
#include <QGuiApplication>
#include <QtQml>
#include <algorithm>
#include <iostream>
#include <shellscalingapi.h>
#include <vector>
#include <windows.h>

/*!
    \class WinWindow
    \inmodule ScreenPlayWallpaper
    \brief  ScreenPlayWindow used for the Windows implementation.
*/

ScreenPlay::WallpaperExitCode WinWindow::start()
{
    connect(
        &m_window, &QQuickView::statusChanged, this, [this](auto status) {
            if (status == QQuickView::Status::Error) {
                destroyThis();
            }
        },
        Qt::QueuedConnection);
    auto* guiAppInst = dynamic_cast<QGuiApplication*>(QGuiApplication::instance());
    if (!debugMode()) {
        connect(m_sdk.get(), &ScreenPlaySDK::sdkDisconnected, this, &WinWindow::destroyThis);
    }
    connect(guiAppInst, &QGuiApplication::screenAdded, this, &WinWindow::configureWindowGeometry);
    connect(guiAppInst, &QGuiApplication::screenRemoved, this, &WinWindow::configureWindowGeometry);
    connect(guiAppInst, &QGuiApplication::primaryScreenChanged, this, &WinWindow::configureWindowGeometry);
    connect(this, &BaseWindow::reloadQML, this, &WinWindow::clearComponentCache);
    connect(&m_checkForFullScreenWindowTimer, &QTimer::timeout, this, &WinWindow::checkForFullScreenWindow);

    const auto screens = QGuiApplication::screens();
    for (const auto& screen : screens) {
        connect(screen, &QScreen::geometryChanged, this, &WinWindow::configureWindowGeometry);
    }

    m_windowsDesktopProperties = std::make_unique<WindowsDesktopProperties>();
    m_windowsIntegration.setWindowHandle(reinterpret_cast<HWND>(m_window.winId()));
    if (!IsWindow(m_windowsIntegration.windowHandle())) {
        qCritical("Could not get a valid window handle!");
        return ScreenPlay::WallpaperExitCode::Invalid_Start_Windows_HandleError;
    }
    qRegisterMetaType<WindowsDesktopProperties*>();
    qRegisterMetaType<WinWindow*>();
    qmlRegisterSingletonInstance<WinWindow>("ScreenPlayWallpaper", 1, 0, "Wallpaper", this);

    m_window.setResizeMode(QQuickView::ResizeMode::SizeRootObjectToView);
    configureWindowGeometry();

    // We do not support autopause for multi monitor wallpaper and
    // wallpaper than contain audio, see BaseWindow::setup().
    if (activeScreensList().length() == 1) {
        if (checkWallpaperVisible()) {
            m_checkForFullScreenWindowTimer.start(10);
        }
    }

    qInfo() << "Setup " << width() << height();
    m_window.setSource(QUrl("qrc:/qml/ScreenPlayWallpaper/qml/Wallpaper.qml"));
    m_window.show();

    QTimer::singleShot(1000, this, [&]() {
        m_windowsIntegration.setupWindowMouseHook();
        // Set up the mouse event handler
        m_windowsIntegration.setMouseEventHandler([this](DWORD mouseButton, UINT type, POINT p) {
            Qt::MouseButton button = Qt::NoButton;
            QEvent::Type eventType = QEvent::None;

            // Determine the Qt mouse event type and button
            switch (type) {
            case WM_LBUTTONDOWN:
                eventType = QEvent::MouseButtonPress;
                button = Qt::LeftButton;
                break;
            case WM_LBUTTONUP:
                eventType = QEvent::MouseButtonRelease;
                button = Qt::LeftButton;
                break;
            case WM_RBUTTONDOWN:
                eventType = QEvent::MouseButtonPress;
                button = Qt::RightButton;
                break;
            case WM_RBUTTONUP:
                eventType = QEvent::MouseButtonRelease;
                button = Qt::RightButton;
                break;
            case WM_MOUSEMOVE:
                eventType = QEvent::MouseMove;
                button = Qt::NoButton;
                break;
                // Add more cases as needed
            }

            // Convert POINT to global position
            QPoint globalPos(p.x, p.y);

            // Convert global position to local position within the target widget
            QPoint localPos = m_window.mapFromGlobal(globalPos);

            // Create the QMouseEvent
            QMouseEvent* qEvent = new QMouseEvent(eventType, localPos, globalPos, button, button, QGuiApplication::keyboardModifiers());

            // Handle the mouse event
            // For example, logging the mouse button and position
            qDebug() << "Mouse event: Button=" << mouseButton << ", Type=" << type
                     << ", Position=(" << p.x << ", " << p.y << ")" << globalPos << localPos << button;
            // Post the event to the target widget
            QCoreApplication::postEvent(&m_window, qEvent);
        });
    });
    return ScreenPlay::WallpaperExitCode::Ok;
}

/*!
  \brief Calls ShowWindow function to set the main window in/visible.
*/
void WinWindow::setVisible(bool show)
{
    if (show) {
        if (!ShowWindow(m_windowsIntegration.windowHandle(), SW_SHOW)) {
            qDebug() << "Cannot set window handle SW_SHOW";
        }

    } else {
        if (!ShowWindow(m_windowsIntegration.windowHandle(), SW_HIDE)) {
            qDebug() << "Cannot set window handle SW_HIDE";
        }
    }
}

/*!
  \brief This function fires a qmlExit() signal to the UI for it to handle
         nice fade out animation first. Then the UI is responsible for calling
         WinWindow::terminate().
*/
void WinWindow::destroyThis()
{
    emit qmlExit();
}

void WinWindow::setupWallpaperForOneScreen(int activeScreen)
{
    auto updateWindowSize = [this](const int width, const int height) {
        setWidth(width);
        setHeight(height);
        m_window.setWidth(width);
        m_window.setHeight(height);
    };
    std::optional<Monitor> monitor = m_windowsIntegration.setupWallpaperForOneScreen(activeScreen, updateWindowSize);
}

/*!
  \brief This method is called if the user want to have one wallpaper on all windows.
*/
void WinWindow::setupWallpaperForAllScreens()
{
    WindowsIntegration::SpanResult span = m_windowsIntegration.setupWallpaperForAllScreens();
    setWidth(span.width);
    setHeight(span.height);
    m_window.setWidth(width());
    m_window.setHeight(height());
}

/*!
  \brief This method is called if the user want to have one wallpaper on multiple windows.
*/
void WinWindow::setupWallpaperForMultipleScreens(const QVector<int>& activeScreensList)
{
    std::vector<int> activeScreens(activeScreensList.begin(), activeScreensList.end());
    WindowsIntegration::SpanResult span = m_windowsIntegration.setupWallpaperForMultipleScreens(activeScreens);
    setWidth(span.width);
    setHeight(span.height);
    m_window.setWidth(width());
    m_window.setHeight(height());
}

/*!
   \brief Sets the size and the parent to the worker handle to be displayed behind the
          desktop icons.
*/
void WinWindow::configureWindowGeometry()
{
    if (!m_windowsIntegration.searchWorkerWindowToParentTo()) {
        qFatal("No worker window found");
    }
    if (!IsWindow(m_windowsIntegration.windowHandleWorker())) {
        qCritical("Could not get a valid window handle wroker!");
        return;
    }

    // WARNING: Setting Window flags must be called *here*!
    SetWindowLongPtr(m_windowsIntegration.windowHandle(), GWL_EXSTYLE, WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT);
    SetWindowLongPtr(m_windowsIntegration.windowHandle(), GWL_STYLE, WS_POPUP);

    // Ether for one Screen or for all
    if ((QGuiApplication::screens().length() == activeScreensList().length()) && (activeScreensList().length() != 1)) {
        setupWallpaperForAllScreens();
    } else if (activeScreensList().length() == 1) {
        setupWallpaperForOneScreen(activeScreensList().at(0));
        setCanFade(true);
    } else if (activeScreensList().length() > 1) {
        setupWallpaperForMultipleScreens(activeScreensList());
    }
    m_window.show();
}

/*!
  \brief Custom exit method to force a redraw of the window so that
         the default desktop wallpaper can be seen agian.
*/
void WinWindow::terminate()
{
    using ScreenPlay::InstalledType::InstalledType;
    if (type() != InstalledType::VideoWallpaper && type() != InstalledType::GifWallpaper) {
        // UnhookWindowsHookEx(g_mouseHook);
    }
    ShowWindow(m_windowsIntegration.windowHandle(), SW_HIDE);

    // Force refresh so that we display the regular
    // desktop wallpaper again
    ShowWindow(m_windowsIntegration.windowHandleWorker(), SW_HIDE);
    ShowWindow(m_windowsIntegration.windowHandleWorker(), SW_SHOW);

    QGuiApplication::quit();
}

/*!
  \brief Call the qml engine clearComponentCache. This function is used for
         refreshing wallpaper when the content has changed. For example this
         is needed for live editing when the content is chached.
*/
void WinWindow::clearComponentCache()
{
    m_window.engine()->clearComponentCache();
}

void WinWindow::checkForFullScreenWindow()
{
    bool hasFullscreenWindow = m_windowsIntegration.checkForFullScreenWindow(m_windowsIntegration.windowHandle());

    setVisualsPaused(hasFullscreenWindow);
}

#include "moc_winwindow.cpp"
