// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#include "ScreenPlayGodotWallpaper.h"
#include "godot_cpp/classes/display_server.hpp"
#include "godot_cpp/classes/engine.hpp"
#include "godot_cpp/classes/scene_tree.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/utility_functions.hpp"

//// ScreenPlayWallpaper
using namespace godot;

void ScreenPlayGodotWallpaper::_bind_methods()
{
    UtilityFunctions::print("ScreenPlayGodotWallpaper _bind_methods");
    ClassDB::bind_method(godot::D_METHOD("init"), &ScreenPlayGodotWallpaper::init);
    ClassDB::bind_method(godot::D_METHOD("connect_to_named_pipe"), &ScreenPlayGodotWallpaper::connect_to_named_pipe);
    ClassDB::bind_method(godot::D_METHOD("send_welcome"), &ScreenPlayGodotWallpaper::send_welcome);
    ClassDB::bind_method(godot::D_METHOD("get_screenPlayConnected"), &ScreenPlayGodotWallpaper::get_screenPlayConnected);
    ClassDB::bind_method(godot::D_METHOD("get_pipeConnected"), &ScreenPlayGodotWallpaper::get_pipeConnected);
    ClassDB::bind_method(godot::D_METHOD("read_from_pipe"), &ScreenPlayGodotWallpaper::read_from_pipe);
    ClassDB::bind_method(godot::D_METHOD("writeToPipe", "message"), &ScreenPlayGodotWallpaper::writeToPipe);
    ClassDB::bind_method(godot::D_METHOD("send_ping"), &ScreenPlayGodotWallpaper::send_ping);
    ClassDB::bind_method(godot::D_METHOD("exit"), &ScreenPlayGodotWallpaper::exit);

    ClassDB::bind_method(godot::D_METHOD("get_activeScreensList"), &ScreenPlayGodotWallpaper::get_activeScreensList);
    ClassDB::bind_method(godot::D_METHOD("set_activeScreensList", "screens"), &ScreenPlayGodotWallpaper::set_activeScreensList);

    ClassDB::bind_method(godot::D_METHOD("get_projectPackageFile"), &ScreenPlayGodotWallpaper::get_projectPackageFile);
    ClassDB::bind_method(godot::D_METHOD("set_projectPackageFile", "projectPackageFile"), &ScreenPlayGodotWallpaper::set_projectPackageFile);

    ClassDB::bind_method(godot::D_METHOD("get_fps"), &ScreenPlayGodotWallpaper::get_fps);
    ClassDB::bind_method(godot::D_METHOD("set_fps", "fps"), &ScreenPlayGodotWallpaper::set_fps);

    ClassDB::bind_method(godot::D_METHOD("get_scale3d"), &ScreenPlayGodotWallpaper::get_scale3d);
    ClassDB::bind_method(godot::D_METHOD("set_scale3d", "scale"), &ScreenPlayGodotWallpaper::set_scale3d);

    ClassDB::bind_method(godot::D_METHOD("get_scale3dMode"), &ScreenPlayGodotWallpaper::get_scale3dMode);
    ClassDB::bind_method(godot::D_METHOD("set_scale3dMode", "mode"), &ScreenPlayGodotWallpaper::set_scale3dMode);

    ClassDB::bind_method(godot::D_METHOD("get_fullPckPath"), &ScreenPlayGodotWallpaper::get_fullPckPath);

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

godot::String ScreenPlayGodotWallpaper::get_fps() const
{
    return m_fps;
}

void ScreenPlayGodotWallpaper::set_fps(const godot::String& fps)
{
    m_fps = fps;
}

float ScreenPlayGodotWallpaper::get_scale3d() const
{
    return m_scale3d;
}

void ScreenPlayGodotWallpaper::set_scale3d(float scale)
{
    m_scale3d = scale;
}

godot::String ScreenPlayGodotWallpaper::get_scale3dMode() const
{
    return m_scale3dMode;
}

void ScreenPlayGodotWallpaper::set_scale3dMode(const godot::String& mode)
{
    m_scale3dMode = mode;
}

godot::String ScreenPlayGodotWallpaper::get_fullPckPath() const
{
    return m_projectPath + godot::String("/") + m_projectPackageFile;
}

bool ScreenPlayGodotWallpaper::configureWindowGeometry()
{
    if (!m_windowsIntegration.searchWorkerWindowToParentTo()) {
        UtilityFunctions::print("No worker window found");
        return false;
    }
    if (!IsWindow(m_windowsIntegration.windowHandleWorker())) {
        UtilityFunctions::print("GD: Could not get a valid window handle worker!");
        return false;
    }
    // WARNING: Setting Window flags must be called *here*!
    SetWindowLongPtr(m_windowsIntegration.windowHandle(), GWL_EXSTYLE, WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT);
    SetWindowLongPtr(m_windowsIntegration.windowHandle(), GWL_STYLE, WS_POPUP);

    return true;
}

bool ScreenPlayGodotWallpaper::init(int activeScreen)
{
    UtilityFunctions::print("ScreenPlayGodotWallpaper::init at ", Variant(activeScreen));
    auto* displayServer = DisplayServer::get_singleton();

    int64_t handle_int = displayServer->window_get_native_handle(godot::DisplayServer::HandleType::WINDOW_HANDLE);
    HWND hwnd = reinterpret_cast<HWND>(handle_int);
    m_windowsIntegration.setWindowHandle(hwnd);

    // Set Godot window to exclusive fullscreen and borderless to prevent it from managing window size
    displayServer->window_set_mode(godot::DisplayServer::WindowMode::WINDOW_MODE_EXCLUSIVE_FULLSCREEN);
    displayServer->window_set_flag(godot::DisplayServer::WindowFlags::WINDOW_FLAG_BORDERLESS, true);

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

    ShowWindow(hwnd, SW_SHOW);

    return true;
}

bool ScreenPlayGodotWallpaper::connect_to_named_pipe()
{
    m_windowsPipe.setPipeName(L"ScreenPlay");
    m_pipeConnected = m_windowsPipe.start();
    return m_pipeConnected;
}

/*!
    \brief Returns the next complete IPC message, or "" when none is pending.

    Raw pipe reads are NOT messages: writes from the main app arrive
    coalesced ({...}{...}) or split across reads. All raw bytes go through
    the same IpcFrameBuffer the Qt endpoints use, and callers receive one
    complete frame per call - loop until "" to drain the queue.
*/
godot::String ScreenPlayGodotWallpaper::read_from_pipe()
{
    // Drain everything the pipe currently has buffered.
    std::string chunk;
    while (m_windowsPipe.readFromPipe(chunk)) {
        m_frameBuffer.append(chunk);
        chunk.clear();
    }
    for (std::string& frame : m_frameBuffer.takeFrames()) {
        m_pendingMessages.push_back(std::move(frame));
    }

    if (m_pendingMessages.empty()) {
        return "";
    }
    const std::string message = std::move(m_pendingMessages.front());
    m_pendingMessages.pop_front();
    godot::UtilityFunctions::print("ScreenPlayGodotWallpaper received message: ", message.c_str());
    return godot::String::utf8(message.c_str(), static_cast<int>(message.size()));
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

void ScreenPlayGodotWallpaper::exit()
{
    m_windowsIntegration.exit();
    // Get the MainLoop and cast it to SceneTree
    godot::MainLoop* main_loop = godot::Engine::get_singleton()->get_main_loop();
    godot::SceneTree* scene_tree = godot::Object::cast_to<godot::SceneTree>(main_loop);

    if (!scene_tree) {
        UtilityFunctions::print("Unable to exit with invalid scene_tree");
        m_windowsPipe.writeToPipe("Unable to exit with invalid scene_tree");
        return;
    }
    m_windowsPipe.writeToPipe("ScreenPlayGodotWallpaper::exit");
    UtilityFunctions::print("ScreenPlayGodotWallpaper::exit");

    scene_tree->quit();

    return;
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

    UtilityFunctions::print("get_activeScreensList", m_activeScreensList);
    return m_activeScreensList;
}
