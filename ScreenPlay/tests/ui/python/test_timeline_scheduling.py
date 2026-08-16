"""Wall-clock-driven timeline scheduling: ScreenPlay must start/stop
wallpapers when a section boundary is moved across the current time.

Layout (two sections, boundary ~1 h from *now*, 15-min snap safe):

    phase 1:  |--- empty ---|====== filled (contains *now*) ======|
    phase 2:  boundary dragged across *now* -> *now* falls into the
              EMPTY section -> the wallpaper must stop.
    phase 3:  boundary dragged back -> the wallpaper must start again.

Verification is pure state (no pixel checks): the manager singleton's
`runningWallpapers` property lists every live wallpaper process with its
section index/state and the process AppState. "Transition done" ==
the entry reports sectionState "Active" and process state "Active".

Everything is relative to the host's current time, so the test picks its
boundary positions around *now* and skips near midnight where the layout
cannot fit.
"""
from __future__ import annotations

import asyncio
import datetime as _dt

import pytest
from chuck import ChuckClient
from helpers import (
    EXAMPLE_VIDEO_NEBULA_H264,
    MANAGER as _MANAGER,
    active_wallpapers as _active_entries,
    drag_handle_to_relative,
    find_index_by_example,
    manager_state as _manager_state,
    open_drawer_and_reset,
    wait_timeline_length,
)


async def _settle_and_get_state(app: ChuckClient, seconds: float = 5.0) -> dict:
    """Give the section transition time to finish, then read manager state."""
    await asyncio.sleep(seconds)
    return await _manager_state(app)


@pytest.mark.asyncio
async def test_moving_section_boundary_across_now_starts_and_stops_wallpaper(
    harness_port: int,
) -> None:
    now = _dt.datetime.now()
    now_rel = (now.hour * 3600 + now.minute * 60 + now.second) / 86400.0

    # Need ~0.04 (~1 h) of room on both sides of *now*, away from the fixed
    # 0.5 split position (15-min snap adds up to ~0.005 jitter).
    if not (0.06 < now_rel < 0.94) or abs(now_rel - 0.5) < 0.06:
        pytest.skip("host time too close to midnight or noon for this layout")

    # Boundary positions on either side of *now*. Sections are
    # [0: 00:00..boundary] [1: boundary..24:00].
    below_now = now_rel - 0.04
    above_now = now_rel + 0.04
    if now_rel < 0.5:
        # Start with the boundary below *now*: filled section = 1.
        start_boundary, moved_boundary = below_now, above_now
        filled_index_start, empty_index_after_move = 1, 0
    else:
        # Mirror: boundary above *now*: filled section = 0.
        start_boundary, moved_boundary = above_now, below_now
        filled_index_start, empty_index_after_move = 0, 1

    async with ChuckClient(port=harness_port) as app:
        await app.wait("gridView", property="visible", value=True, timeout=10000)
        video_index = await find_index_by_example(app, EXAMPLE_VIDEO_NEBULA_H264)
        if video_index is None:
            pytest.skip("example video content not present (is it enabled/shipped?)")
        await open_drawer_and_reset(app, video_index)

        # Split at the fixed centre, then move the boundary near *now*.
        await app.click("drawerTimelineAddArea")
        await wait_timeline_length(app, 2)
        await drag_handle_to_relative(app, "drawerTimelineHandle0", start_boundary)

        # Fill ONLY the section containing *now*; the other stays empty.
        await app.click(f"drawerTimelineSelect{filled_index_start}")
        await app.wait(
            "drawerTimelineRoot",
            property="selectedTimelineIndex",
            value=filled_index_start,
            timeout=5000,
        )
        await app.click("monitorItem0")
        await app.click("btnLaunchContent")

        # Phase 1: the filled section is the current one -> wallpaper runs.
        await app.wait(_MANAGER, property="activeWallpaperCounter", value=1, timeout=20000)
        state = await _settle_and_get_state(app)
        active = _active_entries(state)
        assert state["activeTimelineIndex"] == filled_index_start
        assert len(active) == 1, f"expected 1 active wallpaper, got: {state['runningWallpapers']}"
        assert active[0]["sectionIndex"] == filled_index_start
        wallpaper_path = active[0]["absolutePath"]
        assert wallpaper_path, "running wallpaper reports no absolutePath"

        # Phase 2: drag the boundary across *now* -> *now* falls into the
        # empty section -> ScreenPlay must stop the wallpaper.
        await drag_handle_to_relative(app, "drawerTimelineHandle0", moved_boundary)
        state = await _settle_and_get_state(app)
        assert state["activeTimelineIndex"] == empty_index_after_move
        assert state["activeWallpaperCounter"] == 0, (
            f"wallpaper kept running although *now* moved into an empty "
            f"section: {state['runningWallpapers']}"
        )
        assert _active_entries(state) == []

        # Phase 3: drag back -> the same wallpaper must come back.
        await drag_handle_to_relative(app, "drawerTimelineHandle0", start_boundary)
        state = await _settle_and_get_state(app)
        active = _active_entries(state)
        assert state["activeTimelineIndex"] == filled_index_start
        assert len(active) == 1, f"wallpaper did not restart: {state['runningWallpapers']}"
        assert active[0]["sectionIndex"] == filled_index_start
        assert active[0]["absolutePath"] == wallpaper_path, (
            "a different wallpaper came back after moving the boundary back"
        )
