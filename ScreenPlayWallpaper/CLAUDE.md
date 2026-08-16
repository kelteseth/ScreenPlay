# ScreenPlayWallpaper

The **wallpaper render process**. The manager launches one instance per running
wallpaper; it renders the content and talks back over `ScreenPlaySDK`. QML module
URI `ScreenPlayWallpaper`; `main.cpp` is the process entry point.

## Per-OS window backends (`src/`)

`BaseWindow` is the abstract base; each platform parents the render surface to
the desktop differently:

- **Windows** — `WinWindow` (parents under `WorkerW` behind the desktop icons;
  `windowsdesktopproperties`, `windowsintegration`).
- **Linux** — `LinuxX11Window` and `LinuxWaylandWindow`.
- **macOS** — `MacWindow` (+ `MacBridge`/`macintegration` Objective-C++ `.mm`).

`.mm` files are Objective-C++ and only compile on macOS — guard additions by
platform in `CMakeLists.txt`.

## Content backends (`qml/`)

One process handles all wallpaper kinds via the `ContentTypes::InstalledType`:
video (`MultimediaView` / `CrossFadeVideoPlayer`), website (`WebsiteWallpaper`,
Qt WebEngine), GIF (`GifWallpaper`), plain QML, and **Godot** (native GDExtension
wrapper under `Godot/`, which links no Qt — see the IPC note in
`ScreenPlaySDK`/`ScreenPlayCore`).

## Key mechanics — the traps

- **Crossfade uses two states.** `BaseWindow` holds `currentState` (active) and
  `targetState` (incoming) — both `WallpaperState` (`wallpaperstate.h`). After a
  transition completes, `targetState` is copied into `currentState` via
  `copyFrom`. `WallpaperState` centralizes volume/fillMode/muted/loops/… so
  settings survive a crash-restart; don't scatter these back onto `BaseWindow`.
- **`FrameRateLimiter` requires the "basic" render loop.** It caps FPS by holding
  back `UpdateRequest` events on an absolute time grid; the threaded render loop
  paces itself without `UpdateRequest` and would bypass the filter, so the
  wallpaper always forces the basic loop. Video additionally lowers decode power
  via `playbackRate = clamp(limit / nativeFps, 0.1, 1.0)`.
- **Graphics API** goes through `ScreenPlay::applyGraphicsApi()` (in
  `ScreenPlayCore`). Qt WebEngine (6.11) has no D3D12 path — HTML wallpapers fall
  back to software under DX12.
- A `replaceWallpaper` (SDK signal) swaps content live; Godot→Godot must reload
  **once** and keep the PCK file-watcher timestamp in sync.

## Tests

`tst_external_process` (in the manager) covers the wallpaper/widget lifecycle
state machine and crash/retry handling.
