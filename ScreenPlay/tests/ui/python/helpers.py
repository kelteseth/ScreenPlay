"""Shared chuck helpers for the ScreenPlay UI tests.

All helpers target the *drawer* Timeline instance (objectNamePrefix
"drawerTimeline"); the ContentSettings view hosts a second instance with a
different prefix.
"""
from __future__ import annotations

import asyncio

import pytest
from chuck import ChuckClient

# C++ singleton with the timeline/wallpaper state (chuck dotted addressing).
MANAGER = "@ScreenPlay/App.screenPlayManager"

# ScreenPlay::ContentTypes::InstalledType enum values (contenttypes.h).
TYPE_VIDEO_WALLPAPER = 1
TYPE_HTML_WALLPAPER = 3

# ScreenPlay::ContentTypes::ExampleContent enum values (contenttypes.h). These
# tag the wallpapers/widgets shipped in Content/, so tests can address a
# specific, guaranteed piece of content instead of assuming the user's library.
EXAMPLE_USER_INSTALLED = 0
EXAMPLE_VIDEO_ASTRONAUT_VP9 = 1
EXAMPLE_VIDEO_NEBULA_H264 = 2
EXAMPLE_VIDEO_SHAPES_AV1 = 3
EXAMPLE_HTML = 4
EXAMPLE_QML = 5
EXAMPLE_QML_PARTICLES = 6
EXAMPLE_GODOT = 7


async def find_index_by_example(app: ChuckClient, example: int, scan: int = 60) -> int | None:
    """Return the grid index whose exampleContent tag matches, or None.

    Shipped example content is copied fresh at build time so it sorts to the
    top of the (install-date-desc) grid and its delegates are instantiated;
    only instantiated delegates are readable, so the scan stops at the first
    gap.
    """
    for i in range(scan):
        try:
            value = (await app.get(f"installedItem{i}", props="exampleContent"))["exampleContent"]
        except Exception:
            break
        if value == example:
            return i
    return None


async def open_item_drawer(app: ChuckClient, index: int) -> None:
    """Open the drawer for a specific installed grid item and wait until usable."""
    await app.wait("gridView", property="visible", value=True, timeout=10000)
    await app.wait(f"installedItem{index}", timeout=10000)
    await app.click(f"installedItem{index}")
    await app.wait("installedDrawer", property="visible", value=True, timeout=10000)
    await app.wait("drawerTimelineIndicator0", timeout=10000)
    # Drawer open transition has to settle before isOnScreen accepts inner items.
    await asyncio.sleep(1.0)


async def manager_state(app: ChuckClient) -> dict:
    """Snapshot of the manager's test-relevant state."""
    return await app.get(
        MANAGER,
        props="runningWallpapers,activeWallpaperCounter,activeTimelineIndex,timelineSectionCount",
    )


def active_wallpapers(state: dict) -> list[dict]:
    """runningWallpapers entries whose section and process are fully Active."""
    return [
        w for w in state["runningWallpapers"]
        if w["sectionState"] == "Active" and w["state"] == "Active"
    ]


async def timeline_length(app: ChuckClient) -> int:
    return (await app.get("drawerTimelineRoot", props="length"))["length"]


async def wait_timeline_length(app: ChuckClient, value: int, timeout_ms: int = 5000) -> None:
    await app.wait("drawerTimelineRoot", property="length", value=value, timeout=timeout_ms)


async def open_first_item_drawer(app: ChuckClient) -> None:
    """Click the first installed item and wait until its drawer is usable."""
    await open_item_drawer(app, 0)


async def open_drawer_and_reset(app: ChuckClient, index: int = 0) -> None:
    """Open an installed item's drawer and reset all timeline sections.

    After reset the timeline has exactly one default section spanning 00:00:00
    to 23:59:59, regardless of whatever was previously persisted.
    """
    await open_item_drawer(app, index)
    await app.click("drawerTimelineBtnReset")
    await wait_timeline_length(app, 1, timeout_ms=10000)


async def open_example_drawer(app: ChuckClient, example: int = EXAMPLE_VIDEO_NEBULA_H264) -> int:
    """Open the drawer for a shipped example (a video by default) and return
    its grid index. Skips the test if that example is not present.

    Tests must never assume `installedItem0` is launchable — with example
    content enabled the top of the grid can be e.g. the Godot example, whose
    launch opens a modal export dialog.
    """
    await app.wait("gridView", property="visible", value=True, timeout=10000)
    index = await find_index_by_example(app, example)
    if index is None:
        pytest.skip("required example content not present (is it enabled/shipped?)")
    await open_item_drawer(app, index)
    return index


async def open_example_drawer_and_reset(app: ChuckClient, example: int = EXAMPLE_VIDEO_NEBULA_H264) -> int:
    """open_example_drawer + reset the timeline to a single full-day section."""
    index = await open_example_drawer(app, example)
    await app.click("drawerTimelineBtnReset")
    await wait_timeline_length(app, 1, timeout_ms=10000)
    return index


async def drag_handle_to_relative(app: ChuckClient, handle: str, target_rel: float) -> None:
    """Drag a timeline handle to ~target_rel (0..1) using pixel-offset drag.

    The handle snaps to 15-minute marks, so the final position can deviate by
    up to 450 s from target_rel — callers must leave margin for that.
    """
    props = await app.get(handle, props="x,lineWidth,endSeconds")
    delta_px = round(target_rel * props["lineWidth"] - props["x"])
    await app.drag(handle, by=(delta_px, 0))
    await asyncio.sleep(0.3)
