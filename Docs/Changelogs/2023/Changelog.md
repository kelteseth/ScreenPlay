# Changelog 2023

Period: 2023-07-16 – 2023-12-31
Versions: v0.15.1-beta1, v0.15.1-beta2, v0.15.2, v0.15.3, v1.0.0-alpha1

---

## Added

- **Godot 4 wallpaper support** — full pipeline including GDExtension project, Windows pipe IPC, export wizard, CI build step, and default Godot project ([`c70fa622`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/c70fa622), [`7ecbd2e8`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/7ecbd2e8), [`1650c33a`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/1650c33a), [`11faefdb`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/11faefdb), [`e2790b9f`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/e2790b9f), [`0a046cec`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/0a046cec))
- **Wayland layer-shell support** — new platform integration for Linux Wayland compositors ([`5d2d5911`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/5d2d5911), [`f546c35f`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/f546c35f))
- **Unified logging class** — centralised `QLoggingCategory`-based logging replacing scattered qDebug calls ([`23873e1f`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/23873e1f))
- **Quick settings in tray icon** — volume, mute, and playback controls accessible from the system tray ([`0e6079fe`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/0e6079fe))
- **Icons in tray icon menu** ([`b942d887`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/b942d887))
- **Key input forwarding to QML wallpapers** ([`067b9535`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/067b9535))
- **macOS DMG installer** — basic `.dmg` package creation and signing scaffolding ([`db08b38f`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/db08b38f), [`c8e8a928`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/c8e8a928), [`0cd141a1`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/0cd141a1))
- **Default content thumbnails** — auto-generated preview images inside the creation wizards ([`fafd85d4`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/fafd85d4))
- **AMD crash workaround** for certain GPU configurations ([`86222a9f`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/86222a9f))
- **SPDX license checker script** ([`126af44f`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/126af44f))
- **Release CI pipeline** — automated build, packaging, and upload to getsp.de ([`f052c43d`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/f052c43d), [`31f5fb7f`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/31f5fb7f))
- AMD universal binary check for macOS ([`7843f63f`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/7843f63f))

## Changed

- **Qt 6.5.2 → Qt 6.6.0** ([`14419984`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/14419984))
- **ffmpeg 5 → ffmpeg 6.1** ([`e2447d3f`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/e2447d3f))
- **Godot 4.1 → Godot 4.2 stable** ([`11063d86`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/11063d86))
- **Enum refactoring** — namespace-scoped enums migrated to `enum class`; merged `ScreenPlayUtil`/`Util` ([`9a583efb`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/9a583efb), [`d15e670c`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/d15e670c))
- **WindowsIntegration refactor** — mouse hook and window positioning consolidated into `windowsintegration` ([`cdc44f05`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/cdc44f05), [`4db63579`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/4db63579))
- **Wizard files moved into Qt resources** ([`c1003343`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/c1003343))
- **CMake versioning** — new `GenerateCMakeVariableHeader.cmake`, defines replaced by `CMakeVariables.h.in` ([`55996b29`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/55996b29))
- **macOS build refactored** to bundle everything inside `ScreenPlay.app` ([`dfe9e60c`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/dfe9e60c))
- **Sidebar sections renamed** for clarity ([`2cf7f2f0`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/2cf7f2f0))
- Default Godot wallpaper content updated ([`69cb0066`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/69cb0066))
- System ffmpeg used on Linux instead of bundled binary ([`e9b1776e`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/e9b1776e))

## Fixed

- Wallpaper not shown when using a tiled desktop background (`e2790b9f`)
- QML enum type errors after Qt 6.6 upgrade ([`befdccd3`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/befdccd3), [`276a917e`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/276a917e))
- Crash on startup with fmt and unbuffered output ([`e4722600`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/e4722600))
- Crash on exit ([`d044963d`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/d044963d))
- Linux compilation / CI ([`55996b29`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/55996b29), [`94024be7`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/94024be7))
- Windows installer creation ([`2ce66161`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/2ce66161))
- Website wallpaper validation ([`7ae1e97c`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/7ae1e97c))
- Uninstalling content items ([`7ea263dc`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/7ea263dc))
- Reserved argument `path` clash ([`26262266`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/26262266))
- macOS launch on Apple Silicon ([`6e4d34c7`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/6e4d34c7))

## Removed

- Unused HTTP file server ([`5d3d3a90`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/5d3d3a90))
- Old/unused utility classes ([`9e01dbac`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/9e01dbac))
- Unnecessary Qt OpenSSL workaround ([`7843f63f`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/7843f63f))
- Old unused source files and code paths ([`c9498b36`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/c9498b36), [`2f7719d0`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/2f7719d0))
