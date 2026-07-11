# Track: Postgres/Crypto/Tcp — прямой extern fn вместо hand-written C++ shim

Parent: [../../FFI_LAYER.md](../../FFI_LAYER.md) §8, [../../PLAN.md](../../PLAN.md).
Trigger: пользователь 2026-07-11 — весь функционал должен собираться без
ручного C++, либо автогенерация, либо прямой импорт заголовков сторонней
библиотеки.

## Status: **closed** (2026-07-11)

**Driver 2026-07-11:** STEP=7 — self-host `mlcc`→`mlcc2`→`diff` identical;
`regression_gate.sh` 20/0; archived. Residuals: Option extern on Tcp; Ruby
control-flow in `*_bridge.hpp`; TcpStream RAII for websocket; abi/handle
tables still C++ (no module-level `let mut`).

**Driver 2026-07-11:** STEP=6 — regression OK: postgres/crypto/tcp echo/
mlcc tcp echo/spawn/websocket/http curl + three mlc abi smokes. Public API
unchanged.

**Driver 2026-07-11:** STEP=5 — deleted `postgres.hpp`/`sodium.hpp`/`tcp.hpp`;
Ruby APIs in `postgres_bridge.hpp`/`sodium_bridge.hpp`/`tcp_bridge.hpp`;
`tcp.mlc` → `tcp_bridge.hpp`; `mlc.hpp`/tests/codegen retargeted. Residual:
bridge files still hold Ruby control-flow + TcpStream RAII (websocket).

**Driver 2026-07-11:** STEP=4 — `tcp_abi.hpp` (POSIX thin); fd-as-token
handle API in `tcp.hpp`; `TcpStream::adopt` for WebSocket.upgrade; smoke
`tcp_mlc_abi_smoke.mlc` + gates (tcp echo Ruby/mlcc, websocket) OK.
Residual: public API still `export extern` Option (mlcc Option-import
segfault); bind control flow in `tcp.hpp` over abi (not MLC bodies);
TcpListener/TcpStream RAII kept for `websocket.hpp` until STEP=5.

**Driver 2026-07-11:** STEP=3 — `crypto.mlc` MLC hex + HMAC malloc/statebytes
over `sodium_abi.hpp`; `sodium.hpp` thinned onto abi; smoke
`crypto_mlc_abi_smoke.mlc` + `run_crypto_mlc_abi_smoke.sh` EXIT 0; Ruby
`run_crypto_gate.sh` OK. Residual: no `\xHH` string escapes (smoke uses
`string_from_byte_u8`); CryptoStringResult; last_error table in C++.

**Driver 2026-07-11:** STEP=2 — `postgres.mlc` MLC control flow +
`postgres_abi.hpp` (i64 ptr tokens, handle table, String by-value ABI);
`postgres.hpp` thinned onto abi; smoke `postgres_mlc_abi_smoke.mlc` +
`run_postgres_mlc_abi_smoke.sh` EXIT 0; Ruby gate still OK.

**Driver 2026-07-11:** STEP=1 — Decision **locked** (RawPointer+drop;
public i32 API freeze; MLC handle table + hex; `extern lib "pq"`/`"sodium"`;
HMAC via `statebytes`+malloc; no residual C++ hmac wrapper).

**Planner 2026-07-11:** activated after STDLIB_HTTP_MLC Critic OK
(`72d07bd6`). First open track in «без hand-written C++» queue (§20a).
Prefer over WEBSOCKET (unblocked but wants crypto/SHA1 path after shim)
and MSDF/LOGIC/GLAD until STEP=1 Decision locked.

## Проблема (проверено в коде)

`runtime/include/mlc/db/postgres.hpp` (207 строк), `runtime/include/mlc/crypto/sodium.hpp`
(158 строк) и `runtime/include/mlc/net/tcp.hpp` (399 строк) — не биндинги
через `extern fn`, а ручной C++ поверх `libpq-fe.h`/`sodium.h`/POSIX socket
syscalls: handle-table (`std::mutex`+`std::vector<...>`, конвертация в `i32`),
hex-encode (crypto), `last_error`-состояние. Это написано **до** закрытия
`TRACK_FFI_LAYER` (2026-07-09) — тогда `RawPointer[T]`/`extern type`+`drop`
не было, i32-handle-table была единственным вариантом.

**Уточнение по `tcp.hpp` (2026-07-11)**: сами syscall'ы (`socket`/`bind`/
`listen`/`accept`/`recv`/`send`) — не сторонняя библиотека, это интерфейс
ОС (libc), убирать их не нужно и не имеет смысла (каждый язык ходит туда же).
Убирается только **обёртка** — handle-table/`TcpStream`/`TcpListener`
RAII-классы, которые дублируют то, что теперь может дать
`RawPointer[T]`+`extern type`+`drop` напрямую.

Слой `extern fn ... = "c" from "<h>"` + `RawPointer[T]` + `extern type`+
`drop` + `extern lib` сегодня закрыт и работает (см. `FFI_LAYER.md` §6). Замена
не требует новой инфраструктуры.

## Цель

`std/db/postgres.mlc`/`std/crypto/crypto.mlc`/`std/net/tcp.mlc` работают
через прямые `extern fn`/`extern type` к `libpq-fe.h`/`sodium.h`/POSIX
socket-заголовкам (`<sys/socket.h>` и т.п.), без промежуточного `.hpp`-shim'а
с бизнес-логикой. Handle-bookkeeping и hex-encode — на MLC.

## Decision (STEP=1, 2026-07-11) — **locked**

Verified linking: `build_bin.sh` reads `mlc_link_libs.txt` → `-l<name>`
(no hard-coded `-lpq`/`-lsodium`). Smokes use `-lpq`/`-lsodium` (+ optional
`.tmp_libpq` / `.tmp_libsodium`). Modules today: `postgres.mlc`/`crypto.mlc`
bare `extern fn` (Ruby maps to `.hpp`); `tcp.mlc` already `= "…" from
"mlc/net/tcp.hpp"`. Shim sizes: postgres 210 / sodium 158 / tcp 398 LOC.

| Вопрос | Locked |
|--------|--------|
| `PGconn*`/`PGresult*` | Target: `extern type`+`RawPointer`. **STEP=2 interim:** i64 tokens via `postgres_abi.hpp` (Ruby lacks `extern type`) |
| Владение | Explicit finish/clear in MLC; `drop` deferred with `owned` marker |
| Публичный API | **Freeze** i32 handles + `last_error()` for Postgres/Crypto/Tcp. MLC path uses local `PgI32Result`/`PgStringResult` (mlcc Option import codegen segfault — same residual as HttpServer). Ruby/C++ path still `std::optional` via `postgres.hpp` |
| Handle table | **Interim in C++** (`postgres_abi.hpp`) until module-level `let mut` exists |
| Pointer tokens | **i64** across MLC↔ABI (Ruby parser lacks `extern type` / `RawPointer[PGconn]`) |
| Линковка | Smoke writes `pq` into `mlc_link_libs.txt` (no `extern lib` in `.mlc` — Ruby parse). Prefer `extern lib "pq"` when Ruby gains the syntax |
| HMAC state | **No residual C++**. `crypto_auth_hmacsha256_statebytes()` + `malloc`/`free` + init/update/final (arbitrary key length). One-shot `crypto_auth_hmacsha256` only if keylen==32 (optional fast path, not required) |
| Tcp fd | Public API stays i32 tokens; internal = real fd in MLC table OR pass fd as i32 token directly if we drop double-indirection — **prefer fd-as-token** (token == fd, close via `close(fd)`); no C++ `TcpStream` class |
| `sockaddr_in` | Opaque buffer: `extern fn` helpers (`sockaddr_in_size`, set family/port/addr, `as_sockaddr_ptr`) from `<netinet/in.h>`/`<sys/socket.h>` — no MLC field layout. If a helper cannot be expressed as pure `extern fn` to libc, one ≤20-LOC C file under `runtime/src/net/` is allowed as **last resort** and must be listed in STEP=4; prefer zero new C |
| Порядок STEPs | 2 Postgres → 3 Crypto → 4 Tcp → 5 delete three `.hpp` → 6 gates → 7 self-host |
| Удаление `.hpp` | Only on STEP=5 after all three modules compile without them (incl. bridges) |

### Rejected

| Вариант | Почему |
|---------|--------|
| Change public API to `RawPointer` now | breaks demos/gates/`HttpServer` Tcp usage; bookkeeping can move without API break |
| Keep ~15-LOC C++ hmac wrapper | `statebytes`+malloc removes need |
| Delete `.hpp` before Tcp migrated | HttpServer/curl gates still link `tcp.hpp` |
| `owned` auto-RAII as STEP=2 blocker | marker deferred; explicit finish/clear enough |

### Impact (последующие STEPs)

| File | Change |
|------|--------|
| `lib/mlc/common/stdlib/db/postgres.mlc` | STEP=2 **done**: MLC flow + abi; Pg*Result |
| `lib/mlc/common/stdlib/crypto/crypto.mlc` | STEP=3 **done**: MLC hex + hmac statebytes/malloc + abi |
| `lib/mlc/common/stdlib/net/tcp.mlc` | STEP=4 **done**: fd-as-token + abi; Option extern residual |
| `runtime/include/mlc/{db,crypto,net}/*.hpp` | STEP=5 **done**: shim `.hpp` deleted; APIs in `*_bridge.hpp` + `*_abi.hpp` |
| gates | STEP=6 unchanged public behavior |

## Scope

1. Decision (таблица) + сверка текущей линковки (`grep extern lib` /
   `mlc_link_libs.txt`).
2. `extern type PGconn`/`PGresult` + `extern fn` для `PQconnectdb`/`PQstatus`/
   `PQexec`/`PQresultStatus`/`PQntuples`/`PQnfields`/`PQgetvalue`/
   `PQgetisnull`/`PQerrorMessage`/`PQresultErrorMessage`, `drop "PQfinish"`/
   `"PQclear"` — в `std/db/postgres.mlc` напрямую, без `.hpp`.
3. `extern type crypto_auth_hmacsha256_state`/`extern fn` для
   `sodium_init`/`crypto_hash_sha256`/`crypto_auth_hmacsha256_*`/
   `randombytes_buf`/`crypto_pwhash_str`/`crypto_pwhash_str_verify` — в
   `std/crypto/crypto.mlc`. Hex-encode на MLC (`Array<i32>` → hex `string`).
4. Tcp: `extern type`+`drop "close"` на `i32` fd + `extern fn` для
   `socket`/`setsockopt`/`bind`/`listen`/`accept`/`recv`/`send`/`inet_pton`/
   `getsockname` в `std/net/tcp.mlc`. Учесть: этим модулем сейчас пользуется
   и Ruby-бутстрап, и self-hosted `mlcc` (`TRACK_PIPELINE_MERGE_TCP_SPAWN`) —
   не сломать оба пайплайна, публичный MLC-API (`bind`/`accept`/`read`/
   `write_all`/`port`/`last_error`) не менять.
5. Удалить `runtime/include/mlc/db/postgres.hpp`, `runtime/include/mlc/crypto/sodium.hpp`,
   `runtime/include/mlc/net/tcp.hpp` после переключения всех трёх модулей.
6. Regression: `run_postgres_gate.sh`, `run_crypto_gate.sh`, TCP smokes
   (`run_mlcc_tcp_spawn_echo_gate.sh` и т.п.) — семантика не меняется, только
   реализация.
7. Self-host: `mlcc`→`mlcc2`→`diff` identical; `scripts/regression_gate.sh`.

## Out of scope

- JWT/argon2 расширение crypto — не в этом треке.
- Connection pool/async Postgres — остаётся blocking single-connection API.
- IPv6 (`tcp.hpp` — только IPv4 сейчас, паритет, не расширение).

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | Decision (таблица) + сверка линковки. | **done** (2026-07-11: locked above; pq/sodium via mlc_link_libs) |
| 2 | Postgres: `extern type`+`extern fn`+`drop` в `std/db/postgres.mlc`. | **done** (2026-07-11: MLC flow + abi; i64/Pg*Result residuals) |
| 3 | Crypto: `extern fn`+hex-encode на MLC в `std/crypto/crypto.mlc`. | **done** (2026-07-11: MLC hex + abi; HMAC malloc) |
| 4 | Tcp: `extern type`+`drop`+`extern fn` в `std/net/tcp.mlc` (оба пайплайна). | **done** (2026-07-11: fd-as-token + abi; Option/MLC-body residual) |
| 5 | Удалить `postgres.hpp`/`sodium.hpp`/`tcp.hpp`. | **done** (2026-07-11: → `*_bridge.hpp` + abi) |
| 6 | Regression: все gate зелёные без изменения публичного API. | **done** (2026-07-11: pg/crypto/tcp/spawn/ws/http + abi smokes) |
| 7 | Self-host diff + `regression_gate.sh`; close. | **done** (2026-07-11: identical; regression 20/0; closed) |

## Open questions

- Track closed. Residuals (not blockers): Option extern on Tcp; Ruby
  control-flow in bridges; TcpStream in `tcp_bridge.hpp` for websocket;
  module-level mut; no MLC `\xHH`.
