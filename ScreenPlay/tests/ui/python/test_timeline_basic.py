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

import pytest
from chuck import ChuckClient


async def _length(app: ChuckClient) -> int:
    return (await app.get("drawerTimelineRoot", props="length"))["length"]


async def _wait_length(app: ChuckClient, value: int, timeout_ms: int = 5000) -> None:
    await app.wait("drawerTimelineRoot", property="length", value=value, timeout=timeout_ms)


async def _open_drawer_and_reset(app: ChuckClient) -> None:
    """Open the first installed item's drawer and reset all timeline sections.

    After reset the timeline has exactly one default section spanning 00:00:00
    to 23:59:59, regardless of whatever was previously persisted.
    """
    await app.wait("gridView", timeout=10000)
    await app.click("installedItem0")
    await app.wait("drawerTimelineIndicator0", timeout=10000)
    # Drawer open transition has to settle before isOnScreen accepts inner items.
    await asyncio.sleep(1.0)

    await app.click("drawerTimelineBtnReset")
    await _wait_length(app, 1, timeout_ms=10000)


@pytest.mark.asyncio
async def test_realistic_user_flow(harness_port: int) -> None:
    async with ChuckClient(port=harness_port) as app:
        await _open_drawer_and_reset(app)

        # 1. Split the timeline.
        await app.click("drawerTimelineAddArea")
        await _wait_length(app, 2)

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
        assert await _length(app) == 2

        # 4. Move handle0 to the right so the next add-bar tap (always at
        # centre = 0.5) lands at a position distinct from the existing handle
        # — otherwise the C++ validator rejects the duplicate.
        # NB: must use swipe rather than drag here. chuck's `drag` synthesizes
        # a single mouseMove, which DragHandler doesn't recognise as a real
        # drag (it expects multiple incremental moves to clear the drag
        # threshold). `swipe` sends 5 intermediate moves and engages properly.
        await app.swipe("drawerTimelineHandle0", direction="right", distance=120)
        await asyncio.sleep(0.3)

        # 5. Add another section. Now handle0 sits well past 0.5, so a tap
        # at 0.5 inserts between the start and handle0 — both halves valid.
        await app.click("drawerTimelineAddArea")
        await _wait_length(app, 3)

        # 6. Select section 0 and remove it.
        await app.click("drawerTimelineSelect0")
        await app.wait(
            "drawerTimelineRemove0",
            property="visible",
            value=True,
            timeout=5000,
        )
        await app.click("drawerTimelineRemove0")
        await _wait_length(app, 2)


@pytest.mark.asyncio
async def test_duplicate_position_is_rejected(harness_port: int) -> None:
    """The TapHandler-driven add-bar always clicks at addHandleWrapper's
    centre (0.5). Tapping it twice in a row used to persist a zero-length
    section at the existing handle; ScreenPlayTimelineManager::addTimelineAt
    now rejects the duplicate. Section count must stay at 2 after the
    second tap.
    """
    async with ChuckClient(port=harness_port) as app:
        await _open_drawer_and_reset(app)

        await app.click("drawerTimelineAddArea")
        await _wait_length(app, 2)

        # Second tap at the same position — should be rejected. We can't
        # `wait` for "no change", so give the C++ side time to settle and
        # then assert.
        await app.click("drawerTimelineAddArea")
        await asyncio.sleep(0.5)
        assert await _length(app) == 2, (
            "duplicate-position split was not rejected — a zero-length "
            "section would otherwise be persisted into the profile"
        )
