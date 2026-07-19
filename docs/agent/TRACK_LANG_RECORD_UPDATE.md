# Track: Lang Record Update (anonymous)

Parent: [TRACK_EDITOR_UX_BACKLOG.md](TRACK_EDITOR_UX_BACKLOG.md) §46 **#29**.
Compiler track (not editor-only). Size **M**.

## Status: **active** (2026-07-19) — STEP=0–4 done; next Critic

## Next step

**STEP=5** — Critic: gates; archive; backlog #29 done → Planner #30.

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `10f7fb89` | Decision freeze + open |
| 1 | `e3e6c72c` | Red e2e + harness |
| 2 | `efd8d9c6` | bare `{` parse + infer/transform type from base |
| 3 | `be970c68` | wire `run_e2e.sh`; retire check script |
| 4 | *(this turn)* | `scroll.mlc` state_with_scroll_* → anonymous update |

## Decision (STEP=0) — **frozen** 2026-07-19

| Item | Choice |
|------|--------|
| Problem | Editor/gui hand-copy all record fields on partial update. Backlog wants `{ ...state, field: v }`. **Named** `TypeName { ...base, field: v }` already works (e2e `record_update.mlc`, MIR, LANGUAGE_REFERENCE). **Anonymous** `{ ...base, field: v }` fails in mlcc today (probe 2026-07-19: check/codegen errors; not `ExprRecordUpdate`) |
| Goal | Anonymous single-spread + overrides → same semantics as named update; type from base (record type of spread expr) and/or expected type. Kill copy-all-fields boilerplate |
| Surface | `{ ...base, field: value, ... }` only (one leading/sole spread + field overrides → `ExprRecordUpdate`). Empty type name at parse OK if checker fills from base |
| Named | Keep `TypeName { ...base, overrides }` unchanged; no regression on existing e2e/unit |
| Ruby / B4 | Full multi-spread RecordLit parity (`{ a: 1, ...base, b: 2 }`) = **out of scope** (docs/MLC.md B4 residual). This track = update form only |
| `lib/mlc/` | Prefer mlcc-only. Touch Ruby only if forced for REG parity; then `scripts/regression_gate.sh` before Critic |
| Editor proof | After green anonymous: migrate ≥1 helper that still copies fields (e.g. `state_with_scroll_*` / tab helpers) to `{ ...state, field: v }` or named equivalent; `demo_live_fs_compile_ok` |
| Module touch | `compiler/frontend/parser`, checker/transform, codegen if needed, `compiler/tests/e2e` (+ unit if useful). Optional `misc/editor/ux/*` on STEP=4 only |
| Gates | Every compiler STEP: `compiler/build.sh` + self-host diff (touched modules only) + Tier B `compiler/tests/build_tests.sh`. Not `--check-only` alone |

### Gates

| Step | Token / gate |
|------|----------------|
| 1 | Red: anonymous e2e (or unit) fails compile/check as expected |
| 2–3 | Green: anonymous e2e stdout; named e2e still green; Tier B + self-host |
| 4 | Editor helper migrate + `demo_live_fs_compile_ok` |
| Critic | Re-run Tier B / self-host summary; REG iff `lib/mlc/` changed |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open track / PLAN / backlog / CONTINUITY | **done** (2026-07-19) |
| 1 | Scenario first: e2e (or harness) anonymous `{ ...p, x: N }` — **red** today | **done** (2026-07-19) — `record_update_anonymous_red_ok` |
| 2 | Parser: bare `{` + spread+fields → `ExprRecordUpdate`; checker infer type from base | **done** (2026-07-19) — `record_update_anonymous_check_ok`; self-host identical; Tier B exit 0 |
| 3 | Codegen polish if needed; e2e **green**; named regression; Tier B + self-host | **done** (2026-07-19) — `run_e2e` 10/0; no codegen polish; no `compiler/*.mlc` → reuse STEP=2 Tier B/self-host |
| 4 | Migrate ≥1 `misc/editor` state helper off copy-all-fields; demo compile | **done** (2026-07-19) — `state_with_scroll_y/x` → `{ ...state, … }`; `demo_live_fs_compile_ok` |
| 5 | Critic: gates; archive; backlog #29 done → Planner #30 | pending |

## Out of scope

Multi-spread / mid-literal spreads (B4 full parity); map/array spreads; changing named-update codegen shape; mass-migrate all editor/gui records; `lib/mlc/` unless REG forces.

## Verify discipline

- One STEP per turn; `compiler/` XOR `misc/editor` (STEP=4).
- Self-host diff + Tier B on every STEP that touches `compiler/` sources (`*.mlc` under frontend/checker/codegen).
- Leave foreign `compiler/out/**`, `CLAUDE.md`, `capture_analyzer.rb` alone.
- Anti-false-done: git log must show claimed files before marking done.
