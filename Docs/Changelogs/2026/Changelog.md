# Changelog 2026

Period: 2026-01-01 – present

---

## [!113](https://gitlab.com/kelteseth/ScreenPlay/-/merge_requests/113) Steam Workshop UI Overhaul — Endless Scrolling, Upload Rewrite & Profile Improvements
#### New

**Workshop — Upload**
- New `SteamUploadView.qml` replacing `UploadProject.qml` with 2-column grid layout, compact item heights, progress page, and items capped at 800 px width ([`3ccecccc`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/3ccecccc))
- `InstalledListFilter` to hide already-published items from the upload list
- Sorting by name/date with ascending/descending toggle
- Update existing workshop content instead of only creating new items ([`e4968b98`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/e4968b98))
- Upload completion fireworks celebration ([`8f0c1322`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/8f0c1322))

**Workshop — Browse**
- Endless scrolling via `loadNextPage()` with `hasMore`/`isLoading` state ([`42ba6a60`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/42ba6a60))
- Gold shine effect on new content ([`7b5c5e2c`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/7b5c5e2c))
- Clickable creator name to filter by author ([`41168ee2`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/41168ee2))
- Video preview fallback via `QtMultimedia` in `WorkshopItem`
- Sidebar subscribe UX and tag empty-state improvements ([`23b78231`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/23b78231))
- UGC query builder (`ugcquerybuilder.h`) for type-safe Steam queries ([`588d44ca`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/588d44ca))

**Workshop — Profile**
- Dedicated `SteamProfileWorkshopItem` detail page with back-button navigation ([`6fd7375e`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/6fd7375e), [`9aaf7568`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/9aaf7568))
- Endless scrolling with loading/end-of-list footer and empty state
- List type and sort order combo boxes (Your Uploads / Subscribed / Favorites)
- `isOwnItem` flag to distinguish own vs. subscribed/favorited items
- Stats section showing workshop content statistics
- Keyboard (Escape) and mouse back-button navigation

**Video import & previews**
- Video import pipeline overhaul — 16:9 crop, single-pass CRF, preview improvements ([`983da1fe`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/983da1fe))
- NSFW (🔞) and Anime (🌸) toggle switches on video import with tooltip hints — saved as tags in `project.json`
- GifOptimizer — adaptive two-phase GIF encoder for Steam 1 MB preview limit ([`d8b69d9b`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/d8b69d9b))
- GIF wallpaper preview extraction on import ([`7e438315`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/7e438315))
- GIF wallpaper preview fallback and MSVC 18 support ([`050f786f`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/050f786f))
- MJPEG video codec enum, H265 string-to-enum mapping fix ([`016994a9`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/016994a9))
- Video info panel, preview player, codec selection QML improvements ([`7c7ff87b`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/7c7ff87b))
- AV1 test content (shapes wallpaper) and LFS tracking for `.mkv` files ([`7da67080`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/7da67080))

**Shared components**
- `MaterialGridView` — reusable grid with scroll defaults and remove/displaced transitions ([`df0c17d8`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/df0c17d8))
- `MaterialListView` — reusable list with matching scroll defaults and transitions ([`81006d3d`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/81006d3d))
- `ImageBlurContainer` — reusable blurred header background ([`d3cacb61`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/d3cacb61))
- `FireworksEffect` — particle fireworks component (MIT, based on [Partycles](https://jonathanleane.github.io/partycles/))
- All app icons consolidated into `ScreenPlayCore/assets/icons/` (60+ SVGs) ([`760e47c1`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/760e47c1))
- Brand icons (Bluesky, GitHub, GitLab, Reddit, Twitch) and Font Awesome icons added
- `SteamAsyncCall`, `SteamTagArray`, `WorkshopItemDetail` helpers for Steam API
- `testhelper.h` added to `ScreenPlayCore` for shared test utilities
- Controls config for Godot wallpapers ([`be932d14`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/be932d14))

**Tests**
- Workshop upload test ([`96f82efc`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/96f82efc))
- Video import and GIF optimizer test suites ([`6e7ed9a9`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/6e7ed9a9))

#### Fixed

**Workshop**
- `workshopid` parsing — use `toVariant()` instead of `toInt()` for uint64 Steam IDs
- `SteamWorkshopView` extra `Flickable` wrapper caused Qt to render all items at once and broke endless scrolling ([`c14ac273`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/c14ac273))
- Workshop list not cleared before a new request ([`2c71d8b0`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/2c71d8b0))
- Steam account profile image not loading ([`834ff988`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/834ff988))
- Sidebar vote tooltip fixed using `HoverHandler` ([`23b78231`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/23b78231))
- Upload button not enabling — fixed via `selectionCount` property
- Upload silently proceeding with invalid UGC update handle ([`a54327cc`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/a54327cc))
- Missing `break` in `submitItemUpdateStatus` switch causing fall-through ([`0f36a7ff`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/0f36a7ff))
- Crash in signal dispatch when upload list is cleared mid-emission ([`8f483ca6`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/8f483ca6))
- Workshop restore auto-clear and `dataChanged` scoped to single row ([`fa363faa`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/fa363faa))
- `SteamProfile` items loaded on `Component.onCompleted` instead of `StackView.onActivated` to avoid reloading on back-navigation
- File system watcher missing workshop downloads ([`7b5c5e2c`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/7b5c5e2c))
- Download button stuck on "Downloading…" after item was installed ([`c011130f`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/c011130f))
- Crash on shutdown when sub-objects outlived `SteamAPI_Shutdown` ([`82543bb8`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/82543bb8))

**Video import**
- Imported video not appearing when using NoConversion — wrong file extension written to `project.json` ([`2813080a`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/2813080a))
- Import could fail on videos with unusual frame rates (division by zero) ([`a8c522d6`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/a8c522d6))

**UI & rendering**
- Image stretching fixed with `PreserveAspectCrop`
- Background blur during scrolling and `StackView` navigation ([`c36fd08d`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/c36fd08d))
- Icon colour inconsistencies ([`22a217c9`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/22a217c9))
- Prevent duplicate page pushes in `ScreenPlayMain`

**Build & paths**
- Example content directory not found in dev builds — now uses `SCREENPLAY_SOURCE_DIR` for dev and exe-relative paths for deploy ([`32481236`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/32481236))

**C++ correctness**
- `QAbstractListModel` mutations on background thread in `loadInstalledContent` ([`2cbaaed8`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/2cbaaed8))
- Incorrect C-style includes for `cstring` and `stdlib.h` ([`d3c6d424`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/d3c6d424))
- Godot export for debug builds ([`44ff68fb`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/44ff68fb))

#### Removed
- **`UploadProject.qml`** — replaced by `SteamUploadView.qml`
- **`WorkshopInstalled.qml`**
- **`InstalledItem.qml`** and **`InstalledItemImage.qml`** from Workshop (consolidated)
- **`Background.qml`** from `ScreenPlayCore/qml`
- **`MultiEffect` blur** and downloading/installed states from `WorkshopItem`
- **Duplicate remove/displaced transitions** from `InstalledView` (moved into `MaterialGridView`)
- **`SteamApiWrapper`** — replaced by `SteamAsyncCall` and split sub-objects

#### Changed

**Workshop refactoring**
- `SteamWorkshopStartPage.qml` → renamed to `SteamWorkshopView.qml`
- `SteamWorkshop` god-class split into `SteamWorkshopSearch`, `SteamWorkshopProfile`, `SteamWorkshopItemOps` sub-objects ([`be8a38c5`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/be8a38c5))
- `ScreenPlayWorkshop` refactored — reduced Steam API boilerplate and improved type safety ([`2aa56401`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/2aa56401))
- Steamworks `SteamAPI_Init` and `SteamAPI_RestartAppIfNecessary` failure modes documented inline at the call site so future debugging does not require chasing the SDK reference ([`ccad7257`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/ccad7257))
- Sidebar refactored to use a single unified struct ([`e450ae9c`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/e450ae9c))
- `SteamWorkshopView` converted from `Flickable` + `GridView` to a pure `GridView` ([`c14ac273`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/c14ac273))
- `SteamProfile` rewritten with `MaterialGridView` and header section matching browse layout
- `UploadProjectBigItem`/`UploadProjectItem` revised with compact layout and `PreserveAspectCrop`
- Delete workshop item dialog stays open with spinner during Steam API call; closes only on success
- Workshop item detail page coerces `publishedFileID` to `String` to avoid JS float precision loss for 64-bit IDs

**UI & data model**
- `SteamWorkshopItem` animations aligned with `InstalledView` card style ([`96d922d0`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/96d922d0))
- `TypeString`, `LastModified`, `IsOnWorkshop` roles added to `InstalledListModel`
- `installedListFilter` property exposed on `ScreenPlayWorkshop`
- `ScreenPlayCore` module alias defaulted to `SPCore`
- Icons moved into `ScreenPlayCore` module ([`760e47c1`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/760e47c1))
- Sidebar background blur revised ([`c36fd08d`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/c36fd08d))
- Endless scroll triggers earlier (2× viewport threshold) for smoother browsing ([`200e2892`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/200e2892))

**Build & dependencies**
- FFmpeg upgraded to 8.1, Windows download switched to full 7z archive ([`3094aa90`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/3094aa90))
- CMake preset inherits reordered so `config-develop` overrides build-type defaults ([`852a6ae1`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/852a6ae1))
- vcpkg updated and doctest added ([`2d0f77ad`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/2d0f77ad))
- Steamworks SDK updated to v1.64; update procedure documented in `ThirdParty/Steam/SteamSDKUpdate.md` ([`3f164c07`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/3f164c07))
- Sentry CLI updated to 3.2.1 in CI ([`bb5e55b0`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/bb5e55b00ff7c03499fa5a08011976d2971234c4))
- Content folder copied to build directory at build time via `copy_directory_if_different` ([`32481236`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/32481236))
- ffmpeg/ffprobe path resolution centralised in `Util::ffmpegExecutable()` / `ffprobeExecutable()` / `isFFmpegBundled()` — collapses four duplicated `#ifdef` blocks across `create.cpp`, `createimportvideo.cpp`, and `tst_GifOptimizer.cpp` ([`180d129a`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/180d129a))
- `Wizards::contentPath()` moved to `Util::bundledExampleContentPath()` so non-Wizards callers can reuse it; per-OS branching converted from `#ifdef` to runtime `QOperatingSystemVersion` ([`60630a1a`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/60630a1a))
- `Qt6::Concurrent` added as a `ScreenPlay` target dependency ([`9e4c8f03`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/9e4c8f03))

---

## [!114](https://gitlab.com/kelteseth/ScreenPlay/-/merge_requests/114) Revise Python scripts with Rich TUI and fix downloads

#### New
- **Rich TUI for Python setup scripts** — `setup.py`, `download_ffmpeg.py`, and `setup_godot.py` rewritten with Rich library for cleaner terminal output ([`34b83844`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/34b83844))
- **`rich_console.py`** — shared Rich console singleton
- **`download_util.py`** — centralised HTTP streaming downloads with real byte-level progress bars (fixes ffmpeg downloads blocked by bot protection — switched from `urllib` to `requests` with proper UA header)
- **Setup step failure checking** — each setup step now checks for failure ([`1a563376`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/1a563376))

#### Fixed
- **FFmpeg download** blocked by bot protection (switched to `requests` with proper User-Agent)
- **ECM version** mismatch ([`43e1708a`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/43e1708a))

#### Removed
- **Fake progress bar** in `setup.py` that was permanently stuck at 100% — replaced with numbered rule dividers per step
- **Raw ANSI codes** in `execute_util.py` — replaced with Rich markup

#### Changed
- **Qt updated** to 6.10.2 ([`962eb055`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/962eb055))
- **vcpkg, qcoro, and Windows ffmpeg** updated ([`6c0daf96`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/6c0daf96))
- **`rich` and `requests`** added to `pyproject.toml` dependencies

---

## [!115](https://gitlab.com/kelteseth/ScreenPlay/-/merge_requests/115) Timeline, External Process & SDK Bug Fixes

#### New

**Wallpaper rendering & power**
- Configurable **FPS limit for video and QML/HTML/GIF wallpapers** — a global default plus a per-wallpaper override saved in `profiles.json`. A new `FrameRateLimiter` throttles the scene graph's `UpdateRequest` delivery on an absolute time grid so the average rate is exact and adjustable at runtime (the wallpaper now always uses the "basic" render loop, keeping animations time-correct). Video additionally lowers decode power via `playbackRate = clamp(limit / nativeFps, 0.1, 1.0)`. Editable from a shared `WallpaperFpsControl` in the video and QML/website panels ([`bfc6cc21`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/bfc6cc21))
- **Vulkan and DirectX 12 graphics-API options**, defaulting to Vulkan when a usable driver is present — routed through a shared `ScreenPlay::applyGraphicsApi()` for both the main window and the wallpaper processes. Avoids the D3D11 flip-model DWM independent-flip loss that degraded main-window pacing (144 Hz → ~85 fps on AMD) while a wallpaper is parented under `WorkerW`. `QSG_RHI_BACKEND` always overrides; DX12 is opt-in (Qt WebEngine 6.11 has no D3D12 path, so HTML wallpapers fall back to software there) ([`9fbfeee7`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/9fbfeee7))
- **Frame pacing overlay** (Ctrl+Shift+F) — fps, mean/stddev/p99/worst frame time, stutter count, animation-step jitter, graphics API and relevant QSG/D3D env overrides; exposed as `App.frameStats` so tests can assert pacing. Toggling it mirrors the overlay into every connected wallpaper via a new `frameStatsOverlay` SDK message ([`0dc2e4ed`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/0dc2e4ed), [`43d6053d`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/43d6053d))
- **Godot volume control** — the native wrapper received `--volume` but never routed it to the audio engine; volume is now applied to Godot's global Master audio bus (0 mutes, otherwise 0..1 mapped to the bus gain in dB) on startup, live change and reload, with a Volume slider added to the Godot panel ([`4a2156f5`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/4a2156f5))
- **Godot `replace` command parser** — a Godot→Godot timeline switch now swaps the actual scene content (project path + package file) and **crossfades** from the previous frame, instead of only leaking settings through generic top-level keys ([`cd27cc84`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/cd27cc84))
- **Shipped example content shown read-only** in the Installed tab via a new `ContentTypes::ExampleContent` enum (one value per shipped folder). Remove/delete is refused, a user's own copy of a folder wins over the shipped example, and it can be hidden via a "Show example content" setting (default on) ([`e3416ae1`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/e3416ae1))

**IPC**
- **`IpcFrameBuffer`** — new Qt-free `ScreenPlayCoreIpcLib` that reassembles brace-matched JSON and legacy `;`-terminated text frames from the raw socket/pipe byte stream for every wallpaper/widget endpoint, including the Godot GDExtension (which cannot link Qt). Fixes silently dropped messages from coalesced writes (`{...}{...}`) and packet-split reads — e.g. `syncAllProperties` settings that never applied ([`d1c791fb`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/d1c791fb))

**Refactoring**
- `WallpaperState` extracted as a dedicated `QObject` to consolidate playback state (volume, fillmode, isLooping, playbackRate, current time, muted) previously scattered across `ScreenPlayWallpaper` — restored settings now survive crash-restart correctly ([`4fa35085`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/4fa35085))

**Tests**
- **chuck_tester WebSocket UI-automation server** (opt-in via `--tester-port`, also bypasses the single-instance guard) with a Python UI test suite under `ScreenPlay/tests/ui/python` and `objectName` prefixes so multiple `Timeline` instances are addressable ([`463f70a5`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/463f70a5))
- `--isolated-appdata` sandboxes `profiles.json`/logs to Qt test-mode dirs so runs never destroy the user's real profile; scheduling tests assert wallpaper rotation via the manager's `runningWallpapers` snapshot instead of pixels ([`e1e2ccd0`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/e1e2ccd0))
- `tst_timeline` — data-driven coverage for `ScreenPlayTimelineManager` (structural invariants, regression coverage for timeline bug fixes) ([`154a4845`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/154a4845))
- `tst_external_process` — widget/wallpaper lifecycle state machine, timer-stop regression for `ScreenPlayWidget::close()`, signal/state coverage ([`154a4845`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/154a4845))
- `tst_sdk` — `SDKConnection` `readyRead` protocol parsing, `sendMessage` round-trip, and `close()` return-value regression coverage ([`979a120c`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/979a120c))

#### Fixed

**Timeline**
- Timeline state-machine deadlocks, stale indices and coroutine use-after-free: capturing-lambda `QCoro::QmlTask` coroutines converted to member coroutines taking arguments by value (a lambda's captures die after the first `co_await`, so later access was a dangling read); a wallpaper that fails to start is dropped from its section instead of stranding the rotation in `Failed`; and add/move/remove re-verify the section identifier after suspending ([`56196039`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/56196039))
- 11 bugs in the timeline flow from QML through to the wallpaper process ([`32a8de68`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/32a8de68))
- `co_await` missing on `removeTimelineAt` in `checkActiveWallpaperTimeline` — coroutine result was discarded ([`93f32d6b`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/93f32d6b))
- Crash in `updateMonitorListModelData` when the timeline is inactive ([`fe75c54c`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/fe75c54c))
- `setActiveTimelineIndex(-1)` triggered when removing the timeline at index 0 with 3+ sections ([`a39f6c9d`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/a39f6c9d))
- `const_cast` undefined behaviour when enforcing full-day span for the basic version — section now stores `effectiveStart`/`effectiveEnd` instead of mutating const input ([`829cf949`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/829cf949))

**External process / wallpaper lifecycle**
- Orphaned detached wallpaper/widget processes now force-killed as a last resort when the cooperative quit path fails (never connected, quit undeliverable, or unresponsive) — previously they kept rendering forever with no owner. The ping-alive handler is connected once in the constructor (reconnecting per crash-restart accumulated a duplicate each time), and a process that has exited now counts as dead for crash detection ([`3c41f200`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/3c41f200))
- Godot→Godot timeline switch triggered a redundant second reload; the switch now reloads once and keeps the PCK file watcher's timestamp in sync ([`cd27cc84`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/cd27cc84))
- `ScreenPlayWidget::close()` did not stop the ping and stability timers, leaving them firing on a closed widget ([`226370f2`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/226370f2))
- Orphaned wallpaper process when live-replace placement fails ([`9cac2c91`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/9cac2c91))
- UI not updated when a wallpaper crashes after exhausting its max retry budget ([`c3d6c38d`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/c3d6c38d))
- Saved settings (volume, fillmode) not re-applied when a wallpaper restarts after a crash ([`30958116`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/30958116))

**SDK**
- `ScreenPlaySDK` rewritten — removed `global_sdkPtr`, fixed infinite recursion, null-pointer crash, protocol mismatch, and revised the cleanup API ([`f87c6f1d`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/f87c6f1d))
- `SDKConnection::readyRead` dropped messages when a ping arrived in the same TCP packet as another message ([`71b05c8a`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/71b05c8a))
- `SDKConnection::close()` always returned `false` because the disconnect is async ([`721ad0cf`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/721ad0cf))
- `SDKConnection` `requestRaise` command was dead code and never fired ([`2c78f2a9`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/2c78f2a9))
- Volume validation logic, double `sdkDisconnected` emission, and `global_sdkPtr` dangling on destruction ([`924e35d5`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/924e35d5))
- `sendMessage` no longer blocks the GUI thread on `waitForBytesWritten` (default 30 s) when a wallpaper hangs with a full pipe; redirected log output is JSON-wrapped (`redirectedLog`) so arbitrary log text can't corrupt the frame stream or swallow ping frames ([`d1c791fb`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/d1c791fb))

**Startup & UI**
- `startup()` never called when `profiles.json` had partial load failures ([`f3323f3b`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/f3323f3b))
- Missing `raise()` calls so the main window reliably comes to the front ([`fa68328e`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/fa68328e))
- **`fi` ligature corruption** ("Conℓgure Content") on machines that also had a per-user Roboto install — two versions of the same family in one font database made Qt 6.11 shape with one file and rasterize with the other. The app font is now the Google Sans Flex variable font. Two latent deployment bugs surfaced and were fixed: `qtquickcontrols2.conf` was embedded under the QML module prefix but `QQuickStyle` only reads the resource root (its values never applied), and `assets/fonts` had no `install()` rule so release builds shipped no fonts at all ([`3c820f7c`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/3c820f7c))

**Misc**
- `GifWallpaper` search-type classification ([`9ca639c7`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/9ca639c7))
- Sentry crashpad handler path ([`09ce97c3`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/09ce97c3))
- `brand_godot.svg` rendering in the Qt SVG renderer ([`996b4a67`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/996b4a67))
- CI: accidentally tracked `qqcoro` gitlink removed ([`dfa70327`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/dfa70327))

#### Changed
- **`LineHandle` reworked to a seconds-based model** — `endSeconds` is the source of truth (x/`timeString` derived); 15-minute snap with Shift for 1-minute steps, hover time label, and `minSectionSeconds` (300 s) enforced in `moveTimelineAt`/`addTimelineAt` and mirrored as QML drag bounds; invalid/zero-length sections rejected on profile load ([`463f70a5`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/463f70a5))
- **`App` converted to a `QML_SINGLETON` `create()` factory** with `CONSTANT` read-only subsystem properties, dropping the old setter boilerplate ([`463f70a5`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/463f70a5))
- **Video and Godot wallpaper settings panels normalized** — uniform compact spacing/margins, combo boxes aligned with their labels, and the wallpaper title moved out of the scrolling card into a fixed header ([`4a2156f5`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/4a2156f5), [`cd27cc84`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/cd27cc84))
- **Qt** updated 6.10.2 → **6.11.1**; FFmpeg Windows 8.1 → 8.1.1; `aqtinstall` pulled from git (releases lag behind Qt versions); `requires-python` bumped to ≥ 3.10 ([`f1c89742`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/f1c89742))
- **chuck_tester** switched from `FetchContent` to a **git submodule** so the framework is editable in-tree (`git submodule update --init` after cloning) ([`36cf3503`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/36cf3503))
- **vcpkg** updated to 19.04.2026 ([`64501b85`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/64501b85))
