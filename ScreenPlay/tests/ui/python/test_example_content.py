"""Shipped example content: loaded, individually addressable, non-removable.

ScreenPlay ships example wallpapers/widgets in Content/ (copied next to the
executable at build time). They load into the Installed list tagged with a
ContentTypes::ExampleContent enum so they can be selected by identity, and
they are read-only — the Remove action is hidden and the C++ model refuses to
delete them.
"""
from __future__ import annotations

import pytest
from chuck import ChuckClient
from helpers import (
    EXAMPLE_HTML,
    EXAMPLE_VIDEO_NEBULA_H264,
    EXAMPLE_VIDEO_SHAPES_AV1,
    find_index_by_example,
)


@pytest.mark.asyncio
async def test_example_content_is_shipped_and_addressable(harness_port: int) -> None:
    """The known example wallpapers load and each is findable by its enum tag."""
    async with ChuckClient(port=harness_port) as app:
        await app.wait("gridView", property="visible", value=True, timeout=10000)
        for example in (EXAMPLE_VIDEO_NEBULA_H264, EXAMPLE_VIDEO_SHAPES_AV1, EXAMPLE_HTML):
            index = await find_index_by_example(app, example)
            assert index is not None, (
                f"example content {example} not loaded — is it shipped next to "
                "the executable and enabled (Settings ▸ Show example content)?"
            )


@pytest.mark.asyncio
async def test_example_content_has_no_remove_action(harness_port: int) -> None:
    """Right-clicking an example opens the context menu without a Remove entry
    (the C++ model additionally refuses deletion as a safety net)."""
    async with ChuckClient(port=harness_port) as app:
        await app.wait("gridView", property="visible", value=True, timeout=10000)
        index = await find_index_by_example(app, EXAMPLE_VIDEO_NEBULA_H264)
        assert index is not None, "example video content not present"

        await app.click(f"installedItem{index}", button="right")
        await app.wait("installedItemContextMenu", property="visible", value=True, timeout=5000)

        # The Remove MenuItem is `visible: exampleContent === UserInstalled`, so
        # for an example it is hidden (present but not visible).
        remove_visible = (await app.get("removeItem", props="visible"))["visible"]
        assert remove_visible is False, (
            "shipped example content must not offer a Remove action"
        )
