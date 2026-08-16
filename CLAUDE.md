# ScreenPlay

Cross-platform (Windows / macOS / Linux) app for **video wallpapers & widgets**,
written in **modern C++23 / Qt 6 / QML**. Hosted on GitLab
(`kelteseth/ScreenPlay`); dual-licensed **AGPL-3.0-only OR a commercial license**
(`LicenseRef-EliasSteurerTachiom`).

> The git repository root is `C:\Code\cpp\ScreenPlay\ScreenPlay` (the outer
> `C:\Code\cpp\ScreenPlay` is not a repo). Run git/glab/cmake from the inner path.

## Architecture

ScreenPlay is a **multi-process** app: a manager GUI launches separate wallpaper
and widget processes and talks to them over an SDK/IPC channel.

| Module | Role |
|---|---|
| `ScreenPlay/` | Main manager GUI (the app the user opens); timeline, monitors, settings, `main.cpp`, `App` singleton |
| `ScreenPlayCore/` | Shared C++/QML core, QML module `ScreenPlayCore` (commonly imported `as SPCore`): icons, `MaterialGridView`, reusable components, `Util`. Also the Qt-free IPC frame reassembly lib |
| `ScreenPlaySDK/` | Wallpaper/widget ↔ manager IPC (`ScreenPlaySDK`, `SDKConnection`) |
| `ScreenPlayWallpaper/` | The wallpaper render process — video / QML / HTML / GIF / Godot backends |
| `ScreenPlayWidget/` | The widget render process |
| `ScreenPlayWorkshop/` | Steam Workshop browse/upload/profile (`SteamSDK`, `steam_appid.txt`) |
| `ScreenPlayWeather/`, `ScreenPlaySysInfo/`, `ScreenPlayShader/` | Bundled widget / effect modules |
| `Tools/` | Python build & dev tooling — see `Tools/CLAUDE.md` |
| `ThirdParty/` | Vendored / submoduled deps (Steam, Godot, ffmpeg, QArchive, qqcoro, chuck_tester) |

`ScreenPlayCore/`, `ScreenPlaySDK/`, `ScreenPlayWallpaper/`, and
`ScreenPlayWorkshop/` each have their own `CLAUDE.md` with the module's
architecture and traps — read it before working in that module.

## Build

Prerequisites are installed by the setup scripts: Qt (via `aqtinstall` into
`../aqt/6.11.1/msvc2022_64`), vcpkg dependencies, ffmpeg, and optionally Godot.
Python tooling is managed with **`uv`** (`pyproject.toml` + `uv.lock`).

```bash
uv run Tools/setup.py                      # one-time: fetch Qt, ffmpeg, deps
cmake --preset windows-msvc-debug          # configure  (Build/MSVC_Debug/)
cmake --build --preset windows-msvc-debug  # build
```

On **Linux**, first run `Tools/setup_linux.sh` — it installs the system packages
(via `sudo apt`, skipped under `CI`) and clones the third-party dependencies,
then use `uv run Tools/setup.py` for Qt/ffmpeg and the `linux-*` CMake presets.

CMake presets live under `CMake/Platform/<OS>/`. Common Windows presets:
`windows-msvc-debug`, `windows-msvc-relwithdebinfo`, `windows-msvc-release`,
`windows-clang-{debug,profile,release}`, and `windows-deploy-release` (Steam +
deploy, tests off). All dev presets inherit `config-develop`.

For a packaged/deploy build use the wrapper, which drives the deploy preset and
versioning from the latest git tag:

```bash
uv run Tools/build.py --preset windows-deploy-release
```

## Testing

- **Unit tests** — `tst_*` targets (e.g. `tst_timeline`, `tst_external_process`,
  `tst_sdk`, `tst_GifOptimizer`) built when `SCREENPLAY_TESTS=ON` (on for dev
  presets). Run via `ctest` in the build dir.
- Pass `--isolated-appdata` to sandbox `profiles.json`/logs to Qt test-mode dirs
  so a test run never destroys the user's real profile.
- **UI / integration tests** use the **chuck_tester** framework (WebSocket UI
  automation, `--tester-port`; Python suites under `ScreenPlay/tests/ui/python`).
  See the `chuck-testing`, `chuck-repair`, `chuck-visual`, and
  `tester-integration` skills.

## Conventions

- **Every** C/C++/QML/CMake/Python source file starts with the SPDX header:
  `SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only`
  (comment-style per language). Checked by `Tools/check_license_header.py`.
- Formatting: `clang-format` (C++), `qmlformat` (QML), `cmake-format` (CMake).
  Run the checks in `Tools/` before committing — see `Tools/CLAUDE.md`.
- QML: this repo uses Qt 6 conventions (no import version numbers, `SPCore`
  module, `Layout.*` sizing). The Qt skills below enforce these.
- Changelogs follow a strict user-facing / net-effect convention — see
  `Docs/Changelogs/CLAUDE.md` and the `changelog` skill.

## Commits

- **Before committing, check the changelog is up to date.** Use `glab` to find
  which MR the current branch belongs to
  (`glab mr list --source-branch "$(git branch --show-current)"`), then verify
  that MR's section in `Docs/Changelogs/<year>/Changelog.md` covers the change
  being committed — add a new entry or amend the existing one as needed (follow
  `Docs/Changelogs/CLAUDE.md`). Don't commit code changes with a stale changelog.
  Also check the MR description still reads soundly for the change (overall
  summary + biggest points; view it with `glab mr view <id>`), and update it if
  the change makes it stale.
- **Run the format checks before committing.** CI's `check` stage fails on
  unformatted code — it runs `check_format_cmake.py`, `check_format_cpp.py`, and
  `check_format_qml.py` with `--check` (Clang 21). Run them locally first (see
  `Tools/CLAUDE.md`) so the pipeline stays green.
- **Never commit without explicit confirmation.** Draft the message, show it,
  and wait for a go-ahead before running `git commit` (same for amending or
  rewriting history).
- **Never mention or advertise Claude/AI** in commit messages or PR/MR bodies —
  no `Co-Authored-By: Claude`, no "Generated with…" trailers.
- Use plain, easy language. Short simple subject, lean body, no fuzz.

## Skills available for this repo

- **Qt (Qt Company plugin `qt-development-skills`)** — `qt-qml` (QML best
  practices when writing/refactoring QML), `qt-cpp-review` / `qt-qml-review`
  (lint & deep review), `qt-cpp-docs` / `qt-qml-docs`, `qt-qml-test`,
  `qt-cmake-project`. Prefer these when producing or reviewing Qt C++/QML.
  Also bundles the Qt Documentation MCP for live API lookups.
- **Project skills** — `changelog` (Docs/Changelogs), `chuck-testing` /
  `chuck-repair` / `chuck-visual` / `chuck-mcp-explorer` (integration tests),
  `tester-integration`.
