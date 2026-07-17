# Design: Separating wallpaper state sync from timeline logic

Status: draft for review, no code changes yet (2026-07-06)
Companion to: `TODO_async_review.md` (branch `fix/timeline-and-process-bugs`)

## 0. The question this answers

> "I want the actual state sync separated from the logic. But not sure if this
> is even 100% possible. We must act depending on callbacks like → replace
> Wallpaper A with Wallpaper B."

Short answer: 100% separation of *knowledge* is possible; 100% separation of
*waiting* is not — and doesn't need to be. The trick is to split three things
that the current code treats as one:

| Concern | What it is | Where it lives |
|---|---|---|
| **Facts** | "Is the process alive? Is the socket up? What did the wallpaper last report?" | Link layer (per process) |
| **Verbs** | "Start and tell me when connected. Replace and tell me when applied. Close and tell me when gone." | Link layer (per process) |
| **Decisions** | "Monitor 2 should show B now, not A. Same runtime → replace live, else close+start." | Logic layer |

"Replace A with B" *feels* like it crosses the boundary because today it is
written as callback soup: send a message, poll a timer, watch `isConnected`,
patch the monitor model at every step. Once the link layer exposes **awaitable
verbs that complete in a terminal state**, the logic layer composes them
sequentially and the callbacks disappear from the logic layer entirely:

```cpp
// Logic layer. No sockets, no PIDs, no timers, no polling.
if (link->canReplaceLive(newData.type())) {
    co_await link->replace(newData);          // completes on ack or timeout
} else {
    co_await link->close();                    // completes when process is gone
    link = co_await m_runtime.spawn(monitor, newData); // completes on Connected
}
```

The logic layer still *waits* on process events — it must, that's its job.
But it never *knows how* the waiting works, and the link layer never *knows
why* it's being asked. That's the whole separation:

> **Layer 1 never knows *why*. Layer 2 never knows *how*.**

## 1. What's entangled today (evidence)

1. **Logic layer implements transport waiting.**
   `ScreenPlayTimelineManager::startWallpaper` polls `isConnected()` 50×50 ms
   with a `co_await timer` loop and duplicates monitor-model writes at every
   exit path (screenplaytimelinemanager.cpp:1673). `ScreenPlayWallpaper::close`
   polls process exit 30×250 ms (screenplaywallpaper.cpp:247).

2. **Transport layer needs timeline knowledge.**
   `ScreenPlayManager::newConnection` matches an incoming socket by searching
   `findStartingOrActiveWallpaperTimelineSection()->wallpaperList`
   (screenplaymanager.cpp:606). A wallpaper connecting after its section left
   `Starting` is dropped (open item 8 in TODO_async_review.md).

3. **Four partially-redundant state stores, all updated imperatively:**
   `ScreenPlayExternalProcess::m_state` + `m_isConnected`; the
   `MonitorListModel` AppState role written by hand in ~8 blocks in the
   timeline manager; `WallpaperTimelineSection::state`; the active-wallpaper
   counter. Every operation must remember to update all of them.

4. **Two liveness protocols entangled on one QTimer.**
   Wallpapers: PID poll every 3 s; the 1 s `ping;` frames they send are
   dropped. Widgets: ping receipt restarts the same timer — while the base
   class's PID-check handler is *also* connected to it, and
   `ScreenPlayWidget::setupSDKConnection` re-connects its timeout lambda on
   every reconnect (duplicate-handler bug, fixed in the base class but still
   present in the widget subclass, screenplaywidget.cpp:129).

5. **`WallpaperTimelineSection` mixes schedule data with runtime state.**
   It holds both the *plan* (time range, `WallpaperData`) and the *live
   processes* (`wallpaperList` of `ScreenPlayWallpaper`, section `State`).
   This is why every transition must carefully move process objects between
   sections (`takeScreenPlayWallpaperByMonitorIndex` + backup/restore in
   `checkActiveWallpaperTimeline`).

## 2. Target architecture

```
             desired state (pure data)                actual state (facts)
  ┌─────────────────────────────────┐        ┌────────────────────────────────┐
  │ Timeline sections               │        │ ScreenPlayExternalProcess      │
  │  = time ranges + WallpaperData  │        │  (one per wallpaper & widget)  │
  │  no processes, no State enum    │        │  - QProcess/PID, socket        │
  └───────────────┬─────────────────┘        │  - handshake, ping both ways   │
                  │                          │  - LinkState + lastReported    │
                  │                          └───────────────┬────────────────┘
        desiredFor(now)                            stateChanged / verbs
                  │                                          │
                  ▼                                          ▼
  ┌──────────────────────────────────────────────────────────────────────────┐
  │ Reconciler (logic layer, ONE serialized coroutine)                       │
  │   diff desired vs actual per monitor →                                   │
  │     absent → spawn | extra → close | changed → replace or close+spawn    │
  └──────────────────────────────────────────────────────────────────────────┘
                  ▲                    ▲                     ▲
             250 ms tick        QML mutation           link → Dead
             (time moved)      (desired changed)      (actual changed)
```

### 2.1 Layer 1: `ScreenPlayExternalProcess`, reused as the link (facts + verbs)

Not a new class: the existing base already owns the process AND
`m_connection`, so it is the de-facto link — half-finished. The design
completes it by absorbing what still lives elsewhere: `SDKConnection`'s
lifecycle, the connect-wait loop from the timeline manager, and the
handshake-match logic from the manager. The existing hierarchy is kept, which
gives widgets the identical fix for free:

```
ScreenPlayExternalProcess            // Layer 1: ALL shared facts + verbs
├── ScreenPlayWallpaper              // adds: replace(), canReplaceLive(), monitors, fillmode/volume
└── ScreenPlayWidget                 // adds: position sync; no replace, no monitors
```

```cpp
class ScreenPlayExternalProcess : public QObject {
    Q_OBJECT
public:
    enum class LinkState {
        Idle,        // constructed, not started
        Launching,   // process spawned, waiting for socket handshake
        Connected,   // socket up, content reported ready
        Closing,     // quit sent, waiting for process exit
        Closed,      // terminal: gone on our request
        Dead         // terminal: gone NOT on our request (crash/timeout/never connected)
    };

    // --- Facts (read-only, always current, safe to bind to) ---
    LinkState state() const;
    qint64 pid() const;
    ContentStatus lastReported() const;     // what the wallpaper/widget itself said
    ContentTypes::InstalledType runtime() const;

    // --- Verbs (each completes in a terminal, observable condition) ---
    QCoro::Task<Result> start();                     // → Connected, or Dead on timeout
    QCoro::Task<Result> setValue(QString key, QVariant value, QString category);
    QCoro::Task<Result> close();                     // → Closed; escalates quit→terminate internally

signals:
    void stateChanged(LinkState);              // the ONLY unsolicited output
    void statusReported(ContentStatus);        // the content's own state report
};

class ScreenPlayWallpaper : public ScreenPlayExternalProcess {
public:
    bool canReplaceLive(ContentTypes::InstalledType other) const;
    QCoro::Task<Result> replace(WallpaperData data); // → acked by wallpaper, or timeout
    // monitors(), wallpaperData() ...
};
```

(If a clearer name is wanted once the dust settles — e.g.
`ExternalContentLink` — do it as a pure cosmetic rename in a final commit,
never mid-migration; keeping the name keeps every intermediate diff small
and reviewable.)

Internals the link owns exclusively (nothing above ever sees these):
- `startDetached`, PID bookkeeping, `terminate()` escalation
- socket, `IpcFrameBuffer`, handshake appID matching (via registry, §2.4)
- liveness: **one** protocol for wallpapers and widgets — ping receipt as
  primary signal, PID poll as backstop — connected once in the constructor
- awaiting internally uses `co_await qCoro(obj, &signal, timeout)` instead of
  the current 50 ms / 250 ms poll loops

**Deliberately absent: restart logic.** Today `ScreenPlayExternalProcess`
carries retry counters, a restart-delay timer, a stability timer and an
in-progress flag. Restart-on-crash is actually *reconciliation* ("the desired
wallpaper is not running") — see §2.3. A crashed link just transitions to
`Dead` and emits. This deletes the entire restart state machine from Layer 1.

### 2.2 Layer 2a: timeline sections become pure data

`WallpaperTimelineSection` loses `wallpaperList`, `state`, and its signals.
What remains: `startTime`, `endTime`, `identifier`, `index`,
`relativePosition`, `QVector<WallpaperData>`. Add/move/remove/split of
sections become synchronous pure-data operations (no `co_await` at all —
they only mutate the plan; the reconciler notices the plan changed).
"Which section is active" stops being a stored `State` and becomes the
derived question `containsTime(now)` — the Starting/Closing/Failed section
state machine and its stuck-state recovery code disappear.

### 2.3 Layer 2b: the reconciler (the only place both worlds meet)

```cpp
// Owned by the runtime orchestrator. m_links: QMap<int /*monitor*/, ScreenPlayWallpaper*>
QCoro::Task<> reconcile() {
    // Serialized: enqueue if one is already running (strand, §3.2).
    const auto desired = m_timeline.desiredFor(QTime::currentTime());
    // desired: monitor -> WallpaperData, pure function of section data

    for (int monitor : allMonitors()) {
        auto* link = m_links.value(monitor, nullptr);
        const bool wantsWallpaper = desired.contains(monitor);
        const bool hasLiveLink    = link && link->state() == LinkState::Connected;

        if (!wantsWallpaper && hasLiveLink)        co_await closeAndDrop(monitor);
        else if (wantsWallpaper && !hasLiveLink)   co_await spawn(monitor, desired[monitor]);
        else if (wantsWallpaper && hasLiveLink
                 && link->current() != desired[monitor]) {
            if (link->canReplaceLive(desired[monitor].type()))
                co_await link->replace(desired[monitor]);
            else { co_await closeAndDrop(monitor); co_await spawn(monitor, desired[monitor]); }
        }
        // failure of any verb: record attempt count; after N failures for the
        // same (monitor, wallpaper), drop it from desired (persist) + notify UI
        // — this replaces dropWallpaperFromSection / handleWallpaperRestartFailed
        // / MAX_RESTART_ATTEMPTS in one place.
    }
}
```

Every trigger funnels into the same function:
- 250 ms tick → time moved, maybe a section boundary crossed
- QML mutation (set/remove wallpaper, move/add/remove section) → mutate the
  *data*, then request reconcile
- `link->stateChanged(Dead)` → crash/timeout → reconcile restarts it (with
  backoff tracked per monitor)

This is the direct answer to the "we must act on callbacks" worry:
**unsolicited events stop being callbacks that implement bespoke recovery
logic; they become mere triggers of one idempotent function.** The current
`checkActiveWallpaperTimeline` case explosion (old/new × has/hasn't, backup &
restore of moved process objects, Failed-section recovery, no-Active
self-heal) is what a reconciler looks like when grown organically — written
declaratively it's one diff, and the self-heal behaviors fall out for free
because reconcile always compares full desired vs. full actual.

### 2.4 Handshake registry (kills entanglement #2)

`ScreenPlayManager` keeps
`QHash<QString /*appID*/, ScreenPlayExternalProcess*> m_pending`, populated
when a link spawns. `newConnection` matches purely by appID — no timeline
queries, and a late-connecting wallpaper still finds its link. Because the
base class is the map value, wallpapers and widgets share one matching path
(today they are two separate search loops in `newConnection`).

### 2.5 UI state becomes derived, not written

`MonitorListModel` subscribes to `stateChanged`/`statusReported` of links (one
generic connection made where the link is created). The ~8 hand-written
monitor-model update blocks in the timeline manager are deleted. Same for the
active-wallpaper counter (count links in `Connected`) and
`runningWallpapers()` (serialize the link map).

### 2.6 True "exact state": the status report frame

Today `AppState::Active` means "the socket connected", i.e. what the main app
*assumes*. For the main app to know the wallpaper's *actual* state, the SDK
sends a status frame on change (and piggybacked on the 1 s ping):

```json
{ "status": { "state": "playing", "file": "x.mp4", "volume": 0.5, "fillMode": "cover" } }
```

The link caches it as `lastReported()` and this is also what makes
`replace()` awaitable-to-completion: the verb completes when a status report
arrives whose content matches the requested data (or on timeout). Cheap now
that `IpcFrameBuffer` framing exists. Reverse direction (wallpaper watches
`mainAppPID` + socket) already works and stays in the SDK.

## 3. What stays honest / known gray zones

1. **Sequencing lives in Layer 2 — by design.** "Close A then start B" is a
   decision about ordering and policy (blank monitor briefly vs. overlap).
   That's logic. What got *removed* from Layer 2 is knowledge of how closing
   and starting are detected.
2. **`canReplaceLive` is a capability question about processes** that Layer 2
   must ask. Exposing it as a link/utility query is fine; Layer 2 still
   doesn't know *why* video→video is replaceable (same runtime binary).
3. **Backoff policy** (don't respawn a crashing wallpaper in a hot loop)
   needs a home. It's policy → reconciler, tracked per (monitor, data), not
   per process object. The link stays stateless across restarts because a
   restart is a *new* link.
4. **Widgets** aren't timeline-scheduled, but they inherit the same Layer 1
   base and therefore get the unified liveness protocol, the registry
   handshake, and awaitable `close()` for free. (Today
   `ScreenPlayWidget::close()` fire-and-forgets the quit command — it never
   awaits process exit, so shutdown ordering is a guess.) Their "reconciler"
   is trivial: desired = widget list from profile; a widget link going `Dead`
   triggers the same respawn-with-backoff policy.
5. **Shutdown** = set desired to empty, reconcile once, await. Replaces the
   current `removeAllWallpaperFromActiveTimlineSections` special path.

## 4. Sanity check: the three flavors of "replace A with B"

| Case | Today | Target |
|---|---|---|
| Same runtime, section active | `replaceLive` fire-and-forget (`sendMessage` result ≠ applied); process object *moved between sections* with backup/restore | `co_await link->replace(data)` — completes on wallpaper's ack; no object moves, sections are data |
| Different runtime, active | `stopWallpaper` → poll close → `startWallpaper` → poll connect, monitor model patched at 6 points | `co_await link->close(); co_await spawn(...)` — model updates itself from state changes |
| Section inactive | `setWallpaperData` guarded by `isConnected()` | pure data edit, reconciler no-ops because section ∌ now |

## 5. Migration order (each step shippable, tests stay green)

1. **Status-report frame in SDK + link cache** — additive protocol change,
   enables awaitable `replace()` later. Testable in `tst_sdk`.
2. **Registry-based handshake matching** — removes timeline knowledge from
   `newConnection`, fixes the late-connect drop. Small, isolated.
3. **Event-driven waits** — swap the two poll loops for
   `co_await qCoro(obj, &signal, timeout)`. Behavior-equivalent, less latency.
4. **Strand: serialize all mutating operations + tick** through one queue in
   the manager. Closes TODO open item 6; identifier guards become redundant
   (keep as asserts).
5. **Complete `ScreenPlayExternalProcess` into the link**: absorb
   `SDKConnection` lifecycle + the connect-wait, introduce `LinkState`,
   unify wallpaper/widget liveness (fixes the widget's duplicate
   ping-handler reconnect bug in passing); delete the restart state machine
   (reconciler not yet in place → keep a minimal "respawn once on Dead"
   shim in the manager).
6. **Reconciler + sections-as-data** — the big one; replaces
   `checkActiveWallpaperTimeline`, section states, `dropWallpaperFromSection`,
   restart shim. Existing `tests/ui/python/test_timeline_*.py` and
   `tst_timeline` are the safety net; `runningWallpapers()` keeps its JSON
   shape so tests keep passing.

Steps 1–4 are low-risk and valuable even if 5–6 are postponed.

## 6. Open questions

- Ack matching for `replace()`: match on echoed request id (cleaner) or on
  status-content equality (no protocol id needed)?
- Should `setValue` be acked too, or stay fire-and-forget (volume sliders
  spam — probably fire-and-forget + periodic status reconciles drift)?
- Godot wallpaper: can the GDExtension side emit status frames, or does the
  link mark `lastReported` as unsupported for Godot and fall back to
  connection-state-only?
- Basic version's single-section constraint: enforce in desired-state
  computation (one place) instead of in `addTimelineFromSettings`.
