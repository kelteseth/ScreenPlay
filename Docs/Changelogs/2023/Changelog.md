# Changelog 2023

Period: 2023-07-16 – 2023-12-31
Versions: v0.15.1-beta1, v0.15.1-beta2, v0.15.2, v0.15.3, v1.0.0-alpha1

---

## Added

- **Godot 4 wallpaper support** — full pipeline including GDExtension project, Windows pipe IPC, export wizard, CI build step, and default Godot project ([`c70fa622`](c70fa622), [`7ecbd2e8`](7ecbd2e8), [`1650c33a`](1650c33a), [`11faefdb`](11faefdb), [`e2790b9f`](e2790b9f), [`0a046cec`](0a046cec))
- **Wayland layer-shell support** — new platform integration for Linux Wayland compositors ([`5d2d5911`](5d2d5911), [`f546c35f`](f546c35f))
- **Unified logging class** — centralised `QLoggingCategory`-based logging replacing scattered qDebug calls ([`23873e1f`](23873e1f))
- **Quick settings in tray icon** — volume, mute, and playback controls accessible from the system tray ([`0e6079fe`](0e6079fe))
- **Icons in tray icon menu** ([`b942d887`](b942d887))
- **Key input forwarding to QML wallpapers** ([`067b9535`](067b9535))
- **macOS DMG installer** — basic `.dmg` package creation and signing scaffolding ([`db08b38f`](db08b38f), [`c8e8a928`](c8e8a928), [`0cd141a1`](0cd141a1))
- **Default content thumbnails** — auto-generated preview images inside the creation wizards ([`fafd85d4`](fafd85d4))
- **AMD crash workaround** for certain GPU configurations ([`86222a9f`](86222a9f))
- **SPDX license checker script** ([`126af44f`](126af44f))
- **Release CI pipeline** — automated build, packaging, and upload to getsp.de ([`f052c43d`](f052c43d), [`31f5fb7f`](31f5fb7f))
- AMD universal binary check for macOS ([`7843f63f`](7843f63f))

## Changed

- **Qt 6.5.2 → Qt 6.6.0** ([`14419984`](14419984))
- **ffmpeg 5 → ffmpeg 6.1** ([`e2447d3f`](e2447d3f))
- **Godot 4.1 → Godot 4.2 stable** ([`11063d86`](11063d86))
- **Enum refactoring** — namespace-scoped enums migrated to `enum class`; merged `ScreenPlayUtil`/`Util` ([`9a583efb`](9a583efb), [`d15e670c`](d15e670c))
- **WindowsIntegration refactor** — mouse hook and window positioning consolidated into `windowsintegration` ([`cdc44f05`](cdc44f05), [`4db63579`](4db63579))
- **Wizard files moved into Qt resources** ([`c1003343`](c1003343))
- **CMake versioning** — new `GenerateCMakeVariableHeader.cmake`, defines replaced by `CMakeVariables.h.in` ([`55996b29`](55996b29))
- **macOS build refactored** to bundle everything inside `ScreenPlay.app` ([`dfe9e60c`](dfe9e60c))
- **Sidebar sections renamed** for clarity ([`2cf7f2f0`](2cf7f2f0))
- Default Godot wallpaper content updated ([`69cb0066`](69cb0066))
- System ffmpeg used on Linux instead of bundled binary ([`e9b1776e`](e9b1776e))

## Fixed

- Wallpaper not shown when using a tiled desktop background (`e2790b9f`)
- QML enum type errors after Qt 6.6 upgrade ([`befdccd3`](befdccd3), [`276a917e`](276a917e))
- Crash on startup with fmt and unbuffered output ([`e4722600`](e4722600))
- Crash on exit ([`d044963d`](d044963d))
- Linux compilation / CI ([`55996b29`](55996b29), [`94024be7`](94024be7))
- Windows installer creation ([`2ce66161`](2ce66161))
- Website wallpaper validation ([`7ae1e97c`](7ae1e97c))
- Uninstalling content items ([`7ea263dc`](7ea263dc))
- Reserved argument `path` clash ([`26262266`](26262266))
- macOS launch on Apple Silicon ([`6e4d34c7`](6e4d34c7))

## Removed

- Unused HTTP file server ([`5d3d3a90`](5d3d3a90))
- Old/unused utility classes ([`9e01dbac`](9e01dbac))
- Unnecessary Qt OpenSSL workaround ([`7843f63f`](7843f63f))
- Old unused source files and code paths ([`c9498b36`](c9498b36), [`2f7719d0`](2f7719d0))
