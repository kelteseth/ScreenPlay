#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>
#include <algorithm>
#include <iostream>
#include <shellscalingapi.h>
#include <vector>

struct WinMonitorStats {

    WinMonitorStats()
    {
        EnumDisplayMonitors(NULL, NULL, MonitorEnum, (LPARAM)this);
    }

    static BOOL CALLBACK MonitorEnum(HMONITOR hMon, HDC hdc, LPRECT lprcMonitor, LPARAM pData);
    std::vector<int> iMonitors;
    std::vector<HMONITOR> hMonitors;
    std::vector<HDC> hdcMonitors;
    std::vector<RECT> rcMonitors;
    std::vector<DEVICE_SCALE_FACTOR> scaleFactor;
    std::vector<std::pair<UINT, UINT>> sizes;
    int index = 0;
};

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