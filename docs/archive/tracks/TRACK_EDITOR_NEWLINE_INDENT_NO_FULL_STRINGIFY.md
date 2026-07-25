# Track: Editor newline auto-indent without full stringify

Parent: [../../PLAN.md](../../PLAN.md) §98.
Residual of [TRACK_EDITOR_EDIT_AUTOCLOSE_NO_FULL_STRINGIFY](TRACK_EDITOR_EDIT_AUTOCLOSE_NO_FULL_STRINGIFY.md)
(§93) / #1d. Size **S**.

## Status: **closed** (2026-07-25) — Critic OK

**Critic 2026-07-25 (STEP=3):** Re-ran `newline_indent_no_full_stringify_stable` ×2
(+ edit_unit + newline_caret + autoclose + clipboard_slice) + HEAD `run_ux_gate` ×2
(110 scenarios). Anti-false-done: `866149a0`…`21b04e45` (STEP=1–2);
`edit_insert_newline` uses `document_line_start_before` + `document_byte_slice`,
zero `document_to_string` on that path; `misc/editor/**` + scripts → REG skip;
no `compiler/`/`lib/mlc/`. **reopen: none**.

Honest residual: `*_red` post-green fails (use stable only); comment toggle /
word-delete flattens, `frame_snapshot`, save stringify, `line_index_from_document`
rewrite, incremental lexer still OOS.

| Gate | Result |
|------|--------|
| `run_ux_newline_indent_no_full_stringify_stable.sh` | `ux_ok newline_indent_no_full_stringify` EXIT=0 (×2) |
| `run_editor_edit_unit.sh` | EXIT=0 |
| `run_ux_newline_caret.sh` | EXIT=0 |
| `run_ux_edit_autoclose_no_full_stringify_stable.sh` | EXIT=0 |
| `run_ux_clipboard_slice_no_full_stringify_stable.sh` | EXIT=0 |
| HEAD `run_ux_gate.sh` (110 scenarios) ×2 | `[ux gate] all ok` EXIT=0 |
| REG / self-host | N/A (editor + scripts) |

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `fbcc9570` | Decision: newline indent via slice / line-start scan |
| 1 | `866149a0` | Red: flatten in `edit_insert_newline`; stable stub |
| 2 | `21b04e45` | `document_line_start_before` + `document_byte_slice`; green |
| 3 | (this) | Critic OK; archive |

## Decision (STEP=0) — **frozen** 2026-07-25

| Item | Choice |
|------|--------|
| Problem | After §93, `edit_insert_newline` still does `document_to_string(working)` to build a `LineIndex` and copy leading indent — full flatten for Enter auto-indent |
| Strategy (v1) | Find current line start by scanning backward from caret (1-byte `document_byte_slice` peeks), then leading indent from `document_byte_slice(doc, line_start, caret)` via `leading_indent_of_line(prefix, 0)` |
| Primary gate | Green: `ux_ok newline_indent_no_full_stringify` — zero `document_to_string` in newline path |
| Module touch | `document/edit.mlc`; scripts |
| REG | no |
| Out of scope | comment toggle; word-delete; `frame_snapshot`; save; `line_index_from_document` rewrite; glyph rebuild; SCRIPT_VM; MIR Epic 5 |

## Out of scope

SCRIPT_VM; MIR Epic 5; LANG_AUTO_CYCLE; comment/word-delete; frame_snapshot; save; incremental lexer; §97 glyph rebuild locals.
