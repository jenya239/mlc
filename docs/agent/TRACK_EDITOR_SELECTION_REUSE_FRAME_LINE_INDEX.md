# Track: Editor selection reuses frame LineIndex

Parent: [../PLAN.md](../PLAN.md) §82.
Residual of [TRACK_EDITOR_SCROLL_REUSE_FRAME_LINE_INDEX](../archive/tracks/TRACK_EDITOR_SCROLL_REUSE_FRAME_LINE_INDEX.md)
(§81) / #1d. Size **S**.

## Status: **open** — STEP=1 done; next Driver STEP=2

## Next step

**STEP=2** — Pass `LineIndex` into selection helpers; green `ux_ok selection_reuses_frame_line_index`.

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `56473a41` | Decision: selection path reuses frame `LineIndex` |
| 1 | (this) | Red harness + stable stub `not implemented` |

## Decision (STEP=0) — **frozen** 2026-07-25

| Item | Choice |
|------|--------|
| Problem | After §80/§81 paint+scroll reuse frame `LineIndex`, selection UX still re-flattens via `line_index_from_document` in `editor_ux_click_text`, `editor_ux_drag_text`, and `editor_ux_selection_draw_report` (`ux/selection_apply.mlc`), while `demo_live` already holds frame `line_index`. Honest #1d residual after §81 |
| Strategy (v1) | Pass `LineIndex` into those three selection helpers (wire callers: drag_autoscroll, app/demo click/drag/paint, scenarios). Drop per-call `line_index_from_document` inside `selection_apply.mlc`. Out of scope: overflow / probe / goto_line flatten; incremental lexer |
| Primary gate | Red: no green `run_ux_selection_reuses_frame_line_index_stable.sh`. Green: `ux_ok selection_reuses_frame_line_index` — the three selection helpers take/reuse `LineIndex` (no `line_index_from_document` in `ux/selection_apply.mlc`); `scroll_reuses_frame_line_index` + `paint_reuses_frame_line_index` + `large_file_no_full_stringify` stay green; auto-discovered by `run_ux_gate` |
| Module touch | `ux/selection_apply.mlc`, callers (`drag_autoscroll`, app/demo, scenarios); scripts |
| REG | no |
| Out of scope | overflow / probe / goto_line; incremental lexer; SCRIPT_VM; MIR Epic 5 |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Red: selection still re-stringifies | **done** |
| 2 | Pass frame LineIndex; green | pending |
| 3 | Critic: stable + full `run_ux_gate` | pending |

<!-- STEP=2 sub-steps: 1) LineIndex param on click/drag/draw_report; 2) wire callers; 3) green stable + related selection scenarios -->

## Out of scope

SCRIPT_VM; MIR Epic 5; LANG_AUTO_CYCLE; overflow/probe/goto LineIndex pass-through.
