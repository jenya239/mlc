# Track: Editor comment toggle without full stringify

Parent: [../PLAN.md](../PLAN.md) §100.
Residual of [TRACK_EDITOR_WORD_DELETE_NO_FULL_STRINGIFY](../archive/tracks/TRACK_EDITOR_WORD_DELETE_NO_FULL_STRINGIFY.md)
(§99) / #1d. Size **S**.

## Status: **open** — STEP=0 done; next Driver STEP=1

## Next step

**STEP=1** — Red: assert `edit_toggle_line_comment` still calls `document_to_string` / `document_from_string`; stable stub `not implemented`.

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | (this) | Decision: comment toggle via slice + local replace |

## Decision (STEP=0) — **frozen** 2026-07-25

| Item | Choice |
|------|--------|
| Problem | After §99, last `document_to_string` in `edit.mlc` is `edit_toggle_line_comment`: full flatten → rebuild every line as a string → `document_from_string` (whole-buffer rewrite) for Ctrl+/ |
| Strategy (v1) | Resolve selected line span without full flatten (reuse `document_line_start_before` / newline peeks). Slice each affected line (or the contiguous multi-line span) via `document_byte_slice`, apply existing `comment_line_body` / `uncomment_line_body`, write back with `document_delete` + `document_insert` (only the touched range). Drop `document_to_string` and `document_from_string` from `edit_toggle_line_comment`. Do **not** rewrite `indent.mlc` tab/outdent, `line_index_from_document`, save, or frame_snapshot this track |
| Primary gate | Red: no green `run_ux_comment_toggle_no_full_stringify_stable.sh`. Green: `ux_ok comment_toggle_no_full_stringify` — zero `document_to_string`/`document_from_string` in `edit_toggle_line_comment`; `run_ux_toggle_line_comment.sh` stays green |
| Module touch | `document/edit.mlc`; scripts |
| REG | no |
| Out of scope | indent tab/outdent flatten; `line_index_from_document` rewrite; save; frame_snapshot; word-nav `line_index.text`; multi_click; SCRIPT_VM; MIR Epic 5 |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Red: comment toggle still full-flattens | pending |
| 2 | Slice + local replace; green | pending |
| 3 | Critic: stable + related + `run_ux_gate` | pending |

<!-- STEP=1: red — document_to_string + document_from_string in edit_toggle_line_comment; stable stub -->
<!-- STEP=2: line-span scan + document_byte_slice + delete/insert; green; toggle_line_comment -->

## Out of scope

SCRIPT_VM; MIR Epic 5; LANG_AUTO_CYCLE; indent.mlc; save; frame_snapshot; incremental lexer; §97 glyph rebuild locals.
