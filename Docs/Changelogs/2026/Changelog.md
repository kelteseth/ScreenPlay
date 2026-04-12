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
- Clickable creator name to filter by author ([`23b78231`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/23b78231))
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
- GifOptimizer — adaptive two-phase GIF encoder for Steam 1 MB preview limit ([`d8b69d9b`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/d8b69d9b))
- GIF wallpaper preview extraction on import ([`7e438315`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/7e438315))
- GIF wallpaper preview fallback and MSVC 18 support ([`050f786f`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/050f786f))
- MJPEG video codec enum, H265 string-to-enum mapping fix ([`016994a9`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/016994a9))
- Video info panel, preview player, codec selection QML improvements ([`7c7ff87b`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/7c7ff87b))
- AV1 test content (shapes wallpaper) and LFS tracking for `.mkv` files ([`7da67080`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/7da67080))

**Shared components**
- `MaterialGridView` — reusable grid with scroll defaults and remove/displaced transitions ([`df0c17d8`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/df0c17d8))
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

**UI & rendering**
- Image stretching fixed with `PreserveAspectCrop`
- Background blur during scrolling and `StackView` navigation ([`c36fd08d`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/c36fd08d))
- Icon colour inconsistencies ([`22a217c9`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/22a217c9))
- Prevent duplicate page pushes in `ScreenPlayMain`

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

**Build & dependencies**
- FFmpeg upgraded to 8.1, Windows download switched to full 7z archive ([`3094aa90`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/3094aa90))
- CMake preset inherits reordered so `config-develop` overrides build-type defaults ([`852a6ae1`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/852a6ae1))
- vcpkg updated and doctest added ([`2d0f77ad`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/2d0f77ad))
- Sentry CLI updated to 3.2.1 in CI ([`bb5e55b0`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/bb5e55b00ff7c03499fa5a08011976d2971234c4))

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
