# Track: Postgres/Crypto/Tcp — прямой extern fn вместо hand-written C++ shim

Parent: [../FFI_LAYER.md](../FFI_LAYER.md) §8, [../PLAN.md](../PLAN.md).
Trigger: пользователь 2026-07-11 — весь функционал должен собираться без
ручного C++, либо автогенерация, либо прямой импорт заголовков сторонней
библиотеки.

## Status: **active** — STEP=1 next (Decision + link check)

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

## Decision (STEP=1)

| Вопрос | Вариант | Рекомендация |
|--------|---------|--------------|
| `PGconn*`/`PGresult*` | `extern type PGconn = "PGconn" from "<libpq-fe.h>"` + `RawPointer[PGconn]`, вместо i32-handle | RawPointer прямой — handle-table был обходом отсутствия RawPointer, сейчас не нужен |
| Владение | `PQfinish`/`PQclear` через `drop "..."` на `extern type` | да, RAII вместо ручного `finish`/`clear` |
| hex-encode (`sha256`, `random_bytes`) | На MLC: `Array<i32>`/`string` побайтовый encode вместо C++ `to_hex` | да — чистая математика, не FFI |
| Мьютекс на `last_error` | Убрать — `RawPointer` возвращается из вызова напрямую, ошибка — `Option`/`Result` из caller, без глобального состояния | да, глобальный mutable state противоречит `CONCURRENCY_V2` (mutable globals запрещены) |
| Линковка | `extern lib "pq"` / нет отдельной либы у sodium (проверить `-lsodium` в `build_bin.sh`) | сверить с текущим `extern lib` списком |
| `tcp.hpp`: `sockaddr_in`/`socklen_t` | Layout-совместимые struct нужны (`bind`/`getsockname` пишут в них по адресу) — RawPointer на stack-allocated struct через `extern type` без полей (opaque, заполняется/читается только через `extern fn` геттеры/сеттеры, не через MLC record field access) | opaque + accessor-функции (`sockaddr_in_set_port`, и т.п., тонкие C++ inline-геттеры при необходимости — но тоньше, чем текущий класс) |
| `tcp.hpp`: `TcpStream`/`TcpListener` RAII-классы | `extern type` + `drop "close"` на голый `i32` file descriptor вместо C++ move-only класса | да — тот же паттерн, что Postgres/Crypto, `close(fd)` как drop-функция |

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
| 1 | Decision (таблица) + сверка линковки. | **pending** — active |
| 2 | Postgres: `extern type`+`extern fn`+`drop` в `std/db/postgres.mlc`. | pending |
| 3 | Crypto: `extern fn`+hex-encode на MLC в `std/crypto/crypto.mlc`. | pending |
| 4 | Tcp: `extern type`+`drop`+`extern fn` в `std/net/tcp.mlc` (оба пайплайна). | pending |
| 5 | Удалить `postgres.hpp`/`sodium.hpp`/`tcp.hpp`. | pending |
| 6 | Regression: все gate зелёные без изменения публичного API. | pending |
| 7 | Self-host diff + `regression_gate.sh`; close. | pending |

## Open questions

- `crypto_auth_hmacsha256_state` — C struct с internal buffer, не opaque
  pointer (stack-allocated в оригинале). Через `RawPointer` нужен
  heap-allocated instance (`extern fn crypto_auth_hmacsha256_state_new`? —
  libsodium не даёт такой функции). Решить на STEP=1: либо
  `extern type ... from <header>` с размером через `sizeof`-подобный трюк
  (нет в MLC), либо оставить `hmac_sha256` как единственную функцию с
  тремя C-вызовами подряд без промежуточного MLC-хранения state (state
  живёт только в одном C++ stack-frame внутри одного `extern fn`-обёртки —
  тогда это **не полностью убирается**, тонкий C++ на 15 строк остаётся;
  зафиксировать как единственное обоснованное исключение, если так).
