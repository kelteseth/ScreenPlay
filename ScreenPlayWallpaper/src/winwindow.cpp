// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#include "winwindow.h"
#include "windowsintegration.h"
#include "ScreenPlayUtil/projectfile.h"
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


HHOOK g_mouseHook;
QPoint g_LastMousePosition { 0, 0 };
QPoint g_globalOffset { 0, 0 };
QQuickView* g_winGlobalHook = nullptr;

/*!
  \brief Windows mouse callback. This hook then forwards the event to the Qt window.
         We must manually call a release event otherwise QML gets stuck.
*/
LRESULT __stdcall MouseHookCallback(int nCode, WPARAM wParam, LPARAM lParam)
{

    Qt::MouseButton mouseButton {};
    Qt::MouseButtons mouseButtons {};
    Qt::KeyboardModifier keyboardModifier {};
    QMouseEvent::Type type { QMouseEvent::Type::MouseMove };

    if (nCode >= 0) {
        switch (wParam) {
        case WM_LBUTTONDOWN:
            mouseButton = Qt::MouseButton::LeftButton;
            mouseButtons.setFlag(Qt::LeftButton);
            type = QMouseEvent::Type::MouseButtonPress;
            break;
        case WM_LBUTTONUP:
            mouseButton = Qt::MouseButton::LeftButton;
            mouseButtons.setFlag(Qt::LeftButton);
            type = QMouseEvent::Type::MouseButtonRelease;
            break;
        case WM_RBUTTONDOWN:
            mouseButton = Qt::MouseButton::RightButton;
            mouseButtons.setFlag(Qt::RightButton);
            type = QMouseEvent::Type::MouseButtonPress;
            break;
        default:
            type = QMouseEvent::Type::MouseMove;
        }
    }

    POINT p {};
    if (GetCursorPos(&p)) {
        g_LastMousePosition.setX(p.x);
        g_LastMousePosition.setY(p.y);
    } else {
        return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
    }

    auto event = QMouseEvent(type, g_LastMousePosition, mouseButton, mouseButtons, keyboardModifier);

    QGuiApplication::sendEvent(g_winGlobalHook, &event);

    if (type == QMouseEvent::Type::MouseButtonPress) {
    }
    QTimer::singleShot(100, [&]() {
        // auto eventPress = QMouseEvent(QMouseEvent::Type::MouseButtonPress, g_LastMousePosition, mouseButton, mouseButtons, {});
        // qInfo() << mouseButton << QGuiApplication::sendEvent(g_winGlobalHook, &eventPress) << g_globalOffset.x() << g_globalOffset.y();
        auto eventRelease = QMouseEvent(QMouseEvent::Type::MouseButtonRelease, g_LastMousePosition, mouseButton, mouseButtons, {});
        QGuiApplication::sendEvent(g_winGlobalHook, &eventRelease);
    });

    return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
}

/*!
  \brief Setup the SetWindowsHookEx hook to be used to receive mouse events.
*/
void WinWindow::setupWindowMouseHook()
{
    using ScreenPlay::InstalledType::InstalledType;

    // MUST be called before setting hook for events!
    if (type() != InstalledType::VideoWallpaper && type() != InstalledType::GifWallpaper) {
        qInfo() << "Enable mousehook";
        g_winGlobalHook = &m_window;

        HINSTANCE hInstance = GetModuleHandle(NULL);
        if (!(g_mouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseHookCallback, hInstance, 0))) {
            qInfo() << "Faild to install mouse hook!";
            return;
        }
    }
}

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
    m_windowHandle = reinterpret_cast<HWND>(m_window.winId());
    if (!IsWindow(m_windowHandle)) {
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

    QTimer::singleShot(1000, this, [&]() {
        setupWindowMouseHook();
    });

    qInfo() << "Setup " << width() << height();
    m_window.setSource(QUrl("qrc:/qml/ScreenPlayWallpaper/qml/Wallpaper.qml"));
    m_window.show();
    return ScreenPlay::WallpaperExitCode::Ok;
}

/*!
  \brief Calls ShowWindow function to set the main window in/visible.
*/
void WinWindow::setVisible(bool show)
{
    if (show) {
        if (!ShowWindow(m_windowHandle, SW_SHOW)) {
            qDebug() << "Cannot set window handle SW_SHOW";
        }

    } else {
        if (!ShowWindow(m_windowHandle, SW_HIDE)) {
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
    WindowsIntegration windowsIntegration;
    auto updateWindowSize = [this](const int width, const int height){
        setWidth(width);
        setHeight(height);
        m_window.setWidth(width);
        m_window.setHeight(height);
    };
    std::optional<Monitor> monitor = windowsIntegration.setupWallpaperForOneScreen(activeScreen,m_windowHandle,m_windowHandleWorker,updateWindowSize);

}

/*!
  \brief This method is called if the user want to have one wallpaper on all windows.
*/
void WinWindow::setupWallpaperForAllScreens()
{
    WindowsIntegration windowsIntegration;
    WindowsIntegration::SpanResult span = windowsIntegration.setupWallpaperForAllScreens(m_windowHandle,m_windowHandleWorker);
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
    WindowsIntegration windowsIntegration;
    WindowsIntegration::SpanResult span = windowsIntegration.setupWallpaperForMultipleScreens(activeScreens,m_windowHandle,m_windowHandleWorker);
    setWidth(span.width);
    setHeight(span.height);
    m_window.setWidth(width());
    m_window.setHeight(height());
}

bool WinWindow::searchWorkerWindowToParentTo()
{

    HWND progman_hwnd = FindWindowW(L"Progman", L"Program Manager");
    const DWORD WM_SPAWN_WORKER = 0x052C;
    SendMessageTimeoutW(progman_hwnd, WM_SPAWN_WORKER, 0xD, 0x1, SMTO_NORMAL,
        10000, nullptr);

    return EnumWindows(SearchForWorkerWindow, reinterpret_cast<LPARAM>(&m_windowHandleWorker));
}

/*!
   \brief Sets the size and the parent to the worker handle to be displayed behind the
          desktop icons.
*/
void WinWindow::configureWindowGeometry()
{
    if (!searchWorkerWindowToParentTo()) {
        qFatal("No worker window found");
    }

    // WARNING: Setting Window flags must be called *here*!
    SetWindowLongPtr(m_windowHandle, GWL_EXSTYLE, WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT);
    SetWindowLongPtr(m_windowHandle, GWL_STYLE, WS_POPUP);

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
        UnhookWindowsHookEx(g_mouseHook);
    }
    ShowWindow(m_windowHandle, SW_HIDE);

    // Force refresh so that we display the regular
    // desktop wallpaper again
    ShowWindow(m_windowHandleWorker, SW_HIDE);
    ShowWindow(m_windowHandleWorker, SW_SHOW);

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

void WinWindow::checkForFullScreenWindow(){
    bool hasFullscreenWindow = WindowsIntegration().checkForFullScreenWindow(m_windowHandle);

    setVisualsPaused(hasFullscreenWindow);
}

#include "moc_winwindow.cpp"
