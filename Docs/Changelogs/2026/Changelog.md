# Changelog 2026

Period: 2026-01-01 – present
Version: v1.0.0 (in progress, branch: revise-steam-workshop)

---

## Added

- **Steam Workshop overhaul** — new upload/update flow, endless scrolling, modern grid UI with animations matching InstalledView ([`3ccecccc`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/3ccecccc), [`42ba6a60`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/42ba6a60), [`55c79b8c`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/55c79b8c), [`e4968b98`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/e4968b98))
- **`MaterialGridView`** — reusable grid component with sensible scroll defaults ([`df0c17d8`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/df0c17d8))
- **Steam profile & workshop item page** — dedicated profile page with uploaded items ([`6fd7375e`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/6fd7375e), [`9aaf7568`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/9aaf7568))
- **Gold shine effect on new workshop content** ([`7b5c5e2c`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/7b5c5e2c))
- **Clickable creator search** — click a creator name to filter by author ([`23b78231`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/23b78231))
- **Sidebar subscribe UX and tag empty-state** improvements ([`23b78231`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/23b78231))
- **Godot project file system watcher** — auto-reloads Godot project on file changes ([`c8dd938e`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/c8dd938e))
- **Qt source setup** added to CI for improved debugging symbols ([`cc33cc53`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/cc33cc53))
- **VSCode `natvis`** Qt type visualisers for the debugger ([`0efae1e9`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/0efae1e9))
- **Workshop upload test** ([`96f82efc`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/96f82efc))
- **Background blur item** reusable component ([`d3cacb61`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/d3cacb61))
- **Controls config** for Godot wallpapers ([`be932d14`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/be932d14))
- **`FireworksEffect`** — reusable particle fireworks component added to `ScreenPlayCore` (MIT, based on [Partycles](https://jonathanleane.github.io/partycles/))
- **Upload completion fireworks** — Workshop upload page shows a fireworks celebration and completion message once all items finish uploading

## Changed

- **Sidebar refactored** to use a single unified struct ([`e450ae9c`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/e450ae9c))
- **`SteamWorkshopView` converted** from Flickable + GridView to a pure GridView ([`c14ac273`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/c14ac273))
- **`SteamWorkshopItem` animations** aligned with InstalledView card style ([`96d922d0`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/96d922d0))
- **Icons moved into ScreenPlayCore** module ([`760e47c1`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/760e47c1))
- Sidebar background blur revised ([`c36fd08d`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/c36fd08d))
- **Delete workshop item dialog** stays open with a spinner while the Steam API call is in flight (can take up to 10 s); closes and pops the page only on success
- **Workshop item detail page** coerces all Steam `publishedFileID` comparisons to `String` to avoid JS float precision loss for 64-bit IDs
- **Sentry CLI updated** to 3.2.1 in CI ([`bb5e55b0`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/bb5e55b00ff7c03499fa5a08011976d2971234c4))

## Fixed

- Godot wallpaper startup ignoring user settings (used hardcoded defaults) ([`f3a05cc4`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/f3a05cc4), [`faf843f0`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/faf843f0))
- Steam account profile image not loading on profile page ([`834ff988`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/834ff988))
- Workshop list not cleared before a new request ([`2c71d8b0`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/2c71d8b0))
- Missing `godotRenderingDriver` CMake entry ([`faf843f0`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/faf843f0))
- Icon colour inconsistencies ([`22a217c9`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/22a217c9))
- File system watcher missing workshop downloads ([`7b5c5e2c`](https://gitlab.com/kelteseth/ScreenPlay/-/commit/7b5c5e2c))
