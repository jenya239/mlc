# Track: Lang Record Update (anonymous)

Parent: [TRACK_EDITOR_UX_BACKLOG.md](TRACK_EDITOR_UX_BACKLOG.md) §46 **#29**.
Compiler track (not editor-only). Size **M**.

## Status: **active** (2026-07-19) — STEP=0 done; next STEP=1

## Next step

**STEP=1** — red e2e/harness for anonymous `{ ...base, field: v }` (must fail today).

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `10f7fb89` | Decision freeze + open |

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
| 1 | Scenario first: e2e (or harness) anonymous `{ ...p, x: N }` — **red** today | pending — fail visible; document expected green stdout |
| 2 | Parser: bare `{` + spread+fields → `ExprRecordUpdate`; checker infer type from base | pending — `--check-only` / partial green; self-host + Tier B |
| 3 | Codegen polish if needed; e2e **green**; named regression; Tier B + self-host | pending — green tokens |
| 4 | Migrate ≥1 `misc/editor` state helper off copy-all-fields; demo compile | pending — `demo_live_fs_compile_ok` |
| 5 | Critic: gates; archive; backlog #29 done → Planner #30 | pending |

<!-- sub-steps STEP=1: 1) add `compiler/tests/e2e/record_update_anonymous.mlc` (or extend suite) using `{ ...p, x: 3 }`; 2) wire into e2e runner if required; 3) confirm red failure; commit without fixing parser -->
<!-- sub-steps STEP=2: 1) bare-`{` path in `exprs.mlc` → update when spread+fields; 2) checker/transform fill type name from base; 3) build.sh + self-host + Tier B -->
<!-- sub-steps STEP=3: 1) fix remaining codegen; 2) green e2e; 3) re-run named `record_update.mlc`; 4) Tier B + self-host -->
<!-- sub-steps STEP=4: 1) pick one `state_with_*` / EditorUxState copy site; 2) rewrite to update syntax; 3) demo_live_fs_compile -->

## Out of scope

Multi-spread / mid-literal spreads (B4 full parity); map/array spreads; changing named-update codegen shape; mass-migrate all editor/gui records; `lib/mlc/` unless REG forces.

## Verify discipline

- One STEP per turn; `compiler/` XOR `misc/editor` (STEP=4).
- Self-host diff + Tier B on every STEP that touches `compiler/`.
- Leave foreign `compiler/out/**`, `CLAUDE.md`, `capture_analyzer.rb` alone.
- Anti-false-done: git log must show claimed files before marking done.
