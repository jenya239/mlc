# Track: Editor paint reuses frame LineIndex

Parent: [../PLAN.md](../PLAN.md) §80.
Residual of [TRACK_EDITOR_LARGE_FILE_NO_FULL_STRINGIFY](../archive/tracks/TRACK_EDITOR_LARGE_FILE_NO_FULL_STRINGIFY.md)
(§46 #1d) / idle CPU / highlight-cache chain (§77–§79). Size **S**.

## Status: **open** — STEP=1 done; next Driver STEP=2

## Next step

**STEP=2** — Pass `frame.line_index` into paint helpers; green `ux_ok paint_reuses_frame_line_index`.

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `2968b98c` | Decision: paint path reuses `frame.line_index` |
| 1 | (this) | Red harness + stable stub `not implemented` |

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
| 2 | Pass frame LineIndex; green | pending |
| 3 | Critic: stable + full `run_ux_gate` | pending |

## Out of scope

SCRIPT_VM; MIR Epic 5; LANG_AUTO_CYCLE.
