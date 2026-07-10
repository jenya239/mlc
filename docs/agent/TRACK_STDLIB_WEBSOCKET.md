# Track: WebSocket (server) over TCP

Parent: [../PLAN.md](../PLAN.md), [../STDLIB_BACKEND.md](../STDLIB_BACKEND.md) §5.4,
[../archive/tracks/TRACK_STDLIB_NET_SERVER.md](../archive/tracks/TRACK_STDLIB_NET_SERVER.md).
Trigger: CRYPTO **closed**; STDLIB_BACKEND §5 next is WebSocket upgrade +
frames on top of existing TCP/HTTP server runtime.

## Status: **open** — STEP=1 next (API Decision)

**Planner 2026-07-10:** opened after closed STDLIB_CRYPTO Critic. Chose
WebSocket over job-queue (§5 order; depends on NET_SERVER). Pipeline
(Decision C): MLC via Ruby `common/stdlib`; runtime C++ under `mlc::net`
(extend existing TCP). No language `spawn` for v1.

## Goal

Minimal **server-side** WebSocket: HTTP/1.1 upgrade handshake, text frame
read/write over an accepted TCP stream. Validate end-to-end echo. Not a
full client library, not WSS.

## Constraints (STDLIB_BACKEND §3)

- Build on `mlc::net` TCP handles / streams (no new socket stack).
- Thin C++ runtime + optional thin `std/net/*.mlc` (Tcp pattern).
- Errors → `Option`/`bool`+`last_error` unless Decision picks otherwise —
  **lock in STEP=1**; avoid Decision/ship split.
- Blocking I/O only in v1.

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | Design: upgrade API (`accept_websocket` / `WsConnection`); frame API (text only?); masking (server→client unmasked); error model; what HTTP headers required. Document in «Decision». | **pending** |
| 2 | Runtime: handshake + frame encode/decode in `runtime/include/mlc/net/` (e.g. `websocket.hpp`); C++ smoke (in-process or paired client). | pending |
| 3 | Stdlib (optional if C++-only like HTTP router): `std/net/websocket.mlc` + registry **or** document C++-only + Tcp reuse — lock in Decision. | pending |
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

## Open questions (resolve in STEP=1)

1. MLC `WebSocket` module (Tcp-style handles) vs C++-only like HTTP router?
2. Text frames only in v1, or binary too?
3. Integrate with `serve_http_with_thread_pool` or standalone upgrade on `TcpStream`?
