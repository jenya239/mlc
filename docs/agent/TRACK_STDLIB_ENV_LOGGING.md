# Track: Env config + structured logging (stdlib)

Parent: [../PLAN.md](../PLAN.md), [../STDLIB_BACKEND.md](../STDLIB_BACKEND.md) §5.6,
[../archive/tracks/TRACK_STDLIB_JOB_QUEUE.md](../archive/tracks/TRACK_STDLIB_JOB_QUEUE.md).
Trigger: JOB_QUEUE **closed**; STDLIB_BACKEND §5 next is env/config + logging
(low complexity, high frequency of use).

## Status: **open** — STEP=4 next (gate)

**Planner 2026-07-11:** opened after closed STDLIB_JOB_QUEUE Critic. Chose
combined **env + logging** track (§5.6 as one unit — both are thin wrappers,
same “filler” slot). No new FFI beyond `getenv` / stdio.

**Driver 2026-07-11:** STEP=1 — Decision locked (two modules Env+Log; JSON
lines stderr; Crypto-style MLC+C++; Option/default for missing env).

**Driver 2026-07-11:** STEP=2 — `env.hpp` / `log.hpp` + smoke
(`run_env_log_runtime_smoke.sh` 14/0); included from `mlc.hpp`.

**Driver 2026-07-11:** STEP=3 — `env.mlc` / `log.mlc` + registry + codegen
includes; `env_log_stdlib_test` 1/18.

## Goal

Minimal **process env** read helpers and **structured log** lines (JSON
lines) for backend apps. Enough to replace ad-hoc `getenv` / `println`
without a full observability stack.

## Constraints (STDLIB_BACKEND §3)

- Thin C++ runtime and MLC `std/` modules (Crypto/Tcp string pattern).
- Env missing key → `Option` / default (not `last_error`).
- No OpenTelemetry, no log shipping, no dotenv in v1.

## Decision (STEP=1, 2026-07-11)

### Surface

- **Two modules**, not one `Config`: `Env` + `Log`.
- C++ namespaces: `mlc::env`, `mlc::log`.
- Headers: `runtime/include/mlc/env/env.hpp`, `runtime/include/mlc/log/log.hpp`.

### Pipeline

- **Ruby MLC + C++ runtime** (Crypto pattern): string-only `extern fn`, no
  callbacks/closures → JobQueue-style C++-only **not** needed.
- Soft bridge only if core `mlc.hpp` must stay free of these headers; prefer
  direct include (no third-party deps — unlike sodium/pq).
- Self-hosted bare import: **out of scope v1** (same residual class as
  WebSocket).

### Env API (v1)

```
Env.get(key: string) -> Option<string>
Env.get_or(key: string, default: string) -> string
Env.has(key: string) -> bool
```

| Method | Semantics |
|--------|-----------|
| `get` | `std::getenv`; `Some(value)` or `None` if unset/null. Empty string is `Some("")`. |
| `get_or` | `get` then default when `None`. |
| `has` | `true` iff `getenv` returns non-null. |

- **No** `last_error` for missing keys (absence is not an error).
- **No** `setenv` / `unsetenv` / dotenv file parser in v1.

### Log API (v1)

```
Log.error(msg: string) -> void
Log.warn(msg: string) -> void
Log.info(msg: string) -> void
Log.debug(msg: string) -> void
```

| Rule | Choice |
|------|--------|
| Format | **JSON lines** — one object per line: `{"level":"<level>","msg":"<escaped>"}` |
| Sink | **stderr** only |
| Levels | `error` / `warn` / `info` / `debug` (string field) |
| Escaping | JSON string escape for `"`, `\`, control chars in `msg` |
| Timestamp | **omitted** in v1 |
| Extra fields | **omitted** in v1 (no key=value bag) |
| Failure | best-effort write; no return / no panic |

Plain `level=… msg=…` text format: **rejected** for v1 (STDLIB_BACKEND already
points at json lines; one format only).

### Error model

- Env: `Option` / `bool` / default string — **no** `last_error`.
- Log: fire-and-forget `void`.

### Non-goals (locked)

- OpenTelemetry / Prometheus / tracing
- File rotation, async queues, log shipping
- dotenv / layered config files / `setenv`
- Structured field bags beyond `level`+`msg`
- Changing `mlc::core::profile`
- Validation schemas (§5.7)

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | Design: `Env.get` / `Env.get_or` API; log levels + sink (stderr/stdout); JSON lines vs plain; MLC modules vs C++-only; error model. Document in «Decision». | **done** |
| 2 | Runtime: `env.hpp` and/or `log.hpp` (+ C++ smoke). | **done** (2026-07-11: `env.hpp`/`log.hpp`; `run_env_log_runtime_smoke.sh` 14/0) |
| 3 | Stdlib: `std/…` modules + registry/bridge **or** document C++-only. | **done** (2026-07-11: `env.mlc`/`log.mlc`; registry; codegen includes; test 1/18) |
| 4 | Gate: script — env roundtrip + log line assert (or capture stderr). | pending |
| 5 | Docs (`STDLIB_BACKEND.md` / `MLC.md`) + example; close (regression_gate if `compiler/**`). | pending |

<!-- sub-steps STEP=1: 1) Env API; 2) Log API/levels; 3) pipeline; 4) non-goals OTel -->
<!-- sub-steps STEP=2: 1) getenv wrapper; 2) log writer; 3) smoke -->
<!-- sub-steps STEP=3: 1) mlc modules; 2) registry; 3) codegen -->
<!-- sub-steps STEP=4: 1) gate script; 2) asserts; 3) SESSION -->
<!-- sub-steps STEP=5: 1) docs+example; 2) archive; 3) Critic -->

## Out of scope (this track)

- OpenTelemetry / Prometheus / distributed tracing
- File log rotation / async log queues
- Full dotenv / layered config files
- Validation schemas (§5.7)
- Changing `mlc::core::profile` (profiling ≠ logging)
