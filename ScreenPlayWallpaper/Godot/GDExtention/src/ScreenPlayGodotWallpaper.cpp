// Copied from godot-cpp/test/src and modified.

#include "ScreenPlayGodotWallpaper.h"
#include "godot_cpp/classes/display_server.hpp"
#include "godot_cpp/classes/global_constants.hpp"
#include "godot_cpp/classes/label.hpp"
#include "godot_cpp/classes/os.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/utility_functions.hpp"

//// ScreenPlayWallpaper
using namespace godot;
int ScreenPlayGodotWallpaper::sInstanceCount = 0;
int ScreenPlayGodotWallpaper::sLastID = 0;

void ScreenPlayGodotWallpaper::_bind_methods()
{
    UtilityFunctions::print("ScreenPlayGodotWallpaper _bind_methods");
    ClassDB::bind_method(godot::D_METHOD("init"), &ScreenPlayGodotWallpaper::init);
    ClassDB::bind_method(godot::D_METHOD("connect_to_named_pipe"), &ScreenPlayGodotWallpaper::connect_to_named_pipe);

    ClassDB::bind_method(godot::D_METHOD("get_activeScreensList"), &ScreenPlayGodotWallpaper::get_activeScreensList);
    ClassDB::bind_method(godot::D_METHOD("set_activeScreensList", "screens"), &ScreenPlayGodotWallpaper::set_activeScreensList);

    ClassDB::bind_method(godot::D_METHOD("get_projectPath"), &ScreenPlayGodotWallpaper::get_projectPath);
    ClassDB::bind_method(godot::D_METHOD("set_projectPath", "path"), &ScreenPlayGodotWallpaper::set_projectPath);

    ClassDB::bind_method(godot::D_METHOD("get_appID"), &ScreenPlayGodotWallpaper::get_appID);
    ClassDB::bind_method(godot::D_METHOD("set_appID", "id"), &ScreenPlayGodotWallpaper::set_appID);

    ClassDB::bind_method(godot::D_METHOD("get_volume"), &ScreenPlayGodotWallpaper::get_volume);
    ClassDB::bind_method(godot::D_METHOD("set_volume", "volume"), &ScreenPlayGodotWallpaper::set_volume);

    ClassDB::bind_method(godot::D_METHOD("get_checkWallpaperVisible"), &ScreenPlayGodotWallpaper::get_checkWallpaperVisible);
    ClassDB::bind_method(godot::D_METHOD("set_checkWallpaperVisible", "visible"), &ScreenPlayGodotWallpaper::set_checkWallpaperVisible);
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

    UtilityFunctions::print(
        "ScreenPlayWallpaper ", itos(mID),
        " created, current instance count: ", itos(sInstanceCount));
}

ScreenPlayGodotWallpaper::~ScreenPlayGodotWallpaper()
{
    sInstanceCount--;
    UtilityFunctions::print(
        "ScreenPlayWallpaper ", itos(mID),
        " destroyed, current instance count: ", itos(sInstanceCount));

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
    if (m_hPipe != INVALID_HANDLE_VALUE) {
        CloseHandle(m_hPipe);
    }
}

void ScreenPlayGodotWallpaper::_process(double delta)
{

    // if (!isPipeActive) {
    //     return;
    // }
    // timesinceLastRead += delta;
    // // 0.05 seconds = 50ms
    // if (timesinceLastRead >= 0.05) {
    //     String data = read_from_pipe();
    //     if (!data.is_empty()) {
    //         UtilityFunctions::print("Received data: " + data);
    //     }
    //     timesinceLastRead = 0.0;
    // }
}

bool ScreenPlayGodotWallpaper::configureWindowGeometry()
{
    if (!m_hook->searchWorkerWindowToParentTo()) {
        UtilityFunctions::print("No worker window found");
        return false;
    }

    RECT rect {};
    if (!GetWindowRect(m_hook->windowHandleWorker, &rect)) {
        UtilityFunctions::print("Unable to get WindoeRect from worker");
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
    auto* displayServer = DisplayServer::get_singleton();
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

    String output = "Setup window activeScreen: " + itos(activeScreen) + " scaling: " + rtos(scaling) + " x: " + itos(x) + " y: " + itos(y) + " width: " + itos(width) + " height: " + itos(height);
    UtilityFunctions::print(output);

    // Must be called twice for some reason when window has scaling...
    if (!SetWindowPos(m_hook->windowHandle, nullptr, x, y, width, height, SWP_HIDEWINDOW)) {
        UtilityFunctions::print("Could not set window pos");
        return false;
    }
    if (!SetWindowPos(m_hook->windowHandle, nullptr, x, y, width, height, SWP_HIDEWINDOW)) {
        UtilityFunctions::print("Could not set window pos 2");
        return false;
    }

    if (SetParent(m_hook->windowHandle, m_hook->windowHandleWorker) == nullptr) {
        UtilityFunctions::print("Could not attach to parent window");
        return false;
    }
    displayServer->window_set_size(godot::Vector2((real_t)width, (real_t)height));
    ShowWindow(m_hook->windowHandle, SW_SHOW);
    const std::string windowTitle = "ScreenPlayWallpaperGodot";
    SetWindowText(hwnd, windowTitle.c_str());
    return true;
}

bool ScreenPlayGodotWallpaper::connect_to_named_pipe()
{
    String pipeName = "ScreenPlay";
    String fullPipeName = "\\\\.\\pipe\\" + pipeName;
    std::wstring wPipeName = std::wstring(fullPipeName.wide_string());

    m_hPipe = CreateFileW(
        wPipeName.c_str(),
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        0,
        NULL);

    if (m_hPipe == INVALID_HANDLE_VALUE) {
        m_isPipeActive = false;
        UtilityFunctions::print("CreateFile failed, error code: " + String::num_int64(GetLastError()));
        return false;
    }

    m_isPipeActive = true;
    return true;
}

godot::String ScreenPlayGodotWallpaper::read_from_pipe()
{
    char buffer[128];
    DWORD bytesRead;
    String result;

    if (ReadFile(m_hPipe, buffer, sizeof(buffer) - 1, &bytesRead, NULL)) {
        buffer[bytesRead] = '\0';
        result = String(buffer);
    } else {
        UtilityFunctions::print("ReadFile from pipe failed, error code: " + String::num_int64(GetLastError()));
    }

    return result;
}

void ScreenPlayGodotWallpaper::connected()
{

    // Ensure you have valid appID and type
    if (m_appID.is_empty()) {
        UtilityFunctions::print("Unable to connect with empty: appid");
        emit_signal("disconnected");
        return;
    }

    // Construct welcome message and write to the named pipe
    godot::String welcomeMessage = godot::String("appID=") + m_appID + ",godotWallpaper";

    std::string stdMessage = welcomeMessage.utf8().get_data();
    DWORD bytesWritten;
    WriteFile(m_hPipe, stdMessage.c_str(), static_cast<DWORD>(stdMessage.size()), &bytesWritten, NULL);

    if (bytesWritten != stdMessage.size()) {
        emit_signal("disconnected");
        return;
    }
}

void ScreenPlayGodotWallpaper::messageReceived(const std::string& key, const std::string& value)
{
    try {
        if (key == "volume") {
            m_volume = std::stof(value);
            return;
        }
        if (key == "appID") {
            m_appID = godot::String(value.c_str());
            return;
        }

        if (key == "projectPath") {
            m_projectPath = godot::String(value.c_str());
            return;
        }

        // If none of the keys match
        // Assuming sceneValueReceived is a signal you've defined
        emit_signal("sceneValueReceived", key.c_str(), value.c_str());

    } catch (const std::invalid_argument& ia) {
        // Invalid argument passed to stof/stoi. Handle error as necessary.
        std::cerr << "Invalid argument: " << ia.what() << std::endl;
    } catch (const std::out_of_range& oor) {
        // Converted number is out of range for float/int. Handle error as necessary.
        std::cerr << "Out of range: " << oor.what() << std::endl;
    }
}
void ScreenPlayGodotWallpaper::set_checkWallpaperVisible(bool visible)
{
    m_checkWallpaperVisible = visible;
}
bool ScreenPlayGodotWallpaper::get_checkWallpaperVisible() const
{
    return m_checkWallpaperVisible;
}

void ScreenPlayGodotWallpaper::set_volume(float vol)
{
    m_volume = vol;
}
float ScreenPlayGodotWallpaper::get_volume() const
{
    return m_volume;
}
void ScreenPlayGodotWallpaper::set_appID(const godot::String& id)
{
    m_appID = id;
}
godot::String ScreenPlayGodotWallpaper::get_appID() const
{
    return m_appID;
}
void ScreenPlayGodotWallpaper::set_projectPath(const godot::String& path)
{
    m_projectPath = path;
}
godot::String ScreenPlayGodotWallpaper::get_projectPath() const
{
    return m_projectPath;
}
void ScreenPlayGodotWallpaper::set_activeScreensList(const godot::PackedInt64Array& screens)
{
    m_activeScreensList = screens;
}
PackedInt64Array ScreenPlayGodotWallpaper::get_activeScreensList() const
{
    return m_activeScreensList;
}
