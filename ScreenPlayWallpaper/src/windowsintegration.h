#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

// Do not change windows.h order !
#include <algorithm>
#include <functional>
#include <iostream>
#include <optional>
#include <vector>

// Do not sort !
#include "WinUser.h"
#include <ShellScalingApi.h>

struct Monitor {
    HMONITOR monitorID; // Handle to the monitor
    int index; // Index of the monitor
    RECT position; // Monitor's position and size
    SIZE size; // Monitor's width and height
    float scaleFactor; // Scale factor (DPI scaling as a factor, e.g., 1.5 for 150% scaling)

    void print() const
    {
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



struct Point {
    int x = 0;
    int y = 0;
};

struct sEnumInfo {
    int iIndex;
    HMONITOR hMonitor;
};

using MouseEventHandler = std::function<void(DWORD mouseButton, UINT type, POINT p)>;
static MouseEventHandler m_mouseEventHandler;
static HHOOK m_mouseHook;

typedef std::function<void(UINT vkCode, bool keyDown)> KeyboardEventHandler;
static HHOOK m_keyboardHook;
static KeyboardEventHandler m_keyboardEventHandler;

class WindowsIntegration {
public:
    struct SpanResult {
        int width = 0;
        int height = 0;
        bool success = false;
    };
    enum class MonitorResultStatus {
        Ok,
        WindowHandleInvalidError,
        WorkerWindowHandleInvalidError,
        MonitorIterationError,
    };
    struct MonitorResult {
        std::optional<Monitor> monitor;
        MonitorResultStatus status = MonitorResultStatus::Ok;
    };

    bool searchWorkerWindowToParentTo();
    float getScaling(const int monitorIndex) const;
    std::vector<Monitor> GetAllMonitors();
    int GetMonitorIndex(HMONITOR hMonitor);
    bool checkForFullScreenWindow(HWND windowHandle);
    WindowsIntegration::MonitorResult setupWallpaperForOneScreen(const int activeScreen, std::function<void(int, int)> updateWindowSize);
    SpanResult setupWallpaperForMultipleScreens(const std::vector<int>& activeScreens);
    SpanResult setupWallpaperForAllScreens();
    HWND windowHandle() const;
    HWND windowHandleWorker() const;
    void setWindowHandle(HWND windowHandle);
    void setupWindowMouseHook();
    void unhookMouse();
    void setMouseEventHandler(MouseEventHandler handler);
    static LRESULT __stdcall MouseHookCallback(int nCode, WPARAM wParam, LPARAM lParam);

    void setupWindowKeyboardHook();
    void unhookKeyboard();
    void setKeyboardEventHandler(KeyboardEventHandler handler);
    static LRESULT __stdcall KeyboardHookCallback(int nCode, WPARAM wParam, LPARAM lParam);

private:
    HWND m_windowHandle {};
    HWND m_windowHandleWorker {};
};
