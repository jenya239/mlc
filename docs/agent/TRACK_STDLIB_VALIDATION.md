# Track: Validation / schema (stdlib)

Parent: [../PLAN.md](../PLAN.md), [../STDLIB_BACKEND.md](../STDLIB_BACKEND.md) §5.7,
[../archive/tracks/TRACK_STDLIB_ENV_LOGGING.md](../archive/tracks/TRACK_STDLIB_ENV_LOGGING.md).
Trigger: ENV_LOGGING **closed** (Critic OK); STDLIB_BACKEND §5 next is
Validation — **blocked on philosophy** (§3 / §5.7): compile-time vs runtime
schema must be locked before implementation.

## Status: **open** — STEP=2 next (pure MLC Validate helpers)

**Planner 2026-07-11:** opened after ENV_LOGGING Critic. Chose Validation over
jumping to TEXT_RENDERING_NATIVE (already open, medium/personal priority) to
finish STDLIB_BACKEND §5 chain. STEP=1 **is** the required philosophy lock.

**Driver 2026-07-11:** STEP=1 — Decision locked (explicit runtime helpers; pure
MLC `Validate`; `Result<(), string>`; no derive/schema DSL).

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

- Module **`Validate`** (`std/validate/validate`), namespace N/A (pure MLC).
- Free functions on the module (Crypto/Env style `import Validate::{…}`).

```
Validate.non_empty(value: string) -> Result<(), string>
Validate.min_length(value: string, minimum: i32) -> Result<(), string>
Validate.max_length(value: string, maximum: i32) -> Result<(), string>
Validate.range_i32(value: i32, minimum: i32, maximum: i32) -> Result<(), string>
```

| Method | Semantics |
|--------|-----------|
| `non_empty` | `Ok(())` if `value.length() > 0`; else `Err("must be non-empty")` |
| `min_length` | `Ok(())` if length ≥ `minimum`; else `Err` with message |
| `max_length` | `Ok(())` if length ≤ `maximum`; else `Err` with message |
| `range_i32` | `Ok(())` if `minimum ≤ value ≤ maximum`; else `Err` with message |

- **No** trait `extend Type : Validate` in v1.
- **No** multi-check aggregator / varargs in v1 (compose with `?` or match).

### Error model

- **`Result<(), string>`** — success `Ok(())`, failure `Err(message)`.
- **First failure only** — no error list / multi-error collect.
- **No** `last_error()` (message is in `Err`).
- If `Result<(), string>` hits a unit-type codegen gap in STEP=2, fallback
  documented then: `Result<bool, string>` with `Ok(true)` — same messages.

### Pipeline

- **Pure MLC** stdlib (like `Option`/`Result` modules) — **no** C++ runtime
  header required for v1 (string/i32 checks are language-level).
- Ruby registry + scanner; codegen needs no new includes.
- Self-hosted bare `Validate` import: **out of scope v1** (Env/WebSocket class).
- **No** `compiler/**` in this track unless STEP=2 discovers a hard blocker
  (then stop and re-Decision — do not silently add derive).

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
| 1 | Philosophy Decision: compile-time (derive/codegen) vs runtime helpers vs hybrid; API surface; error model; non-goals. Document in «Decision». | **done** |
| 2 | Implement pure MLC `Validate` helpers (+ Ruby smoke/unit). | **pending** |
| 3 | Registry/aliases + stdlib test (codegen/import). | pending |
| 4 | Gate: script — valid/invalid fixtures assert. | pending |
| 5 | Docs (`STDLIB_BACKEND.md` / `MLC.md`) + example; close (regression_gate if `compiler/**`). | pending |

<!-- sub-steps STEP=1: 1) compile-time vs runtime; 2) API sketch; 3) error model; 4) non-goals -->
<!-- sub-steps STEP=2: 1) validate.mlc; 2) unit smoke; 3) SESSION -->
<!-- sub-steps STEP=3: 1) registry; 2) stdlib test; 3) SESSION -->
<!-- sub-steps STEP=4: 1) gate script; 2) fixtures; 3) SESSION -->
<!-- sub-steps STEP=5: 1) docs+example; 2) archive; 3) Critic -->

## Out of scope (this track)

- Full JSON Schema / Ajv-class engines
- ORM / DB constraint sync
- OpenAPI request validation middleware (may follow API_CLIENT)
- Reflection-based DI or decorator validators
- Changing Json derive (API_CLIENT) unless Decision requires a tiny hook
