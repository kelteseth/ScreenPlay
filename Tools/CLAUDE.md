# Tools — build & dev scripts

Python tooling for ScreenPlay. Managed with **`uv`** (deps in the root
`pyproject.toml`); run scripts as `uv run Tools/<script>.py`. `requires-python
>= 3.10`. Every script carries the SPDX license header.

## Setup & build

| Script | Purpose |
|---|---|
| `setup.py` | One-time environment setup: fetch Qt via `aqtinstall`, ffmpeg, vcpkg deps, Godot. Flags like `--skip-aqt` skip steps. Rich TUI output. Cross-platform. |
| `setup_linux.sh` | Linux-only prerequisite step: installs system packages (`sudo apt`, skipped under `CI`) and clones third-party dependencies. Run before `setup.py` on Linux. |
| `build.py --preset <name>` | Configure + build + (for deploy presets) package. Reads the CMake preset and derives the version from the latest git tag. |
| `build_godot.py`, `setup_godot.py` | Godot native wrapper build / setup. |
| `download_ffmpeg.py`, `download_util.py` | Streaming downloads with real progress bars and a proper User-Agent (raw `urllib` is blocked by bot protection). |
| `build_and_publish_steam.py`, `ci_release.py` | CI/release automation — normally only run in CI. |

## Pre-commit checks — run these before committing

The format checks **reformat files in place by default**; pass `--check` (or
`-c`) to only verify without writing, and `--stage-only` (`-s`) to limit to
git-staged files.

```bash
uv run Tools/check_format_cpp.py       # clang-format C/C++
uv run Tools/check_format_qml.py       # qmlformat QML
uv run Tools/check_format_cmake.py     # cmake-format
uv run Tools/check_license_header.py   # verify SPDX header on every source file
uv run Tools/check_moc_includes.py     # verify each QObject .cpp #includes its moc
```

- `check_moc_includes.py -a` / `--addinclude` inserts the missing
  `#include "moc_*.cpp"` automatically instead of only reporting.
- `check_license_header.py` expects the header in the first or second line:
  `SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only`.
- Shared helpers live in `format_util.py`, `execute_util.py`, `rich_console.py`,
  `util.py` — reuse them rather than re-implementing file discovery, threaded
  execution, or console output.
