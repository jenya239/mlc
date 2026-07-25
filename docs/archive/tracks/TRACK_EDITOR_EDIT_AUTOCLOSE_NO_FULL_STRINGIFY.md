# Track: Editor edit autoclose without full stringify

Parent: [../../PLAN.md](../../PLAN.md) §93.
Residual of [TRACK_EDITOR_CLIPBOARD_SLICE_NO_FULL_STRINGIFY](TRACK_EDITOR_CLIPBOARD_SLICE_NO_FULL_STRINGIFY.md)
(§92) / #1d. Size **S**.

## Status: **closed** (2026-07-25) — Critic OK

**Critic 2026-07-25 (STEP=3):** Re-ran `edit_autoclose_no_full_stringify_stable` ×2
(+ typing_paren + edit_unit + clipboard_slice) + HEAD `run_ux_gate` ×2.
Anti-false-done: `0d3dd8a8`…`3bf241e4` (STEP=1–2); `edit_insert_text_autoclose`
has zero `document_to_string`, uses `document_byte_slice` for wrap+peek;
`misc/editor/**` + scripts → REG skip; no `compiler/`/`lib/mlc/`.
**reopen: none**.

Honest residual: `*_red` post-green fails (use stable only); newline/comment/word-delete
flattens, `frame_snapshot`, save stringify, incremental lexer still OOS.

| Gate | Result |
|------|--------|
| `run_ux_edit_autoclose_no_full_stringify_stable.sh` | `ux_ok edit_autoclose_no_full_stringify` EXIT=0 (×2) |
| `run_ux_typing_paren_autocloses.sh` | EXIT=0 |
| `run_editor_edit_unit.sh` | EXIT=0 |
| `run_ux_clipboard_slice_no_full_stringify_stable.sh` | EXIT=0 |
| HEAD `run_ux_gate.sh` (109 scenarios) ×2 | `[ux gate] all ok` EXIT=0 |
| REG / self-host | N/A (editor + scripts) |

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | (prior) | Decision: autoclose via `document_byte_slice` |
| 1 | `0d3dd8a8` | Red: ≥2 `document_to_string` in autoclose; stable stub |
| 2 | `3bf241e4` | wrap+peek via `document_byte_slice`; green |
| 3 | (this) | Critic OK; archive |

## Decision (STEP=0) — **frozen** 2026-07-25

| Item | Choice |
|------|--------|
| Problem | After §92, `edit_insert_text_autoclose` still does `document_to_string` twice — wrap selected range + peek caret byte for skip-over |
| Strategy (v1) | Use `document_byte_slice` for selection wrap and caret peek |
| Primary gate | Green: autoclose path has no `document_to_string` |
| Module touch | `document/edit.mlc`; scripts |
| REG | no |
| Out of scope | newline/comment/word-delete flattens; frame_snapshot; save; incremental lexer; SCRIPT_VM; MIR Epic 5 |

## Out of scope

SCRIPT_VM; MIR Epic 5; LANG_AUTO_CYCLE; newline/comment/word-delete; frame_snapshot; save; incremental/diff lexer.
