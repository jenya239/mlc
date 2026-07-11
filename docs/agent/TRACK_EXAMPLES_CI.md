# Track: Compile-smoke coverage for misc/examples + misc/gui

Parent: [../PLAN.md](../PLAN.md), [../GUI.md](../GUI.md).
Trigger: 2026-07-11 — `gui_button_demo.mlc` not in regression_gate; duplicate extern.
See [TRACK_FFI_EXTERN_DEDUP](TRACK_FFI_EXTERN_DEDUP.md) for root cause. This track = coverage.

## Status: **active** — STEP=9 done; STEP=10 next

## Next step

**STEP=10** — fix cluster D one-offs (`array_hof_demo`, `debug_*`, `index_access_demo`, `pointer_types_demo`, `safety_demo`, `todo_summary`).

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | `scripts/run_examples_compile_sweep.sh` | **done** (2026-07-12) |
| 2 | Soft-skip sysdeps + allowlist | **done** (2026-07-12) |
| 3 | Full sweep inventory | **done** (2026-07-12) — ok=77 fail=30 skip=0 |
| 7 | Fix cluster A: bare Ruby imports | **done** (2026-07-12) |
| 8 | Fix cluster B: `to_string` demos | **done** (2026-07-12) |
| 9 | Fix cluster C: ABI sodium/libpq soft-skip + `mlc_link_libs` (-L for `.tmp_*`) | **done** (2026-07-12) |
| 10 | Fix cluster D: type/demo one-offs | pending |
| 11 | Fix cluster E: `vm_*` C++ codegen/link | pending |
| 4 | Wire sweep в `scripts/regression_gate.sh` | pending |
| 5 | Docs: одна строка про sweep-gate | pending |
| 6 | Verify-gate: `regression_gate.sh` exit 0 | pending |

### STEP=9 done notes

- Soft-skip via `find_sodium_mode` / `find_libpq_mode` (system cxx probe or `.tmp_libsodium` / `.tmp_libpq`).
- On hit: write `mlc_link_libs.txt` (`-L…` then `sodium`/`pq`); `build_bin.sh` passes `-`-prefixed lines through.
- Verify: ONLY four abi demos → ok=4 fail=0.

### STEP=10 sub-steps (Driver)

1. Re-sweep ONLY the six D files; capture first errors.
2. Fix minimally (mlcc syntax); allowlist only if blocked on language/compiler.
3. ONLY six → fail=0.
