# Track: Editor selection highlight on visual wrap rows

Parent: [../../PLAN.md](../../PLAN.md) §54.
Residual of [TRACK_EDITOR_PROPORTIONAL_HIT_TEST](TRACK_EDITOR_PROPORTIONAL_HIT_TEST.md)
(§53 Decision OOS). Size **M**.

## Status: **closed** (2026-07-24) — Critic OK

**Critic 2026-07-24 (STEP=3):** Re-ran `wrap_selection_visual_stable` ×2 + `proportional_hit_test_stable` +
`wrap_budget_advance_stable` + `demo_live` compile + HEAD `run_ux_gate` (69 scenarios).
Anti-false-done: `7c078e1e`…`33fcee28` (STEP=0–2); `selection_fill_visual_rows` + demo_live wire;
`misc/editor/**` + scripts → REG skip; no `compiler/`/`lib/mlc/`.
**reopen: none**.

Honest residual: multi-cursor selection fills OOS; `*_red` post-green fails (use stable only).

| Gate | Result |
|------|--------|
| `run_ux_wrap_selection_visual_stable.sh` | `ux_ok wrap_selection_visual_stable` EXIT=0 (×2) |
| `run_ux_proportional_hit_test_stable.sh` | `ux_ok proportional_hit_test_stable` EXIT=0 |
| `run_ux_wrap_budget_advance_stable.sh` | `ux_ok wrap_budget_advance_stable` EXIT=0 |
| `run_editor_demo_live_fs_compile.sh` | `demo_live_fs_compile_ok` EXIT=0 |
| HEAD `run_ux_gate.sh` (69 scenarios) | `[ux gate] all ok` EXIT=0 |
| REG / self-host | N/A (editor + scripts) |

## Next step

**closed** — Critic OK. Queue → Planner (authorized queue empty; select next or document idle).

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `7c078e1e` | Decision: visual-row selection fill |
| 1 | `4ddb3c3c` | Red/stable stub |
| 2 | `33fcee28` | `selection_fill_visual_rows` + demo wire |
| 3 | (this) | Critic close + archive |

## Decision (STEP=0) — **frozen** 2026-07-24

| Item | Choice |
|------|--------|
| Problem | Selection fill used doc-line Y map, ignoring soft-wrap visual rows |
| Strategy (v1) | Intersect selection bytes with each visible VisualRow; fill via pixel_x_for_byte_offset |
| Primary gate | `ux_ok wrap_selection_visual_stable`; full `run_ux_gate` |
| Module touch | `word_wrap.mlc` + `demo_live` + scripts/scenario |
| REG | no |
| Out of scope | Multi-cursor fills; ICU; SCRIPT_VM; MIR Epic 5 |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Red: no wrap-selection visual script | **done** |
| 2 | Implement visual-row selection fill; green | **done** |
| 3 | Critic: stable + full `run_ux_gate` | **done** — Critic OK |

## Out of scope

ICU; multi-cursor; SCRIPT_VM; MIR Epic 5.
