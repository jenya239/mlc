# Track: Postgres driver (libpq) stdlib

Parent: [../PLAN.md](../PLAN.md), [../STDLIB_BACKEND.md](../STDLIB_BACKEND.md) §5.2,
[../FFI_LAYER.md](../FFI_LAYER.md) §5.
Trigger: FFI_LAYER **closed**; first real FFI proof-of-concept is libpq
(connect + exec + status + clear). Backend apps need a minimal query API.

## Status: **open** — STEP=1 next (API Decision)

**Planner 2026-07-10:** opened after closed NET_SERVER / SPAWN /
RUBY_PARITY. Pipeline note (Decision C): MLC `std/` surface via **Ruby**
`common/stdlib` (same as `Tcp`); runtime C++ under `mlc::db` usable from
both. Do **not** require language `spawn` for v1.

## Goal

Minimal blocking Postgres client: connect, exec simple query, read
status/error, clear result, disconnect. Validate FFI (`extern`/`RawPointer`/
`-lpq`) end-to-end. Not an ORM.

## Constraints (STDLIB_BACKEND §3)

- No dynamic ORM query builder; no reflection DI.
- Prefer C++ runtime header + thin `std/db/*.mlc` (Tcp pattern).
- Errors → `Result` / `Option`+`last_error` (pick in Decision); no panic on
  connection/query failure.
- Optional: skip live DB in CI if no Postgres — provide compile/link smoke
  + documented manual gate.

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | Design: public API (`PgConnection` / `PgResult` or opaque handles); error model; which libpq calls in v1 (`PQconnectdb`, `PQstatus`, `PQerrorMessage`, `PQexec`, `PQclear`, `PQfinish`, maybe `PQntuples`/`PQgetvalue`); link flag `-lpq`. Document in «Decision». | **pending** |
| 2 | Runtime: `runtime/include/mlc/db/postgres.hpp` — RAII or free-fn wrappers over libpq; C++ smoke (compile+link `-lpq`; live query optional). | pending |
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

## Open questions (resolve in STEP=1)

1. Opaque `i32` handles + `Option`/`last_error` (Tcp style) vs typed
   `Result<PgConnection, string>`?
2. Require system `libpq` for all builds, or feature-gate / stub when missing?
3. Live DB in default CI — yes/no (prefer env-gated)?
