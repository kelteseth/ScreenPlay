# ScreenPlayCore

Shared C++ library **and** QML module used by the manager and every
wallpaper/widget process. QML module URI is **`ScreenPlayCore`**, conventionally
imported `as SPCore`. Put anything reused across two or more modules here rather
than duplicating it.

## C++ — key pieces (`src/`, `inc/public/ScreenPlayCore/`)

- **`IpcFrameBuffer`** (`ipcframebuffer.h`) — reassembles IPC frames from the raw
  socket byte stream (JSON `{...}` brace-matched + legacy `;`-terminated text).
  **Deliberately Qt-free (`std::` only)**: the identical code is also compiled
  into the Godot GDExtension wallpaper, which cannot link Qt, via the separate
  **`ScreenPlayCoreIpcLib`** target. Do not add Qt includes to it. Qt callers
  convert with `QByteArray::constData` / `QString::fromStdString` (frames UTF-8).
- **`Util`** (`util.h`) — shared helpers. Use the centralized ones instead of
  re-adding `#ifdef` blocks: `Util::ffmpegExecutable()` / `ffprobeExecutable()` /
  `isFFmpegBundled()`, and `Util::bundledExampleContentPath()`.
- **`ContentTypes`** (`contenttypes.h`) — `InstalledType` / `ExampleContent`
  enums shared by the manager, workshop, and wallpaper.
- **`applyGraphicsApi()`** (`graphicsapi.h`) — single entry point that selects
  Vulkan / DX12 / GL for the main window and wallpaper processes. `QSG_RHI_BACKEND`
  always overrides it.
- Other shared services: `ProcessManager`, `FrameStats`, `LicenseManager`,
  `LoggingHandler`, `GodotHandler`, `ProjectFile`, `ArchiveReader`/`SpArchive`,
  `steamenumsgenerated` (generated — do not hand-edit).

## QML — reusable components (`qml/`)

`MaterialGridView`, `MaterialListView`, `ImageBlurContainer`, `FireworksEffect`,
`FrameStatsOverlay`, dialogs, selectors, icons (`assets/icons/`, consolidated
here so all modules share one set). Prefer extending these over building new
one-off components in a feature module.
