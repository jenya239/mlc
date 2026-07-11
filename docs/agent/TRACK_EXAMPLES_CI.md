# Track: Compile-smoke coverage for misc/examples + misc/gui

Parent: [../PLAN.md](../PLAN.md), [../GUI.md](../GUI.md).
Trigger: 2026-07-11 — `gui_button_demo.mlc` not in regression_gate; duplicate extern.
See [TRACK_FFI_EXTERN_DEDUP](TRACK_FFI_EXTERN_DEDUP.md). This track = coverage.

## Status: **active** — STEP=10 done; STEP=11 next

## Next step

**STEP=11** — fix cluster E: `vm_*` C++ codegen/link failures.

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | `scripts/run_examples_compile_sweep.sh` | **done** (2026-07-12) |
| 2 | Soft-skip sysdeps + allowlist | **done** (2026-07-12) |
| 3 | Full sweep inventory | **done** (2026-07-12) |
| 7 | Fix cluster A: bare Ruby imports | **done** (2026-07-12) |
| 8 | Fix cluster B: `to_string` demos | **done** (2026-07-12) |
| 9 | Fix cluster C: ABI sodium/libpq | **done** (2026-07-12) |
| 10 | Fix cluster D: one-offs | **done** (2026-07-12) |
| 11 | Fix cluster E: `vm_*` C++ codegen/link | pending |
| 4 | Wire sweep в `scripts/regression_gate.sh` | pending |
| 5 | Docs: одна строка про sweep-gate | pending |
| 6 | Verify-gate: `regression_gate.sh` exit 0 | pending |

### STEP=10 done notes

- Rewrote 7 demos for mlcc (`[i32]`, `string`, `.to_string()`, no `unsafe`).
- `pointer_types_demo` → Shared smoke (Owned/Weak/unsafe tour deferred).
- Verify: ONLY seven → ok=7 fail=0.

### STEP=11 sub-steps (Driver)

1. ONLY re-sweep vm_match_*, vm_pop, vm_question*, vm_record_match.
2. Fix or allowlist each; prefer minimal source fix over compiler change.
3. ONLY those → fail=0.
