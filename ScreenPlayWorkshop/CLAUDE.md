# ScreenPlayWorkshop

Steam Workshop integration — browse, upload, and profile management. QML module
URI `ScreenPlayWorkshop`. App ID **`672870`**; `steam_appid.txt` lets a dev build
attach to Steam without going through the launcher.

## Structure (`src/`)

`SteamWorkshop` is the entry object, split into three `unique_ptr` sub-objects
exposed as `CONSTANT` QML properties (`steamworkshop.h`):

- **`SteamWorkshopSearch`** (`search`) — UGC browse queries; build them with
  `UgcQueryBuilder` (`ugcquerybuilder.h`) for type-safe, correct queries.
- **`SteamWorkshopProfile`** (`profile`) — your uploads / subscribed / favorites.
- **`SteamWorkshopItemOps`** (`itemOps`) — create/update/delete/subscribe.

Plus `SteamAccount`, list models (`InstalledListModel`, `UploadListModel`,
`SteamWorkshopListModel`, `InstalledListFilter`), and the image providers
(`steamimageprovider`, `steamqmlimageprovider`). `steamenumsgenerated` lives in
`ScreenPlayCore` and is generated — don't hand-edit.

## Steam SDK traps

- **Shutdown order matters.** `~SteamWorkshop` stops the poll timer, **resets the
  sub-objects, then** calls `SteamAPI_Shutdown()`. Sub-objects that outlive
  `SteamAPI_Shutdown` crash on teardown — preserve this order if you add more.
- **Callbacks need pumping.** Steam async results arrive via `STEAM_CALLBACK` +
  `SteamAPI_RunCallbacks()` on `m_pollTimer`. An async call is not done when the
  function returns — the callback is.
- **Guard concurrent queries with `QueryGuard`** (RAII, sets/clears
  `queryActive`). Call `guard.dismiss()` once the async call is in flight so the
  callback — not the destructor — owns clearing the flag.
- **64-bit IDs lose precision as numbers.** `publishedFileID`/`workshopid` are
  `uint64`: pass them as `QVariant` (parse with `toVariant()`, never `toInt()`),
  and in QML coerce `publishedFileID` to `String` before use — JS doubles can't
  hold a 64-bit ID exactly.
- Two distinct error paths surface to QML: `steamErrorRestart`
  (`SteamAPI_RestartAppIfNecessary`) and `steamErrorAPIInit` (`SteamAPI_Init`).

## Tests

`tst_ScreenPlayWorkshop` (`src/tst_ScreenPlayWorkshop.cpp`) + the upload test.
