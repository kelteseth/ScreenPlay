// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#include "ScreenPlayUtil/projectfile.h"
#include "winwindow.h"
#include <QtQml>
#include <QGuiApplication>
#include <algorithm>
#include <iostream>
#include <vector>

/*!
    \class WinWindow
    \inmodule ScreenPlayWallpaper
    \brief  ScreenPlayWindow used for the Windows implementation.
*/

/*!
  \brief Searches for the worker window for our window to parent to.
*/
BOOL WINAPI SearchForWorkerWindow(HWND hwnd, LPARAM lparam)
{
    // 0xXXXXXXX "" WorkerW
    //   ...
    //   0xXXXXXXX "" SHELLDLL_DefView
    //     0xXXXXXXXX "FolderView" SysListView32
    // 0xXXXXXXXX "" WorkerW                           <---- We want this one
    // 0xXXXXXXXX "Program Manager" Progman
    if (FindWindowExW(hwnd, nullptr, L"SHELLDLL_DefView", nullptr))
        *reinterpret_cast<HWND*>(lparam) = FindWindowExW(nullptr, hwnd, L"WorkerW", nullptr);
    return TRUE;
}

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

    configureWindowGeometry();

    // We do not support autopause for multi monitor wallpaper
    if (this->activeScreensList().length() == 1) {
        if (checkWallpaperVisible()) {
            m_checkForFullScreenWindowTimer.start(10);
        }
    }

    QTimer::singleShot(1000, this, [&]() {
        setupWindowMouseHook();
    });

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

struct sEnumInfo {
    int iIndex;
    HMONITOR hMonitor;
};

BOOL CALLBACK GetMonitorByIndex(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
    auto* info = (sEnumInfo*)dwData;
    if (--info->iIndex < 0) {
        info->hMonitor = hMonitor;
        return FALSE;
    }
    return TRUE;
}

/*!
  \brief This method is called if the user want to have one wallpaper on one window.
*/
void WinWindow::setupWallpaperForOneScreen(int activeScreen)
{

    const QRect screenRect = QGuiApplication::screens().at(activeScreen)->geometry();
    const int boderWidth = 2;
    const float scaling = getScaling(activeScreen);
    const int borderOffset = -1;

    ScreenPlayUtil::WinMonitorStats monitors;
    const int width = std::abs(monitors.rcMonitors[activeScreen].right - monitors.rcMonitors[activeScreen].left) + boderWidth;
    const int height = std::abs(monitors.rcMonitors[activeScreen].top - monitors.rcMonitors[activeScreen].bottom) + boderWidth;
    const int x = monitors.rcMonitors[activeScreen].left + m_zeroPoint.x() + borderOffset;
    const int y = monitors.rcMonitors[activeScreen].top + m_zeroPoint.y() + borderOffset;
    qInfo() << QString("Setup window activeScreen: %1 scaling: %2 x: %3 y: %4 width: %5 height: %6").arg(activeScreen).arg(scaling).arg(x).arg(y).arg(width).arg(height);
    // Also set it in BaseWindow. This is needed for Windows fade in.
    setWidth(width - boderWidth);
    setHeight(height - boderWidth);
    {
        // Must be called twice for some reason when window has scaling...
        if (!SetWindowPos(m_windowHandle, nullptr, x, y, width, height, SWP_HIDEWINDOW)) {
            qFatal("Could not set window pos");
        }
        if (!SetWindowPos(m_windowHandle, nullptr, x, y, width, height, SWP_HIDEWINDOW)) {
            qFatal("Could not set window pos");
        }
    }

    if (SetParent(m_windowHandle, m_windowHandleWorker) == nullptr) {
        qFatal("Could not attach to parent window");
    }
}

/*!
  \brief This method is called if the user want to have one wallpaper on all windows.
*/
void WinWindow::setupWallpaperForAllScreens()
{
    ScreenPlayUtil::WinMonitorStats monitors;
    QRect rect;
    for (int i = 0; i < monitors.iMonitors.size(); i++) {
        const int width = std::abs(monitors.rcMonitors[i].right - monitors.rcMonitors[i].left);
        const int height = std::abs(monitors.rcMonitors[i].top - monitors.rcMonitors[i].bottom);
        qInfo() << width << height;
        rect.setWidth(rect.width() + width);
        rect.setHeight(rect.height() + height);
    }
    int offsetX = 0;
    int offsetY = 0;
    for (int i = 0; i < monitors.iMonitors.size(); i++) {
        const int x = monitors.rcMonitors[i].left;
        const int y = monitors.rcMonitors[i].top;
        qInfo() << x << y;
        if (x < offsetX) {
            offsetX = x;
        }
        if (y < offsetY) {
            offsetY += y;
        }
    }
    if (!SetWindowPos(m_windowHandle, nullptr, offsetX, offsetY, rect.width(), rect.height(), SWP_NOSIZE | SWP_NOMOVE)) {
        qFatal("Could not set window pos: ");
    }
    if (!SetWindowPos(m_windowHandle, nullptr, offsetX, offsetY, rect.width(), rect.height(), SWP_NOSIZE | SWP_NOMOVE)) {
        qFatal("Could not set window pos: ");
    }
    if (SetParent(m_windowHandle, m_windowHandleWorker) == nullptr) {
        qFatal("Could not attach to parent window");
    }
    qInfo() << rect.width() << rect.height() << offsetX << offsetY;
    m_window.setHeight(rect.height());
    m_window.setWidth(rect.width());
    m_window.setY(offsetY);
    m_window.setX(offsetX + 1920);
    qInfo() << m_window.geometry();
}

/*!
  \brief This method is called if the user want to have one wallpaper on multiple windows.
*/
void WinWindow::setupWallpaperForMultipleScreens(const QVector<int>& activeScreensList)
{
    QRect rect;
    QScreen* upperLeftScreen { nullptr };
    // Check for the upper left screen first so we get x and y positions
    for (const int screen : activeScreensList) {
        QScreen* screenTmp = QGuiApplication::screens().at(screen);
        if (upperLeftScreen != nullptr) {
            if (screenTmp->geometry().x() < upperLeftScreen->geometry().x() || screenTmp->geometry().y() < upperLeftScreen->geometry().y()) {
                upperLeftScreen = screenTmp;
            }
        } else {
            upperLeftScreen = screenTmp;
        }
        rect.setWidth(screenTmp->geometry().width() + rect.width());
        rect.setHeight(screenTmp->geometry().height() + rect.height());
    }

    rect.setX(upperLeftScreen->geometry().x());
    rect.setY(upperLeftScreen->geometry().y());

    if (!SetWindowPos(m_windowHandle, nullptr, rect.x() + m_zeroPoint.x(), rect.y() + m_zeroPoint.y(), rect.width(), rect.height(), SWP_SHOWWINDOW)) {
        qFatal("Could not set window pos: ");
    }
    if (SetParent(m_windowHandle, m_windowHandleWorker) == nullptr) {
        qFatal("Could not attach to parent window");
    }
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
  \brief Reads the physicalDotsPerInch and mapps them to scaling factors.
         This is needed to detect if the user has different monitors with
         different scaling factors.

         screen->physicalDotsPerInch()
         100% -> 109  -> 1
         125% -> 120 -> 1.25
         150% -> 161 -> 1.5
         ...
*/
float WinWindow::getScaling(const int monitorIndex)
{
    QScreen* screen = QGuiApplication::screens().at(monitorIndex);
    const int factor = screen->physicalDotsPerInch();
    switch (factor) {
    case 72:
        return 1;
    case 107:
        return 1.5;
    case 109:
        return 1;
    case 161:
        return 1;
    default:
        qWarning() << "Monitor with factor: " << factor << " detected! This is not supported!";
        return 1;
    }
}
/*!
   \brief Returns true of at least one monitor has active scaling enabled.
*/
bool WinWindow::hasWindowScaling()
{
    const auto screens = QGuiApplication::screens();
    for (int i = 0; i < screens.count(); i++) {
        if (getScaling(i) != 1) {
            return true;
        }
    }
    return false;
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

    RECT rect {};
    if (!GetWindowRect(m_windowHandleWorker, &rect)) {
        qFatal("Unable to get WindoeRect from worker");
    }

    // Windows coordante system begins at 0x0 at the
    // main monitors upper left and not at the most left top monitor.
    // This can be easily read from the worker window.
    m_zeroPoint = { std::abs(rect.left), std::abs(rect.top) };
    g_globalOffset = m_zeroPoint;

    // WARNING: Setting Window flags must be called *here*!
    SetWindowLongPtr(m_windowHandle, GWL_EXSTYLE, WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT);
    SetWindowLongPtr(m_windowHandle, GWL_STYLE, WS_POPUPWINDOW);

    // Ether for one Screen or for all
    if ((QGuiApplication::screens().length() == activeScreensList().length()) && (activeScreensList().length() != 1)) {
        setupWallpaperForAllScreens();
    } else if (activeScreensList().length() == 1) {
        setupWallpaperForOneScreen(activeScreensList().at(0));
        setCanFade(true);
    } else if (activeScreensList().length() > 1) {
        setupWallpaperForMultipleScreens(activeScreensList());
    }

    // Instead of setting "renderType: Text.NativeRendering" every time  we can set it here once
    m_window.setTextRenderType(QQuickWindow::TextRenderType::NativeTextRendering);
    m_window.setResizeMode(QQuickView::ResizeMode::SizeRootObjectToView);
    m_window.setWidth(width());
    m_window.setHeight(height());
    qInfo() << "Setup " << width() << height();
    m_window.setSource(QUrl("qrc:/qml/ScreenPlayWallpaper/qml/Wallpaper.qml"));
    m_window.hide();
}

BOOL CALLBACK FindTheDesiredWnd(HWND hWnd, LPARAM lParam)
{
    DWORD dwStyle = (DWORD)GetWindowLong(hWnd, GWL_STYLE);
    if ((dwStyle & WS_MAXIMIZE) != 0) {
        *(reinterpret_cast<HWND*>(lParam)) = hWnd;
        return false; // stop enumerating
    }
    return true; // keep enumerating
}

BOOL CALLBACK GetMonitorByHandle(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
    Q_UNUSED(hdcMonitor)
    Q_UNUSED(lprcMonitor)

    auto info = (sEnumInfo*)dwData;
    if (info->hMonitor == hMonitor)
        return FALSE;
    ++info->iIndex;
    return TRUE;
}

int GetMonitorIndex(HMONITOR hMonitor)
{
    sEnumInfo info;
    info.hMonitor = hMonitor;

    if (EnumDisplayMonitors(NULL, NULL, GetMonitorByHandle, (LPARAM)&info))
        return -1;

    return info.iIndex;
}

/*!
  \brief This method is called via a fixed interval to detect if a window completely
         covers a monitor. If then sets visualPaused for QML to pause the content.
*/
void WinWindow::checkForFullScreenWindow()
{

    HWND hFoundWnd = nullptr;
    EnumWindows(&FindTheDesiredWnd, reinterpret_cast<LPARAM>(&hFoundWnd));

    // True if one window has WS_MAXIMIZE
    if (hFoundWnd != nullptr) {
        DWORD dwFlags = 0;
        HMONITOR monitor = MonitorFromWindow(hFoundWnd, dwFlags);
        HMONITOR wallpaper = MonitorFromWindow(m_windowHandle, dwFlags);
        int monitorIndex = GetMonitorIndex(monitor);
        int wallpaperIndex = GetMonitorIndex(wallpaper);
        // qDebug() << monitorIndex << wallpaperIndex;

        // If the window that has WS_MAXIMIZE is at the same monitor as this wallpaper
        if (monitorIndex == wallpaperIndex) {

            setVisualsPaused(true);
        } else {
            setVisualsPaused(false);
        }

    } else {
        setVisualsPaused(false);
    }
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

#include "moc_winwindow.cpp"
