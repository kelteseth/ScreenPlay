# Changelog 2025

Period: 2025-01-01 – 2025-12-31
Versions: v1.0.0-alpha2 through v1.0.0-alpha10

---

## Added

- **macOS Mission Control Spaces support** — wallpapers follow spaces correctly ([`2fe0670e`](2fe0670e))
- **Godot content settings page** — scaling mode, rendering engine, and FPS limit configurable per-wallpaper ([`19a0f885`](19a0f885), [`c888c358`](c888c358), [`dc3bc1ef`](dc3bc1ef))
- **Sentry crash reporting** integrated into main app, wallpaper, and widget processes ([`b6e5faf8`](b6e5faf8), [`69760eca`](69760eca))
- **Video codec detection** — automatic conversion skip when codec is already compatible ([`56c8fb47`](56c8fb47))
- **Crossfade wallpaper transition** ([`9f15c825`](9f15c825), [`50534c72`](50534c72))
- **Example content wizard** — browse and install built-in examples from the UI ([`80a9c1e9`](80a9c1e9))
- **Graphics API selector** for wallpapers ([`b8a60a93`](b8a60a93))
- **Crash and automatic restart handling** for wallpaper/widget processes ([`d69a016f`](d69a016f)), including `ffe3f6ed`
- **Steam DLC check** ([`4c9ab644`](4c9ab644))
- **`profiles.json` sanity check** with user-visible error report ([`9f17ce47`](9f17ce47))
- **`UIAppStateSignals`** — global UI state signals for cross-component coordination ([`9c0741fd`](9c0741fd))
- **`ScreenPlayCoreWindowsExtrasLib`** — Windows-specific extras extracted into own library ([`f9c755a7`](f9c755a7))
- **WebP previews** replacing GIF for video thumbnails ([`ada97040`](ada97040))
- **Hover support on `MonitorSelectionItem`** ([`1e5470b5`](1e5470b5))
- **GPU-accelerated animations in InstalledView** ([`9d6a8069`](9d6a8069))
- **Pre-release warning banner and description** ([`87d98aea`](87d98aea))
- **Wallpaper replacement** while another is already running ([`0ddceb0d`](0ddceb0d))
- **VSCode `natvis` definitions** for Qt types ([`0efae1e9`](0efae1e9))
- **`uv.lock`** added for reproducible Python tooling ([`2101224e`](2101224e))

## Changed

- **Qt 6.8 → 6.9 → 6.9.1 → 6.10** ([`339ee2e9`](339ee2e9), [`dc70d2f5`](dc70d2f5), [`ae7d83dc`](ae7d83dc))
- **Godot 4.3 → 4.4.1 → 4.5 → 4.5.1** ([`516fed8a`](516fed8a), [`ae7d83dc`](ae7d83dc), [`87cee0d7`](87cee0d7))
- **Steam SDK v1.60 → v1.62** ([`b2555dec`](b2555dec))
- **`App` promoted to `QML_SINGLETON`** ([`575c9438`](575c9438), [`ecf539b2`](ecf539b2))
- **CMake build system refactored** — dedicated build presets per OS, `Build/` directory in repo root ([`7e7c32f9`](7e7c32f9), [`c1215c50`](c1215c50))
- **All Python tools migrated to `uv`** ([`194c1d8b`](194c1d8b))
- **Content arguments converted to data gadgets** ([`b8a60a93`](b8a60a93))
- **External process abstracted into base class** ([`3793d427`](3793d427))
- **macOS app bundle setup refactored** — window icon set only on Windows ([`1eb99201`](1eb99201))
- **Mouse hook enabled only when actually needed** ([`27c9f35a`](27c9f35a))
- **Coroutine return types unified** to always carry a result ([`044f2efb`](044f2efb))
- **QML logging categories expanded** ([`94dfb6bd`](94dfb6bd), [`a05aad30`](a05aad30))
- **Timeline wallpaper transition handling refactored** ([`e8c0dd8a`](e8c0dd8a))
- **Project settings UI revised** ([`0bf8d4ab`](0bf8d4ab))
- **`InstalledDrawer` revised** with collapsed/expanded animation ([`86c5bbf2`](86c5bbf2))
- CI migrated to self-hosted runners; Clang updated to version 21 ([`a300c89f`](a300c89f))

## Fixed

- Godot wallpaper black screen on exit ([`9493d46a`](9493d46a))
- Desktop wallpaper transition on Windows ([`6cde80c2`](6cde80c2), [`789173ec`](789173ec))
- `isPlaying` property causing binding loop ([`2dc736db`](2dc736db))
- Pause/mute state out of sync between Navigation bar and tray icon ([`29410f4c`](29410f4c))
- Color selector not applying ([`9ce47d27`](9ce47d27))
- Volume and video time setters ([`02fd94d4`](02fd94d4))
- Monitor selection ([`9fc8756d`](9fc8756d))
- WEBM file import ([`ec752d20`](ec752d20))
- Video drag-and-drop not working ([`ef2a677c`](ef2a677c))
- "Always minimize ScreenPlay" setting not persisted on macOS ([`47d1ba23`](47d1ba23))
- Logging directory creation on macOS ([`1ad0f539`](1ad0f539))
- `playbackRate` and `videoTime` removed to fix spurious setter calls ([`121ab1ac`](121ab1ac))
- Multiple crash fixes across startup, timeline activation and wallpaper close ([`c430ae6d`](c430ae6d))
- CMake 4.0 compatibility (preset parsing lowercase, `VCPKG_VERSION` bump) ([`48ee9bca`](48ee9bca), [`f1d69dc4`](f1d69dc4))

## Removed

- **Qt5Compat module** fully removed ([`339ee2e9`](339ee2e9))
- `playbackRate` and `videoTime` properties ([`121ab1ac`](121ab1ac))
- Duplicate SDK connection ([`002e3724`](002e3724))
- Godot export functionality for non-Windows targets (temporarily) ([`4ee62459`](4ee62459))
- Unused decoder setting ([`f3a05cc4`](f3a05cc4))
