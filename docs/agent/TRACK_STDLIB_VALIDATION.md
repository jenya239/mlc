# Track: Validation / schema (stdlib)

Parent: [../PLAN.md](../PLAN.md), [../STDLIB_BACKEND.md](../STDLIB_BACKEND.md) §5.7,
[../archive/tracks/TRACK_STDLIB_ENV_LOGGING.md](../archive/tracks/TRACK_STDLIB_ENV_LOGGING.md).
Trigger: ENV_LOGGING **closed** (Critic OK); STDLIB_BACKEND §5 next is
Validation — **blocked on philosophy** (§3 / §5.7): compile-time vs runtime
schema must be locked before implementation.

## Status: **open** — STEP=1 next (Philosophy Decision)

**Planner 2026-07-11:** opened after ENV_LOGGING Critic. Chose Validation over
jumping to TEXT_RENDERING_NATIVE (already open, medium/personal priority) to
finish STDLIB_BACKEND §5 chain. STEP=1 **is** the required philosophy lock.

## Goal

Minimal **input validation** for backend apps (request bodies / env-derived
config / record fields) without copying reflection-heavy Zod/ActiveModel
patterns. Enough to replace ad-hoc `if` chains for common string/number/
presence checks.

## Constraints (STDLIB_BACKEND §3)

- No runtime reflection / decorator magic.
- Prefer compile-time or explicit hand-written validators over dynamic schema
  objects — **lock in STEP=1**.
- Errors → `Option` / `Result` / bool+message — **lock in STEP=1**.
- No full JSON Schema / OpenAPI validation engine in v1 (API_CLIENT already
  covers OpenAPI codegen MVP).

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | Philosophy Decision: compile-time (derive/codegen) vs runtime helpers vs hybrid; API surface; error model; non-goals. Document in «Decision». | **pending** |
| 2 | Implement runtime and/or codegen scaffolding per Decision (+ smoke). | pending |
| 3 | Stdlib module / derive hooks + registry (or document C++-only / codegen-only). | pending |
| 4 | Gate: script — valid/invalid fixtures assert. | pending |
| 5 | Docs (`STDLIB_BACKEND.md` / `MLC.md`) + example; close (regression_gate if `compiler/**`). | pending |

<!-- sub-steps STEP=1: 1) compile-time vs runtime; 2) API sketch; 3) error model; 4) non-goals -->
<!-- sub-steps STEP=2: 1) headers or derive path; 2) smoke; 3) SESSION -->
<!-- sub-steps STEP=3: 1) mlc module or skip; 2) registry; 3) codegen -->
<!-- sub-steps STEP=4: 1) gate script; 2) fixtures; 3) SESSION -->
<!-- sub-steps STEP=5: 1) docs+example; 2) archive; 3) Critic -->

## Out of scope (this track)

- Full JSON Schema / Ajv-class engines
- ORM / DB constraint sync
- OpenAPI request validation middleware (may follow API_CLIENT)
- Reflection-based DI or decorator validators
- Changing Json derive (API_CLIENT) unless Decision requires a tiny hook

## Open questions (resolve in STEP=1)

1. **Compile-time vs runtime?** Derive/`const` checks vs `Validate.check_*`
   helpers vs hybrid (codegen emits calls to runtime helpers).
2. **Surface:** `Validate` module vs `extend Type : Validate` trait vs pure
   free functions?
3. **Error model:** `Result<(), string>` / `Option` / collect multi-error?
4. **Pipeline:** Ruby+C++ (Crypto) vs C++-only vs compiler derive (needs
   `compiler/**`)?
