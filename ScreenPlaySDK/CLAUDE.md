# ScreenPlaySDK

The IPC **client** linked into every wallpaper/widget process. It connects back
to the manager (`ScreenPlay`) over a `QLocalSocket` and turns the raw byte
stream into `incomingMessage(key, value)` signals. One class:
`ScreenPlaySDK` (`inc/public/ScreenPlaySDK/screenplaysdk.h`).

The manager side of the same connection lives in `ScreenPlay/src/` (`SDKConnection`);
keep the two protocol-compatible.

## Wire protocol — the part that bites

- **All framing goes through `ScreenPlay::IpcFrameBuffer`** (from `ScreenPlayCore`).
  Never parse `m_socket.readAll()` directly: local sockets coalesce writes
  (`{...}{...}` in one `readyRead`) and split single frames across packets.
  Reading raw silently drops or corrupts messages.
- Two frame kinds coexist: JSON objects (`{...}`, brace-matched, self-delimiting)
  and legacy `;`-terminated text (`ping;`, `appID=...,type;`) — the latter is
  what the Godot GDExtension wallpaper speaks, so it must keep working.
- **`ping` frames keep the connection alive** (`m_pingAliveTimer`). A ping can
  arrive in the same packet as another message — that's exactly why the frame
  buffer exists.

## Traps

- **`sendMessage` must not block the caller.** Do not `waitForBytesWritten` — a
  hung peer with a full pipe froze the manager's GUI thread (30 s default) here
  before. Redirected log output is JSON-wrapped (`redirectedLog`) so arbitrary
  log text can't corrupt the frame stream.
- There is **no** `global_sdkPtr`. The Qt message handler reaches the instance
  via `static std::atomic<ScreenPlaySDK*> s_instance`; a previous global pointer
  caused infinite recursion, a null-deref crash, and dangled on destruction.
- `close()` is **async** — the disconnect completes later; don't assume it
  returns a meaningful synchronous result (it used to always return `false`).
- Emit `sdkDisconnected` exactly once.

## Tests

`tst_sdk` covers `readyRead` protocol parsing, `sendMessage` round-trip, and the
`close()` regression. Run it after any protocol change on either side.
