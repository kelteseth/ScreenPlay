// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#include "ScreenPlayGodotWallpaper.h"
#include "godot_cpp/classes/display_server.hpp"
#include "godot_cpp/classes/global_constants.hpp"
#include "godot_cpp/classes/label.hpp"
#include "godot_cpp/classes/os.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/utility_functions.hpp"

//// ScreenPlayWallpaper
using namespace godot;

ScreenPlayGodotWallpaper::ScreenPlayGodotWallpaper()
{
}

ScreenPlayGodotWallpaper::~ScreenPlayGodotWallpaper()
{
}

void ScreenPlayGodotWallpaper::_bind_methods()
{
    UtilityFunctions::print("ScreenPlayGodotWallpaper _bind_methods");
    ClassDB::bind_method(godot::D_METHOD("init"), &ScreenPlayGodotWallpaper::init);
    ClassDB::bind_method(godot::D_METHOD("connect_to_named_pipe"), &ScreenPlayGodotWallpaper::connect_to_named_pipe);
    ClassDB::bind_method(godot::D_METHOD("send_welcome"), &ScreenPlayGodotWallpaper::send_welcome);
    ClassDB::bind_method(godot::D_METHOD("get_screenPlayConnected"), &ScreenPlayGodotWallpaper::get_screenPlayConnected);
    ClassDB::bind_method(godot::D_METHOD("get_pipeConnected"), &ScreenPlayGodotWallpaper::get_pipeConnected);
    ClassDB::bind_method(godot::D_METHOD("read_from_pipe"), &ScreenPlayGodotWallpaper::read_from_pipe);
    ClassDB::bind_method(godot::D_METHOD("send_ping"), &ScreenPlayGodotWallpaper::send_ping);
    ClassDB::bind_method(godot::D_METHOD("exit"), &ScreenPlayGodotWallpaper::exit);

    ClassDB::bind_method(godot::D_METHOD("get_activeScreensList"), &ScreenPlayGodotWallpaper::get_activeScreensList);
    ClassDB::bind_method(godot::D_METHOD("set_activeScreensList", "screens"), &ScreenPlayGodotWallpaper::set_activeScreensList);

    ClassDB::bind_method(godot::D_METHOD("get_projectPackageFile"), &ScreenPlayGodotWallpaper::get_projectPackageFile);
    ClassDB::bind_method(godot::D_METHOD("set_projectPackageFile", "projectPackageFile"), &ScreenPlayGodotWallpaper::set_projectPackageFile);

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

godot::String ScreenPlayGodotWallpaper::get_projectPackageFile() const
{
    return m_projectPackageFile;
}

void ScreenPlayGodotWallpaper::set_projectPackageFile(const godot::String& projectPackageFile)
{
    m_projectPackageFile = projectPackageFile;
}

bool ScreenPlayGodotWallpaper::configureWindowGeometry()
{
    if (!m_windowsIntegration.searchWorkerWindowToParentTo()) {
        UtilityFunctions::print("No worker window found");
        return false;
    }
    if (!IsWindow(m_windowsIntegration.windowHandle())) {
        UtilityFunctions::print("GD: Could not get a valid window handle worker!");
        return false;
    }
    // WARNING: Setting Window flags must be called *here*!
    SetWindowLongPtr(m_windowsIntegration.windowHandle(), GWL_EXSTYLE, WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT);
    SetWindowLongPtr(m_windowsIntegration.windowHandle(), GWL_STYLE, WS_POPUPWINDOW);

    return true;
}

bool ScreenPlayGodotWallpaper::init(int activeScreen)
{
    auto* displayServer = DisplayServer::get_singleton();

    int64_t handle_int = displayServer->window_get_native_handle(godot::DisplayServer::HandleType::WINDOW_HANDLE);
    HWND hwnd = reinterpret_cast<HWND>(handle_int);
    m_windowsIntegration.setWindowHandle(hwnd);
    ShowWindow(m_windowsIntegration.windowHandle(), SW_HIDE);
    if (!IsWindow(hwnd)) {
        UtilityFunctions::print("ScreenPlayGodotWallpaper::init Could not get a valid window handle !", activeScreen, handle_int);
        UtilityFunctions::print("init hwnd: ", (int64_t)hwnd, activeScreen, handle_int);

        std::vector<ScreenPlay::WindowsMonitor> monitors = ScreenPlay::WindowsUtils().getAllMonitors();
        for (const auto& monitor : monitors) {
            UtilityFunctions::print(monitor.toString().c_str());
        }
        return false;
    }

    if (!configureWindowGeometry()) {
        return false;
    }
    hideFromTaskbar(m_windowsIntegration.windowHandle());
    auto updateWindowSize = [&displayServer](const int width, const int height) {
        displayServer->window_set_size(godot::Vector2((real_t)width, (real_t)height));
    };

    ScreenPlay::WindowsIntegration::MonitorResult monitor = m_windowsIntegration.setupWallpaperForOneScreen(activeScreen, updateWindowSize);
    if (monitor.status != ScreenPlay::WindowsIntegration::MonitorResultStatus::Ok) {
        UtilityFunctions::print("setupWallpaperForOneScreen failed status: ", (int)monitor.status);
        return false;
    }
    displayServer->window_set_size(godot::Vector2((real_t)monitor.monitor->size.cx, (real_t)monitor.monitor->size.cy));

    // SetWindowText(m_windowsIntegration.windowHandle(), "ScreenPlayWallpaperGodot");
    ShowWindow(m_windowsIntegration.windowHandle(), SW_SHOW);

    // m_windowsIntegration.setupWindowMouseHook();
    //  Set up the mouse event handler
    // m_windowsIntegration.setMouseEventHandler([this](DWORD mouseButton, UINT type, POINT p) {
    //     Ref<InputEventMouseButton> mouse_event;
    //     Ref<InputEventMouseMotion> motion_event;
    //     switch (type) {
    //     case WM_LBUTTONDOWN:
    //     case WM_LBUTTONUP:
    //     case WM_RBUTTONDOWN:
    //     case WM_RBUTTONUP:
    //         mouse_event.instantiate();
    //         mouse_event->set_position(Vector2(p.x, p.y));
    //         mouse_event->set_global_position(Vector2(p.x, p.y)); // Assuming global == local for this context
    //         mouse_event->set_button_index(
    //             type == WM_LBUTTONDOWN || type == WM_LBUTTONUP ? MOUSE_BUTTON_LEFT : MOUSE_BUTTON_RIGHT);
    //         mouse_event->set_pressed(type == WM_LBUTTONDOWN || type == WM_RBUTTONDOWN);
    //         break;
    //     case WM_MOUSEMOVE:
    //         motion_event.instantiate();
    //         motion_event->set_position(Vector2(p.x, p.y));
    //         motion_event->set_global_position(Vector2(p.x, p.y));
    //         break;
    //         // Add more cases as needed
    //     }

    //     if (mouse_event.is_valid()) {
    //         get_tree()->get_root()->get_viewport()->push_input(mouse_event);
    //     }
    //     if (motion_event.is_valid()) {
    //         get_tree()->get_root()->get_viewport()->push_input(motion_event);
    //     }
    // }); 2
    return true;
}

bool ScreenPlayGodotWallpaper::connect_to_named_pipe()
{
    m_windowsPipe.setPipeName(L"ScreenPlay");
    m_pipeConnected = m_windowsPipe.start();
    return m_pipeConnected;
}

godot::String ScreenPlayGodotWallpaper::read_from_pipe()
{
    std::string outMsg;
    if (!m_windowsPipe.readFromPipe(outMsg)) {
        // No new message
        return "";
    }
    return godot::String(outMsg.c_str());
}

bool ScreenPlayGodotWallpaper::writeToPipe(const godot::String& message)
{
    std::string stdMessage = message.utf8().get_data();

    return m_windowsPipe.writeToPipe(stdMessage);
}

bool ScreenPlayGodotWallpaper::send_ping()
{
    if (!m_screenPlayConnected || !m_pipeConnected) {
        UtilityFunctions::print("ScreenPlay hasn't connected to us yet!");
        return false;
    }
    const godot::String msg = "ping;";
    if (!writeToPipe(msg)) {
        return false;
    }

    return true;
}

bool ScreenPlayGodotWallpaper::send_welcome()
{
    // Ensure you have valid appID and type
    if (m_appID.is_empty()) {
        UtilityFunctions::print("Unable to connect with empty: appid");
        return false;
    }

    // Construct welcome message and write to the named pipe
    // See void ScreenPlay::SDKConnection::readyRead()
    godot::String msg = godot::String("appID=") + m_appID + ",godotWallpaper;";
    if (!writeToPipe(msg)) {
        return false;
    }

    m_screenPlayConnected = true;
    return true;
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
bool ScreenPlayGodotWallpaper::exit()
{
    // Somehow this gets called at editor startup
    // so just return if not initialized

    ShowWindow(m_windowsIntegration.windowHandle(), SW_HIDE);

    // Force refresh so that we display the regular
    // desktop wallpaper again
    ShowWindow(m_windowsIntegration.windowHandleWorker(), SW_SHOW);
    ShowWindow(m_windowsIntegration.windowHandleWorker(), SW_HIDE);
    return true;
}
void ScreenPlayGodotWallpaper::set_checkWallpaperVisible(bool visible)
{
    m_checkWallpaperVisible = visible;
}
bool ScreenPlayGodotWallpaper::get_screenPlayConnected() const
{
    return m_screenPlayConnected;
}
bool ScreenPlayGodotWallpaper::get_pipeConnected() const
{
    return m_pipeConnected;
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
