# Track: Editor overflow reuses frame LineIndex

Parent: [../PLAN.md](../PLAN.md) §84.
Residual of [TRACK_EDITOR_GOTO_LINE_REUSE_FRAME_LINE_INDEX](../archive/tracks/TRACK_EDITOR_GOTO_LINE_REUSE_FRAME_LINE_INDEX.md)
(§83) / #1d. Size **S**.

## Status: **open** — STEP=0 done; next Driver STEP=1

## Next step

**STEP=1** — Red: assert overflow helpers still call `line_index_from_document`; stable stub `not implemented`.

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | (this) | Decision: overflow path reuses frame `LineIndex` |

## Decision (STEP=0) — **frozen** 2026-07-25

| Item | Choice |
|------|--------|
| Problem | After §80–§83 paint/scroll/selection/goto reuse frame `LineIndex`, overflow still re-flattens via `line_index_from_document` in `content_rect_for_state` → `editor_ux_collect_overflows` / `editor_ux_refresh_overflows` (`ux/overflow.mlc`), while `draw_frame` / `demo_live` already hold frame `line_index`. Honest #1d residual after §83 |
| Strategy (v1) | Pass `LineIndex` into overflow collect/refresh (and internal content-rect helper); wire `draw_frame.mlc` + `overflow_editor_text` scenario (+ any other callers). Drop per-call `line_index_from_document` inside `overflow.mlc`. Out of scope: probe snapshot flatten; incremental lexer |
| Primary gate | Red: no green `run_ux_overflow_reuses_frame_line_index_stable.sh`. Green: `ux_ok overflow_reuses_frame_line_index` — overflow helpers take/reuse `LineIndex` (no `line_index_from_document` in `ux/overflow.mlc`); `overflow_editor_text` + `large_file_no_full_stringify` stay green; auto-discovered by `run_ux_gate` |
| Module touch | `ux/overflow.mlc`, callers (`ux/draw_frame.mlc`, scenarios); scripts |
| REG | no |
| Out of scope | probe; incremental lexer; SCRIPT_VM; MIR Epic 5 |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Red: overflow still re-stringifies | pending |
| 2 | Pass frame LineIndex; green | pending |
| 3 | Critic: stable + full `run_ux_gate` | pending |

<!-- STEP=1 sub-steps: 1) red asserts ≥1 line_index_from_document in overflow.mlc; 2) stable stub; 3) docs -->
<!-- STEP=2 sub-steps: 1) LineIndex param on collect/refresh (+ content_rect); 2) wire draw_frame + scenarios; 3) green stable -->

## Out of scope

SCRIPT_VM; MIR Epic 5; LANG_AUTO_CYCLE; probe LineIndex pass-through.
