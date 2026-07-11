# Track: Compile-smoke coverage for misc/examples + misc/gui

Parent: [../PLAN.md](../PLAN.md), [../GUI.md](../GUI.md).
Trigger: 2026-07-11 — `gui_button_demo.mlc` not in regression_gate.
See [TRACK_FFI_EXTERN_DEDUP](TRACK_FFI_EXTERN_DEDUP.md). This track = coverage.

## Status: **active** — STEP=11 done; STEP=4 next

## Next step

**STEP=4** — wire `run_examples_compile_sweep.sh` into `scripts/regression_gate.sh`.

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | `scripts/run_examples_compile_sweep.sh` | **done** (2026-07-12) |
| 2 | Soft-skip sysdeps + allowlist | **done** (2026-07-12) |
| 3 | Full sweep inventory | **done** (2026-07-12) |
| 7 | Fix cluster A | **done** (2026-07-12) |
| 8 | Fix cluster B | **done** (2026-07-12) |
| 9 | Fix cluster C | **done** (2026-07-12) |
| 10 | Fix cluster D | **done** (2026-07-12) |
| 11 | Fix cluster E: `vm_*` | **done** (2026-07-12) |
| 4 | Wire sweep в `scripts/regression_gate.sh` | pending |
| 5 | Docs: одна строка про sweep-gate | pending |
| 6 | Verify-gate: `regression_gate.sh` exit 0 | pending |

### STEP=11 done notes

- Match demos: helper takes sum type (mlcc must visit `Choice`, not `Left{…}`).
- Unit variants → payload variants; or-patterns expanded; record match → field access.
- `vm_pop`: last index (no `Array.pop`).
- `vm_question*`: `type Result<T,E>` + `i32!i32` + `?`.
- Verify: ONLY eight → ok=8 fail=0.

### STEP=4 sub-steps (Driver)

1. Call sweep at end of `regression_gate.sh` (fail gate on fail).
2. Ensure soft-skip still exit 0 when deps missing.
3. Smoke: run gate or at least invoke sweep from gate path.
