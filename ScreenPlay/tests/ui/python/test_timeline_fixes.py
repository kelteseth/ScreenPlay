"""Regression tests for the timeline/process fixes (see TODO_async_review.md).

Covered here:
 - Removing the *active* (current-time) section must hand the active index to
   the section that expands backwards over the freed range (the old code
   pointed at the section *before* the removed one and briefly started its
   wallpapers).
 - removeTimelineAt is identifier-guarded; the normal UI remove flow passes
   the identifier and still works.

The active-section test builds a deterministic 3-section layout around the
host's current wall-clock time:

    |----- 0 -----|----- 1 (contains *now*) -----|----- 2 -----|

then removes section 1 and asserts the expanded old section 2 (now at
index 1) became the active one.
"""
from __future__ import annotations

import datetime as _dt

import pytest
from chuck import ChuckClient
from helpers import drag_handle_to_relative, open_example_drawer_and_reset, wait_timeline_length


@pytest.mark.asyncio
async def test_remove_active_section_activates_expanding_neighbor(harness_port: int) -> None:
    now = _dt.datetime.now()
    now_rel = (now.hour * 3600 + now.minute * 60 + now.second) / 86400.0

    # The layout below needs room on both sides of *now* and must not collide
    # with the fixed 0.5 split position (15-min snap adds up to ~0.005 jitter).
    if not (0.06 < now_rel < 0.94) or abs(now_rel - 0.5) < 0.06:
        pytest.skip("host time too close to midnight or noon for this layout")

    boundary = now_rel - 0.04 if now_rel < 0.5 else now_rel + 0.04

    async with ChuckClient(port=harness_port) as app:
        await open_example_drawer_and_reset(app)

        # Split once at the fixed centre position -> 2 sections.
        await app.click("drawerTimelineAddArea")
        await wait_timeline_length(app, 2)

        # Move the centre handle next to *now*, then split at the centre
        # again -> 3 sections with *now* inside the middle one.
        await drag_handle_to_relative(app, "drawerTimelineHandle0", boundary)
        await app.click("drawerTimelineAddArea")
        await wait_timeline_length(app, 3)

        # The 250ms rotation check must mark the middle section active.
        await app.wait(
            "drawerTimelineRoot",
            property="activeTimelineIndex",
            value=1,
            timeout=5000,
        )

        # Select and remove the active middle section.
        await app.click("drawerTimelineSelect1")
        await app.wait(
            "drawerTimelineRemove1",
            property="visible",
            value=True,
            timeout=5000,
        )
        await app.click("drawerTimelineRemove1")
        await wait_timeline_length(app, 2)

        # The *next* section expanded backwards over the freed range, so the
        # section covering the current time now sits at index 1. The old
        # index-1 bug reported index 0 here (and briefly started section 0's
        # wallpapers).
        await app.wait(
            "drawerTimelineRoot",
            property="activeTimelineIndex",
            value=1,
            timeout=5000,
        )
        # And the C++ singleton agrees (dotted singleton addressing).
        manager = await app.get(
            "@ScreenPlay/App.screenPlayManager",
            props="activeTimelineIndex,timelineSectionCount",
        )
        assert manager["activeTimelineIndex"] == 1
        assert manager["timelineSectionCount"] == 2


@pytest.mark.asyncio
async def test_remove_first_section_keeps_survivor_active(harness_port: int) -> None:
    """Two-section path: removing index 0 leaves the survivor spanning the
    whole day, active and selected at index 0."""
    async with ChuckClient(port=harness_port) as app:
        await open_example_drawer_and_reset(app)

        await app.click("drawerTimelineAddArea")
        await wait_timeline_length(app, 2)

        await app.click("drawerTimelineSelect0")
        await app.wait("drawerTimelineRemove0", property="visible", value=True, timeout=5000)
        await app.click("drawerTimelineRemove0")
        await wait_timeline_length(app, 1)

        await app.wait(
            "drawerTimelineRoot",
            property="activeTimelineIndex",
            value=0,
            timeout=5000,
        )
        manager = await app.get(
            "@ScreenPlay/App.screenPlayManager",
            props="activeTimelineIndex,selectedTimelineIndex,timelineSectionCount",
        )
        assert manager["activeTimelineIndex"] == 0
        assert manager["selectedTimelineIndex"] == 0
        assert manager["timelineSectionCount"] == 1
