#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <algorithm>
#include <iostream>
#include <shellscalingapi.h>
#include <vector>

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
static BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData) {
    std::vector<Monitor>* pMonitors = reinterpret_cast<std::vector<Monitor>*>(dwData);

    MONITORINFOEX  info;
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


std::vector<Monitor> GetAllMonitors();


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

struct ResizeResult {
    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;
    bool success = false;
};
ResizeResult ResizeWindowToMonitor(HWND hwnd, int monitorIndex);

struct Point {
    int x = 0;
    int y = 0;
};

struct WindowsHook {
    bool searchWorkerWindowToParentTo();
    float getScaling(const int monitorIndex) const;
    bool hasWindowScaling() const;
    HWND windowHandle {};
    HWND windowHandleWorker {};
    Point zeroPoint;
};

class WindowsIntegration {
public:
    WindowsIntegration();
};
