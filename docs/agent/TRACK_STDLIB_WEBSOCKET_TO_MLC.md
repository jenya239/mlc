# Track: WebSocket framing/handshake — на MLC, не C++

Parent: [../FFI_LAYER.md](../FFI_LAYER.md) §8,
[TRACK_STDLIB_HTTP_MLC](../archive/tracks/TRACK_STDLIB_HTTP_MLC.md) (dependency closed),
[TRACK_FFI_SHIM_MIGRATION](../archive/tracks/TRACK_FFI_SHIM_MIGRATION.md) (closed),
[../PLAN.md](../PLAN.md).
Trigger: пользователь 2026-07-11 — без ручного C++ везде. `websocket.hpp` —
RFC 6455 framing/handshake, не биндинг к внешней библиотеке.

## Status: **active** — STEP=4 next (handshake on HttpServer)

**Driver 2026-07-11:** STEP=3 — MLC `encode_unmasked_frame` /
`encode_masked_frame` / `try_decode_frame` (`WsFrameDecode`);
`encode_text_frame`/`encode_close_frame`; smoke
`websocket_frame_smoke.mlc` (unmasked + masked XOR + len126) EXIT 0;
sha1 smoke + websocket gate still OK. Public upgrade/read/write remain
extern.

**Driver 2026-07-11:** STEP=2 — MLC `sha1_hex`/`sec_websocket_accept` in
`websocket.mlc`; `websocket_abi.hpp` byte helpers; smoke
`websocket_sha1_smoke.mlc` (empty SHA1 + RFC 6455 Accept) EXIT 0; websocket
gate still OK. Public upgrade/read/write remain extern.

**Driver 2026-07-11:** STEP=1 — Decision **locked**. Verified local
`.tmp_libsodium/usr/include` — **0** `sha1` matches in sodium headers.
Current C++ already has local SHA1+base64 in `websocket.hpp` (no sodium).
Public API freeze; SHA1/base64 → private fns in `websocket.mlc` (not Crypto).

**Planner 2026-07-11:** activated after FFI_SHIM Critic OK (`effde300`).

## Проблема

`runtime/include/mlc/net/websocket.hpp` (~564 LOC) + `websocket_http.hpp`
(~300 LOC): upgrade handshake (`Sec-WebSocket-Accept` = base64(SHA1(key +
GUID))), frame parsing, text-frame read/write. Не биндинг к внешней
библиотеке — байтовая логика на C++. `websocket.mlc` сегодня `export extern`
→ `websocket.hpp`.

## Цель

`std/net/websocket.mlc` — чистый MLC frame parser/writer + handshake поверх
`Tcp` + MLC `HttpRequest`/`HttpResponse`. SHA1/base64 на MLC (libsodium без
SHA1). Удалить `websocket.hpp` / `websocket_http.hpp` на STEP=5.

## Decision (STEP=1, 2026-07-11) — **locked**

Verified: `rg -i sha1` on `.tmp_libsodium/usr/include/**/*.h` → **0 hits**.
`crypto.mlc` exposes only SHA-256/HMAC/pwhash. Existing `websocket.hpp`
already documents «SHA1 + base64 local (no libsodium)».

| Вопрос | Locked |
|--------|--------|
| SHA1 | **MLC** — port compact SHA-1 from `websocket.hpp` (no new dep) |
| Base64 | **MLC encode** (accept-header only; decode not required for server) |
| Placement | Private fns in `websocket.mlc` (not `Crypto` public API) |
| Frame masking | Byte XOR parity with current C++ |
| Public API | **Freeze** `upgrade`/`read_text`/`write_text`/`close`/`last_error` + i32 handles |
| Option returns | Same residual as Tcp — prefer keep Option extern until MLC bodies; local sum if Option import segfaults |
| `websocket_http.hpp` | Delete with `websocket.hpp` on STEP=5 (HTTP parse moves to HttpServer MLC) |
| Порядок | 2 SHA1+b64 → 3 frames → 4 handshake → 5 delete hpp → 6 gates/close |

### Rejected

| Вариант | Почему |
|---------|--------|
| libsodium / OpenSSL SHA1 | sodium has no SHA1; new dep out of scope |
| Expose `Crypto.sha1` public | handshake-only; keep Crypto = sodium surface |
| Expand to binary/ping/pong | parity with current C++ only |

## Scope

1. Decision + sodium SHA1 check — **done**.
2. MLC SHA1 + base64 + RFC 6455 known-answer (`dGhlIHNhbXBsZSBub25jZQ==` → accept).
3. Frame parser/writer на MLC.
4. Handshake на MLC поверх HttpServer.
5. Удалить `websocket.hpp`/`websocket_http.hpp`; переключить модуль.
6. `run_websocket_gate.sh` + self-host; close.

## Out of scope

- Binary frames, ping/pong, close-frame edge cases beyond current C++.
- Compression (`permessage-deflate`).
- `TRACK_CONCURRENCY_SPAWN_DETACH`.

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | Decision + libsodium SHA1 availability check. | **done** (2026-07-11: locked; 0 sodium sha1 hits) |
| 2 | MLC SHA1 + base64. | **done** (2026-07-11: sha1_hex + sec_websocket_accept; RFC smoke) |
| 3 | Frame parser/writer на MLC. | **done** (2026-07-11: encode/decode + mask XOR smoke) |
| 4 | Handshake на MLC (HttpServer types). | pending |
| 5 | Удалить `websocket.hpp`; переключить модуль. | pending |
| 6 | Regression + self-host diff; close. | pending |

<!-- STEP=2 sub-steps: 1) port sha1+base64 to websocket.mlc; 2) RFC 6455 key→accept vector smoke; 3) mlcc --check-only / link smoke -->
<!-- STEP=3 sub-steps: 1) port frame parse/write; 2) text frame roundtrip; 3) mask XOR parity -->
<!-- STEP=4 sub-steps: 1) upgrade via HttpServer + Accept; 2) Tcp fd wire; 3) demo/gate -->
<!-- STEP=5 sub-steps: 1) MLC bodies; 2) delete hpp; 3) fix includes/bridge -->
<!-- STEP=6 sub-steps: 1) websocket gate; 2) self-host; 3) close → Critic -->
