# Track: Editor goto-line reuses frame LineIndex

Parent: [../../PLAN.md](../../PLAN.md) §83.
Residual of [TRACK_EDITOR_SELECTION_REUSE_FRAME_LINE_INDEX](TRACK_EDITOR_SELECTION_REUSE_FRAME_LINE_INDEX.md)
(§82) / #1d. Size **S**.

## Status: **closed** (2026-07-25) — Critic OK

**Critic 2026-07-25 (STEP=3):** Re-ran `goto_line_reuses_frame_line_index_stable` ×2 +
`goto_line_scrolls_caret` + `selection_reuses_frame_line_index` +
`large_file_no_full_stringify` + `demo_live` compile + HEAD `run_ux_gate`.
Anti-false-done: `0f6806e8`…`051d9440` (STEP=0–2); goto helpers take
`LineIndex` (no `line_index_from_document` in `ux/goto_line.mlc`);
`misc/editor/**` + scripts → REG skip; no `compiler/`/`lib/mlc/`.
**reopen: none**.

Honest residual: `*_red` post-green fails (use stable only); overflow / probe
own flatten + incremental/diff lexer still OOS.

| Gate | Result |
|------|--------|
| `run_ux_goto_line_reuses_frame_line_index_stable.sh` | `ux_ok goto_line_reuses_frame_line_index` EXIT=0 (×2) |
| `run_ux_goto_line_scrolls_caret.sh` | `ux_ok goto_line_scrolls_caret` EXIT=0 |
| `run_ux_selection_reuses_frame_line_index_stable.sh` | `ux_ok selection_reuses_frame_line_index` EXIT=0 |
| `run_ux_large_file_no_full_stringify.sh` | `ux_ok large_file_no_full_stringify` EXIT=0 |
| `run_editor_demo_live_fs_compile.sh` | `demo_live_fs_compile_ok` EXIT=0 |
| HEAD `run_ux_gate.sh` (98 scenarios) | `[ux gate] all ok` EXIT=0 |
| REG / self-host | N/A (editor + scripts) |

## Next step

**closed** — Critic OK. Authorized queue empty → Planner.

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `0f6806e8` | Decision: goto-line path reuses frame `LineIndex` |
| 1 | `eb729639` | Red harness + stable stub `not implemented` |
| 2 | `051d9440` | Pass `LineIndex` into goto helpers; green |
| 3 | (this) | Critic OK; archive |

## Decision (STEP=0) — **frozen** 2026-07-25

| Item | Choice |
|------|--------|
| Problem | After §80–§82 paint/scroll/selection reuse frame `LineIndex`, goto-line still re-flattens via `line_index_from_document` in `editor_ux_goto_line` and `editor_ux_goto_line_from_selection` (`ux/goto_line.mlc`), while `demo_live` already holds frame `line_index`. Honest #1d residual after §82 |
| Strategy (v1) | Pass `LineIndex` into those two goto helpers (wire `app/state.mlc` + `demo_live` + scenarios). Drop per-call `line_index_from_document` inside `goto_line.mlc`. Out of scope: overflow / probe flatten; incremental lexer |
| Primary gate | Red: no green `run_ux_goto_line_reuses_frame_line_index_stable.sh`. Green: `ux_ok goto_line_reuses_frame_line_index` — the two goto helpers take/reuse `LineIndex` (no `line_index_from_document` in `ux/goto_line.mlc`); `goto_line_scrolls_caret` + `selection_reuses_frame_line_index` + `large_file_no_full_stringify` stay green; auto-discovered by `run_ux_gate` |
| Module touch | `ux/goto_line.mlc`, callers (`app/state.mlc`, demo, scenarios); scripts |
| REG | no |
| Out of scope | overflow / probe; incremental lexer; SCRIPT_VM; MIR Epic 5 |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Red: goto-line still re-stringifies | **done** |
| 2 | Pass frame LineIndex; green | **done** |
| 3 | Critic: stable + full `run_ux_gate` | **done** |

## Out of scope

SCRIPT_VM; MIR Epic 5; LANG_AUTO_CYCLE; overflow/probe LineIndex pass-through.
