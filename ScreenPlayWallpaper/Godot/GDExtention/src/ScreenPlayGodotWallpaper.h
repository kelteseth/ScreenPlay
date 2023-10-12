#pragma once
// Copied from godot-cpp/test/src and modified.

#include "godot_cpp/classes/control.hpp"
#include "godot_cpp/classes/global_constants.hpp"
#include "godot_cpp/classes/timer.hpp"
#include "godot_cpp/classes/viewport.hpp"
#include "godot_cpp/core/binder_common.hpp"
#include "godot_cpp/variant/string.hpp"

#include <memory>
#include <string>
#include <vector>

#include "ScreenPlayGodotWallpaper.h"
#include "windowshook.h"

class ScreenPlayGodotWallpaper : public godot::Node {
    GDCLASS(ScreenPlayGodotWallpaper, Node)

public:
    ScreenPlayGodotWallpaper();
    ~ScreenPlayGodotWallpaper() override;

    bool init(int activeScreen);
    bool connect_to_named_pipe();
    bool send_welcome();
    godot::String read_from_pipe();
    void messageReceived(const std::string& key, const std::string& value);

    godot::PackedInt64Array get_activeScreensList() const;
    void set_activeScreensList(const godot::PackedInt64Array& screens);
    godot::String get_projectPath() const;
    void set_projectPath(const godot::String& path);
    godot::String get_appID() const;
    void set_appID(const godot::String& id);
    float get_volume() const;
    void set_volume(float vol);
    bool get_checkWallpaperVisible() const;
    void set_checkWallpaperVisible(bool visible);
    bool get_screenPlayConnected() const;
    bool get_pipeConnected() const;
    bool ping_alive_screenplay();
    bool exit();

protected:
    static void _bind_methods();

private:
    bool configureWindowGeometry();
    void hideFromTaskbar(HWND hwnd);

private:
    static int sInstanceCount;
    static int sLastID;

    int mID;
    godot::String m_appID = "";
    godot::String m_projectPath = "";
    std::unique_ptr<WindowsHook> m_hook;
    HANDLE m_hPipe;
    double m_timesinceLastRead = 0.0;
    bool m_pipeConnected = false;
    bool m_screenPlayConnected = false;

    godot::PackedInt64Array m_activeScreensList;
    float m_volume = 0.0;
    bool m_checkWallpaperVisible = false;
};
