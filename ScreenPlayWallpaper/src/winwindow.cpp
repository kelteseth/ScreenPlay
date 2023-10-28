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

/*
 * Adjusting a Window's Position and Size for Different Monitor DPI Scale Factors:
 *
 * Windows allows users to set different DPI (dots per inch) scale factors for each monitor. This DPI scaling can lead to
 * discrepancies in the positioning and size of windows, especially if we want to place a window on a monitor with a different
 * scale factor than the one it was originally on.
 *
 * In our scenario, we want to move and resize a window (`m_windowHandle`) to fit perfectly within a target monitor. However,
 * both the window and the target monitor can have different DPI scale factors, so we need to account for these when calculating
 * the window's new position and size.
 *
 * Steps:
 *
 * 1. Retrieve the DPI scale factor for the window:
 *    - This gives us the current scale factor of the window based on its original monitor.
 *
 * 2. Retrieve the DPI scale factor for the target monitor:
 *    - This gives us the scale factor of the monitor where we want to place the window.
 *
 * 3. Calculate the window's new position:
 *    - The new position should be relative to the `WorkerW` window's coordinates.
 *    - Adjust the position based on the ratio of the window's DPI scale factor to the target monitor's DPI scale factor.
 *      This ensures that the window is positioned correctly on the monitor regardless of any differences in scale factors.
 *
 * 4. Calculate the window's new size:
 *    - Adjust the size of the window based on the ratio of the window's DPI scale factor to the target monitor's DPI scale factor.
 *      This ensures that the window fits perfectly within the monitor, taking into account any differences in scale factors.
 *
 * By following this approach, we can accurately position and resize the window on any monitor, regardless of differences in DPI
 * scale factors.
 */

void WinWindow::setupWallpaperForOneScreen(int activeScreen)
{
    std::vector<Monitor> monitors = GetAllMonitors();
    for (const auto& monitor : monitors) {
        monitor.print();
        if (monitor.index != activeScreen)
            continue;

        SetWindowPos(m_windowHandle, HWND_TOP,
            monitor.position.left, monitor.position.top,
            monitor.size.cx, monitor.size.cy,
            SWP_NOZORDER | SWP_NOACTIVATE);
        setWidth(monitor.size.cx);
        setHeight(monitor.size.cy);
        m_window.setWidth(width());
        m_window.setHeight(height());

        RECT oldRect;
        GetWindowRect(m_windowHandle, &oldRect);
        std::cout << "Old Window Position: (" << oldRect.left << ", " << oldRect.top << ")" << std::endl;

        float windowDpiScaleFactor = static_cast<float>(GetDpiForWindow(m_windowHandle)) / 96.0f;
        float targetMonitorDpiScaleFactor = monitor.scaleFactor;
        std::cout << "Window DPI Scale Factor: " << windowDpiScaleFactor << std::endl;
        std::cout << "Target Monitor DPI Scale Factor: " << targetMonitorDpiScaleFactor << std::endl;

        SetParent(m_windowHandle, m_windowHandleWorker);
        RECT parentRect;
        GetWindowRect(m_windowHandleWorker, &parentRect);
        std::cout << "WorkerW Window Position: (" << parentRect.left << ", " << parentRect.top << ")" << std::endl;

        int newX = static_cast<int>((oldRect.left - parentRect.left) * (windowDpiScaleFactor / targetMonitorDpiScaleFactor));
        int newY = static_cast<int>((oldRect.top - parentRect.top) * (windowDpiScaleFactor / targetMonitorDpiScaleFactor));
        std::cout << "Calculated New Position: (" << newX << ", " << newY << ")" << std::endl;

        int newWidth = static_cast<int>(monitor.size.cx * (windowDpiScaleFactor / targetMonitorDpiScaleFactor));
        int newHeight = static_cast<int>(monitor.size.cy * (windowDpiScaleFactor / targetMonitorDpiScaleFactor));
        std::cout << "Calculated New Size: (" << newWidth << "x" << newHeight << ")" << std::endl;

        SetWindowPos(m_windowHandle, NULL, newX, newY, newWidth, newHeight, SWP_NOZORDER | SWP_NOACTIVATE);

    }
    return;
}

/*!
  \brief This method is called if the user want to have one wallpaper on all windows.
*/
void WinWindow::setupWallpaperForAllScreens()
{
    WinMonitorStats monitors;
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
   \brief Returns scaling factor as reported by Windows.
*/
float WinWindow::getScaling(const int monitorIndex) const
{
    // Get all monitors
    int monitorCount = GetSystemMetrics(SM_CMONITORS);

    if (monitorIndex < 0 || monitorIndex >= monitorCount) {
        // Invalid monitor index
        return 1.0f;
    }

    DISPLAY_DEVICE displayDevice;
    ZeroMemory(&displayDevice, sizeof(displayDevice));
    displayDevice.cb = sizeof(displayDevice);

    // Enumerate through monitors until we find the one we're looking for
    for (int i = 0; EnumDisplayDevices(NULL, i, &displayDevice, 0); i++) {
        if (i == monitorIndex) {
            DEVMODE devMode;
            ZeroMemory(&devMode, sizeof(devMode));
            devMode.dmSize = sizeof(devMode);

            // Get settings for selected monitor
            if (!EnumDisplaySettings(displayDevice.DeviceName, ENUM_CURRENT_SETTINGS, &devMode)) {
                // Unable to get monitor settings
                return 1.0f;
            }

            // Get DPI for selected monitor
            HMONITOR hMonitor = MonitorFromPoint({ devMode.dmPosition.x, devMode.dmPosition.y }, MONITOR_DEFAULTTONEAREST);
            UINT dpiX = 0, dpiY = 0;
            if (SUCCEEDED(GetDpiForMonitor(hMonitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY))) {
                return (float)dpiX / 96.0f; // Standard DPI is 96
            }
        }
    }

    // If we reach here, it means we couldn't find the monitor with the given index or couldn't get the DPI.
    return 1.0f;
}

/*!
   \brief Returns true of at least one monitor has active scaling enabled.
*/
bool WinWindow::hasWindowScaling() const
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
    // m_zeroPoint = { std::abs(rect.left), std::abs(rect.top) };
    // g_globalOffset = m_zeroPoint;

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
