# Track: GLFW window content scale (HiDPI)

Parent: [../../PLAN.md](../../PLAN.md) §37; residual from
[TRACK_STDLIB_IO_FS.md](TRACK_STDLIB_IO_FS.md) STEP=7 deferred.

## Status: **closed** (2026-07-16) — Critic OK

**Critic 2026-07-16 (STEP=4):** Re-ran compile + smoke + REG (MLC_JOBS=2).
Anti-false-done: `651b6b2b`…`06e84ebe` (STEP=0–3). **reopen: none**.

| Gate | Result |
|------|--------|
| `run_glfw_content_scale_compile.sh` | `content_scale_compile_ok` |
| `run_glfw_content_scale_smoke.sh` | `content_scale_ok` |
| `run_editor_demo_live_fs_compile.sh` | `demo_live_fs_compile_ok` (STEP=3) |
| `MLC_JOBS=2 scripts/regression_gate.sh` | 20 passed, 0 failed; sweep ok=148 fail=0 skip=1 |

## Next step

**closed** — Critic OK. Queue → Planner.

### STEP=4 done (2026-07-16)

- Critic gates + REG; archive.

### STEPs 0–3 done in git (2026-07-16)

| Step | Commit (abbrev) | Gate |
|------|-----------------|------|
| 0 | `651b6b2b` | Decision freeze |
| 1 | `bb2fdfac` | `content_scale_compile_ok` |
| 2 | `9562d0ce` | `content_scale_ok` |
| 3 | `06e84ebe` | `demo_live_fs_compile_ok` |

## Decision (frozen) — summary

`glfw_gl_window_content_scale_x/y() -> f64` via `glfwGetWindowContentScale`;
missing window → `1.0`; demo_live `font_size` × scale_y. No `compiler/`.

## Out of scope (unchanged)

Fontconfig; full HiDPI layout; SCRIPT_VM; MIR Epic 5.
