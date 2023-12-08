#include "windowsintegration.h"

/*!
   \brief Returns scaling factor as reported by Windows.
*/
float WindowsIntegration::getScaling(const int monitorIndex) const
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

bool WindowsIntegration::searchWorkerWindowToParentTo()
{
    HWND progman_hwnd = FindWindowW(L"Progman", L"Program Manager");
    const DWORD WM_SPAWN_WORKER = 0x052C;
    SendMessageTimeoutW(progman_hwnd, WM_SPAWN_WORKER, 0xD, 0x1, SMTO_NORMAL,
        10000, nullptr);

    return EnumWindows(SearchForWorkerWindow, reinterpret_cast<LPARAM>(&m_windowHandleWorker));
}
/*!
  \brief Searches for the worker window for our window to parent to.
*/
BOOL SearchForWorkerWindow(HWND hwnd, LPARAM lparam)
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

std::vector<Monitor> WindowsIntegration::getAllMonitors()
{
    std::vector<Monitor> monitors;

    // Use the static MonitorEnumProc callback for EnumDisplayMonitors
    EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, reinterpret_cast<LPARAM>(&monitors));

    return monitors;
}

int WindowsIntegration::GetMonitorIndex(HMONITOR hMonitor)
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
bool WindowsIntegration::checkForFullScreenWindow(HWND windowHandle)
{

    HWND hFoundWnd = nullptr;
    EnumWindows(&FindTheDesiredWnd, reinterpret_cast<LPARAM>(&hFoundWnd));

    // True if one window has WS_MAXIMIZE
    if (hFoundWnd != nullptr) {
        DWORD dwFlags = 0;
        HMONITOR monitor = MonitorFromWindow(hFoundWnd, dwFlags);
        HMONITOR wallpaper = MonitorFromWindow(windowHandle, dwFlags);
        int monitorIndex = GetMonitorIndex(monitor);
        int wallpaperIndex = GetMonitorIndex(wallpaper);
        // qDebug() << monitorIndex << wallpaperIndex;

        // If the window that has WS_MAXIMIZE is at the same monitor as this wallpaper
        return monitorIndex == wallpaperIndex;

    } else {
        return false;
    }
}
/*
 * Adjusting a Window's Position and Size for Different Monitor DPI Scale Factors:
 *      * Windows allows users to set different DPI (dots per inch) scale factors for each monitor. This DPI scaling can lead to
 * discrepancies in the positioning and size of windows, especially if we want to place a window on a monitor with a different
 * scale factor than the one it was originally on.
 *      * In our scenario, we want to move and resize a window (`m_windowHandle`) to fit perfectly within a target monitor. However,
 * both the window and the target monitor can have different DPI scale factors, so we need to account for these when calculating
 * the window's new position and size.
 *      * Steps:
 *      * 1. Retrieve the DPI scale factor for the window:
 *    - This gives us the current scale factor of the window based on its original monitor.
 *      * 2. Retrieve the DPI scale factor for the target monitor:
 *    - This gives us the scale factor of the monitor where we want to place the window.
 *      * 3. Calculate the window's new position:
 *    - The new position should be relative to the `WorkerW` window's coordinates.
 *    - Adjust the position based on the ratio of the window's DPI scale factor to the target monitor's DPI scale factor.
 *      This ensures that the window is positioned correctly on the monitor regardless of any differences in scale factors.
 *      * 4. Calculate the window's new size:
 *    - Adjust the size of the window based on the ratio of the window's DPI scale factor to the target monitor's DPI scale factor.
 *      This ensures that the window fits perfectly within the monitor, taking into account any differences in scale factors.
 *      * By following this approach, we can accurately position and resize the window on any monitor, regardless of differences in DPI
 * scale factors.
 */

WindowsIntegration::MonitorResult WindowsIntegration::setupWallpaperForOneScreen(const int activeScreen, std::function<void(int, int)> updateWindowSize)
{
    if (!IsWindow(m_windowHandle)) {
        std::cout << "Could not get a valid window handle !";
        return { std::nullopt, MonitorResultStatus::WindowHandleInvalidError };
    }

    if (!IsWindow(m_windowHandleWorker)) {
        std::cout << "Could not get a valid window handle wroker!";
        return { std::nullopt, MonitorResultStatus::WorkerWindowHandleInvalidError };
    }

    std::vector<Monitor> monitors = getAllMonitors();
    for (const auto& monitor : monitors) {
        monitor.print();
        if (monitor.index != activeScreen)
            continue;

        SetWindowPos(m_windowHandle, HWND_TOP,
            monitor.position.left, monitor.position.top,
            monitor.size.cx, monitor.size.cy,
            SWP_NOZORDER | SWP_NOACTIVATE);

        // Must be called here to fix window positions!
        updateWindowSize(monitor.size.cx, monitor.size.cy);

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
        return { monitor, MonitorResultStatus::Ok };
    }
    return { std::nullopt, MonitorResultStatus::MonitorIterationError };
}
/**
 *  Spans the window across multiple monitors.
 *  * This function takes a vector of monitor indices and adjusts the window's
 * size and position to span across the specified monitors. It determines the
 * window's new size by finding the bounding rectangle that covers all selected
 * monitors. The window's new position is set to the top-left corner of this
 * bounding rectangle.
 *  * Note: This function assumes that all monitors have the same DPI scaling. If
 * they don't, the window may not fit perfectly across the monitors.
 *  * @param activeScreens A vector containing the indices of monitors across
 * which the window should span.
 *  * Usage:
 * setupWallpaperForMultipleScreens({0, 1}); // Spans the window across monitors 0 and 1.
 *  * Internals:
 * 1. For each monitor in `activeScreens`, determine its bounding rectangle.
 * 2. Compute the window's new width as `rightmost - leftmost` and its new
 *    height as `bottommost - topmost`.
 * 3. Adjust the window's position and size to fit this bounding rectangle.
 */
WindowsIntegration::SpanResult WindowsIntegration::setupWallpaperForMultipleScreens(const std::vector<int>& activeScreens)
{
    std::vector<Monitor> monitors = getAllMonitors();

    int leftmost = INT_MAX;
    int topmost = INT_MAX;
    int rightmost = INT_MIN;
    int bottommost = INT_MIN;

    for (const auto& monitorIndex : activeScreens) {
        if (monitorIndex < monitors.size()) {
            const Monitor& monitor = monitors[monitorIndex];
            leftmost = (std::min)(leftmost, static_cast<int>(monitor.position.left));
            topmost = (std::min)(topmost, static_cast<int>(monitor.position.top));
            rightmost = (std::max)(rightmost, static_cast<int>(monitor.position.right));
            bottommost = (std::max)(bottommost, static_cast<int>(monitor.position.bottom));
        }
    }

    int newWidth = rightmost - leftmost;
    int newHeight = bottommost - topmost;

    RECT oldRect;
    GetWindowRect(m_windowHandle, &oldRect);

    float windowDpiScaleFactor = static_cast<float>(GetDpiForWindow(m_windowHandle)) / 96.0f;

    SetParent(m_windowHandle, m_windowHandleWorker);
    RECT parentRect;
    GetWindowRect(m_windowHandleWorker, &parentRect);

    int newX = static_cast<int>((leftmost - parentRect.left) * windowDpiScaleFactor);
    int newY = static_cast<int>((topmost - parentRect.top) * windowDpiScaleFactor);

    SetWindowPos(m_windowHandle, NULL, newX, newY, newWidth, newHeight, SWP_NOZORDER | SWP_NOACTIVATE);
    SpanResult result;
    result.width = rightmost - leftmost;
    result.height = bottommost - topmost;
    result.success = true;

    return result;
}
/**
 *  Sets up the wallpaper to span across all connected screens.
 *      * This function retrieves information about all connected monitors, including their positions, dimensions, and scale factors.
 * It then calculates the combined dimensions needed to span the window across all these monitors, taking into account
 * the scale factors. The function also handles reparenting the window to the WorkerW window, ensuring it remains below
 * other desktop icons. The calculated position and dimensions are adjusted to account for any potential scaling differences
 * between the window and the combined monitor setup.
 *      * @note This function assumes that the window's DPI scale factor is based on a default of 96 DPI. Adjustments are made
 *       based on this assumption.
 *      * @note The function currently multiplies the scale factors of all monitors to get an overall scale factor. This may need
 *       adjustments based on specific application needs.
 *      * @return SpanResult A structure containing the combined width and height of the monitors, and a success flag indicating
 *                    whether the operation was successful.
 *  * @retval SpanResult::width The combined width of all monitors after scaling.
 * @retval SpanResult::height The combined height of all monitors after scaling.
 * @retval SpanResult::success A boolean flag indicating the success of the operation. Currently, it always returns `true`
 *                             assuming all operations are successful. This can be adjusted based on error checks as needed.
 */
WindowsIntegration::SpanResult WindowsIntegration::setupWallpaperForAllScreens()
{
    std::vector<Monitor> monitors = getAllMonitors();

    int leftmost = INT_MAX;
    int topmost = INT_MAX;
    int rightmost = INT_MIN;
    int bottommost = INT_MIN;
    float overallScaleFactor = 1.0f; // assuming default is no scaling

    // Calculate the combined dimensions of all monitors
    for (const auto& monitor : monitors) {
        leftmost = (std::min)(leftmost, static_cast<int>(monitor.position.left));
        topmost = (std::min)(topmost, static_cast<int>(monitor.position.top));
        rightmost = (std::max)(rightmost, static_cast<int>(monitor.position.right));
        bottommost = (std::max)(bottommost, static_cast<int>(monitor.position.bottom));
        overallScaleFactor *= monitor.scaleFactor; // Adjust as per your scaling needs
    }

    int scaledWidth = static_cast<int>((rightmost - leftmost) * overallScaleFactor);
    int scaledHeight = static_cast<int>((bottommost - topmost) * overallScaleFactor);

    // Set the position and size of the window to span all monitors
    SetWindowPos(m_windowHandle, HWND_TOP, leftmost, topmost, scaledWidth, scaledHeight, SWP_NOZORDER | SWP_NOACTIVATE);

    // Reparenting and scaling logic
    RECT oldRect;
    GetWindowRect(m_windowHandle, &oldRect);
    float windowDpiScaleFactor = static_cast<float>(GetDpiForWindow(m_windowHandle)) / 96.0f;
    SetParent(m_windowHandle, m_windowHandleWorker);
    RECT parentRect;
    GetWindowRect(m_windowHandleWorker, &parentRect);
    int newX = static_cast<int>((oldRect.left - parentRect.left) * (windowDpiScaleFactor / overallScaleFactor));
    int newY = static_cast<int>((oldRect.top - parentRect.top) * (windowDpiScaleFactor / overallScaleFactor));

    SetWindowPos(m_windowHandle, NULL, newX, newY, scaledWidth, scaledHeight, SWP_NOZORDER | SWP_NOACTIVATE);

    // Return the combined span of all monitors
    SpanResult result;
    result.width = scaledWidth;
    result.height = scaledHeight;
    result.success = true; // Assuming the operations are successful; adjust as needed

    return result;
}

HWND WindowsIntegration::windowHandle() const
{
    return m_windowHandle;
}

HWND WindowsIntegration::windowHandleWorker() const
{
    return m_windowHandleWorker;
}

void WindowsIntegration::setWindowHandle(HWND windowHandle)
{
    m_windowHandle = windowHandle;
}

BOOL GetMonitorByHandle(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{

    auto info = (sEnumInfo*)dwData;
    if (info->hMonitor == hMonitor)
        return FALSE;
    ++info->iIndex;
    return TRUE;
}

BOOL FindTheDesiredWnd(HWND hWnd, LPARAM lParam)
{
    DWORD dwStyle = (DWORD)GetWindowLong(hWnd, GWL_STYLE);
    if ((dwStyle & WS_MAXIMIZE) != 0) {
        *(reinterpret_cast<HWND*>(lParam)) = hWnd;
        return false; // stop enumerating
    }
    return true; // keep enumerating
}

BOOL MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
    std::vector<Monitor>* pMonitors = reinterpret_cast<std::vector<Monitor>*>(dwData);

    MONITORINFOEX info;
    info.cbSize = sizeof(info);
    GetMonitorInfo(hMonitor, &info);

    UINT dpiX, dpiY;
    GetDpiForMonitor(hMonitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY);

    Monitor monitor;
    monitor.monitorID = hMonitor;
    monitor.index = pMonitors->size(); // Set index based on the current size of the vector
    monitor.position = info.rcMonitor;
    monitor.size.cx = info.rcMonitor.right - info.rcMonitor.left;
    monitor.size.cy = info.rcMonitor.bottom - info.rcMonitor.top;
    monitor.scaleFactor = static_cast<float>(dpiX) / 96.0f;

    pMonitors->push_back(monitor);

    return true;
}

void WindowsIntegration::setMouseEventHandler(MouseEventHandler handler)
{
    m_mouseEventHandler = handler;
}

LRESULT __stdcall WindowsIntegration::MouseHookCallback(int nCode, WPARAM wParam, LPARAM lParam)
{
    // Initialize mouse event variables
    DWORD mouseButton = 0;
    UINT type = WM_MOUSEMOVE;
    POINT p {};

    if (nCode >= 0) {
        switch (wParam) {
        case WM_LBUTTONDOWN:
            mouseButton = MK_LBUTTON;
            type = WM_LBUTTONDOWN;
            break;
        case WM_LBUTTONUP:
            mouseButton = MK_LBUTTON;
            type = WM_LBUTTONUP;
            break;
        case WM_RBUTTONDOWN:
            mouseButton = MK_RBUTTON;
            type = WM_RBUTTONDOWN;
            break;
        default:
            type = WM_MOUSEMOVE;
        }
    }

    if (GetCursorPos(&p)) {
        // Check if the callback function is set and call it
        if (m_mouseEventHandler) {
            m_mouseEventHandler(mouseButton, type, p);
        }

    } else {
        return CallNextHookEx(m_mouseHook, nCode, wParam, lParam);
    }

    return CallNextHookEx(m_mouseHook, nCode, wParam, lParam);
}

void WindowsIntegration::setupWindowMouseHook()
{
    HINSTANCE hInstance = GetModuleHandle(NULL);
    if (!(m_mouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseHookCallback, hInstance, 0))) {
        OutputDebugStringW(L"Failed to install mouse hook!");
        return;
    }
}

void WindowsIntegration::setupWindowKeyboardHook()
{
    HINSTANCE hInstance = GetModuleHandle(NULL);
    if (!(m_keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardHookCallback, hInstance, 0))) {
        OutputDebugStringW(L"Failed to install keyboard hook!");
        return;
    }
}

void WindowsIntegration::setKeyboardEventHandler(KeyboardEventHandler handler)
{
    m_keyboardEventHandler = std::move(handler);
}

LRESULT __stdcall WindowsIntegration::KeyboardHookCallback(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode >= 0) {
        KBDLLHOOKSTRUCT* kbStruct = (KBDLLHOOKSTRUCT*)lParam;
        bool keyDown = wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN;
        // Check if the callback function is set and call it
        if (m_keyboardEventHandler) {
            m_keyboardEventHandler(kbStruct->vkCode, keyDown);
        }
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

void WindowsIntegration::unhookMouse()
{
    if (m_mouseHook != NULL) {
        UnhookWindowsHookEx(m_mouseHook);
        m_mouseHook = NULL;
    }
}

void WindowsIntegration::unhookKeyboard()
{
    if (m_keyboardHook != NULL) {
        UnhookWindowsHookEx(m_keyboardHook);
        m_keyboardHook = NULL;
    }
}