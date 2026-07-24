# Track: Editor scroll reuses frame LineIndex

Parent: [../PLAN.md](../PLAN.md) §81.
Residual of [TRACK_EDITOR_PAINT_REUSE_FRAME_LINE_INDEX](../archive/tracks/TRACK_EDITOR_PAINT_REUSE_FRAME_LINE_INDEX.md)
(§80) / #1d. Size **S**.

## Status: **open** — STEP=0 done; next Driver STEP=1

## Next step

**STEP=1** — Red: assert scroll helpers still call `line_index_from_document`; stable stub `not implemented`.

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | (this) | Decision: scroll path reuses frame `LineIndex` |

## Decision (STEP=0) — **frozen** 2026-07-25

| Item | Choice |
|------|--------|
| Problem | After §80 paint helpers reuse frame `LineIndex`, scroll UX still re-flattens via `line_index_from_document` in `editor_ux_clamp_scroll_y`, `editor_ux_caret_in_viewport`, and `editor_ux_ensure_caret_visible` (`ux/scroll.mlc`), while `demo_live` / frame snapshot already hold `line_index`. Honest #1d / idle residual after §80 |
| Strategy (v1) | Pass `LineIndex` into those three scroll helpers (and wire callers: `app/state.mlc`, `ux/goto_line.mlc`, `ux/drag_autoscroll.mlc`, `demo_live` / scenarios). Drop per-call `line_index_from_document` inside them. Out of scope this track: selection_apply / overflow / probe / goto_line's own flatten / mass-convert all remaining call sites |
| Primary gate | Red: no green `run_ux_scroll_reuses_frame_line_index_stable.sh`. Green: `ux_ok scroll_reuses_frame_line_index` — the three scroll helpers take/reuse `LineIndex` (no `line_index_from_document` in `ux/scroll.mlc`); `paint_reuses_frame_line_index` + `large_file_no_full_stringify` stay green; auto-discovered by `run_ux_gate` |
| Module touch | `ux/scroll.mlc`, callers (`app/state.mlc`, `goto_line`, `drag_autoscroll`, scenarios); scripts |
| REG | no |
| Out of scope | selection_apply / overflow / probe flatten; incremental lexer; SCRIPT_VM; MIR Epic 5 |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Red: scroll still re-stringifies | pending |
| 2 | Pass frame LineIndex; green | pending |
| 3 | Critic: stable + full `run_ux_gate` | pending |

<!-- STEP=1 sub-steps: 1) scripts/run_ux_scroll_reuses_frame_line_index_red.sh asserts three helpers still call line_index_from_document; 2) stable stub exit 1 not implemented; 3) update TRACK/PLAN/SESSION -->
<!-- STEP=2 sub-steps: 1) add LineIndex param to clamp_scroll_y/caret_in_viewport/ensure_caret_visible; 2) wire app/state + goto_line + drag_autoscroll + scenarios; 3) green stable + related scroll scenarios -->

## Out of scope

SCRIPT_VM; MIR Epic 5; LANG_AUTO_CYCLE; selection/overflow/probe LineIndex pass-through.
