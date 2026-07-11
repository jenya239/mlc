# Track: Compile-smoke coverage for misc/examples + misc/gui

Parent: [../PLAN.md](../PLAN.md), [../GUI.md](../GUI.md).
Trigger: 2026-07-11 — `gui_button_demo.mlc` not in regression_gate.
See [TRACK_FFI_EXTERN_DEDUP](TRACK_FFI_EXTERN_DEDUP.md). This track = coverage.

## Status: **active** — STEP=5 done; STEP=6 next

## Next step

**STEP=6** — verify-gate: `bash scripts/regression_gate.sh` exit 0.

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
| 4 | Wire sweep в `scripts/regression_gate.sh` | **done** (2026-07-12) |
| 5 | Docs: README + DEVELOPMENT sweep-gate line | **done** (2026-07-12) |
| 6 | Verify-gate: `regression_gate.sh` exit 0 | pending |

### STEP=5 done notes

- `README.md` §Тесты: `bash scripts/regression_gate.sh` + sweep note.
- `DEVELOPMENT.md` Tier C: same.

### STEP=6 sub-steps (Driver)

1. `bash scripts/regression_gate.sh` (programs + full examples sweep).
2. Exit 0 → mark STEP=6 done, track **closed**, `next` = Critic.
3. On fail: fix or allowlist, do not close.
