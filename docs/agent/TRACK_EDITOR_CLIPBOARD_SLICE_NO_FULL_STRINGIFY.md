# Track: Editor clipboard slice without full stringify

Parent: [../PLAN.md](../PLAN.md) §92.
Residual of [TRACK_EDITOR_TREE_CLICK_NO_STRINGIFY](../archive/tracks/TRACK_EDITOR_TREE_CLICK_NO_STRINGIFY.md)
(§91) / #1d. Size **S**.

## Status: **open** — STEP=1 done; next Driver STEP=2

## Next step

**STEP=2** — `document_byte_slice` + wire clipboard; green stable.

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `eb759388` | Decision: piece-range slice for clipboard copy/cut |
| 1 | (this) | Red: `document_to_string` in slice; stable stub |

## Decision (STEP=0) — **frozen** 2026-07-25

| Item | Choice |
|------|--------|
| Problem | After §91, `clipboard_edit.mlc` `document_slice` still does `document_to_string(document)` then `substring` for Ctrl+C/X — full flatten even for a small selection. Honest #1d residual after §91 |
| Strategy (v1) | Export `document_byte_slice` (or equivalent) on `piece_table` that walks only pieces overlapping `[byte_start, byte_end)`; use it in `clipboard_edit.document_slice`. Out of scope: `frame_snapshot` single flatten; `edit.mlc` autoclose/newline flattens; save stringify; incremental lexer |
| Primary gate | Red: no green `run_ux_clipboard_slice_no_full_stringify_stable.sh`. Green: `ux_ok clipboard_slice_no_full_stringify` — `clipboard_edit` has no `document_to_string`; clipboard unit / copy-paste scenario stay green |
| Module touch | `document/piece_table.mlc`; `document/clipboard_edit.mlc`; scripts; optional scenario |
| REG | no |
| Out of scope | frame_snapshot; edit.mlc flattens; save; incremental lexer; SCRIPT_VM; MIR Epic 5 |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Red: clipboard slice still full-flattens | **done** |
| 2 | `document_byte_slice` + wire clipboard; green | pending |
| 3 | Critic: stable + related + `run_ux_gate` | pending |

<!-- STEP=2 sub-steps: 1) piece_table document_byte_slice; 2) clipboard_edit use it; 3) green stable + clipboard unit -->

## Out of scope

SCRIPT_VM; MIR Epic 5; LANG_AUTO_CYCLE; frame_snapshot; edit.mlc flattens; save; incremental/diff lexer.
