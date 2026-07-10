# Track: Validation / schema (stdlib)

Parent: [../PLAN.md](../PLAN.md), [../STDLIB_BACKEND.md](../STDLIB_BACKEND.md) §5.7,
[../archive/tracks/TRACK_STDLIB_ENV_LOGGING.md](../archive/tracks/TRACK_STDLIB_ENV_LOGGING.md).
Trigger: ENV_LOGGING **closed** (Critic OK); STDLIB_BACKEND §5 next is
Validation — **blocked on philosophy** (§3 / §5.7): compile-time vs runtime
schema must be locked before implementation.

## Status: **open** — STEP=5 next (docs+close)

**Planner 2026-07-11:** opened after ENV_LOGGING Critic. Chose Validation over
jumping to TEXT_RENDERING_NATIVE (already open, medium/personal priority) to
finish STDLIB_BACKEND §5 chain. STEP=1 **is** the required philosophy lock.

**Driver 2026-07-11:** STEP=1 — Decision locked (explicit runtime helpers;
`Result<(), string>`; no derive/schema DSL).

**Driver 2026-07-11:** STEP=2 — `validate.hpp` + extern `validate.mlc`; smoke
12/0. Amended: pure-MLC stdlib bodies are **not** inlined by Ruby codegen →
C++ implements (Env pattern).

**Driver 2026-07-11:** STEP=3 — registry + LEGACY_ALIASES + header_generator;
`validate_stdlib_test` 1/10.

**Driver 2026-07-11:** STEP=4 — `run_validate_gate.sh` (smoke 12/0 + stdlib 1/10).

## Goal

Minimal **input validation** for backend apps (request bodies / env-derived
config / record fields) without copying reflection-heavy Zod/ActiveModel
patterns. Enough to replace ad-hoc `if` chains for common string/number/
presence checks.

## Constraints (STDLIB_BACKEND §3)

- No runtime reflection / decorator magic.
- Explicit hand-written validators (not dynamic schema objects).
- Errors → `Result<(), string>` (first failure message).
- No full JSON Schema / OpenAPI validation engine in v1.

## Decision (STEP=1, 2026-07-11)

### Philosophy

| Choice | Locked |
|--------|--------|
| Model | **Explicit runtime helpers** — call sites name checks; no schema builder objects |
| Compile-time derive `{ Validate }` | **rejected for v1** (needs `compiler/**`; overlaps API_CLIENT Json story) |
| Dynamic schema / Zod-like DSL | **rejected** (§3 anti-reflection) |
| Hybrid codegen→runtime | **deferred** — not needed for string/i32 helpers |

Rationale: §3 forbids reflection-based validators; Env/Crypto show thin
stdlib wins without compiler changes. Derive Validate is a separate track if
ever wanted.

### Surface

- Module **`Validate`** (`std/validate/validate`), C++ `mlc::validate`.
- Header: `runtime/include/mlc/validate/validate.hpp`.

```
Validate.non_empty(value: string) -> Result<(), string>
Validate.min_length(value: string, minimum: i32) -> Result<(), string>
Validate.max_length(value: string, maximum: i32) -> Result<(), string>
Validate.range_i32(value: i32, minimum: i32, maximum: i32) -> Result<(), string>
```

| Method | Semantics |
|--------|-----------|
| `non_empty` | `Ok(())` if non-empty; else `Err("must be non-empty")` |
| `min_length` / `max_length` | byte length (`byte_size`) vs bound; ASCII-oriented v1 |
| `range_i32` | `Ok(())` if `minimum ≤ value ≤ maximum`; else `Err("value out of range")` |

- **No** trait `extend Type : Validate` in v1.
- **No** multi-check aggregator / varargs in v1 (compose with `?` or match).

### Error model

- **`Result<(), string>`** — success `Ok(())` (`std::tuple<>` in C++), failure
  `Err(message)`.
- **First failure only** — no error list / multi-error collect.
- **No** `last_error()`.

### Pipeline

- **C++ runtime + MLC extern** (Env pattern). STEP=2 found pure-MLC stdlib
  function bodies are **not emitted** into consumer TUs by Ruby codegen
  (calls become `mlc::validate::*` with no definition) — same class as Array
  helpers living in C++ headers.
- Ruby registry in STEP=3; codegen includes `validate.hpp`.
- Self-hosted bare `Validate` import: **out of scope v1**.
- **No** `compiler/**` / derive in this track.

### Non-goals (locked)

- `derive { Validate }` / attribute schemas
- JSON Schema / Ajv / OpenAPI request middleware
- Schema builder objects (`object({…})`)
- Multi-error aggregation
- ORM / DB constraint sync
- Changing Json derive (API_CLIENT)

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | Philosophy Decision. | **done** |
| 2 | Runtime `validate.hpp` + extern `validate.mlc` (+ C++ smoke). | **done** (2026-07-11: smoke 12/0; codegen include) |
| 3 | Registry/aliases + stdlib test (codegen/import). | **done** (2026-07-11: registry; test 1/10) |
| 4 | Gate: script — valid/invalid fixtures assert. | **done** (2026-07-11: `run_validate_gate.sh` — smoke 12/0 + stdlib 1/10) |
| 5 | Docs (`STDLIB_BACKEND.md` / `MLC.md`) + example; close (regression_gate if `compiler/**`). | pending |

<!-- sub-steps STEP=2: 1) validate.hpp; 2) extern mlc; 3) smoke -->
<!-- sub-steps STEP=3: 1) registry; 2) stdlib test; 3) SESSION -->
<!-- sub-steps STEP=4: 1) gate script; 2) fixtures; 3) SESSION -->
<!-- sub-steps STEP=5: 1) docs+example; 2) archive; 3) Critic -->

## Out of scope (this track)

- Full JSON Schema / Ajv-class engines
- ORM / DB constraint sync
- OpenAPI request validation middleware (may follow API_CLIENT)
- Reflection-based DI or decorator validators
- Changing Json derive (API_CLIENT) unless Decision requires a tiny hook
