# Track: Validation / schema (stdlib)

Parent: [../../PLAN.md](../../PLAN.md), [../../STDLIB_BACKEND.md](../../STDLIB_BACKEND.md) §5.7,
[TRACK_STDLIB_ENV_LOGGING.md](TRACK_STDLIB_ENV_LOGGING.md).
Trigger: ENV_LOGGING **closed** (Critic OK); STDLIB_BACKEND §5 next is
Validation — philosophy lock required before implementation.

## Status: **closed** (2026-07-11) — STEP=1–5 **done**

**Planner 2026-07-11:** opened after ENV_LOGGING Critic.
**Driver 2026-07-11:** STEP=1 — Decision (explicit helpers; Result; no derive).
**Driver 2026-07-11:** STEP=2 — `validate.hpp` + extern MLC; smoke 12/0
  (amended: C++ — pure stdlib not inlined by Ruby codegen).
**Driver 2026-07-11:** STEP=3 — registry; test 1/10.
**Driver 2026-07-11:** STEP=4 — `run_validate_gate.sh` (12/0 + 1/10).
**Driver 2026-07-11:** STEP=5 — MLC.md/STDLIB/example; archived.
No `compiler/**` (regression_gate N/A).

**Critic 2026-07-11:** STEP=1–5 vs `bd8295e2`…`32c3c9ea`; PLAN/archive/STDLIB/MLC
aligned; gate re-OK 12/0+1/10; **reopen: none**. Residuals: example not
compile-gated (Ruby codegen only); mlcc bare `Validate` not in v1; STEP=1
“pure MLC” amended in STEP=2 to C++ (documented).

## Decision (STEP=1, 2026-07-11)

### Philosophy

| Choice | Locked |
|--------|--------|
| Model | **Explicit runtime helpers** — no schema builder objects |
| Compile-time derive `{ Validate }` | **rejected for v1** |
| Dynamic schema / Zod-like DSL | **rejected** (§3) |

### Surface

- Module **`Validate`** (`std/validate/validate`), C++ `mlc::validate`.
- Header: `runtime/include/mlc/validate/validate.hpp`.

```
Validate.non_empty(value: string) -> Result<(), string>
Validate.min_length(value: string, minimum: i32) -> Result<(), string>
Validate.max_length(value: string, maximum: i32) -> Result<(), string>
Validate.range_i32(value: i32, minimum: i32, maximum: i32) -> Result<(), string>
```

### Error model

- `Result<(), string>` first failure; no multi-error; no `last_error`.

### Pipeline

- C++ runtime + MLC extern (Env pattern). Pure-MLC stdlib bodies are not
  inlined by Ruby codegen.

### Non-goals (locked)

- derive Validate, JSON Schema, multi-error, OpenAPI middleware, ORM sync

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | Philosophy Decision. | **done** |
| 2 | Runtime + extern MLC (+ smoke). | **done** (smoke 12/0) |
| 3 | Registry + stdlib test. | **done** (1/10) |
| 4 | Gate. | **done** (12/0 + 1/10) |
| 5 | Docs + example; close. | **done** (2026-07-11: `validate_demo.mlc`; archived) |

## Deliverables

- Runtime: `runtime/include/mlc/validate/validate.hpp`
- MLC: `lib/mlc/common/stdlib/validate/validate.mlc`
- Gate: `scripts/run_validate_gate.sh`
- Example: `misc/examples/validate_demo.mlc`
