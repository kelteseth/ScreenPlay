"""End-to-end timeline UI flow against the production ScreenPlayApp.

The harness fixture in conftest.py spawns ScreenPlayApp.exe with --tester-port
per test, chuck connects to the WebSocket server attached to the main window.

There are two Timeline instances in the QML tree (drawer + ContentSettingsView);
`objectNamePrefix` differentiates them. We target the drawer's surfaces here.

Each test opens the drawer for the first installed wallpaper, then clicks the
in-drawer Reset button to clear all sections (the user's persisted profile
may carry state from previous runs — including invalid zero-length sections
left over from earlier bug repros — and the reset guarantees a clean
length == 1 starting point).
"""
from __future__ import annotations

import asyncio
import datetime as _dt

import pytest
from chuck import ChuckClient
from helpers import (
    EXAMPLE_HTML,
    EXAMPLE_VIDEO_NEBULA_H264,
    EXAMPLE_VIDEO_SHAPES_AV1,
    MANAGER,
    active_wallpapers,
    drag_handle_to_relative,
    find_index_by_example,
    manager_state,
    open_drawer_and_reset,
    open_example_drawer_and_reset,
    timeline_length,
    wait_timeline_length,
)


@pytest.mark.asyncio
async def test_realistic_user_flow(harness_port: int) -> None:
    async with ChuckClient(port=harness_port) as app:
        await open_example_drawer_and_reset(app)

        # 1. Split the timeline.
        await app.click("drawerTimelineAddArea")
        await wait_timeline_length(app, 2)

        # 2. Select the new right-hand section.
        await app.click("drawerTimelineSelect1")
        await app.wait(
            "drawerTimelineRoot",
            property="selectedTimelineIndex",
            value=1,
            timeout=5000,
        )

        # 3. Arm monitor 0 and press Start.
        await app.click("monitorItem0")
        await app.click("btnLaunchContent")
        await asyncio.sleep(3.0)
        assert await timeline_length(app) == 2

        # 4. Move handle0 to the right so the next add-bar tap (always at
        # centre = 0.5) lands at a position distinct from the existing handle
        # — otherwise the C++ validator rejects the duplicate.
        # drag(by=...) sends incremental mouse moves so DragHandler engages
        # (a single-jump move never clears the drag threshold).
        await app.drag("drawerTimelineHandle0", by=(120, 0))
        await asyncio.sleep(0.3)

        # 5. Add another section. Now handle0 sits well past 0.5, so a tap
        # at 0.5 inserts between the start and handle0 — both halves valid.
        await app.click("drawerTimelineAddArea")
        await wait_timeline_length(app, 3)

        # 6. Select section 0 and remove it.
        await app.click("drawerTimelineSelect0")
        await app.wait(
            "drawerTimelineRemove0",
            property="visible",
            value=True,
            timeout=5000,
        )
        await app.click("drawerTimelineRemove0")
        await wait_timeline_length(app, 2)


@pytest.mark.asyncio
async def test_duplicate_position_is_rejected(harness_port: int) -> None:
    """The TapHandler-driven add-bar always clicks at addHandleWrapper's
    centre (0.5). Tapping it twice in a row used to persist a zero-length
    section at the existing handle; ScreenPlayTimelineManager::addTimelineAt
    now rejects the duplicate. Section count must stay at 2 after the
    second tap.
    """
    async with ChuckClient(port=harness_port) as app:
        await open_example_drawer_and_reset(app)

        await app.click("drawerTimelineAddArea")
        await wait_timeline_length(app, 2)

        # Second tap at the same position — should be rejected. We can't
        # `wait` for "no change", so give the C++ side time to settle and
        # then assert.
        await app.click("drawerTimelineAddArea")
        await asyncio.sleep(0.5)
        assert await timeline_length(app) == 2, (
            "duplicate-position split was not rejected — a zero-length "
            "section would otherwise be persisted into the profile"
        )


async def _drag_center_handle_to(app: ChuckClient, handle_count: int, target_rel: float) -> None:
    """After an add-split (which always inserts a handle at the fixed centre
    0.5), find the handle nearest 0.5 and drag it to target_rel.

    Lets us build an arbitrary layout despite every split landing at 0.5.
    """
    best_i, best_d = 0, 1.0
    for i in range(handle_count):
        props = await app.get(f"drawerTimelineHandle{i}", props="x,lineWidth")
        rel = props["x"] / props["lineWidth"]
        d = abs(rel - 0.5)
        if d < best_d:
            best_d, best_i = d, i
    await drag_handle_to_relative(app, f"drawerTimelineHandle{best_i}", target_rel)


@pytest.mark.parametrize(
    "b_example",
    [
        pytest.param(EXAMPLE_VIDEO_SHAPES_AV1, id="video-to-video"),
        pytest.param(
            EXAMPLE_HTML,
            id="video-to-html",
            marks=pytest.mark.xfail(
                reason="KNOWN BUG: the shipped HTML wallpaper's start transiently "
                "fails on a rapid restart; after 2 failed restarts "
                "WallpaperTimelineSection drops it from wallpaperList permanently, "
                "so the section stays empty forever (video->video does NOT hit "
                "this — see TODO_async_review.md open items 3/6). Remove the xfail "
                "once the HTML-wallpaper rapid-restart is fixed.",
                strict=False,
            ),
        ),
    ],
)
@pytest.mark.asyncio
async def test_complex_timeline_editing_flow(harness_port: int, b_example: int) -> None:
    """Torture flow: add many sections, move handles so the wall-clock *now*
    crosses boundaries (forcing wallpaper start/stop), and delete sections —
    all verified in code via the manager's runningWallpapers snapshot, never
    by pixels.

    Both wallpapers are shipped example content (deterministic across machines):
    A is an example video, B is a second example video (video-to-video, passes)
    or the example HTML wallpaper (video-to-html, reproduces the drop bug).

    The layout is anchored to the host clock so *now* sits in section 1 of a
    four-section day:

        [0: empty][1: NOW, filled A][2: empty->filled B][3: empty]

    Steps:
     1. Build 4 sections.
     2. Start wallpaper A in section 1 (contains *now*).
     3. Nudge the far boundary (section 2|3) — A must keep running, same
        process (appID unchanged): an untouched boundary is not a restart.
     4. Drag section 1's right boundary LEFT past *now* -> *now* moves into
        the empty section 2 -> A must stop.
     5. Point the drawer at wallpaper B and start it in section 2 (now the
        current one).
     6. Oscillate that boundary across *now* several times, forcing repeated
        A <-> B swaps; after every crossing exactly one wallpaper runs with
        the expected content (stress test for the transition path).
     7. Delete the RUNNING section 1 -> section 2 (holding B) expands back
        over *now* and B takes over: still exactly one running wallpaper,
        now B's content.
     8. Close all via the nav quick action.
    """
    now = _dt.datetime.now()
    now_rel = (now.hour * 3600 + now.minute * 60 + now.second) / 86400.0

    # Lay out [0 | b0 | b1 | b2 | 1] with *now* inside section 1 = [b0, b1].
    # Every add-split lands at 0.5, so all boundaries must stay clear of 0.5
    # (±0.05, 15-min snap adds ~0.01). When *now* is near noon its section
    # brackets 0.5, so push the boundaries out asymmetrically.
    if not (0.18 < now_rel < 0.75):
        pytest.skip("host time too close to the day edges for this 4-section layout")
    if now_rel < 0.5:
        b0, b1 = now_rel - 0.10, max(now_rel + 0.10, 0.57)
    else:
        b0, b1 = min(now_rel - 0.10, 0.43), now_rel + 0.10
    b2 = min(b1 + 0.15, 0.92)

    async with ChuckClient(port=harness_port) as app:
        await app.wait("gridView", property="visible", value=True, timeout=10000)
        # A and B are shipped example content, addressed by their enum tag so
        # the test never depends on the user's own library.
        a_index = await find_index_by_example(app, EXAMPLE_VIDEO_NEBULA_H264)
        b_index = await find_index_by_example(app, b_example)
        if a_index is None or b_index is None:
            pytest.skip("required example content not present (is it enabled/shipped?)")

        await open_drawer_and_reset(app, a_index)

        # 1. Build 4 sections. Each add splits at the fixed centre 0.5, and the
        # min-width guard rejects a split that lands too close to an existing
        # handle — so build at safe, well-separated positions first, dragging
        # the fresh centre handle far from 0.5 each time...
        await app.click("drawerTimelineAddArea")
        await wait_timeline_length(app, 2)
        await _drag_center_handle_to(app, 1, 0.20)
        await app.click("drawerTimelineAddArea")
        await wait_timeline_length(app, 3)
        await _drag_center_handle_to(app, 2, 0.80)
        await app.click("drawerTimelineAddArea")
        await wait_timeline_length(app, 4)
        await _drag_center_handle_to(app, 3, 0.35)

        # ...then reposition each boundary to its real target. Dragging never
        # collides with 0.5, so a target near the centre is fine now. Go
        # right-to-left so no handle crosses its neighbour mid-move.
        await drag_handle_to_relative(app, "drawerTimelineHandle2", b2)
        await drag_handle_to_relative(app, "drawerTimelineHandle1", b1)
        await drag_handle_to_relative(app, "drawerTimelineHandle0", b0)

        # *now* is between b0 and b1 -> section 1 is the active one.
        await app.wait(MANAGER, property="activeTimelineIndex", value=1, timeout=10000)

        # 2. Start wallpaper A in the current-time section.
        await app.click("drawerTimelineSelect1")
        await app.wait(
            "drawerTimelineRoot", property="selectedTimelineIndex", value=1, timeout=5000
        )
        await app.click("monitorItem0")
        await app.click("btnLaunchContent")
        await app.wait(MANAGER, property="activeWallpaperCounter", value=1, timeout=20000)
        await asyncio.sleep(3.0)
        running = active_wallpapers(await manager_state(app))
        assert len(running) == 1 and running[0]["sectionIndex"] == 1
        app_id_a = running[0]["appID"]
        path_a = running[0]["absolutePath"]

        # 3. Nudge the section 2|3 boundary (handle 2) — it does not touch
        # *now*, so A must keep running as the SAME process.
        await drag_handle_to_relative(app, "drawerTimelineHandle2", b2 + 0.04)
        await asyncio.sleep(3.0)
        running = active_wallpapers(await manager_state(app))
        assert len(running) == 1 and running[0]["appID"] == app_id_a, (
            "moving a boundary away from the current time restarted the wallpaper"
        )

        # 4. Drag section 1's right boundary (handle 1) LEFT past *now* ->
        # *now* falls into the empty section 2 -> A must stop.
        await drag_handle_to_relative(app, "drawerTimelineHandle1", now_rel - 0.05)
        await app.wait(MANAGER, property="activeTimelineIndex", value=2, timeout=10000)
        await asyncio.sleep(3.0)
        assert (await manager_state(app))["activeWallpaperCounter"] == 0, (
            "wallpaper kept running after *now* moved into an empty section"
        )

        # 5. Point the drawer at wallpaper B (a different example) and start it
        # in section 2 (now the current one). Clicking another grid item while
        # the drawer is open swaps its content without resetting the timeline
        # (InstalledDrawer only resets on a fresh open).
        await app.click(f"installedItem{b_index}")
        await asyncio.sleep(0.5)
        await app.click("drawerTimelineSelect2")
        await app.wait(
            "drawerTimelineRoot", property="selectedTimelineIndex", value=2, timeout=5000
        )
        await app.click("monitorItem0")
        await app.click("btnLaunchContent")
        await app.wait(MANAGER, property="activeWallpaperCounter", value=1, timeout=20000)
        await asyncio.sleep(3.0)
        running = active_wallpapers(await manager_state(app))
        assert len(running) == 1 and running[0]["sectionIndex"] == 2
        path_b = running[0]["absolutePath"]
        assert path_b != path_a, "A and B must be different wallpapers for this test"

        # 6. Oscillate: drag section 1's right boundary back and forth across
        # *now* several times. Each crossing forces ScreenPlay to swap the
        # running content (A <-> B). This hammers the transition/replacement
        # path looking for a race: a stuck counter, a lingering second
        # process, the wrong content, or a crash (which drops the socket and
        # fails the test outright).
        async def _assert_only(section: int, path: str, label: str) -> None:
            # Poll manager state directly (a bare app.wait would throw on the
            # first miss and hide the real numbers). Give the transition —
            # stop the old wallpaper, start the new one — up to ~20 s.
            state = await manager_state(app)
            for _ in range(20):
                state = await manager_state(app)
                running = active_wallpapers(state)
                if (
                    state["activeTimelineIndex"] == section
                    and state["activeWallpaperCounter"] == 1
                    and len(running) == 1
                ):
                    break
                await asyncio.sleep(1.0)
            running = active_wallpapers(state)
            assert (
                state["activeTimelineIndex"] == section
                and state["activeWallpaperCounter"] == 1
                and len(running) == 1
            ), (
                f"{label}: expected exactly one running wallpaper in section {section} "
                f"but activeTimelineIndex={state['activeTimelineIndex']} "
                f"counter={state['activeWallpaperCounter']} running={state['runningWallpapers']}"
            )
            assert running[0]["sectionIndex"] == section, f"{label}: wrong section {running[0]}"
            assert running[0]["absolutePath"] == path, (
                f"{label}: wrong content {running[0]['absolutePath']} (expected {path})"
            )

        for i in range(4):
            # -> *now* into section 1 (wallpaper A)
            await drag_handle_to_relative(app, "drawerTimelineHandle1", now_rel + 0.05)
            await _assert_only(1, path_a, f"oscillation {i} -> A")
            # -> *now* into section 2 (wallpaper B)
            await drag_handle_to_relative(app, "drawerTimelineHandle1", now_rel - 0.05)
            await _assert_only(2, path_b, f"oscillation {i} -> B")

        # Leave *now* in section 1 (A running) for the delete-takeover step.
        await drag_handle_to_relative(app, "drawerTimelineHandle1", now_rel + 0.05)
        await _assert_only(1, path_a, "pre-delete -> A")

        # 7. Delete the RUNNING section 1: section 2 (holding B) expands back
        # over *now* and B must take over — one running wallpaper, now B's
        # content.
        await app.click("drawerTimelineSelect1")
        await app.wait("drawerTimelineRemove1", property="visible", value=True, timeout=5000)
        await app.click("drawerTimelineRemove1")
        await wait_timeline_length(app, 3)
        await _assert_only(1, path_b, "delete-takeover -> B")

        # 8. Stop everything like a user would.
        await app.click("miCloseAll")
        await app.wait(MANAGER, property="activeWallpaperCounter", value=0, timeout=20000)
