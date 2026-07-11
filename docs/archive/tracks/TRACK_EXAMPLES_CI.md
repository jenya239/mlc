# Track: Compile-smoke coverage for misc/examples + misc/gui

Parent: [../PLAN.md](../../PLAN.md), [../GUI.md](../../GUI.md).
Trigger: 2026-07-11 — `gui_button_demo.mlc` not in regression_gate.
See [TRACK_FFI_EXTERN_DEDUP](../../agent/TRACK_FFI_EXTERN_DEDUP.md). This track = coverage.

## Status: **closed** (2026-07-12) — awaiting Critic

**Driver 2026-07-12 STEP=6:** `regression_gate.sh` EXIT=0 — programs 20/0; examples sweep ok=106 fail=0 skip=1 (`dynrecord_demo`). TRACK archived. Close → Critic.

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | `scripts/run_examples_compile_sweep.sh` | **done** |
| 2 | Soft-skip sysdeps + allowlist | **done** |
| 3 | Full sweep inventory | **done** |
| 7–11 | Fix clusters A–E | **done** |
| 4 | Wire into `regression_gate.sh` | **done** |
| 5 | Docs | **done** |
| 6 | Verify-gate | **done** (2026-07-12) — EXIT=0; 20/0; sweep 106/0/1 |

## Delivered

- `scripts/run_examples_compile_sweep.sh` + allowlist + ABI `-L` soft-skip
- Wired after program PASS in `scripts/regression_gate.sh`
- Fixed broken examples (Ruby imports, `.to_string()`, vm_* match helpers, etc.)
