# Track: WebSocket (server) over TCP

Parent: [../PLAN.md](../PLAN.md), [../STDLIB_BACKEND.md](../STDLIB_BACKEND.md) §5.4,
[../archive/tracks/TRACK_STDLIB_NET_SERVER.md](../archive/tracks/TRACK_STDLIB_NET_SERVER.md).
Trigger: CRYPTO **closed**; STDLIB_BACKEND §5 next is WebSocket upgrade +
frames on top of existing TCP/HTTP server runtime.

## Status: **open** — STEP=3 next (stdlib `websocket.mlc`)

**Planner 2026-07-10:** opened after closed STDLIB_CRYPTO Critic. Chose
WebSocket over job-queue (§5 order; depends on NET_SERVER). Pipeline
(Decision C): MLC via Ruby `common/stdlib`; runtime C++ under `mlc::net`
(extend existing TCP). No language `spawn` for v1.

**Driver 2026-07-10:** STEP=2 — `websocket.hpp` + C++ smoke (RFC Accept + echo).

## Decision (STEP=1, 2026-07-10)

### Pipeline

- Runtime: `runtime/include/mlc/net/websocket.hpp` under `mlc::net`.
- MLC: `lib/mlc/common/stdlib/net/websocket.mlc`, module `WebSocket`
  (Tcp-style opaque `i32` handles + `last_error`) — **not** C++-only.
- Soft bridge include in codegen (like postgres/crypto bridges).
- Blocking I/O only; no `spawn`.

### Upgrade (server)

Required request: `GET`, `Upgrade: websocket`, `Connection: Upgrade`,
`Sec-WebSocket-Key`, `Sec-WebSocket-Version: 13`. Optional `Sec-WebSocket-Protocol`
ignored in v1 (no subprotocol negotiation).

Response: `101 Switching Protocols` + `Sec-WebSocket-Accept` =
`base64(SHA1(key + GUID))` per RFC 6455. Use existing crypto/SHA1 — prefer
libsodium or a tiny SHA1 in-net if sodium not linked; **Decision:** implement
SHA1 locally in `websocket.hpp` (avoid hard `-lsodium` for WS) or reuse
OpenSSL-free public-domain SHA1 (~small). Do **not** require libsodium for WS.

### Types / methods (MLC)

```
WebSocket.upgrade(stream: i32) -> Option<i32>
WebSocket.read_text(connection: i32) -> Option<string>
WebSocket.write_text(connection: i32, data: string) -> bool
WebSocket.close(connection: i32) -> unit
WebSocket.last_error() -> string
```

`upgrade` reads HTTP request from the Tcp stream handle (or uses already-
buffered request in C++ helper — STEP=2 may expose
`upgrade_from_request(TcpStream&, HttpRequest)` for C++ tests).

C++ mirrors: free functions on `TcpStream` / handle table.
Note: C++ error accessor is `websocket_last_error()` (Tcp already owns
`mlc::net::last_error()`).

### Frames (v1)

| In | Out |
|----|-----|
| Text opcode (0x1), FIN=1, payload ≤ **1 MiB** | Binary (0x2), continuation, RSV, fragmented messages |
| Client→server masked; server→client **unmasked** | Per-message deflate |
| Close (0x8): reply close + tear down | Full close status codes beyond echo |
| Ping (0x9): auto **Pong** with same payload | Unsolicited ping from server |

`read_text`: returns payload string; `None` on protocol error / close / non-text
(after handling ping/close). `write_text`: single FIN text frame.

### Error model

- `Option` / `bool` + `last_error()` (Tcp/Postgres shipped pattern).
- No Decision-only `Result` for MLC v1.
- No panic on protocol/I/O failure.

### Integration

- **Standalone** on accepted `TcpStream` / Tcp handle — **not** wired into
  `serve_http_with_thread_pool` in this track (user/demo calls upgrade after
  accept). Optional later.

### Non-goals (locked)

WSS/TLS, WS client, HTTP/2, job-queue.

### Open questions — resolved

1. **MLC module?** Yes — `WebSocket` + handles (not C++-only).
2. **Text only?** Yes; binary rejected.
3. **ThreadPool serve?** No — standalone upgrade on stream.

## Goal

Minimal **server-side** WebSocket: HTTP/1.1 upgrade handshake, text frame
read/write over an accepted TCP stream. Validate end-to-end echo. Not a
full client library, not WSS.

## Constraints (STDLIB_BACKEND §3)

- Build on `mlc::net` TCP handles / streams (no new socket stack).
- Thin C++ runtime + optional thin `std/net/*.mlc` (Tcp pattern).
- Errors → `Option`/`bool`+`last_error` (Decision); no panic on protocol failure.
- Blocking I/O only in v1.

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | Design: upgrade API (`accept_websocket` / `WsConnection`); frame API (text only?); masking (server→client unmasked); error model; what HTTP headers required. Document in «Decision». | **done** (2026-07-10: see Decision) |
| 2 | Runtime: handshake + frame encode/decode in `runtime/include/mlc/net/websocket.hpp`; C++ smoke (in-process or paired client). | **done** (2026-07-10: SHA1+base64 local; upgrade/read_text/write_text; `run_websocket_runtime_smoke.sh` 19/0) |
| 3 | Stdlib: `std/net/websocket.mlc` + registry + codegen bridge (Decision: MLC module yes). | pending |
| 4 | Gate: script — upgrade + text echo roundtrip (C++ client or `websocat` if available). | pending |
| 5 | Docs (`STDLIB_BACKEND.md` / `MLC.md`) + example; close (regression_gate if `compiler/**`). | pending |

<!-- sub-steps STEP=1: 1) API table; 2) MLC surface vs C++-only; 3) text-only vs binary; 4) non-goals TLS/client -->
<!-- sub-steps STEP=2: 1) Sec-WebSocket-Accept; 2) frame parser; 3) test_websocket.cpp -->
<!-- sub-steps STEP=3: 1) mlc module or skip; 2) registry; 3) codegen smoke -->
<!-- sub-steps STEP=4: 1) gate script; 2) echo assert; 3) SESSION -->
<!-- sub-steps STEP=5: 1) docs+example; 2) archive; 3) Critic -->

## Out of scope (this track)

- WSS / TLS
- WebSocket **client** API
- Per-message deflate, extensions, ping/pong auto-reply (may add minimal later)
- Job-queue, env/logging, JWT

## Open questions

None for v1 — resolved in Decision above.
