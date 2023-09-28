#pragma once
// Copied from godot-cpp/test/src and modified.

#include "godot_cpp/classes/control.hpp"
#include "godot_cpp/classes/global_constants.hpp"
#include "godot_cpp/classes/viewport.hpp"
#include "godot_cpp/core/binder_common.hpp"
#include "godot_cpp/variant/string.hpp"

#include <memory>

#include "ScreenPlayGodotWallpaper.h"
#include "windowshook.h"

class ScreenPlayGodotWallpaper : public godot::Node {
    GDCLASS(ScreenPlayGodotWallpaper, Node)

public:
    ScreenPlayGodotWallpaper();
    ~ScreenPlayGodotWallpaper() override;

    bool init(int activeScreen);
    bool create_named_pipe(godot::String pipeName);
    godot::String read_from_pipe();
    void _process(double delta);

protected:
    static void _bind_methods();

private:
    bool configureWindowGeometry();
    void hideFromTaskbar(HWND hwnd);

private:
    static int sInstanceCount;
    static int sLastID;

    int mID;
    std::unique_ptr<WindowsHook> m_hook;
    HANDLE hPipe;
    double timesinceLastRead = 0.0;
    bool isPipeActive = false;
};
