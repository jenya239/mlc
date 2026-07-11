# Track: WebSocket framing/handshake — на MLC, не C++

Parent: [../FFI_LAYER.md](../FFI_LAYER.md) §8,
[TRACK_STDLIB_HTTP_MLC](../archive/tracks/TRACK_STDLIB_HTTP_MLC.md) (dependency closed),
[TRACK_FFI_SHIM_MIGRATION](../archive/tracks/TRACK_FFI_SHIM_MIGRATION.md) (closed),
[../PLAN.md](../PLAN.md).
Trigger: пользователь 2026-07-11 — без ручного C++ везде. `websocket.hpp` —
RFC 6455 framing/handshake, не биндинг к внешней библиотеке.

## Status: **active** — STEP=1 next (Decision + SHA1 check)

**Planner 2026-07-11:** activated after FFI_SHIM Critic OK (`effde300`).
Dependencies satisfied (HTTP_MLC + FFI_SHIM). Prefer over MSDF/LOGIC/GLAD
(§20 order: WebSocket was queued after shim). Note: `TRACK_CONCURRENCY_SPAWN_DETACH`
remains higher for real MT HTTP accept-loops — not this track.

## Проблема

`runtime/include/mlc/net/websocket.hpp` (`#include` — только внутренние
`http_request.hpp`/`http_server.hpp`/`tcp_bridge.hpp` + STL, ни одной внешней
библиотеки): upgrade handshake (`Sec-WebSocket-Accept` = base64(SHA1(key +
GUID))), frame parsing (opcode/mask/payload length varints), text-frame
read/write. Всё — байтовая логика, портируемая на MLC.

## Цель

`std/net/websocket.mlc` работает через чистый MLC frame parser/writer +
handshake, поверх `Tcp` + MLC `HttpRequest`/`HttpResponse`. SHA1 для
`Sec-WebSocket-Accept`: libsodium не даёт SHA1 — **MLC SHA1** (~80 LOC).

## Decision (STEP=1) — lock on Driver turn

| Вопрос | Вариант | Рекомендация |
|--------|---------|--------------|
| SHA1 для handshake | (a) чистый MLC; (b) внешняя зависимость | (a) — libsodium без SHA1; RFC 6455 требует |
| Base64 | MLC encode | да, ~30 строк |
| Frame masking | XOR на byte string/`Array` | паритет с C++ |
| Public API | Freeze `upgrade`/`read_text`/`write_text`/`close`/`last_error` i32 handles | да |
| `websocket_http.hpp` | Keep until handshake uses MLC HttpServer types | delete with `.hpp` on STEP=5 if unused |

## Scope

1. Decision + verify no `crypto_hash_sha1` in local sodium headers.
2. MLC SHA1 + base64 (module or private fns in websocket.mlc).
3. Frame parser/writer (opcode, mask, payload length 7/16/64-bit) на MLC.
4. Handshake на MLC поверх `HttpRequest`/`HttpResponse`.
5. Удалить `websocket.hpp` (+ thin bridge if any); переключить `websocket.mlc`.
6. Regression: `run_websocket_gate.sh`; self-host diff; close.

## Out of scope

- Binary frames, ping/pong, close-frame edge cases — паритет с текущим C++.
- Compression (`permessage-deflate`).
- Fixing `spawn` fire-and-forget (`TRACK_CONCURRENCY_SPAWN_DETACH`).

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | Decision + libsodium SHA1 availability check. | pending |
| 2 | MLC SHA1 + base64. | pending |
| 3 | Frame parser/writer на MLC. | pending |
| 4 | Handshake на MLC (HttpServer types). | pending |
| 5 | Удалить `websocket.hpp`; переключить модуль. | pending |
| 6 | Regression + self-host diff; close. | pending |

<!-- STEP=1 sub-steps: 1) grep sodium headers / docs for sha1; 2) lock Decision table in TRACK; 3) note API freeze + SHA1=MLC; 4) PLAN/DEVELOPMENT already active -->
<!-- STEP=2 sub-steps: 1) `sha1`/`base64_encode` in crypto or websocket.mlc; 2) known-answer vectors (RFC 6455 key→accept); 3) mlcc smoke -->
<!-- STEP=3 sub-steps: 1) port frame parse/write from websocket.hpp; 2) text frame roundtrip smoke; 3) mask XOR parity -->
<!-- STEP=4 sub-steps: 1) upgrade via HttpServer parse + Sec-WebSocket-Accept; 2) wire Tcp fd; 3) demo/gate path -->
<!-- STEP=5 sub-steps: 1) websocket.mlc → MLC bodies / thin externs only; 2) delete websocket.hpp; 3) fix includes -->
<!-- STEP=6 sub-steps: 1) run_websocket_gate.sh; 2) self-host diff; 3) regression_gate; close → Critic -->
