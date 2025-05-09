// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
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

#include "ScreenPlayCore/windowsutils.h"

namespace ScreenPlay {
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
    WindowsIntegration();
    ~WindowsIntegration();

    // Prevent copying
    WindowsIntegration(const WindowsIntegration&) = delete;
    WindowsIntegration& operator=(const WindowsIntegration&) = delete;
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
        std::optional<WindowsMonitor> monitor;
        MonitorResultStatus status = MonitorResultStatus::Ok;
    };

    bool searchWorkerWindowToParentTo();
    float getScaling(const int monitorIndex) const;
    int GetMonitorIndex(HMONITOR hMonitor);
    bool checkForFullScreenWindow(HWND windowHandle);
    WindowsIntegration::MonitorResult setupWallpaperForOneScreen(const int activeScreen, std::function<void(int, int)> updateWindowSize);
    SpanResult setupWallpaperForMultipleScreens(const std::vector<int>& activeScreens);
    SpanResult setupWallpaperForAllScreens();
    HWND windowHandle() const;
    HWND windowHandleWorker() const;
    void setWindowHandle(HWND windowHandle);
    void exit();
    void setupWindowMouseHook();
    void setupWindowKeyboardHook();
    // Unhook methods
    void unhookMouse();
    void unhookKeyboard();
    void unhookAll();

    void setKeyboardEventHandler(KeyboardEventHandler handler);
    void setMouseEventHandler(MouseEventHandler handler);

    // Status check methods
    bool isMouseHookInstalled() const { return m_mouseHook != NULL; }
    bool isKeyboardHookInstalled() const { return m_keyboardHook != NULL; }

private:
    void processMouseEvent(WPARAM wParam, LPARAM lParam);
    void processKeyboardEvent(WPARAM wParam, LPARAM lParam);

    static LRESULT __stdcall KeyboardHookCallback(int nCode, WPARAM wParam, LPARAM lParam);
    static LRESULT __stdcall MouseHookCallback(int nCode, WPARAM wParam, LPARAM lParam);

private:
    static WindowsIntegration* instance;
    HWND m_windowHandle {};
    HWND m_windowHandleWorker {};
    // Hook handles
    HHOOK m_mouseHook;
    HHOOK m_keyboardHook;
    // Event handlers
    MouseEventHandler m_mouseEventHandler;
    KeyboardEventHandler m_keyboardEventHandler;
};
}
