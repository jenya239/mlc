# Track: Postgres driver (libpq) stdlib

Parent: [../PLAN.md](../PLAN.md), [../STDLIB_BACKEND.md](../STDLIB_BACKEND.md) §5.2,
[../FFI_LAYER.md](../FFI_LAYER.md) §5.
Trigger: FFI_LAYER **closed**; first real FFI proof-of-concept is libpq
(connect + exec + status + clear). Backend apps need a minimal query API.

## Status: **open** — STEP=3 next (`postgres.mlc` stdlib)

**Planner 2026-07-10:** opened after closed NET_SERVER / SPAWN /
RUBY_PARITY. Pipeline note (Decision C): MLC `std/` surface via **Ruby**
`common/stdlib` (same as `Tcp`); runtime C++ under `mlc::db` usable from
both. Do **not** require language `spawn` for v1.

## Decision (STEP=1, 2026-07-10)

### Pipeline

- MLC: `lib/mlc/common/stdlib/db/postgres.mlc` (Ruby stdlib; Tcp pattern).
- Runtime: `runtime/include/mlc/db/postgres.hpp` under `mlc::db`.
- Link: `extern lib "pq"` / `-lpq` for programs that use the module.
- No language `spawn`; blocking only.

### Types / handles (MLC surface)

| Name | Role |
|------|------|
| connection handle | Opaque positive `i32` token (not a raw `PGconn*`). |
| result handle | Opaque positive `i32` token (not a raw `PGresult*`). |
| cell values | `string` only in v1 (`PQgetvalue` → copy to `mlc::String`). |

No MLC `extern type PGconn` in v1 stdlib — C++ owns pointers behind the
handle table (same as Tcp fd table). Direct FFI `RawPointer` demos stay
out of this track's public API.

### Methods (v1)

```
Postgres.connect(conninfo: string) -> Option<i32>
Postgres.exec(connection: i32, sql: string) -> Option<i32>
Postgres.ntuples(result: i32) -> i32
Postgres.nfields(result: i32) -> i32
Postgres.getvalue(result: i32, row: i32, column: i32) -> Option<string>
Postgres.clear(result: i32) -> unit
Postgres.finish(connection: i32) -> unit
Postgres.last_error() -> string
```

`conninfo` is a libpq conninfo string (e.g. `host=… dbname=…` or URI).

### Error model

- Failures → `None` / empty Option + `last_error()` (Tcp shipping pattern).
  **Do not** ship Decision-only `Result<PgConnection,_>` in MLC v1 (NET_SERVER
  Critic residual: Decision Result vs MLC Option+handles).
- C++ runtime may use `mlc::Result` / RAII internally; MLC boundary stays
  Option+handles.
- No panic on connect/query failure.

### libpq calls in v1

| Call | Use |
|------|-----|
| `PQconnectdb` | connect |
| `PQstatus` / `PQerrorMessage` | status + `last_error` |
| `PQexec` | exec |
| `PQresultStatus` | treat non-`PGRES_TUPLES_OK` / `PGRES_COMMAND_OK` as error |
| `PQntuples` / `PQnfields` / `PQgetvalue` | read cells |
| `PQclear` / `PQfinish` | free |

Out of v1 API: prepared statements, `PQsend*`, COPY, NOTICE callbacks.

### Build / CI

1. **libpq:** required to **compile+link** postgres runtime tests and any
   binary that imports `Postgres`. Default `mlcc` / core runtime build does
   **not** hard-require `-lpq`. If headers/libs missing: skip postgres C++
   smoke with a clear message (no stub that silently succeeds queries).
2. **Live DB:** env-gated only (`DATABASE_URL` or `PGHOST`+`PGDATABASE`).
   Default CI = link/compile smoke when libpq present; no live `SELECT 1`
   unless env set.

### Non-goals (locked)

ORM, pool, transactions helpers, COPY, LISTEN/NOTIFY, async/`spawn`,
typed row mapping beyond string cells.

### Open questions — resolved

1. **Handles vs Result?** Option+`i32`+`last_error` (Tcp as shipped).
2. **libpq always?** No — only for postgres consumers/tests; skip if absent.
3. **Live DB in CI?** No by default; env-gated.

## Goal

Minimal blocking Postgres client: connect, exec simple query, read
status/error, clear result, disconnect. Validate FFI (`extern`/`RawPointer`/
`-lpq`) end-to-end. Not an ORM.

## Constraints (STDLIB_BACKEND §3)

- No dynamic ORM query builder; no reflection DI.
- Prefer C++ runtime header + thin `std/db/*.mlc` (Tcp pattern).
- Errors → `Option`+`last_error` (Decision); no panic on connection/query failure.
- Optional: skip live DB in CI if no Postgres — provide compile/link smoke
  + documented manual gate.

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | Design: public API (`PgConnection` / `PgResult` or opaque handles); error model; which libpq calls in v1 (`PQconnectdb`, `PQstatus`, `PQerrorMessage`, `PQexec`, `PQclear`, `PQfinish`, maybe `PQntuples`/`PQgetvalue`); link flag `-lpq`. Document in «Decision». | **done** (2026-07-10: see Decision) |
| 2 | Runtime: `runtime/include/mlc/db/postgres.hpp` — handle-table free fns over libpq; C++ smoke (compile+link `-lpq`; live query optional). | **done** (2026-07-10: `mlc::db::*`; `scripts/run_postgres_runtime_smoke.sh` 7/0) |
| 3 | Stdlib: `lib/mlc/common/stdlib/db/postgres.mlc` + registry/scanner; codegen include; smoke compile from MLC (Ruby pipeline). | pending |
| 4 | Gate: script under `scripts/` or `test/mlc/` — at least link smoke; if `PGHOST`/`DATABASE_URL` set, run `SELECT 1` roundtrip. | pending |
| 5 | Docs (`STDLIB_BACKEND.md` / short `MLC.md` note) + example under `misc/examples/`; `regression_gate.sh` (self-host if `compiler/**` touched) + close. | pending |

<!-- sub-steps STEP=1: 1) list types/methods in Decision; 2) Result vs Option+handles (mirror Tcp); 3) non-goals (ORM, pool, prepared stmts, COPY) -->
<!-- sub-steps STEP=2: 1) postgres.hpp wrap PQ*; 2) test_postgres.cpp smoke; 3) include from mlc.hpp if needed -->
<!-- sub-steps STEP=3: 1) postgres.mlc externs; 2) registry; 3) MLC compile smoke -->
<!-- sub-steps STEP=4: 1) gate script; 2) env-gated live query; 3) SESSION verify -->
<!-- sub-steps STEP=5: 1) docs+example; 2) regression_gate; 3) archive -->

## Out of scope (this track)

- Connection pool, prepared statements, transactions helpers, COPY, LISTEN/NOTIFY
- Async/`spawn` query API
- ORM / sqlx-style compile-time SQL checking
- Crypto / WebSocket / other STDLIB_BACKEND items

## Open questions

None for v1 — resolved in Decision above.
