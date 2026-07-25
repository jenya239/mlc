# Track: Editor newline auto-indent without full stringify

Parent: [../PLAN.md](../PLAN.md) §98.
Residual of [TRACK_EDITOR_EDIT_AUTOCLOSE_NO_FULL_STRINGIFY](../archive/tracks/TRACK_EDITOR_EDIT_AUTOCLOSE_NO_FULL_STRINGIFY.md)
(§93) / #1d. Size **S**.

## Status: **open** — STEP=2 done; next Critic STEP=3

## Next step

**STEP=3** — Critic: stable×2 + edit_unit/newline_caret + `run_ux_gate`×2.

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `fbcc9570` | Decision: newline indent via slice / line-start scan |
| 1 | `866149a0` | Red: `edit_insert_newline` still flattens; stable stub |
| 2 | (this) | Green: `document_line_start_before` + `document_byte_slice` |

## Decision (STEP=0) — **frozen** 2026-07-25

| Item | Choice |
|------|--------|
| Problem | After §93, `edit_insert_newline` still does `document_to_string(working)` to build a `LineIndex` and copy leading indent — full flatten for Enter auto-indent |
| Strategy (v1) | Find current line start by scanning backward from caret (1-byte `document_byte_slice` peeks or a tiny helper), then take leading spaces/tabs from `document_byte_slice(doc, line_start, caret)` (or a bounded prefix). Drop `document_to_string` from `edit_insert_newline`. Prefer reuse of `leading_indent_of_line` on the sliced prefix (adjust API if needed: indent-from-string-at-0). Do **not** rewrite `line_index_from_document` / comment toggle / word-delete this track |
| Primary gate | Red: no green `run_ux_newline_indent_no_full_stringify_stable.sh`. Green: `ux_ok newline_indent_no_full_stringify` — `edit_insert_newline` has zero `document_to_string`; newline/edit related gates stay green |
| Module touch | `document/edit.mlc`; scripts; optional tiny helper in `piece_table.mlc` if scan belongs there |
| REG | no |
| Out of scope | comment toggle; word-delete; `frame_snapshot`; save; `line_index_from_document` rewrite; glyph rebuild; SCRIPT_VM; MIR Epic 5 |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Red: newline still full-flattens | **done** |
| 2 | Slice/scan indent; green | **done** |
| 3 | Critic: stable + related + `run_ux_gate` | pending |

## Out of scope

SCRIPT_VM; MIR Epic 5; LANG_AUTO_CYCLE; comment/word-delete; frame_snapshot; save; incremental lexer; §97 glyph rebuild locals.
