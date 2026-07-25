# Track: Editor word-delete without full stringify

Parent: [../../PLAN.md](../../PLAN.md) §99.
Residual of [TRACK_EDITOR_NEWLINE_INDENT_NO_FULL_STRINGIFY](TRACK_EDITOR_NEWLINE_INDENT_NO_FULL_STRINGIFY.md)
(§98) / #1d. Size **S**.

## Status: **closed** (2026-07-25) — Critic OK

**Critic 2026-07-25 (STEP=3):** Re-ran `word_delete_no_full_stringify_stable` ×2
(+ word_boundary_unit + edit_unit + ctrl_backspace + newline_indent) + HEAD
`run_ux_gate` ×2 (111 scenarios). Anti-false-done: `e1da7834`…`741ec105`
(STEP=1–2); both word-delete fns use `word_boundary_*_document` (document peeks),
zero `document_to_string` on those paths; `misc/editor/**` + scripts → REG skip;
no `compiler/`/`lib/mlc/`. **reopen: none**.

Honest residual: `*_red` post-green fails (use stable only); comment toggle
flatten, `frame_snapshot`, save stringify, `line_index_from_document` rewrite,
keyboard word-nav via `line_index.text`, multi_click, incremental lexer still OOS.

| Gate | Result |
|------|--------|
| `run_ux_word_delete_no_full_stringify_stable.sh` | `ux_ok word_delete_no_full_stringify` EXIT=0 (×2) |
| `run_editor_word_boundary_unit.sh` | EXIT=0 |
| `run_editor_edit_unit.sh` | EXIT=0 |
| `run_ux_ctrl_backspace_deletes_word.sh` | EXIT=0 |
| `run_ux_newline_indent_no_full_stringify_stable.sh` | EXIT=0 |
| HEAD `run_ux_gate.sh` (111 scenarios) ×2 | `[ux gate] all ok` EXIT=0 |
| REG / self-host | N/A (editor + scripts) |

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `fc00e47b` | Decision: word-delete via document byte peeks |
| 1 | `e1da7834` | Red: flatten in both word-delete fns; stable stub |
| 2 | `741ec105` | `word_boundary_*_document` + wire; green |
| 3 | (this) | Critic OK; archive |

## Decision (STEP=0) — **frozen** 2026-07-25

| Item | Choice |
|------|--------|
| Problem | After §98, `edit_delete_word_*` still full-flatten for `word_boundary_*` |
| Strategy (v1) | Document-aware boundary helpers via 1-byte `document_byte_slice` + `is_word_char` |
| Primary gate | Green: `ux_ok word_delete_no_full_stringify` |
| Module touch | `document/edit.mlc`; `document/word_boundary.mlc`; scripts |
| REG | no |
| Out of scope | comment toggle; frame_snapshot; save; line_index rewrite; word-nav via `line_index.text`; multi_click; SCRIPT_VM; MIR Epic 5 |

## Out of scope

SCRIPT_VM; MIR Epic 5; LANG_AUTO_CYCLE; comment toggle; frame_snapshot; save; incremental lexer; §97 glyph rebuild locals.
