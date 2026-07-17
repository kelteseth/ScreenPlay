# ScreenPlay UI Tests — master list

Every UI test we have or plan, driven against the production `ScreenPlayApp.exe`
via chuck_tester (`--tester-port`, see `python/conftest.py`). Categories follow
the main navigation bar, plus the app shell around it and the wallpaper runtime.

Update the **Status / Test** columns when adding tests — this file is the
single source of truth for what is covered.

Legend:
- **P0** — smoke, must never break, first CI wave
- **P1** — core user flows, second wave
- **P2** — nice to have / needs extra infra (fixtures, ffmpeg, Godot)
- Status: ✅ implemented · — planned
- Test column: file / test function in `python/`
- Names in `code style` are QML `objectName`s; "needs objectName" = QML edit required first.

## 0. App shell & startup

| # | Test | Prio | Status | Test |
|---|------|------|--------|------|
| 0.1 | App starts, main window visible, title contains version + feature level | P0 | ✅ | `test_app_basic.py::test_window_opens_and_each_nav_tab_shows_its_page` |
| 0.2 | Nav tabs switch: Settings / Community / Create / Installed | P0 | ✅ | `test_app_basic.py::test_window_opens_and_each_nav_tab_shows_its_page` |
| 0.3 | Second instance guard: launch without `--tester-port` while app runs exits -5 | P1 | — | pure `subprocess` test |
| 0.4 | Close with no active content → app exits | P0 | — | |
| 0.5 | Close with active wallpaper → exit dialog; Cancel keeps app alive | P1 | — | `ExitPopup` needs objectNames |
| 0.6 | "Always minimize to tray" → close hides window instead of dialog | P2 | — | |
| 0.7 | Theme switch Dark/Light applies to window (Material.theme) | P1 | — | via Settings combo |

## 1. Installed (default tab — main surface)

| # | Test | Prio | Status | Test |
|---|------|------|--------|------|
| 1.1 | Installed list loads, `gridView` count > 0 | P0 | ✅ | `test_app_basic.py::test_installed_content_lists_and_click_opens_drawer` |
| 1.2 | Search field filters the grid (hit + no-hit) | P1 | — | search TextField needs objectName |
| 1.3 | Filter tabs All / Scenes / Videos / Widgets | P1 | — | tab buttons need objectNames |
| 1.4 | Sort order toggle flips first item | P2 | — | `btnSortOrder` needs objectName |
| 1.5 | Click item → `installedDrawer` opens | P0 | ✅ | `test_app_basic.py::test_installed_content_lists_and_click_opens_drawer` |
| 1.6 | Context menu: Open folder / Remove item (count decreases) | P1 | — | `installedItemContextMenu`, `removeItem` |
| 1.17 | Example content ships, is enum-tagged and individually addressable | P0 | ✅ | `test_example_content.py::test_example_content_is_shipped_and_addressable` |
| 1.18 | Example content is read-only (no Remove action; C++ deinstall guard) | P0 | ✅ | `test_example_content.py::test_example_content_has_no_remove_action` |
| 1.7 | Select monitor + Launch → wallpaper starts (quick actions enable) | P0 | ✅ | `test_app_basic.py::test_wallpaper_starts_and_close_all_stops_it` |
| 1.8 | Launch each content type: video / QML / HTML / website | P1 | — | one fixture per type; Godot P2 |
| 1.9 | Widget launch → widget counter > 0, process alive | P1 | — | |
| 1.10 | Timeline: split / select / drag handle / remove | P0 | ✅ | `test_timeline_basic.py::test_realistic_user_flow` |
| 1.11 | Timeline: duplicate-position split rejected | P0 | ✅ | `test_timeline_basic.py::test_duplicate_position_is_rejected` |
| 1.12 | Timeline: removing active section activates expanding neighbor | P0 | ✅ | `test_timeline_fixes.py::test_remove_active_section_activates_expanding_neighbor` (+ `test_remove_first_section_keeps_survivor_active`) |
| 1.13 | Timeline: reset restores single 00:00–23:59 section | P0 | ✅ | exercised as fixture in every timeline test |
| 1.14 | Timeline sections switch content over time (short sections around *now*) | P2 | — | slow, own marker |
| 1.15 | Moving a section boundary across the current time starts/stops the wallpaper (filled + empty sections, verified via `runningWallpapers`) | P0 | ✅ | `test_timeline_scheduling.py::test_moving_section_boundary_across_now_starts_and_stops_wallpaper` |
| 1.16 | Complex flow: build 4 sections, two different example wallpapers (A, B), oscillate the boundary across *now* to force repeated A↔B swaps, then delete-takeover | P0 | ✅ video-to-video / ⚠️ xfail video-to-html | `test_timeline_basic.py::test_complex_timeline_editing_flow` — parametrized. **video-to-video passes**; **video-to-html reproduces a bug**: the HTML wallpaper's rapid restart fails and it's then permanently dropped from the section (TODO_async_review items 3/6) |

## 2. Quick actions (nav bar)

| # | Test | Prio | Status | Test |
|---|------|------|--------|------|
| 2.1 | Buttons disabled with no active content, enabled after launch | P0 | ✅ | `test_app_basic.py::test_wallpaper_starts_and_close_all_stops_it` |
| 2.2 | Mute all toggles wallpaper audio (`miMuteAll`) | P1 | — | |
| 2.3 | Pause all toggles playback (`miStopAll`) | P1 | — | |
| 2.4 | Close all → buttons disabled again, no orphan wallpaper processes | P0 | ✅ (button state) / — (process check) | `test_app_basic.py::test_wallpaper_starts_and_close_all_stops_it` |

## 3. Configure Content (ContentSettingsView)

| # | Test | Prio | Status | Test |
|---|------|------|--------|------|
| 3.1 | `miConfig` opens the view when content is active | P0 | — | |
| 3.2 | Per-monitor wallpaper listed with preview/name | P1 | — | |
| 3.3 | Video controls: volume / fill mode apply to running wallpaper | P1 | — | |
| 3.4 | Second Timeline instance works (settings `objectNamePrefix`) | P1 | — | same helpers, different prefix |
| 3.5 | Remove single wallpaper from one monitor | P1 | — | |

## 4. Create

| # | Test | Prio | Status | Test |
|---|------|------|--------|------|
| 4.1 | Tab opens, `wizardsListView` shows wizards | P0 | — | |
| 4.2 | QML wallpaper wizard: name → `btnSave` → Installed count +1 | P1 | — | cheapest real creation flow |
| 4.3 | HTML wallpaper / widget / website wizards create items | P1 | — | website via local URL, no network |
| 4.4 | GIF / Godot wizards validate required fields (save disabled until valid) | P2 | — | |
| 4.5 | Import video & convert (tiny mp4 fixture, exercises ffmpeg) | P2 | — | slow, own marker |
| 4.6 | Wizard-created content removed again via context menu | P1 | — | cleanup doubles as remove test |

## 5. Workshop (Steam only)

| # | Test | Prio | Status | Test |
|---|------|------|--------|------|
| 5.1 | Workshop tab enabled ⇔ build is Steam (title says platform) | P0 | ✅ | `test_app_basic.py::test_window_opens_and_each_nav_tab_shows_its_page` |
| 5.2 | Steam build: subscribe/download flow | — | — | out of scope |

## 6. Community

| # | Test | Prio | Status | Test |
|---|------|------|--------|------|
| 6.1 | Tab opens without error (must pass offline) | P0 | ✅ | `test_app_basic.py::test_window_opens_and_each_nav_tab_shows_its_page` |
| 6.2 | Newsfeed populates when network available | P2 | — | skip-if-offline marker |

## 7. Settings

| # | Test | Prio | Status | Test |
|---|------|------|--------|------|
| 7.1 | Tab opens, view renders | P0 | ✅ | `test_app_basic.py::test_window_opens_and_each_nav_tab_shows_its_page` |
| 7.2 | All four sections render (General / Wallpaper / Godot / License) | P0 | — | section headers need objectNames |
| 7.3 | Theme combo changes window theme | P1 | — | ties into 0.7 |
| 7.4 | "Start wallpaper muted" persists across app restart (isolated appdata) | P1 | — | restart = second harness spawn |
| 7.5 | Fill mode / graphics API combos persist | P2 | — | |
| 7.6 | Godot FPS / scaling settings persist | P2 | — | |
| 7.7 | License section shows version + feature level | P0 | — | CI needs Pro `license.json` |
| 7.8 | Language combo changes a known label | P2 | — | |

## 8. Wallpaper runtime & process hygiene (cross-cutting)

| # | Test | Prio | Status | Test |
|---|------|------|--------|------|
| 8.1 | Zero `ScreenPlayWallpaper`/`Widget` processes remain after a session | P0 | — | session-scoped autouse fixture |
| 8.2 | Kill wallpaper process externally → app recovers, no crash | P1 | — | guards async-review items (3)/(8) |
| 8.3 | App exit with running wallpaper cleans up children | P0 | — | |
| 8.4 | Profile round-trip: launch, quit, relaunch → wallpaper auto-restores | P1 | — | |

---

## Running locally

```bash
# build ScreenPlayApp first (Build/MSVC_Release), then:
cd ScreenPlay/tests/ui/python
uv run pytest -v                    # everything
uv run pytest test_app_basic.py -v  # basic smoke wave only
```

The harness spawns one app process per test with `--tester-port=<free port>`
and `--isolated-appdata` (profiles/logs sandboxed; the machine's Pro
`license.json` is mirrored in; the content storage path is shared).

## CI integration (Windows shell runner, GUI)

Planned job (goes into `.gitlab/ci/check_jobs.yml` or its own `test` stage):

```yaml
ui_tests_windows:
  stage: check
  extends: [.base_windows_build]   # runner tag `windows` must be the GUI shell runner
  script:
    - uv run python Tools/build.py --preset=windows-release
    - cd ScreenPlay/tests/ui/python
    - uv sync
    - uv run pytest -v --junitxml=report.xml
  variables:
    SCREENPLAY_APP: $CI_PROJECT_DIR/Build/MSVC_Release/ScreenPlay/ScreenPlayApp.exe
  artifacts:
    when: always
    reports: { junit: ScreenPlay/tests/ui/python/report.xml }
    paths: [ ScreenPlay/tests/ui/python/screenshots/ ]
```

Runner prerequisites (one-time machine setup):
1. **Interactive desktop session** — the shell runner must run in a logged-in
   GUI session (not session 0), otherwise wallpaper windows fail.
2. **Pro license** — `license.json` in `%LOCALAPPDATA%/ScreenPlay/ScreenPlay/`;
   `--isolated-appdata` mirrors it into the sandbox.
3. **Seeded content** — the content path lives in the registry and is shared.
   Commit small fixtures (video, QML, HTML wallpaper, widget) and add a
   `--storage-path` CLI override so runs are machine-independent (1.1 asserts
   count > 0 and fails without content).
4. **Hygiene guard** — kill stray ScreenPlay processes before/after the run (8.1).

Suite conventions:
- `pytest-timeout` (~120 s/test) so a hung `wait` can't stall the pipeline.
- Markers: `slow` (1.14, 4.5), `network` (6.2), `steam` (5.x).
- On failure capture `app.screenshot()` + `app.tree()` as artifacts.
- Rollout: wave 1 = P0, wave 2 = P1, wave 3 = P2 once fixtures exist.
