# Async / IPC Review Findings (QML ↔ ScreenPlay ↔ Wallpaper)

Review date: 2026-06-12 (branch `fix/timeline-and-process-bugs`)

## Status (2026-06-12)

All findings below are FIXED unless marked open:

- (1) IPC framing → `ScreenPlayCore::IpcFrameBuffer` (brace-matched JSON +
  legacy `;` text frames, partial-read buffering); logs JSON-wrapped as
  `redirectedLog`. tst_sdk covers coalesced/split/`;`-in-string/log frames.
  The class is Qt-free (std:: types, target `ScreenPlayCoreIpcLib`) and is
  compiled into the Godot GDExtension too: `read_from_pipe()` now returns
  one reassembled frame per call (GDScript drains in a loop) instead of one
  raw pipe read, and the unframed `ECHO_FROM_GODOT:` debug write-back (which
  corrupted the app-side stream) is gone. A WebSocket transport was
  considered and rejected: Godot's networking is broken on machines without
  an IP address, and a wallpaper must work fully offline.
- (2) destroy-during-emission → removals deferred via `QTimer::singleShot(0)`
  (widget ClosedGracefully, wallpaper restartFailed, unconnected-client prune).
- (3) transition failures → broken wallpapers are dropped
  (`dropWallpaperFromSection`, `startSectionWallpapers`), transition always
  completes; Failed sections recover to Inactive; no-Active self-heal in tick.
- (4) startup always starts the rotation timer (scope guard).
- (5) `isRunning` optional value respected (`!running.value_or(false)`).
- (6) content-timer suspend counter (`suspendContentTimer`/`resumeContentTimer`).
  OPEN: full serialization of concurrent QML-triggered mutations (busy
  flag/queue) — the counter only fixes the timer restart race.
- (7) removeTimelineAt/moveTimelineAt identifier-guarded + bounds-checked;
  QML passes identifier and resolves the section by object after the await.
- (8) orphan processes → `ScreenPlayExternalProcess::terminate()` fallback in
  wallpaper/widget close(), failed starts, and Failed-section recovery;
  unconnected clients pruned on disconnect. The connect wait in
  `startWallpaper` is adaptive (keyed to the child PID via
  `ScreenPlayExternalProcess::processState()`): a dead process fails within
  500 ms instead of burning the old fixed 2.5 s window, and a slow-loading
  wallpaper gets up to 30 s while its process is alive - so a late
  connection is no longer dropped in newConnection. The 30 s cap only reaps
  alive-but-hung processes (terminated by the failed-start path).
- (9) `sendMessage` no longer blocks (write+flush, no waitForBytesWritten).
- (10) removing the active section activates `timelineAfter->index`
  (the section that expands), not `index - 1`. UI regression test:
  tests/ui/python/test_timeline_fixes.py.
- Low items: `.arg` field-width bugs, parse-error `return`→`continue`,
  duplicate ping-timer connects, missing `break`, IIFE lambda coroutines →
  member coroutines, `pingAlive` → `disconnected()`, never-connected close()
  result/state consistency — all fixed.
- Also fixed while testing: `removeTimelineAt` rejected removing an
  *Inactive* current-time section (pre-existing time-of-day-dependent
  failure in tst_timeline).

## High — likely real-world crashes or stuck states

### 1. IPC messages have no framing; coalesced or split packets are silently dropped
- `SDKConnection::sendMessage` (ScreenPlay/src/sdkconnection.cpp:110) writes compact JSON
  with **no `;` terminator**, but the wallpaper side splits on `;` and parses each chunk
  as a single JSON document (ScreenPlaySDK/src/screenplaysdk.cpp:114).
- `ScreenPlayWallpaper::syncAllProperties()` (ScreenPlay/src/screenplaywallpaper.cpp:402-429)
  sends many messages back-to-back → they coalesce into `{...}{...}{...}` → parse error →
  all settings silently dropped.
- `ScreenPlaySDK::redirectMessage` (screenplaysdk.cpp:188) writes arbitrary unframed log text
  into the same socket — can eat `ping;` frames (widgets DO consume `pingAliveReceived`).
- Neither side buffers partial reads: a large `replace` payload split across two reads is
  dropped entirely.
- Fix: terminate every JSON frame with the delimiter (or length-prefix), buffer until a
  complete frame arrives, route log redirection through the same framing.
  Testable in tst_sdk.cpp with coalesced and split writes.

### 2. Objects destroyed synchronously while one of their own signals is mid-emission (UAF)
- Widget close chain: `SDKConnection::readyRead` emits `jsonMessageReceived` → `widgetClose`
  handler calls `setState(ClosedGracefully)` (screenplaywidget.cpp:100-107) → manager's
  `stateChanged` lambda calls `removeWidget` (screenplaymanager.cpp:269-277) → erases the
  `shared_ptr`, destroying the widget AND its member `m_connection` while
  `SDKConnection::readyRead` is still iterating/emitting.
- Wallpaper restart-failure chain: `restartFailed` emitted from the wallpaper's own
  member-timer lambda (screenplayexternalprocess.cpp:30-55); handler in
  `WallpaperTimelineSection::addWallpaper` (wallpapertimelinesection.cpp:159-169) runs
  `std::erase_if(wallpaperList, ...)` synchronously, destroying the emitting object.
- Fix: defer removals via `QMetaObject::invokeMethod(..., Qt::QueuedConnection)` or
  `QTimer::singleShot(0, ...)`.

### 3. Failed timeline transition permanently wedges the state machine
- `checkActiveWallpaperTimeline` (screenplaytimelinemanager.cpp:326-436): failure paths set
  new section → `Failed`, `co_return`, but **old section stays `Closing` forever**.
- Next tick: `Failed` branch calls `removeTimelineAt`, which fails because
  `stopTimelineAndClearWallpaperData` requires `Active` (line 1346) → infinite 250 ms retry
  loop; the stuck `Closing` section makes every future invocation bail at the
  "in transition state" check (lines 277-280). Rotation dead until app restart.
- Also: auto-deleting an entire timeline section because ONE wallpaper failed to start is
  destructive — should only drop the broken wallpaper.

### 4. Startup failure never starts the rotation timer
- `startup()` (screenplaytimelinemanager.cpp:555-570) `co_return`s before
  `m_contentTimer.start()` when `activateTimeline` fails. One missing video at boot →
  timeline never rotates the whole session. Start the timer regardless.

### 5. Dead-process detection broken: `isRunning` result ignored
- screenplayexternalprocess.cpp:104-112: `if (running.has_value()) { /* running */ }` —
  `optional<bool>` containing `false` (process dead) is treated as alive.
- Fix: `if (!running.value_or(false)) handleTimeoutOrCrash();`

## Medium — races and leaks under concurrency

### 6. `m_contentTimer` stop/start scope guards not overlap-safe; no operation serialization
- Every mutating coroutine does `m_contentTimer.stop()` + `qScopeGuard{ start() }`.
  First-finishing coroutine restarts the timer while a second is still suspended
  mid-mutation. Stopping the timer also doesn't stop an in-flight check coroutine.
- Nothing prevents two concurrent `setWallpaperAtActiveMonitorTimelineIndex` on the same
  section (double-click → duplicate `addWallpaper` for one monitor).
- Fix: nesting counter for the timer guard at minimum; better, busy-flag/queue so
  QML-triggered timeline mutations serialize.

### 7. Stale index race in QML `removeSection`
- Timeline.qml:346-372 captures `index` before awaiting `removeTimelineAt` (can suspend for
  seconds). Concurrent remove/drag reorders sections; C++ `removeTimelineAt`
  (screenplaytimelinemanager.cpp:877) validates **only index, not identifier** → wrong
  section can be removed.
- Fix: pass + verify identifier like the other APIs; disable section UI while pending
  (same pattern as `btnReset.resetting`).

### 8. Orphaned wallpaper processes
- Processes are `startDetached` (screenplaywallpaper.cpp:210, screenplaywidget.cpp:55).
- `ScreenPlayWallpaper::close()` with no connection returns success without killing the
  detached process (screenplaywallpaper.cpp:228-232). `ProcessManager::terminateProcess`
  exists but is never used as fallback.
- Wallpaper connecting after the 2.5 s window in `startWallpaper`
  (screenplaytimelinemanager.cpp:1540-1565) is dropped in `newConnection` → orphan.
- `m_unconnectedClients` entries never pruned when a socket disconnects before sending
  `appID` — slow leak on a `WorldAccessOption` socket anyone can connect to.
- Note: with `startDetached`, `processExit`/`handleProcessError`
  (screenplayexternalprocess.cpp:23-26) are dead code — they never fire.

### 9. Blocking waits on the GUI thread
- `SDKConnection::sendMessage` → `waitForBytesWritten()` default **30 s** timeout
  (sdkconnection.cpp:117); hung wallpaper with full pipe freezes UI; `syncAllProperties`
  does it N times in a row.
- `SDKConnection::close()` → `waitForDisconnected(500)` per wallpaper, sequential at
  shutdown. Make writes async or use short timeouts.

### 10. `removeTimelineAt`: wrong section started when removing the active section
- screenplaytimelinemanager.cpp (>2 sections, `removeActiveTimelineSection` path):
  `newTimelineIndex = (index == 0) ? 0 : index - 1` — but the section that expands to
  cover the current time is the **next** one (`timelineAfter->startTime = endTime`),
  which sits at `index` after `removeAt`. The code briefly starts the wallpapers of the
  section *before* the removed one; the 250 ms checker corrects it afterwards
  (start → stop → start churn). The in-code comment asserts the wrong rationale.
- Fix: use `index` (the expanded `timelineAfter`) for the active path; two-section case
  is already correct.

## Low — correctness nits

- `QString::arg(int, int)` field-width bug: screenplaymanager.cpp:355-357 and 415-419 —
  `.arg(timelineIndex, monitorIndex)` treats the 2nd int as field width; `%2` never
  substituted (419 also has the values semantically swapped).
- sdkconnection.cpp:96-97: `return` on JSON parse error drops all remaining messages in the
  packet — should be `continue` (ping branch was already fixed this way).
- Duplicate ping-timer connections: `setupSDKConnection`
  (screenplayexternalprocess.cpp:103-114) connects a fresh lambda to the member
  `m_pingAliveTimer` on every reconnect → N handlers after N restarts. Use
  `Qt::UniqueConnection` or connect once in constructor.
- `newConnection` match loop (screenplaymanager.cpp:562-567): missing `break` after erase —
  skips next element, keeps comparing moved-from entries. Benign today; add `break`.
- IIFE capturing-lambda coroutines (`QCoro::QmlTask([this,...]() -> Task {...}())`):
  closure object dies at end of full expression while coroutine may be suspended. Currently
  safe ONLY because all captures are read before the first `co_await`. Safer: private member
  coroutine functions taking args by value.
- `ScreenPlaySDK::pingAlive` (screenplaysdk.cpp:199-208) emits `sdkDisconnected` directly
  without resetting `m_isConnected` via `disconnected()` → inconsistent state.
- `ScreenPlayWallpaper::close()` never-connected case: sets state `ClosingFailed` but
  returns `success = true` — pick one story (distinct "never connected, abandoned" outcome
  + process kill from finding 8).

## Suggested order

1. (1) IPC framing + (2) destroy-during-emission — both match "random rare crash /
   settings not applied" bug reports; framing is unit-testable.
2. (3) + (4) timeline state machine recovery.
3. (5) one-liner.
4. (6)/(7) concurrency guards.
5. (8)/(9) process lifecycle + blocking waits.
6. Low items opportunistically alongside the above.
