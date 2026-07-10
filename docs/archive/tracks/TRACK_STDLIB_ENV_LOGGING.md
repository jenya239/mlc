# Track: Env config + structured logging (stdlib)

Parent: [../../PLAN.md](../../PLAN.md), [../../STDLIB_BACKEND.md](../../STDLIB_BACKEND.md) §5.6,
[TRACK_STDLIB_JOB_QUEUE.md](TRACK_STDLIB_JOB_QUEUE.md).
Trigger: JOB_QUEUE **closed**; STDLIB_BACKEND §5 next is env/config + logging
(low complexity, high frequency of use).

## Status: **closed** (2026-07-11) — STEP=1–5 **done**

**Planner 2026-07-11:** opened after closed STDLIB_JOB_QUEUE Critic.
**Driver 2026-07-11:** STEP=1 — Decision (Env+Log; JSON lines; Crypto pipeline).
**Driver 2026-07-11:** STEP=2 — `env.hpp` / `log.hpp` + smoke 14/0.
**Driver 2026-07-11:** STEP=3 — `env.mlc` / `log.mlc` + registry; test 1/18.
**Driver 2026-07-11:** STEP=4 — `run_env_log_gate.sh` (smoke 14/0 + stdlib 1/18).
**Driver 2026-07-11:** STEP=5 — MLC.md/STDLIB/example; archived.
No `compiler/**` (regression_gate N/A).

## Decision (STEP=1, 2026-07-11)

### Surface

- **Two modules**, not one `Config`: `Env` + `Log`.
- C++ namespaces: `mlc::env`, `mlc::log`.
- Headers: `runtime/include/mlc/env/env.hpp`, `runtime/include/mlc/log/log.hpp`.

### Pipeline

- **Ruby MLC + C++ runtime** (Crypto pattern): string-only `extern fn`, no
  callbacks/closures → JobQueue-style C++-only **not** needed.
- Direct include from `mlc.hpp` and Ruby codegen (no third-party deps).
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
| 1 | Design: API Decision. | **done** |
| 2 | Runtime: `env.hpp` / `log.hpp` (+ C++ smoke). | **done** (smoke 14/0) |
| 3 | Stdlib: `env.mlc` / `log.mlc` + registry/codegen. | **done** (test 1/18) |
| 4 | Gate: `run_env_log_gate.sh`. | **done** (smoke 14/0 + stdlib 1/18) |
| 5 | Docs (`STDLIB_BACKEND.md` / `MLC.md`) + example; close. | **done** (2026-07-11: `env_log_demo.mlc`; archived) |

## Out of scope (this track)

- OpenTelemetry / Prometheus / distributed tracing
- File log rotation / async log queues
- Full dotenv / layered config files
- Validation schemas (§5.7)
- Changing `mlc::core::profile` (profiling ≠ logging)

## Deliverables

- Runtime: `runtime/include/mlc/env/env.hpp`, `runtime/include/mlc/log/log.hpp`
- MLC: `lib/mlc/common/stdlib/env/env.mlc`, `log/log.mlc`
- Gate: `scripts/run_env_log_gate.sh`
- Example: `misc/examples/env_log_demo.mlc`
