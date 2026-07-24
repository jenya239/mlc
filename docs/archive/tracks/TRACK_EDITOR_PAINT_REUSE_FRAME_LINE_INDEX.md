# Track: Editor paint reuses frame LineIndex

Parent: [../../PLAN.md](../../PLAN.md) §80.
Residual of [TRACK_EDITOR_LARGE_FILE_NO_FULL_STRINGIFY](TRACK_EDITOR_LARGE_FILE_NO_FULL_STRINGIFY.md)
(§46 #1d) / idle CPU / highlight-cache chain (§77–§79). Size **S**.

## Status: **closed** (2026-07-25) — Critic OK

**Critic 2026-07-25 (STEP=3):** Re-ran `paint_reuses_frame_line_index_stable` ×2 +
`current_line_highlight_draws` + `trailing_ws_visualized` + `status_bar_unit` +
`large_file_no_full_stringify` + `demo_live` compile + HEAD `run_ux_gate`.
Anti-false-done: `2968b98c`…`094e0c4d` (STEP=0–2); paint helpers take
`LineIndex` (no `line_index_from_document`); `misc/editor/**` + scripts → REG
skip; no `compiler/`/`lib/mlc/`.
**reopen: none**.

Honest residual: `*_red` post-green fails (use stable only); nav/scroll/selection
`line_index_from_document` call sites and incremental/diff lexer still OOS.

| Gate | Result |
|------|--------|
| `run_ux_paint_reuses_frame_line_index_stable.sh` | `ux_ok paint_reuses_frame_line_index` EXIT=0 (×2) |
| `run_ux_current_line_highlight_draws.sh` | `ux_ok current_line_highlight_draws` EXIT=0 |
| `run_ux_trailing_ws_visualized.sh` | `ux_ok trailing_ws_visualized` EXIT=0 |
| `run_editor_status_bar_unit.sh` | `[mlc-editor] status_bar_unit ok` EXIT=0 |
| `run_ux_large_file_no_full_stringify.sh` | `ux_ok large_file_no_full_stringify` EXIT=0 |
| `run_editor_demo_live_fs_compile.sh` | `demo_live_fs_compile_ok` EXIT=0 |
| HEAD `run_ux_gate.sh` (95 scenarios) | `[ux gate] all ok` EXIT=0 |
| REG / self-host | N/A (editor + scripts) |

## Next step

**closed** — Critic OK. Authorized queue empty → Planner.

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `2968b98c` | Decision: paint path reuses `frame.line_index` |
| 1 | `fda0ef30` | Red harness + stable stub `not implemented` |
| 2 | `094e0c4d` | Pass `LineIndex` into three paint helpers; green |
| 3 | (this) | Critic OK; archive |

## Decision (STEP=0) — **frozen** 2026-07-25

| Item | Choice |
|------|--------|
| Problem | After `#1d` frame snapshot, idle paint still re-flattens via `line_index_from_document` (= `document_to_string` + line scan) in current-line HL (`ux/current_line_hl.mlc`), trailing-ws (`ux/trailing_ws.mlc`), and status bar (`ui/status_bar.mlc`), while `demo_live` already has `frame.line_index`. Honest large-file / idle CPU residual after §79 |
| Strategy (v1) | Pass `LineIndex` (from frame snapshot) into those three paint helpers; drop per-paint `line_index_from_document`. Keep geometry/RGB unchanged. Out of scope this track: nav/scroll/selection helpers that also call `line_index_from_document` |
| Primary gate | Red: no green `run_ux_paint_reuses_frame_line_index_stable.sh`. Green: `ux_ok paint_reuses_frame_line_index` — the three paint helpers take/reuse `LineIndex` (no `line_index_from_document` on that paint path); `large_file_no_full_stringify` stays green; auto-discovered by `run_ux_gate` |
| Module touch | `ux/current_line_hl.mlc`, `ux/trailing_ws.mlc`, `ui/status_bar.mlc`, `demo_live` call sites; scripts/scenario |
| REG | no |
| Out of scope | Incremental lexer; ICU; SCRIPT_VM; MIR Epic 5; mass-convert all `line_index_from_document` call sites |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Red: paint still re-stringifies | **done** |
| 2 | Pass frame LineIndex; green | **done** |
| 3 | Critic: stable + full `run_ux_gate` | **done** |

## Out of scope

SCRIPT_VM; MIR Epic 5; LANG_AUTO_CYCLE.
