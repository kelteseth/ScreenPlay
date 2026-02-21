# Changelog 2026

Period: 2026-01-01 – present
Version: v1.0.0 (in progress, branch: revise-steam-workshop)

---

## Added

- **Steam Workshop overhaul** — new upload/update flow, endless scrolling, modern grid UI with animations matching InstalledView ([`3ccecccc`](3ccecccc), [`42ba6a60`](42ba6a60), [`55c79b8c`](55c79b8c), [`e4968b98`](e4968b98))
- **`MaterialGridView`** — reusable grid component with sensible scroll defaults ([`df0c17d8`](df0c17d8))
- **Steam profile & workshop item page** — dedicated profile page with uploaded items ([`6fd7375e`](6fd7375e), [`9aaf7568`](9aaf7568))
- **Gold shine effect on new workshop content** ([`7b5c5e2c`](7b5c5e2c))
- **Clickable creator search** — click a creator name to filter by author ([`23b78231`](23b78231))
- **Sidebar subscribe UX and tag empty-state** improvements ([`23b78231`](23b78231))
- **Godot project file system watcher** — auto-reloads Godot project on file changes ([`c8dd938e`](c8dd938e))
- **Qt source setup** added to CI for improved debugging symbols ([`cc33cc53`](cc33cc53))
- **VSCode `natvis`** Qt type visualisers for the debugger ([`0efae1e9`](0efae1e9))
- **Workshop upload test** ([`96f82efc`](96f82efc))
- **Background blur item** reusable component ([`d3cacb61`](d3cacb61))
- **Controls config** for Godot wallpapers ([`be932d14`](be932d14))

## Changed

- **Sidebar refactored** to use a single unified struct ([`e450ae9c`](e450ae9c))
- **`SteamWorkshopView` converted** from Flickable + GridView to a pure GridView ([`c14ac273`](c14ac273))
- **`SteamWorkshopItem` animations** aligned with InstalledView card style ([`96d922d0`](96d922d0))
- **Icons moved into ScreenPlayCore** module ([`760e47c1`](760e47c1))
- Sidebar background blur revised ([`c36fd08d`](c36fd08d))

## Fixed

- Godot wallpaper startup ignoring user settings (used hardcoded defaults) ([`f3a05cc4`](f3a05cc4), [`faf843f0`](faf843f0))
- Steam account profile image not loading on profile page ([`834ff988`](834ff988))
- Workshop list not cleared before a new request ([`2c71d8b0`](2c71d8b0))
- Missing `godotRenderingDriver` CMake entry ([`faf843f0`](faf843f0))
- Icon colour inconsistencies ([`22a217c9`](22a217c9))
- File system watcher missing workshop downloads ([`7b5c5e2c`](7b5c5e2c))
