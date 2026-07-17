"""Spawn ScreenPlayApp with --tester-port for chuck-driven UI tests.

The app binary path can be overridden via SCREENPLAY_APP env var. Otherwise
we search Build/MSVC_{Release,Debug}/ScreenPlay/ScreenPlayApp.exe.

A free TCP port is picked per session and passed via --tester-port so leftover
processes and parallel runs don't collide. The Qt bin dir is prepended to
PATH from the build's CMakeCache.txt so the spawned process finds its DLLs.
"""
from __future__ import annotations

import asyncio
import os
import re
import socket
import subprocess
import sys
import time
from contextlib import closing
from pathlib import Path

import pytest

# chuck commands that emulate user input — only these get slow-motion pacing.
_INTERACTION_COMMANDS = {"click", "keys", "drag", "swipe"}


@pytest.fixture(autouse=True)
def _slowmo(monkeypatch: pytest.MonkeyPatch) -> None:
    """Pace chuck interactions like a human user.

    SCREENPLAY_TEST_SLOWMO_MS > 0 adds a pause after every click/keys/drag/
    swipe so UI animations (300 ms StackView transitions, drawer slides)
    visibly complete. The workspace .env sets it for VS Code Testing-panel
    runs; plain terminal/CI runs default to 0 (fastest).
    """
    ms = int(os.environ.get("SCREENPLAY_TEST_SLOWMO_MS", "0"))
    if ms <= 0:
        return
    from chuck.client import Client

    orig_call = Client.call

    async def paced_call(self, method, **params):  # noqa: ANN001, ANN003
        result = await orig_call(self, method, **params)
        if method in _INTERACTION_COMMANDS:
            await asyncio.sleep(ms / 1000)
        return result

    monkeypatch.setattr(Client, "call", paced_call)

REPO_ROOT = Path(__file__).resolve().parents[4]


def _find_app() -> Path:
    explicit = os.environ.get("SCREENPLAY_APP")
    if explicit:
        p = Path(explicit)
        if p.is_file():
            return p
        raise FileNotFoundError(f"SCREENPLAY_APP={explicit} is not a file")

    exe_name = "ScreenPlayApp.exe" if sys.platform == "win32" else "ScreenPlayApp"
    candidates = [
        REPO_ROOT / "Build" / "MSVC_Release" / "ScreenPlay" / exe_name,
        REPO_ROOT / "Build" / "MSVC_Debug" / "ScreenPlay" / exe_name,
        REPO_ROOT / "build" / "ScreenPlay" / exe_name,
    ]
    candidates.extend(REPO_ROOT.glob(f"Build/**/{exe_name}"))
    for c in candidates:
        if c.is_file():
            return c
    raise FileNotFoundError(
        f"Could not locate {exe_name}. Build the ScreenPlayApp target or set "
        "SCREENPLAY_APP to the binary path."
    )


def _qt_bin_from_cache(app_exe: Path) -> Path | None:
    """Resolve the Qt bin dir from the build's CMakeCache.txt."""
    cache = app_exe.parent.parent / "CMakeCache.txt"
    if not cache.is_file():
        return None
    match = re.search(
        r"^Qt6Core_DIR:PATH=(.+)$",
        cache.read_text(encoding="utf-8", errors="replace"),
        re.MULTILINE,
    )
    if not match:
        return None
    qt_root = Path(match.group(1)).resolve().parent.parent.parent
    qt_bin = qt_root / "bin"
    return qt_bin if qt_bin.is_dir() else None


def _pick_free_port() -> int:
    with closing(socket.socket(socket.AF_INET, socket.SOCK_STREAM)) as s:
        s.bind(("127.0.0.1", 0))
        return s.getsockname()[1]


def _wait_for_port(port: int, timeout_s: float = 20.0) -> None:
    deadline = time.monotonic() + timeout_s
    while time.monotonic() < deadline:
        with closing(socket.socket(socket.AF_INET, socket.SOCK_STREAM)) as s:
            s.settimeout(0.5)
            try:
                s.connect(("127.0.0.1", port))
                return
            except OSError:
                time.sleep(0.2)
    raise RuntimeError(f"ScreenPlayApp did not open port {port} within {timeout_s}s")


def _wipe_sandbox_profile() -> None:
    """Delete the persisted profiles.json in the Qt test-mode appdata.

    The sandbox survives between runs, so wallpapers a previous test saved
    would auto-restore on the next app start — a startup race every test
    would have to defend against. Wiping it gives each test the app's
    first-run state (one empty full-day timeline section). license.json is
    left alone; main.cpp re-mirrors it anyway.
    """
    if sys.platform != "win32":
        return
    local = os.environ.get("LOCALAPPDATA")
    if not local:
        return
    profile = Path(local) / "qttest" / "ScreenPlay" / "ScreenPlay" / "profiles.json"
    profile.unlink(missing_ok=True)


@pytest.fixture
def harness_port() -> int:
    app_exe = _find_app()
    port = _pick_free_port()
    _wipe_sandbox_profile()
    env = os.environ.copy()
    qt_bin = _qt_bin_from_cache(app_exe)
    if qt_bin is not None:
        env["PATH"] = f"{qt_bin}{os.pathsep}{env.get('PATH', '')}"

    proc = subprocess.Popen(
        # --isolated-appdata redirects profiles.json/logs to Qt's test-mode
        # directories so test runs never touch (or destroy) the user's real
        # wallpaper profile. The content storage path lives in the registry
        # and is intentionally shared, so installed wallpapers stay visible.
        [str(app_exe), f"--tester-port={port}", "--isolated-appdata"],
        env=env,
        cwd=str(app_exe.parent),
    )
    try:
        _wait_for_port(port)
        yield port
    finally:
        proc.terminate()
        try:
            proc.wait(timeout=8)
        except subprocess.TimeoutExpired:
            proc.kill()
            proc.wait(timeout=2)
