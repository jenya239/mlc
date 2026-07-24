# Track: Editor selection highlight on visual wrap rows

Parent: [../PLAN.md](../PLAN.md) §54.
Residual of [TRACK_EDITOR_PROPORTIONAL_HIT_TEST](../archive/tracks/TRACK_EDITOR_PROPORTIONAL_HIT_TEST.md)
(§53 Decision OOS). Size **M**.

## Status: **active** (2026-07-24) — STEP=2 done → Critic STEP=3

## Next step

**STEP=3** — Critic: stable + full `run_ux_gate`.

## Decision (STEP=0) — **frozen** 2026-07-24

| Item | Choice |
|------|--------|
| Problem | Selection fill in `demo_live` still iterates **doc lines** and maps `row = highlight_line - visible.first_line`, ignoring soft-wrap visual rows → wrong Y (and clipped X) when a selected line wraps |
| Strategy (v1) | For each visible `VisualRow`, intersect `[row.byte_start, row.byte_end)` with selection byte range; draw fill via `pixel_x_for_byte_offset` on that row; keep collapsed/empty selection path unchanged |
| Primary gate | Red: no `run_ux_wrap_selection_visual_stable.sh`. Green: `ux_ok wrap_selection_visual_stable` — wrapped selected line paints one rect per intersecting visual row at correct Y; auto-discovered by `run_ux_gate` |
| Module touch | `demo_live` selection draw (or thin helper under `ux/`/`layout/`), `scripts/run_ux_wrap_selection_visual_*.sh`, `ux_scenarios/` |
| REG | no |
| Out of scope | Multi-cursor fills; find-highlight rewrite; ICU; SCRIPT_VM; MIR Epic 5; reopen §53 numbered STEPs |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Red: no wrap-selection visual script | **done** |
| 2 | Implement visual-row selection fill; green | **done** |
| 3 | Critic: stable + full `run_ux_gate` | open |

<!-- STEP=1: red proves stable script missing or documents gap -->
<!-- STEP=2: intersect selection with visual_rows; SKIP if font/shape missing -->
<!-- STEP=3: Critic archive -->

## Out of scope

ICU; multi-cursor; SCRIPT_VM; MIR Epic 5.
