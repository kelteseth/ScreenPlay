# Changelog 2024

Period: 2024-01-01 – 2024-12-31
Version: v1.0.0-alpha2 (branch: version1)

---

## Added

- **Wallpaper Timeline system** — full UI (line handles, hour marks, drag-to-resize segments), backend saving/loading, wallpaper thumbnails per slot, add/remove/move timelines ([`471a4072`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/471a4072), [`cb11bd08`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/cb11bd08), [`99b0647f`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/99b0647f), [`a1e2c58c`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/a1e2c58c), [`8be66039`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/8be66039), [`734a24b7`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/734a24b7))
- **Timeline wallpaper loading** — profiles saved/restored across restarts ([`72ba97c6`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/72ba97c6), [`413c4ebe`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/413c4ebe))
- **Fedora Linux support** ([`25b569fa`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/25b569fa))
- **`startWallpaperMuted` option** — wallpapers can be started in muted state ([`bf616a95`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/bf616a95))
- **Extended SysInfo** — CPU info and real-time network statistics ([`44621e40`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/44621e40))
- **PID as secondary alive-check for running content** ([`73932d89`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/73932d89))
- **Visual Studio launch configuration** ([`cb32bc2d`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/cb32bc2d))
- **QML function type annotations** ([`b74a8109`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/b74a8109))

## Changed

- **Qt 6.5 → 6.7 → 6.7.2** ([`2aedecb9`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/2aedecb9), [`734a24b7`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/734a24b7))
- **Steam SDK v1.57 → v1.60** ([`586716d6`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/586716d6))
- **`ScreenPlayUtil` renamed to `ScreenPlayCore`** — shared functionality module ([`9ce8920d`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/9ce8920d))
- **Sidebar replaced by bottom InstalledDrawer** with Timeline integration ([`f9cafc89`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/f9cafc89), [`152e1ce4`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/152e1ce4))
- **Drawer position moved inside InstalledView** ([`aee0387f`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/aee0387f))
- **Video wizard consolidated** into a single wizard ([`bd5516fb`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/bd5516fb))
- **Wizards refactored to use QCoro** for async flow ([`b41242a9`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/b41242a9))
- **`WallpaperData` struct introduced** — `ScreenPlayManager` now operates on structured data ([`ff02791e`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/ff02791e))
- **CMake presets split into per-OS files** ([`dac7a2d8`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/dac7a2d8))
- **QML list model items use `required` properties** ([`c88500df`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/c88500df))
- `ScreenPlaySysInfo` refactored into a proper library ([`80468ac9`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/80468ac9))
- Log files relocated to system temp directory ([`ee47a6d9`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/ee47a6d9))
- QCoro third-party tests and examples disabled to reduce build noise ([`c8dc0e3f`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/c8dc0e3f))

## Fixed

- **Windows 11 24H2 desktop integration regression** ([`e0632716`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/e0632716))
- Monitor background rendered gray instead of transparent ([`8ad70f8c`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/8ad70f8c))
- Overdraw of installed content cards ([`39d6b15e`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/39d6b15e))
- Small screen size layout issues ([`bf616a95`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/bf616a95))
- Default volume and playback rate not applied ([`3856c99d`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/3856c99d))
- Windows mouse hook not firing reliably ([`8e40cce1`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/8e40cce1))
- GIFs not playing on hover ([`af358ac6`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/af358ac6))
- Version check for installed items ([`b74a8109`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/b74a8109))
- `tst_ScreenPlayWorkshop` test suite ([`5d4cb885`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/5d4cb885))
- Widget startup after core refactor ([`a3a1aeb3`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/a3a1aeb3))
- Preview image missing from timeline slot ([`f2aea13a`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/f2aea13a))
- Video controls missing when only a single timeline segment is present ([`ced1a62b`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/ced1a62b))
- Godot `WorldEnvironment` not loading with transparency enabled ([`1f3ab94e`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/1f3ab94e))
- Coroutine signal emit ordering ([`d358c4f9`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/d358c4f9))
- Import/export of wallpaper packages ([`1a4a14ae`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/1a4a14ae))

## Removed

- Outdated Windows media workaround ([`052b8ab8`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/052b8ab8))
- Old sidebar source files after InstalledDrawer migration ([`421a75ef`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/421a75ef))
