#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

// Do not change windows.h order !
#include <algorithm>
#include <format>
#include <functional>
#include <iostream>
#include <optional>
#include <vector>

// Do not sort !
#include "WinUser.h"
#include <ShellScalingApi.h>

namespace ScreenPlay {
struct WindowsMonitor {
    HMONITOR monitorID; // Handle to the monitor
    int index; // Index of the monitor
    RECT position; // Monitor's position and size
    SIZE size; // Monitor's width and height
    float scaleFactor; // Scale factor (DPI scaling as a factor, e.g., 1.5 for 150% scaling)
    std::string name; // Monitor name/model
    std::string toString() const
    {
        return std::format(
            "Monitor Info:\n"
            "Monitor ID: {}\n"
            "Index: {}\n"
            "Name: {}\n"
            "Position: ({}, {}, {}, {})\n"
            "Size: ({}x{})\n"
            "Scale Factor: {}\n",
            (int64_t)monitorID, index, name, position.left, position.top,
            position.right, position.bottom, size.cx, size.cy, scaleFactor);
    }
    void print() const
    {
        std::cout << toString() << std::endl;
    }
};

class WindowsUtils {
public:
    std::vector<WindowsMonitor> getAllMonitors();
};
}