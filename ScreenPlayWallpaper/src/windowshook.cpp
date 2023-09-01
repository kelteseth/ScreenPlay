#include "windowshook.h"

BOOL CALLBACK WinMonitorStats::MonitorEnum(HMONITOR hMon, HDC hdc, LPRECT lprcMonitor, LPARAM pData)
{
    WinMonitorStats* pThis = reinterpret_cast<WinMonitorStats*>(pData);
    auto scaleFactor = DEVICE_SCALE_FACTOR::DEVICE_SCALE_FACTOR_INVALID;
    GetScaleFactorForMonitor(hMon, &scaleFactor);

    UINT x = 0;
    UINT y = 0;
    GetDpiForMonitor(hMon, MONITOR_DPI_TYPE::MDT_RAW_DPI, &x, &y);
    pThis->sizes.push_back({ x, y });
    pThis->scaleFactor.push_back(scaleFactor);
    pThis->hMonitors.push_back(hMon);
    pThis->hdcMonitors.push_back(hdc);
    pThis->rcMonitors.push_back(*lprcMonitor);
    pThis->iMonitors.push_back(static_cast<int>(pThis->hdcMonitors.size()));

    // qInfo() << std::abs(lprcMonitor->right - lprcMonitor->left) << std::abs(lprcMonitor->top - lprcMonitor->bottom);

    return TRUE;
}
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

bool WindowsHook::searchWorkerWindowToParentTo()
{

    HWND progman_hwnd = FindWindowW(L"Progman", L"Program Manager");
    const DWORD WM_SPAWN_WORKER = 0x052C;
    SendMessageTimeoutW(progman_hwnd, WM_SPAWN_WORKER, 0xD, 0x1, SMTO_NORMAL,
        10000, nullptr);

    return EnumWindows(SearchForWorkerWindow, reinterpret_cast<LPARAM>(&windowHandleWorker));
}

/*!
   \brief Returns scaling factor as reported by Windows.
*/
float WindowsHook::getScaling(const int monitorIndex) const
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
bool WindowsHook::hasWindowScaling() const
{
    auto enumMonitorCallback = [](HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData) -> BOOL {
        int scaling = GetDeviceCaps(hdcMonitor, LOGPIXELSX) / 96;
        if (scaling != 1) {
            *(bool*)dwData = true;
            return false; // Stop enumeration
        }
        return true; // Continue enumeration
    };

    bool hasScaling = false;
    EnumDisplayMonitors(NULL, NULL, enumMonitorCallback, (LPARAM)&hasScaling);

    return hasScaling;
}