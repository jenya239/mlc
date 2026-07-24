# Track: Editor scroll reuses frame LineIndex

Parent: [../../PLAN.md](../../PLAN.md) §81.
Residual of [TRACK_EDITOR_PAINT_REUSE_FRAME_LINE_INDEX](TRACK_EDITOR_PAINT_REUSE_FRAME_LINE_INDEX.md)
(§80) / #1d. Size **S**.

## Status: **closed** (2026-07-25) — Critic OK

**Critic 2026-07-25 (STEP=3):** Re-ran `scroll_reuses_frame_line_index_stable` ×2 +
`wheel_scroll` + `goto_line_scrolls_caret` + `drag_past_edge_autoscrolls` +
`paint_reuses_frame_line_index` + `large_file_no_full_stringify` + `demo_live`
compile + HEAD `run_ux_gate`.
Anti-false-done: `bbdcccf5`…`672fe791` (STEP=0–2); scroll helpers take
`LineIndex` (no `line_index_from_document` in `ux/scroll.mlc`); `misc/editor/**`
+ scripts → REG skip; no `compiler/`/`lib/mlc/`.
**reopen: none**.

Honest residual: `*_red` post-green fails (use stable only); selection_apply /
overflow / probe / goto_line own flatten + incremental/diff lexer still OOS.

| Gate | Result |
|------|--------|
| `run_ux_scroll_reuses_frame_line_index_stable.sh` | `ux_ok scroll_reuses_frame_line_index` EXIT=0 (×2) |
| `run_ux_wheel_scroll.sh` | `ux_ok wheel_scroll_keeps_caret_visible` EXIT=0 |
| `run_ux_goto_line_scrolls_caret.sh` | `ux_ok goto_line_scrolls_caret` EXIT=0 |
| `run_ux_drag_past_edge_autoscrolls.sh` | `ux_ok drag_past_edge_autoscrolls` EXIT=0 |
| `run_ux_paint_reuses_frame_line_index_stable.sh` | `ux_ok paint_reuses_frame_line_index` EXIT=0 |
| `run_ux_large_file_no_full_stringify.sh` | `ux_ok large_file_no_full_stringify` EXIT=0 |
| `run_editor_demo_live_fs_compile.sh` | `demo_live_fs_compile_ok` EXIT=0 |
| HEAD `run_ux_gate.sh` (96 scenarios) | `[ux gate] all ok` EXIT=0 |
| REG / self-host | N/A (editor + scripts) |

## Next step

**closed** — Critic OK. Authorized queue empty → Planner.

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `bbdcccf5` | Decision: scroll path reuses frame `LineIndex` |
| 1 | `122f8b62` | Red harness + stable stub `not implemented` |
| 2 | `672fe791` | Pass `LineIndex` into scroll helpers; green |
| 3 | (this) | Critic OK; archive |

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
| 1 | Red: scroll still re-stringifies | **done** |
| 2 | Pass frame LineIndex; green | **done** |
| 3 | Critic: stable + full `run_ux_gate` | **done** |

## Out of scope

SCRIPT_VM; MIR Epic 5; LANG_AUTO_CYCLE; selection/overflow/probe LineIndex pass-through.
