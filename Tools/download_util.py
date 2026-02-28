#!/usr/bin/python3
# SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only
"""Shared streaming download helper used across all ScreenPlay setup scripts."""

import os
import requests
from rich.progress import (
    Progress,
    BarColumn,
    DownloadColumn,
    TransferSpeedColumn,
    TimeRemainingColumn,
)
from rich_console import console

HEADERS = {
    "User-Agent": (
        "Mozilla/5.0 (Windows NT 10.0; Win64; x64) "
        "AppleWebKit/537.36 (KHTML, like Gecko) "
        "Chrome/122.0.0.0 Safari/537.36"
    )
}

DOWNLOAD_COLUMNS = (
    "[progress.description]{task.description}",
    BarColumn(),
    DownloadColumn(),
    TransferSpeedColumn(),
    TimeRemainingColumn(),
)


def stream_download(url: str, dest_path: str, label: str,
                    progress=None, outer_task=None) -> str:
    """Download *url* to *dest_path* with a Rich progress bar.

    Standalone (progress=None):
        Creates its own Progress context for the duration of the download.

    Embedded (progress + outer_task provided):
        Reuses the caller's live bar in-place — temporarily replaces its
        total/completed with byte counts so the fill animates, then restores
        the original step values when done.  No second renderer is created.

    Returns dest_path.
    """
    console.print(f"[bold cyan]Downloading[/] [yellow]{label}[/]")
    console.print(f"  URL: [dim]{url}[/]")

    response = requests.get(url, headers=HEADERS, stream=True, timeout=120)
    response.raise_for_status()
    total = int(response.headers.get("content-length", 0))

    def _stream(p, task):
        downloaded = 0
        with open(dest_path, "wb") as f:
            for chunk in response.iter_content(chunk_size=1024 * 64):
                if chunk:
                    f.write(chunk)
                    downloaded += len(chunk)
                    if total:
                        mb_done = downloaded / 1_048_576
                        mb_total = total / 1_048_576
                        p.update(
                            task,
                            advance=len(chunk),
                            description=(
                                f"[bold cyan]{label}[/] "
                                f"[dim]{mb_done:.0f}/{mb_total:.0f} MB[/]"
                            ),
                        )
                    else:
                        p.update(task, advance=len(chunk))

    if progress is not None and outer_task is not None:
        saved_total = progress._tasks[outer_task].total
        progress.update(outer_task, total=total or None, completed=0)
        _stream(progress, outer_task)
        progress.update(outer_task, total=saved_total, completed=saved_total)
    else:
        with Progress(*DOWNLOAD_COLUMNS, console=console) as p:
            task = p.add_task(f"[cyan]{label}", total=total or None)
            _stream(p, task)

    console.print(f"  [green]Saved[/] → {dest_path}")
    return dest_path
