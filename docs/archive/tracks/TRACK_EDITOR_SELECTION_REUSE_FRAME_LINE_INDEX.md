# Track: Editor selection reuses frame LineIndex

Parent: [../../PLAN.md](../../PLAN.md) §82.
Residual of [TRACK_EDITOR_SCROLL_REUSE_FRAME_LINE_INDEX](TRACK_EDITOR_SCROLL_REUSE_FRAME_LINE_INDEX.md)
(§81) / #1d. Size **S**.

## Status: **closed** (2026-07-25) — Critic OK

**Critic 2026-07-25 (STEP=3):** Re-ran `selection_reuses_frame_line_index_stable` ×2 +
`multi_line_selection` + `selection_full_line_uses_display_columns` +
`drag_past_edge_autoscrolls` + `scroll_reuses_frame_line_index` +
`paint_reuses_frame_line_index` + `large_file_no_full_stringify` + `demo_live`
compile + HEAD `run_ux_gate`.
Anti-false-done: `56473a41`…`e55bca9c` (STEP=0–2); selection helpers take
`LineIndex` (no `line_index_from_document` in `ux/selection_apply.mlc`);
`misc/editor/**` + scripts → REG skip; no `compiler/`/`lib/mlc/`.
**reopen: none**.

Honest residual: `*_red` post-green fails (use stable only); overflow / probe /
goto_line own flatten + incremental/diff lexer still OOS.

| Gate | Result |
|------|--------|
| `run_ux_selection_reuses_frame_line_index_stable.sh` | `ux_ok selection_reuses_frame_line_index` EXIT=0 (×2) |
| `run_ux_multi_line_selection.sh` | EXIT=0 |
| `run_ux_selection_full_line_uses_display_columns.sh` | EXIT=0 |
| `run_ux_drag_past_edge_autoscrolls.sh` | `ux_ok drag_past_edge_autoscrolls` EXIT=0 |
| `run_ux_scroll_reuses_frame_line_index_stable.sh` | `ux_ok scroll_reuses_frame_line_index` EXIT=0 |
| `run_ux_paint_reuses_frame_line_index_stable.sh` | `ux_ok paint_reuses_frame_line_index` EXIT=0 |
| `run_ux_large_file_no_full_stringify.sh` | `ux_ok large_file_no_full_stringify` EXIT=0 |
| `run_editor_demo_live_fs_compile.sh` | `demo_live_fs_compile_ok` EXIT=0 |
| HEAD `run_ux_gate.sh` (97 scenarios) | `[ux gate] all ok` EXIT=0 |
| REG / self-host | N/A (editor + scripts) |

## Next step

**closed** — Critic OK. Authorized queue empty → Planner.

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `56473a41` | Decision: selection path reuses frame `LineIndex` |
| 1 | `23c7074c` | Red harness + stable stub `not implemented` |
| 2 | `e55bca9c` | Pass `LineIndex` into selection helpers; green |
| 3 | (this) | Critic OK; archive |

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
| 2 | Pass frame LineIndex; green | **done** |
| 3 | Critic: stable + full `run_ux_gate` | **done** |

## Out of scope

SCRIPT_VM; MIR Epic 5; LANG_AUTO_CYCLE; overflow/probe/goto LineIndex pass-through.
