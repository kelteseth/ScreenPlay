#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <algorithm>
#include <iostream>
#include <shellscalingapi.h>
#include <vector>
#include <optional>

// Do not sort !
#include "WinUser.h"
#include <ShellScalingApi.h>

struct Monitor {
    HMONITOR monitorID;        // Handle to the monitor
    int index;                 // Index of the monitor
    RECT position;             // Monitor's position and size
    SIZE size;                 // Monitor's width and height
    float scaleFactor;        // Scale factor (DPI scaling as a factor, e.g., 1.5 for 150% scaling)

    void print() const {
        std::cout << "Monitor Info:" << std::endl;
        std::cout << "Monitor ID: " << monitorID << std::endl;
        std::cout << "Index: " << index << std::endl;
        std::cout << "Position: (" << position.left << ", " << position.top << ", "
                  << position.right << ", " << position.bottom << ")" << std::endl;
        std::cout << "Size: (" << size.cx << "x" << size.cy << ")" << std::endl;
        std::cout << "Scale Factor: " << scaleFactor << std::endl;
    }
};

// Static callback function for EnumDisplayMonitors
BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData);
BOOL CALLBACK GetMonitorByHandle(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData);
BOOL CALLBACK FindTheDesiredWnd(HWND hWnd, LPARAM lParam);
BOOL WINAPI SearchForWorkerWindow(HWND hwnd, LPARAM lparam);

struct WinMonitorStats {


    WinMonitorStats()
    {
        EnumDisplayMonitors(NULL, NULL, MonitorEnum, (LPARAM)this);
    }


   static  BOOL CALLBACK MonitorEnum(HMONITOR hMon, HDC hdc, LPRECT lprcMonitor, LPARAM pData)
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
        pThis->iMonitors.push_back(pThis->hdcMonitors.size());

        return TRUE;
    }
    std::vector<int> iMonitors;
    std::vector<HMONITOR> hMonitors;
    std::vector<HDC> hdcMonitors;
    std::vector<RECT> rcMonitors;
    std::vector<DEVICE_SCALE_FACTOR> scaleFactor;
    std::vector<std::pair<UINT, UINT>> sizes;
};

struct Point {
    int x = 0;
    int y = 0;
};

struct sEnumInfo {
    int iIndex;
    HMONITOR hMonitor;
};


class WindowsIntegration {
public:

    bool searchWorkerWindowToParentTo();
    float getScaling(const int monitorIndex) const;
    bool hasWindowScaling() const;
    HWND windowHandle {};
    HWND windowHandleWorker {};
    Point zeroPoint;
    std::vector<Monitor> GetAllMonitors();



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
    bool checkForFullScreenWindow(HWND windowHandle)
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

    // Define the result structure
    struct SpanResult {
        int width = 0;
        int height = 0;
        bool success = false;
    };

    /*
     * Adjusting a Window's Position and Size for Different Monitor DPI Scale Factors:
     *      * Windows allows users to set different DPI (dots per inch) scale factors for each monitor. This DPI scaling can lead to
     * discrepancies in the positioning and size of windows, especially if we want to place a window on a monitor with a different
     * scale factor than the one it was originally on.
     *      * In our scenario, we want to move and resize a window (`windowHwnd`) to fit perfectly within a target monitor. However,
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

    std::optional<Monitor> setupWallpaperForOneScreen(const int activeScreen, HWND windowHwnd, HWND parentWindowHwnd)
    {
        std::vector<Monitor> monitors = GetAllMonitors();
        for (const auto& monitor : monitors) {
            monitor.print();
            if (monitor.index != activeScreen)
                continue;

            SetWindowPos(windowHwnd, HWND_TOP,
                monitor.position.left, monitor.position.top,
                monitor.size.cx, monitor.size.cy,
                SWP_NOZORDER | SWP_NOACTIVATE);

            RECT oldRect;
            GetWindowRect(windowHwnd, &oldRect);
            std::cout << "Old Window Position: (" << oldRect.left << ", " << oldRect.top << ")" << std::endl;

            float windowDpiScaleFactor = static_cast<float>(GetDpiForWindow(windowHwnd)) / 96.0f;
            float targetMonitorDpiScaleFactor = monitor.scaleFactor;
            std::cout << "Window DPI Scale Factor: " << windowDpiScaleFactor << std::endl;
            std::cout << "Target Monitor DPI Scale Factor: " << targetMonitorDpiScaleFactor << std::endl;

            SetParent(windowHwnd, parentWindowHwnd);
            RECT parentRect;
            GetWindowRect(parentWindowHwnd, &parentRect);
            std::cout << "WorkerW Window Position: (" << parentRect.left << ", " << parentRect.top << ")" << std::endl;

            int newX = static_cast<int>((oldRect.left - parentRect.left) * (windowDpiScaleFactor / targetMonitorDpiScaleFactor));
            int newY = static_cast<int>((oldRect.top - parentRect.top) * (windowDpiScaleFactor / targetMonitorDpiScaleFactor));
            std::cout << "Calculated New Position: (" << newX << ", " << newY << ")" << std::endl;

            int newWidth = static_cast<int>(monitor.size.cx * (windowDpiScaleFactor / targetMonitorDpiScaleFactor));
            int newHeight = static_cast<int>(monitor.size.cy * (windowDpiScaleFactor / targetMonitorDpiScaleFactor));
            std::cout << "Calculated New Size: (" << newWidth << "x" << newHeight << ")" << std::endl;

            SetWindowPos(windowHwnd, NULL, newX, newY, newWidth, newHeight, SWP_NOZORDER | SWP_NOACTIVATE);
            return {monitor};
        }
        return std::nullopt;
    }
    /**
     *  Spans the window across multiple monitors.
     *
     * This function takes a vector of monitor indices and adjusts the window's
     * size and position to span across the specified monitors. It determines the
     * window's new size by finding the bounding rectangle that covers all selected
     * monitors. The window's new position is set to the top-left corner of this
     * bounding rectangle.
     *
     * Note: This function assumes that all monitors have the same DPI scaling. If
     * they don't, the window may not fit perfectly across the monitors.
     *
     * @param activeScreens A vector containing the indices of monitors across
     * which the window should span.
     *
     * Usage:
     * setupWallpaperForMultipleScreens({0, 1}); // Spans the window across monitors 0 and 1.
     *
     * Internals:
     * 1. For each monitor in `activeScreens`, determine its bounding rectangle.
     * 2. Compute the window's new width as `rightmost - leftmost` and its new
     *    height as `bottommost - topmost`.
     * 3. Adjust the window's position and size to fit this bounding rectangle.
     */
    SpanResult setupWallpaperForMultipleScreens(const std::vector<int>& activeScreens, HWND windowHwnd, HWND parentWindowHwnd)
    {
        std::vector<Monitor> monitors = GetAllMonitors();

        int leftmost = INT_MAX;
        int topmost = INT_MAX;
        int rightmost = INT_MIN;
        int bottommost = INT_MIN;

        for (const auto& monitorIndex : activeScreens) {
            if (monitorIndex < monitors.size()) {
                const Monitor& monitor = monitors[monitorIndex];
                leftmost = std::min(leftmost, static_cast<int>(monitor.position.left));
                topmost = std::min(topmost, static_cast<int>(monitor.position.top));
                rightmost = std::max(rightmost, static_cast<int>(monitor.position.right));
                bottommost = std::max(bottommost, static_cast<int>(monitor.position.bottom));
            }
        }


        int newWidth = rightmost - leftmost;
        int newHeight = bottommost - topmost;

        RECT oldRect;
        GetWindowRect(windowHwnd, &oldRect);

        float windowDpiScaleFactor = static_cast<float>(GetDpiForWindow(windowHwnd)) / 96.0f;

        SetParent(windowHwnd, parentWindowHwnd);
        RECT parentRect;
        GetWindowRect(parentWindowHwnd, &parentRect);

        int newX = static_cast<int>((leftmost - parentRect.left) * windowDpiScaleFactor);
        int newY = static_cast<int>((topmost - parentRect.top) * windowDpiScaleFactor);

        SetWindowPos(windowHwnd, NULL, newX, newY, newWidth, newHeight, SWP_NOZORDER | SWP_NOACTIVATE);
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
     *
     * @retval SpanResult::width The combined width of all monitors after scaling.
     * @retval SpanResult::height The combined height of all monitors after scaling.
     * @retval SpanResult::success A boolean flag indicating the success of the operation. Currently, it always returns `true`
     *                             assuming all operations are successful. This can be adjusted based on error checks as needed.
     */
    SpanResult setupWallpaperForAllScreens( HWND windowHwnd, HWND parentWindowHwnd) {
        std::vector<Monitor> monitors = GetAllMonitors();

        int leftmost = INT_MAX;
        int topmost = INT_MAX;
        int rightmost = INT_MIN;
        int bottommost = INT_MIN;
        float overallScaleFactor = 1.0f;  // assuming default is no scaling

               // Calculate the combined dimensions of all monitors
        for (const auto& monitor : monitors) {
            leftmost = std::min(leftmost, static_cast<int>(monitor.position.left));
            topmost = std::min(topmost, static_cast<int>(monitor.position.top));
            rightmost = std::max(rightmost, static_cast<int>(monitor.position.right));
            bottommost = std::max(bottommost, static_cast<int>(monitor.position.bottom));
            overallScaleFactor *= monitor.scaleFactor;  // Adjust as per your scaling needs
        }

        int scaledWidth = static_cast<int>((rightmost - leftmost) * overallScaleFactor);
        int scaledHeight = static_cast<int>((bottommost - topmost) * overallScaleFactor);

               // Set the position and size of the window to span all monitors
        SetWindowPos(windowHwnd, HWND_TOP, leftmost, topmost, scaledWidth, scaledHeight, SWP_NOZORDER | SWP_NOACTIVATE);

               // Reparenting and scaling logic
        RECT oldRect;
        GetWindowRect(windowHwnd, &oldRect);
        float windowDpiScaleFactor = static_cast<float>(GetDpiForWindow(windowHwnd)) / 96.0f;
        SetParent(windowHwnd, parentWindowHwnd);
        RECT parentRect;
        GetWindowRect(parentWindowHwnd, &parentRect);
        int newX = static_cast<int>((oldRect.left - parentRect.left) * (windowDpiScaleFactor / overallScaleFactor));
        int newY = static_cast<int>((oldRect.top - parentRect.top) * (windowDpiScaleFactor / overallScaleFactor));

        SetWindowPos(windowHwnd, NULL, newX, newY, scaledWidth, scaledHeight, SWP_NOZORDER | SWP_NOACTIVATE);

        // Return the combined span of all monitors
        SpanResult result;
        result.width = scaledWidth;
        result.height = scaledHeight;
        result.success = true;  // Assuming the operations are successful; adjust as needed

        return result;
    }


};
