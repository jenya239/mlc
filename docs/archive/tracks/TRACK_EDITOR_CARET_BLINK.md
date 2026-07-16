# Track: Editor caret blink (UX L8 + live)

Parent: [../../PLAN.md](../../PLAN.md) §41; residual from
[../../GUI_UX_TESTING.md](../../GUI_UX_TESTING.md) L8 and
[TRACK_UX_HEADLESS.md](TRACK_UX_HEADLESS.md).

## Status: **closed** (2026-07-16) — Critic OK

**Critic 2026-07-16 (STEP=3):** Re-ran unit + UX scenario + demo_live compile.
No `lib/mlc/` → REG skipped (Decision). Anti-false-done:
`de3a1dfc`…`9e758204` (STEP=0–2). **reopen: none**.

| Gate | Result |
|------|--------|
| `run_editor_caret_blink_unit.sh` | `caret_blink_unit ok` |
| `run_ux_caret_blink_phases.sh` | `ux_ok caret_blink_phases` |
| `run_editor_demo_live_fs_compile.sh` | `demo_live_fs_compile_ok` |

## Next step

**closed** — Critic OK. Queue → Planner.

### STEP=3 done (2026-07-16)

- Critic gates; archive.

### STEPs 0–2 done in git (2026-07-16)

| Step | Commit (abbrev) | Gate |
|------|-----------------|------|
| 0 | `de3a1dfc` | Decision frozen |
| 1 | `c1db7123` | `caret_blink_unit ok` |
| 2 | `9e758204` | scenario + ux gate + demo_live |

## Decision (frozen) — summary

`caret_blink_period_ms` / `caret_blink_visible`; period 1000 ms; L8 scenario
via `UxDriver` `SetClock`; `demo_live` uses `frame_index * 16`; no `sleep`;
no `compiler/` / no `lib/mlc/`.

## Out of scope (unchanged)

IME; grapheme; tree-sitter; SCRIPT_VM; MIR Epic 5; live FBO caret MAE.
