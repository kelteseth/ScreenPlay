// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/classes/scene_tree.hpp"
#include "godot_cpp/classes/timer.hpp"
#include "godot_cpp/classes/viewport.hpp"
#include "godot_cpp/classes/window.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/vector2.hpp"

#include <string>

#include "ScreenPlayGodotWallpaper.h"
#include "WindowsPipe.h"
#include "windowsintegration.h"

class ScreenPlayGodotWallpaper : public godot::Node {
    GDCLASS(ScreenPlayGodotWallpaper, Node)

public:
    ScreenPlayGodotWallpaper();
    ~ScreenPlayGodotWallpaper() override;

    bool init(int activeScreen);
    bool connect_to_named_pipe();
    bool send_welcome();
    bool writeToPipe(const godot::String& message);
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
    bool send_ping();
    bool exit();

    godot::String get_projectPackageFile() const;
    void set_projectPackageFile(const godot::String& projectPackageFile);
    void _ready() override;

protected:
    static void _bind_methods();

private:
    bool configureWindowGeometry();
    void hideFromTaskbar(HWND hwnd);
    void _on_pipe_read_timer_timeout();

private:
    OVERLAPPED overlappedRead = {};
    OVERLAPPED overlappedWrite = {};

    godot::String m_appID = "";
    godot::String m_projectPath = "";
    godot::String m_projectPackageFile = "";
    ScreenPlay::WindowsIntegration m_windowsIntegration;
    double m_timesinceLastRead = 0.0;
    bool m_pipeConnected = false;
    bool m_screenPlayConnected = false;
    WindowsPipe m_windowsPipe;
    godot::Timer* m_pipeReadTimer = nullptr;

    godot::PackedInt64Array m_activeScreensList;
    float m_volume = 1.0f;
    bool m_checkWallpaperVisible = false;
};
