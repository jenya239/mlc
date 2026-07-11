# Track: WebSocket framing/handshake — на MLC, не C++

Parent: [../FFI_LAYER.md](../FFI_LAYER.md) §8,
[TRACK_STDLIB_HTTP_MLC](../archive/tracks/TRACK_STDLIB_HTTP_MLC.md) (dependency closed),
[../PLAN.md](../PLAN.md).
Trigger: пользователь 2026-07-11 — без ручного C++ везде. `websocket.hpp` —
RFC 6455 framing/handshake, не биндинг к внешней библиотеке.

## Status: open — unblocked (HTTP_MLC closed); queue after FFI_SHIM Decision

**Planner 2026-07-11:** HTTP dependency satisfied. Still prefer
`TRACK_FFI_SHIM_MIGRATION` first (crypto/SHA1 notes in Goal). Not activated.

## Проблема

`runtime/include/mlc/net/websocket.hpp` (`#include` — только внутренние
`http_request.hpp`/`http_server.hpp`/`tcp.hpp` + STL, ни одной внешней
библиотеки): upgrade handshake (`Sec-WebSocket-Accept` = base64(SHA1(key +
GUID))), frame parsing (opcode/mask/payload length varints), text-frame
read/write. Всё — байтовая логика, портируемая на MLC.

## Цель

`std/net/websocket.mlc` работает через чистый MLC frame parser/writer +
handshake, поверх `Tcp` + (после `STDLIB_HTTP_MLC`) MLC `HttpRequest`/
`HttpResponse`. Единственная нужная внешняя примитив — SHA1 (для
`Sec-WebSocket-Accept`): взять из `crypto` модуля (после
[TRACK_FFI_SHIM_MIGRATION](TRACK_FFI_SHIM_MIGRATION.md) — libsodium не
даёт SHA1 напрямую, только SHA256/512 — проверить на STEP=1, возможна
необходимость implement SHA1 на чистом MLC, это ~80 строк известного
алгоритма, не блокер).

## Decision (STEP=1)

| Вопрос | Вариант | Рекомендация |
|--------|---------|--------------|
| SHA1 для handshake | (a) чистый MLC-алгоритм (SHA1 — публичный, ~80 строк); (b) внешняя зависимость | (a) — libsodium сознательно не включает SHA1 (считается legacy/insecure для крипто, но обязателен по RFC 6455 handshake), писать на MLC |
| Base64 | MLC-реализация (encode достаточно для этого use-case) | да, ~30 строк, чистая математика |
| Frame masking (XOR client→server) | Побайтовый XOR на `Array<i32>`/`string` | прямое портирование C++ логики 1:1 |

## Scope

1. Decision + проверка libsodium API (нет ли `crypto_hash_sha1` в каком-то
   виде — если нет, писать MLC SHA1).
2. MLC SHA1 + base64 (если не переиспользуть что-то из `crypto`-трека).
3. Frame parser/writer (opcode, mask, payload length 7/16/64-bit) на MLC.
4. Handshake на MLC поверх `HttpRequest`/`HttpResponse` (после HTTP_MLC).
5. Удалить `websocket.hpp`; переключить `std/net/websocket.mlc`.
6. Regression: `run_websocket_gate.sh` (адаптировать), self-host diff.

## Out of scope

- Binary frames, ping/pong, close-frame edge cases — если текущий C++ их не
  покрывал, не добавлять сейчас (паритет, не расширение).
- Compression extensions (`permessage-deflate`) — не было, не добавлять.

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | Decision + libsodium SHA1 availability check. | pending |
| 2 | MLC SHA1 + base64. | pending |
| 3 | Frame parser/writer на MLC. | pending |
| 4 | Handshake на MLC (после `STDLIB_HTTP_MLC` closed). | pending |
| 5 | Удалить `websocket.hpp`; переключить модуль. | pending |
| 6 | Regression + self-host diff; close. | pending |
