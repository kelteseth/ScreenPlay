"""Basic app-shell smoke tests — the first things a user sees and clicks.

Covers TESTS.md ids 0.1, 1.1, 1.5, 2.1, 2.4 and 5.1: window comes up,
all navigation tabs switch, the Workshop tab state matches the build
platform, the installed grid populates, the drawer opens, and launching
plus closing content drives the quick-action buttons.

Each test gets its own app process via the harness_port fixture.
"""
from __future__ import annotations

import asyncio

import pytest
from chuck import ChuckClient
from helpers import open_example_drawer


@pytest.mark.asyncio
async def test_window_opens_and_each_nav_tab_shows_its_page(harness_port: int) -> None:
    """Main window shows with a versioned title, every nav tab switches to
    its view, and the Workshop tab is only enabled on Steam builds."""
    async with ChuckClient(port=harness_port) as app:
        await app.wait("mainContent", property="windowVisible", value=True, timeout=10000)
        title = (await app.get("mainContent", props="windowTitle"))["windowTitle"]
        assert "ScreenPlay v" in title, f"unexpected window title: {title!r}"

        await app.click("Settings")
        await app.wait("settingsView", timeout=5000)

        await app.click("Community")
        await app.wait("communityView", timeout=5000)

        await app.click("Create")
        await app.wait("createSidebar", timeout=5000)

        await app.click("Installed")
        await app.wait("gridView", timeout=5000)

        # The title states the platform (see ScreenPlayMain.qml), the tab
        # state must match it: Workshop is Steam-only.
        workshop_enabled = (await app.get("Workshop", props="enabled"))["enabled"]
        if "Steam" in title:
            assert workshop_enabled, "Workshop tab disabled on a Steam build"
        else:
            assert not workshop_enabled, "Workshop tab enabled on a standalone build"


@pytest.mark.asyncio
async def test_installed_content_lists_and_click_opens_drawer(harness_port: int) -> None:
    """The installed list populates from the content folder and clicking an
    item opens the configuration drawer."""
    async with ChuckClient(port=harness_port) as app:
        await app.wait("gridView", property="visible", value=True, timeout=10000)
        count = (await app.get("gridView", props="count"))["count"]
        assert count > 0, (
            "no installed content found — example content should ship with the "
            "app (see TESTS.md, CI setup)"
        )
        # Open a known example video's drawer (never assume installedItem0 —
        # example content can reorder the grid).
        await open_example_drawer(app)


@pytest.mark.asyncio
async def test_wallpaper_starts_and_close_all_stops_it(harness_port: int) -> None:
    """Quick actions are disabled while nothing runs; launching a wallpaper
    enables them; Close All stops everything and disables them again."""
    async with ChuckClient(port=harness_port) as app:
        await app.wait("gridView", timeout=10000)
        # Fresh sandbox profile (conftest wipes it): nothing runs at startup.
        assert not (await app.get("miCloseAll", props="enabled"))["enabled"]

        # Open a known example video's drawer (item 0 may be e.g. the Godot
        # example, whose launch opens a modal export dialog).
        await open_example_drawer(app)
        # The fresh profile has exactly one full-day section — necessarily
        # the *active* one, so launching starts the wallpaper now instead of
        # merely scheduling it for a future section.
        await app.wait("drawerTimelineRoot", property="length", value=1, timeout=5000)
        await app.click("drawerTimelineSelect0")
        await app.click("monitorItem0")
        await app.click("btnLaunchContent")

        # Wallpaper process spawn + SDK connect can take a few seconds.
        await app.wait("miCloseAll", property="enabled", value=True, timeout=20000)

        # Let the wallpaper actually play for a moment — a user watches it,
        # and a crash/restart inside this window would disable the quick
        # actions again and fail the assert below.
        await asyncio.sleep(3.0)
        assert (await app.get("miCloseAll", props="enabled"))["enabled"], (
            "wallpaper stopped running within 3 s of starting"
        )

        # The drawer is non-modal (NoAutoClose), so the nav bar stays clickable.
        await app.click("miCloseAll")
        await app.wait("miCloseAll", property="enabled", value=False, timeout=20000)
