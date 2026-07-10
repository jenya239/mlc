# Track: Env config + structured logging (stdlib)

Parent: [../PLAN.md](../PLAN.md), [../STDLIB_BACKEND.md](../STDLIB_BACKEND.md) §5.6,
[../archive/tracks/TRACK_STDLIB_JOB_QUEUE.md](../archive/tracks/TRACK_STDLIB_JOB_QUEUE.md).
Trigger: JOB_QUEUE **closed**; STDLIB_BACKEND §5 next is env/config + logging
(low complexity, high frequency of use).

## Status: **open** — STEP=1 next (API Decision)

**Planner 2026-07-11:** opened after closed STDLIB_JOB_QUEUE Critic. Chose
combined **env + logging** track (§5.6 as one unit — both are thin wrappers,
same “filler” slot). No new FFI beyond `getenv` / stdio.

## Goal

Minimal **process env** read helpers and **structured log** lines (JSON or
key=value) for backend apps. Enough to replace ad-hoc `getenv` / `println`
without a full observability stack.

## Constraints (STDLIB_BACKEND §3)

- Thin C++ runtime and/or MLC `std/` modules (Crypto/Tcp pattern preferred if
  surface is string-based).
- Errors → `Option`/`bool`+`last_error` **or** Decision-locked alternative —
  **lock in STEP=1**.
- No OpenTelemetry, no log shipping agents, no dotenv file parser mandatory
  in v1 (optional if Decision wants).

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | Design: `Env.get` / `Env.get_or` API; log levels + sink (stderr/stdout); JSON lines vs plain; MLC modules vs C++-only; error model. Document in «Decision». | **pending** |
| 2 | Runtime: `env.hpp` and/or `log.hpp` (+ C++ smoke). | pending |
| 3 | Stdlib: `std/…` modules + registry/bridge **or** document C++-only. | pending |
| 4 | Gate: script — env roundtrip + log line assert (or capture stderr). | pending |
| 5 | Docs (`STDLIB_BACKEND.md` / `MLC.md`) + example; close (regression_gate if `compiler/**`). | pending |

<!-- sub-steps STEP=1: 1) Env API; 2) Log API/levels; 3) pipeline; 4) non-goals OTel -->
<!-- sub-steps STEP=2: 1) getenv wrapper; 2) log writer; 3) smoke -->
<!-- sub-steps STEP=3: 1) mlc modules or skip; 2) registry; 3) codegen -->
<!-- sub-steps STEP=4: 1) gate script; 2) asserts; 3) SESSION -->
<!-- sub-steps STEP=5: 1) docs+example; 2) archive; 3) Critic -->

## Out of scope (this track)

- OpenTelemetry / Prometheus / distributed tracing
- File log rotation / async log queues
- Full dotenv / layered config files (v1 may be getenv-only)
- Validation schemas (§5.7)
- Changing `mlc::core::profile` (profiling ≠ logging)

## Open questions (resolve in STEP=1)

1. **One track surface or two modules?** `Env` + `Log` vs single `Config`?
2. **Log format:** JSON lines (one object per line) vs `level=… msg=…` text?
3. **Pipeline:** Ruby MLC + C++ runtime (Crypto pattern) vs C++-only?
4. **Missing env key:** `Option` / default / `last_error`?
