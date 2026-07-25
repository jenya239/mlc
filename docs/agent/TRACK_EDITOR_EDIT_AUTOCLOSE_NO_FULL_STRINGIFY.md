# Track: Editor edit autoclose without full stringify

Parent: [../PLAN.md](../PLAN.md) §93.
Residual of [TRACK_EDITOR_CLIPBOARD_SLICE_NO_FULL_STRINGIFY](../archive/tracks/TRACK_EDITOR_CLIPBOARD_SLICE_NO_FULL_STRINGIFY.md)
(§92) / #1d. Size **S**.

## Status: **open** — STEP=1 **done**; next Driver STEP=2

## Next step

**STEP=2** — `document_byte_slice` in autoclose; green stable.

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | (prior) | Decision: autoclose via `document_byte_slice` |
| 1 | (this) | Red: ≥2 `document_to_string` in autoclose; stable stub |

## Decision (STEP=0) — **frozen** 2026-07-25

| Item | Choice |
|------|--------|
| Problem | After §92, `edit_insert_text_autoclose` still does `document_to_string` twice — wrap selected range + peek caret byte for skip-over. Full flatten for a 1-byte peek / selection slice. Honest #1d residual after §92 |
| Strategy (v1) | Use `document_byte_slice` for selection wrap and caret peek (or tiny 1-byte slice). Drop those `document_to_string` sites in autoclose. Out of scope: newline/indent flatten; comment toggle; word-delete flatten; `frame_snapshot`; save; incremental lexer |
| Primary gate | Red: no green `run_ux_edit_autoclose_no_full_stringify_stable.sh`. Green: `ux_ok edit_autoclose_no_full_stringify` — autoclose path has no `document_to_string`; typing_paren / related edit gates stay green |
| Module touch | `document/edit.mlc`; scripts |
| REG | no |
| Out of scope | newline/comment/word-delete flattens; frame_snapshot; save; incremental lexer; SCRIPT_VM; MIR Epic 5 |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Red: autoclose still full-flattens | **done** (`run_ux_edit_autoclose_no_full_stringify_red.sh`) |
| 2 | `document_byte_slice` in autoclose; green | pending |
| 3 | Critic: stable + related + `run_ux_gate` | pending |

<!-- STEP=1: red — ≥2 document_to_string(document) in edit_insert_text_autoclose; stable stub -->
<!-- STEP=2 sub-steps: 1) replace wrap+peek with document_byte_slice; 2) green stable; 3) typing_paren scenario -->

## Out of scope

SCRIPT_VM; MIR Epic 5; LANG_AUTO_CYCLE; newline/comment/word-delete; frame_snapshot; save; incremental/diff lexer.
