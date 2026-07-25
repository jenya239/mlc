# Track: Editor word-delete without full stringify

Parent: [../PLAN.md](../PLAN.md) §99.
Residual of [TRACK_EDITOR_NEWLINE_INDENT_NO_FULL_STRINGIFY](../archive/tracks/TRACK_EDITOR_NEWLINE_INDENT_NO_FULL_STRINGIFY.md)
(§98) / #1d. Size **S**.

## Status: **open** — STEP=0 done; next Driver STEP=1

## Next step

**STEP=1** — Red: assert `edit_delete_word_backward` / `edit_delete_word_forward` still call `document_to_string`; stable stub `not implemented`.

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | (this) | Decision: word-delete via document byte peeks |

## Decision (STEP=0) — **frozen** 2026-07-25

| Item | Choice |
|------|--------|
| Problem | After §98, `edit_delete_word_backward` / `edit_delete_word_forward` still do `document_to_string(document)` so `word_boundary_prev` / `word_boundary_next` can scan — full flatten on Ctrl+Backspace / Ctrl+Delete |
| Strategy (v1) | Add document-aware boundary helpers (in `word_boundary.mlc` or tiny locals in `edit.mlc`) that scan with 1-byte `document_byte_slice` + existing `is_word_char`. Wire both delete-word exports to those helpers. Drop `document_to_string` from both. Do **not** rewrite comment toggle / navigation `line_index.text` / multi_click this track |
| Primary gate | Red: no green `run_ux_word_delete_no_full_stringify_stable.sh`. Green: `ux_ok word_delete_no_full_stringify` — both word-delete fns have zero `document_to_string`; word_boundary/edit related gates stay green |
| Module touch | `document/edit.mlc`; optionally `document/word_boundary.mlc`; scripts |
| REG | no |
| Out of scope | comment toggle; `frame_snapshot`; save; `line_index_from_document`; keyboard word-nav via `line_index.text`; multi_click; SCRIPT_VM; MIR Epic 5 |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Red: word-delete still full-flattens | pending |
| 2 | Document peeks; green | pending |
| 3 | Critic: stable + related + `run_ux_gate` | pending |

<!-- STEP=1: red — document_to_string in both word-delete fns; stable stub -->
<!-- STEP=2: document-aware boundary + wire; green; word_boundary_unit / edit_unit -->

## Out of scope

SCRIPT_VM; MIR Epic 5; LANG_AUTO_CYCLE; comment toggle; frame_snapshot; save; incremental lexer; §97 glyph rebuild locals.
