# Track: Editor overflow reuses frame LineIndex

Parent: [../../PLAN.md](../../PLAN.md) §84.
Residual of [TRACK_EDITOR_GOTO_LINE_REUSE_FRAME_LINE_INDEX](TRACK_EDITOR_GOTO_LINE_REUSE_FRAME_LINE_INDEX.md)
(§83) / #1d. Size **S**.

## Status: **closed** (2026-07-25) — Critic OK

**Critic 2026-07-25 (STEP=3):** Re-ran `overflow_reuses_frame_line_index_stable` ×2 +
`overflow_editor_text` + `draw_report` + `large_file_no_full_stringify` +
`demo_live` compile + HEAD `run_ux_gate`.
Anti-false-done: `93490972`…`348ffc43` (STEP=0–2); overflow helpers take
`LineIndex` (no `line_index_from_document` in `ux/overflow.mlc`);
`misc/editor/**` + scripts → REG skip; no `compiler/`/`lib/mlc/`.
**reopen: none**.

Honest residual: `*_red` post-green fails (use stable only); probe snapshot
own flatten + incremental/diff lexer still OOS.

| Gate | Result |
|------|--------|
| `run_ux_overflow_reuses_frame_line_index_stable.sh` | `ux_ok overflow_reuses_frame_line_index` EXIT=0 (×2) |
| `run_ux_overflow_editor_text.sh` | `ux_ok overflow_editor_text` EXIT=0 |
| `run_ux_draw_report.sh` | `ux_ok draw_report_headless` EXIT=0 |
| `run_ux_large_file_no_full_stringify.sh` | `ux_ok large_file_no_full_stringify` EXIT=0 |
| `run_editor_demo_live_fs_compile.sh` | `demo_live_fs_compile_ok` EXIT=0 |
| HEAD `run_ux_gate.sh` (99 scenarios) | `[ux gate] all ok` EXIT=0 |
| REG / self-host | N/A (editor + scripts) |

## Next step

**closed** — Critic OK. Authorized queue empty → Planner.

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `93490972` | Decision: overflow path reuses frame `LineIndex` |
| 1 | `2b504057` | Red harness + stable stub `not implemented` |
| 2 | `348ffc43` | Pass `LineIndex` into overflow/draw_frame; green |
| 3 | (this) | Critic OK; archive |

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
| 1 | Red: overflow still re-stringifies | **done** |
| 2 | Pass frame LineIndex; green | **done** |
| 3 | Critic: stable + full `run_ux_gate` | **done** |

## Out of scope

SCRIPT_VM; MIR Epic 5; LANG_AUTO_CYCLE; probe LineIndex pass-through.
