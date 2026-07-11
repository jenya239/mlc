# Track: Compile-smoke coverage for misc/examples + misc/gui

Parent: [../PLAN.md](../PLAN.md), [../GUI.md](../GUI.md).
Trigger: 2026-07-11 — `gui_button_demo.mlc` not in regression_gate.
See [TRACK_FFI_EXTERN_DEDUP](TRACK_FFI_EXTERN_DEDUP.md). This track = coverage.

## Status: **active** — STEP=4 done; STEP=5 next

## Next step

**STEP=5** — docs: one line in README and/or DEVELOPMENT about the sweep-gate.

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
| 5 | Docs: одна строка про sweep-gate | pending |
| 6 | Verify-gate: `regression_gate.sh` exit 0 | pending |

### STEP=4 done notes

- After program PASS, `regression_gate.sh` runs `scripts/run_examples_compile_sweep.sh`.
- Full sweep before wire: ok=106 fail=0 skip=1 (`dynrecord_demo` allowlist).

### STEP=5 sub-steps (Driver)

1. One line in `README.md` and/or `docs/agent/DEVELOPMENT.md` mentioning examples compile sweep in regression_gate.
2. No code changes.
