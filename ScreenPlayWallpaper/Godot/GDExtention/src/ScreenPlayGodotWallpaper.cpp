// Copied from godot-cpp/test/src and modified.

#include "ScreenPlayGodotWallpaper.h"
#include "godot_cpp/classes/display_server.hpp"
#include "godot_cpp/classes/global_constants.hpp"
#include "godot_cpp/classes/label.hpp"
#include "godot_cpp/classes/os.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/utility_functions.hpp"

//// ScreenPlayWallpaper

int ScreenPlayGodotWallpaper::sInstanceCount = 0;
int ScreenPlayGodotWallpaper::sLastID = 0;

void ScreenPlayGodotWallpaper::_bind_methods()
{
    godot::ClassDB::bind_method(godot::D_METHOD("init"), &ScreenPlayGodotWallpaper::init);
    godot::ClassDB::bind_method(godot::D_METHOD("create_named_pipe"), &ScreenPlayGodotWallpaper::create_named_pipe);
}

void ScreenPlayGodotWallpaper::hideFromTaskbar(HWND hwnd)
{
    LONG lExStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
    lExStyle |= WS_EX_TOOLWINDOW; // Add WS_EX_TOOLWINDOW
    lExStyle &= ~WS_EX_APPWINDOW; // Remove WS_EX_APPWINDOW
    SetWindowLong(hwnd, GWL_EXSTYLE, lExStyle);
}

ScreenPlayGodotWallpaper::ScreenPlayGodotWallpaper()
{
    mID = ++sLastID;
    sInstanceCount++;

    godot::UtilityFunctions::print(
        "ScreenPlayWallpaper ", godot::itos(mID),
        " created, current instance count: ", godot::itos(sInstanceCount));
}

ScreenPlayGodotWallpaper::~ScreenPlayGodotWallpaper()
{
    sInstanceCount--;
    godot::UtilityFunctions::print(
        "ScreenPlayWallpaper ", godot::itos(mID),
        " destroyed, current instance count: ", godot::itos(sInstanceCount));

    // Somehow this gets called at editor startup
    // so just return if not initialized
    if (m_hook) {

        ShowWindow(m_hook->windowHandle, SW_HIDE);

        // Force refresh so that we display the regular
        // desktop wallpaper again
        ShowWindow(m_hook->windowHandleWorker, SW_HIDE);
        ShowWindow(m_hook->windowHandleWorker, SW_SHOW);
    }
    // Destructor
    if (hPipe != INVALID_HANDLE_VALUE) {
        CloseHandle(hPipe);
    }
}

void ScreenPlayGodotWallpaper::_process(double delta)
{

    if (!isPipeActive) {
        return;
    }
    timesinceLastRead += delta;
    // 0.05 seconds = 50ms
    if (timesinceLastRead >= 0.05) {
        godot::String data = read_from_pipe();
        if (!data.is_empty()) {
            godot::UtilityFunctions::print("Received data: " + data);
        }
        timesinceLastRead = 0.0;
    }
}

bool ScreenPlayGodotWallpaper::configureWindowGeometry()
{
    if (!m_hook->searchWorkerWindowToParentTo()) {
        godot::UtilityFunctions::print("No worker window found");
        return false;
    }

    RECT rect {};
    if (!GetWindowRect(m_hook->windowHandleWorker, &rect)) {
        godot::UtilityFunctions::print("Unable to get WindoeRect from worker");
        return false;
    }

    // Windows coordante system begins at 0x0 at the
    // main monitors upper left and not at the most left top monitor.
    // This can be easily read from the worker window.
    m_hook->zeroPoint = { std::abs(rect.left), std::abs(rect.top) };

    // WARNING: Setting Window flags must be called *here*!
    SetWindowLongPtr(m_hook->windowHandle, GWL_EXSTYLE, WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT);
    SetWindowLongPtr(m_hook->windowHandle, GWL_STYLE, WS_POPUPWINDOW);
    return true;
}

bool ScreenPlayGodotWallpaper::init(int activeScreen)
{
    auto* displayServer = godot::DisplayServer::get_singleton();
    int64_t handle_int = displayServer->window_get_native_handle(godot::DisplayServer::HandleType::WINDOW_HANDLE, activeScreen);
    HWND hwnd = reinterpret_cast<HWND>(static_cast<intptr_t>(handle_int));
    m_hook = std::make_unique<WindowsHook>();
    m_hook->windowHandle = hwnd;
    hideFromTaskbar(hwnd);
    if (!configureWindowGeometry()) {
        return false;
    }
    ShowWindow(m_hook->windowHandle, SW_HIDE);

    const int borderWidth = 2;
    const float scaling = m_hook->getScaling(activeScreen); // Assuming getScaling is your own function
    const int borderOffset = -1;

    WinMonitorStats monitors; // Assuming this is your own function
    const int width = static_cast<int>(std::abs(monitors.rcMonitors[activeScreen].right - monitors.rcMonitors[activeScreen].left) / scaling) + borderWidth;
    const int height = static_cast<int>(std::abs(monitors.rcMonitors[activeScreen].top - monitors.rcMonitors[activeScreen].bottom) / scaling) + borderWidth;

    const int x = monitors.rcMonitors[activeScreen].left + m_hook->zeroPoint.x + borderOffset; // Assuming m_zeroPoint is a POINT struct
    const int y = monitors.rcMonitors[activeScreen].top + m_hook->zeroPoint.y + borderOffset;

    godot::String output = "Setup window activeScreen: " + godot::itos(activeScreen) + " scaling: " + godot::rtos(scaling) + " x: " + godot::itos(x) + " y: " + godot::itos(y) + " width: " + godot::itos(width) + " height: " + godot::itos(height);
    godot::UtilityFunctions::print(output);

    // Must be called twice for some reason when window has scaling...
    if (!SetWindowPos(m_hook->windowHandle, nullptr, x, y, width, height, SWP_HIDEWINDOW)) {
        godot::UtilityFunctions::print("Could not set window pos");
        return false;
    }
    if (!SetWindowPos(m_hook->windowHandle, nullptr, x, y, width, height, SWP_HIDEWINDOW)) {
        godot::UtilityFunctions::print("Could not set window pos 2");
        return false;
    }

    if (SetParent(m_hook->windowHandle, m_hook->windowHandleWorker) == nullptr) {
        godot::UtilityFunctions::print("Could not attach to parent window");
        return false;
    }
    displayServer->window_set_size(godot::Vector2((real_t)width, (real_t)height));
    ShowWindow(m_hook->windowHandle, SW_SHOW);
    return true;
}

bool ScreenPlayGodotWallpaper::create_named_pipe(godot::String pipeName)
{
    godot::String fullPipeName = "\\\\.\\pipe\\" + pipeName;
    std::wstring wPipeName = std::wstring(fullPipeName.wide_string());

    hPipe = CreateNamedPipeW(
        wPipeName.c_str(),
        PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
        1,
        1024 * 16,
        1024 * 16,
        NMPWAIT_USE_DEFAULT_WAIT,
        NULL);

    if (hPipe == INVALID_HANDLE_VALUE) {
        isPipeActive = false;
        godot::UtilityFunctions::print("CreateNamedPipe failed, error code: " + godot::String::num_int64(GetLastError()));
        return false;
    }

    if (ConnectNamedPipe(hPipe, NULL) == FALSE) {
        CloseHandle(hPipe);
        godot::UtilityFunctions::print("ConnectNamedPipe failed, error code: " + godot::String::num_int64(GetLastError()));
        isPipeActive = false;
        return false;
    }
    isPipeActive = true;
    return true;
}

godot::String ScreenPlayGodotWallpaper::read_from_pipe()
{
    char buffer[128];
    DWORD bytesRead;
    godot::String result;

    if (ReadFile(hPipe, buffer, sizeof(buffer) - 1, &bytesRead, NULL)) {
        buffer[bytesRead] = '\0';
        result = godot::String(buffer);
    } else {
        godot::UtilityFunctions::print("ReadFile from pipe failed, error code: " + godot::String::num_int64(GetLastError()));
    }

    return result;
}
