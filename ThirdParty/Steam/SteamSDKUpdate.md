# Updating the Steamworks SDK

This document describes how to update the bundled Steamworks SDK. It is
intentionally explicit — copy the listed files only; everything else in the
upstream SDK zip (sample game `steamworksexample/`, `tools/`, `glmgr/`, the
retail installer disk image, etc.) must **not** be checked in.

The current SDK version is recorded in `ScreenPlayWorkshop/SteamSDK/SteamSDK.md`
and at the top of `ScreenPlayWorkshop/SteamSDK/Readme.txt`.

## 1. Download

Sign in at <https://partner.steamgames.com/home> and download the latest
Steamworks SDK zip (e.g. `steamworks_sdk_164.zip`). Drop the zip into the repo
root. **Do not commit the zip** — it stays untracked and gets removed once
the update is finished.

## 2. Files that live in this repo

The SDK is split across two directories:

| Repo path                                                                 | Purpose                                             | Source in upstream zip                              |
| ------------------------------------------------------------------------- | --------------------------------------------------- | --------------------------------------------------- |
| `ScreenPlayWorkshop/SteamSDK/public/steam/`                               | Public headers + `steam_api.json`                   | `sdk/public/steam/` (entire tree, incl. `lib/`)     |
| `ScreenPlayWorkshop/SteamSDK/redistributable_bin/win64/steam_api64.lib`   | Windows import lib (linker)                         | `sdk/redistributable_bin/win64/steam_api64.lib`     |
| `ScreenPlayWorkshop/SteamSDK/redistributable_bin/linux64/libsteam_api.so` | Linux shared lib (linker)                           | `sdk/redistributable_bin/linux64/libsteam_api.so`   |
| `ScreenPlayWorkshop/SteamSDK/redistributable_bin/osx/libsteam_api.dylib`  | macOS dylib (linker)                                | `sdk/redistributable_bin/osx/libsteam_api.dylib`    |
| `ScreenPlayWorkshop/SteamSDK/Readme.txt`                                  | Upstream changelog (used as our reference)          | `sdk/Readme.txt`                                    |
| `ThirdParty/Steam/win64/steam_api64.dll`                                  | Windows runtime DLL (installed next to executables) | `sdk/redistributable_bin/win64/steam_api64.dll`     |
| `ThirdParty/Steam/win64/steam_api64.lib`                                  | Windows import lib (kept in sync with the one above)| `sdk/redistributable_bin/win64/steam_api64.lib`     |
| `ThirdParty/Steam/linux64/libsteam_api.so`                                | Linux runtime shared lib                            | `sdk/redistributable_bin/linux64/libsteam_api.so`   |
| `ThirdParty/Steam/osx/libsteam_api.dylib`                                 | macOS runtime dylib                                 | `sdk/redistributable_bin/osx/libsteam_api.dylib`    |

Anything outside that table — `sdk/steamworksexample/`, `sdk/tools/`,
`sdk/glmgr/`, `sdk/redistributable_bin/steam_api.{dll,lib}` (32-bit),
`sdk/redistributable_bin/{linux32,linuxarm64,androidarm64}/` — is **not used**
and must not be added.

## 3. Update procedure

### 3.1 Extract to a temp folder

Extract the SDK zip into a throwaway folder at the repo root (e.g.
`steamworks_sdk_164_extracted/`). This folder is not checked in and is
deleted at the end of the procedure.

### 3.2 Replace headers and link libs

Overwrite — do not merge. Files removed upstream must disappear here too
(e.g. `isteammusicremote.h` was dropped in 1.63).

- Replace the entire `ScreenPlayWorkshop/SteamSDK/public/steam/` tree with
  `sdk/public/steam/` from the extracted SDK (delete the old tree first so
  removed files actually go away — `lib/` and `steam_api.json` come along
  for the ride).
- Replace the three platform link libs under
  `ScreenPlayWorkshop/SteamSDK/redistributable_bin/{win64,linux64,osx}/`
  with their counterparts from `sdk/redistributable_bin/`.
- Replace `ScreenPlayWorkshop/SteamSDK/Readme.txt` with `sdk/Readme.txt`
  (this is what records the upstream version + changelog).

### 3.3 Replace runtime binaries

Replace the runtime binaries under
`ThirdParty/Steam/{win64,linux64,osx}/` with the matching files from
`sdk/redistributable_bin/`. Note that Windows needs both `steam_api64.dll`
*and* `steam_api64.lib` here; the `.lib` must be the same one used in
step 3.2.

### 3.4 Fix file encodings

A handful of upstream headers ship as cp1252. The repo expects UTF-8.

Run `convert_steam_sdk_to_utf8.py` from inside
`ScreenPlayWorkshop/SteamSDK/`. The script walks `public/steam/*.h` and
rewrites any file that fails UTF-8 decoding as UTF-8. Re-running on
already-converted files is a no-op.

### 3.5 Regenerate the Qt enum header

Run `generate_qt_enums.py` from inside `ScreenPlayWorkshop/SteamSDK/`.
It reads `public/steam/steam_api.json` and writes
`ScreenPlayWorkshop/SteamSDK/steamenumsgenerated.h`.

Copy that generated file over
`ScreenPlayCore/inc/public/ScreenPlayCore/steamenumsgenerated.h`, then
run the project's clang-format wrapper — **do not** call `clang-format`
directly; always go through `Tools/check_format_cpp.py` so the right
style is applied:

> Run `python Tools/check_format_cpp.py` from the repo root.

The intermediate `ScreenPlayWorkshop/SteamSDK/steamenumsgenerated.h` is
not checked in. `Tools/check_format_cpp.py` explicitly skips
`ScreenPlayWorkshop/SteamSDK/`, which is why the file must be copied into
`ScreenPlayCore/` *before* formatting.

### 3.6 Sync the CMake header list

`ScreenPlayWorkshop/SteamSDK/CMakeLists.txt` enumerates the public
headers. Compare it against `ScreenPlayWorkshop/SteamSDK/public/steam/`
and add or remove entries as needed (keep the list sorted
alphabetically). When a header file disappears from the SDK, its line
in `CMakeLists.txt` must go too — CMake will otherwise fail on the
missing file.

### 3.7 Bump the version notes

- Update the `Version:` line in `ScreenPlayWorkshop/SteamSDK/SteamSDK.md`.
- `ScreenPlayWorkshop/SteamSDK/Readme.txt` was already overwritten in
  step 3.2; verify its top line matches the version you just shipped.

### 3.8 Clean up

- Delete the temporary `steamworks_sdk_<ver>_extracted/` folder at the
  repo root.
- Delete `steamworks_sdk_<ver>.zip` at the repo root (optional — it's
  untracked anyway).
- Delete the intermediate
  `ScreenPlayWorkshop/SteamSDK/steamenumsgenerated.h` — only the copy
  under `ScreenPlayCore/inc/public/ScreenPlayCore/` is checked in.

## 4. Verify

1. Configure & build with `-DSCREENPLAY_STEAM=ON` on each target platform.
2. Run `tst_ScreenPlayWorkshop` and `tst_WorkshopUpload` — they exercise
   the ISteamUGC bindings and will surface signature drift.
3. Read `ScreenPlayWorkshop/SteamSDK/Readme.txt` for breaking changes in
   the upstream changelog (removed/renamed interfaces) and fix call sites
   in `ScreenPlayWorkshop/src/` accordingly. Recent examples:
   - **1.62**: `ISteamFriends::SetPersonaName` removed,
     `EMouseCursor` → `EHTMLMouseCursor`.
   - **1.61**: `RequestCurrentStats` removed; `AddTimelineEvent` →
     `AddInstantaneousTimelineEvent`.
   - **1.60**: `CSteamAPIContext` removed — use `Steam<interface>()`
     accessors.
