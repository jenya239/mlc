# Track: Editor drop-file opens without stringify

Parent: [../PLAN.md](../PLAN.md) §87.
Residual of [TRACK_EDITOR_PROBE_REUSE_LINE_INDEX_TEXT](../archive/tracks/TRACK_EDITOR_PROBE_REUSE_LINE_INDEX_TEXT.md)
(§86) / #1d. Size **S**.

## Status: **open** — STEP=2 done; next Critic STEP=3

## Next step

**STEP=3** — Critic: stable×2 + related + full `run_ux_gate`.

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `cca9e565` | Decision: open dropped path without `document_to_string` |
| 1 | `f41c931e` | Red: ≥1 `document_to_string` in `drop_file.mlc`; stable stub |
| 2 | (this) | `tab_set_open_buffer`; drop stringify; green stable |

## Decision (STEP=0) — **frozen** 2026-07-25

| Item | Choice |
|------|--------|
| Problem | After §86, production `ux/**` has zero `document_to_string` except `ux/drop_file.mlc`: `editor_ux_open_dropped_path` full-flattens `buffer.document` for `tab_set_open` then immediately replaces the buffer via `tab_set_update_active_buffer`. Honest #1d residual after §86 |
| Strategy (v1) | Open with already-loaded `OpenBuffer` without flatten — add `tab_set_open_buffer(tabs, path, buffer)` **or** `tab_set_open(…, "")` + `tab_set_update_active_buffer`; drop `document_to_string` import/use from `ux/drop_file.mlc`. Out of scope: `frame_snapshot` single flatten; edit/save/clipboard stringifies; incremental lexer |
| Primary gate | Red: no green `run_ux_drop_file_no_stringify_stable.sh`. Green: `ux_ok drop_file_no_stringify` — no `document_to_string` in `ux/drop_file.mlc`; `drop_file_opens_tab` + `large_file_no_full_stringify` stay green; auto-discovered by `run_ux_gate` |
| Module touch | `ux/drop_file.mlc`; maybe `workspace/tabs.mlc`; scripts; small scenario |
| REG | no |
| Out of scope | frame_snapshot; edit/save stringify; incremental lexer; SCRIPT_VM; MIR Epic 5 |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Red: drop_file still stringifies | **done** |
| 2 | Open without stringify; green | **done** |
| 3 | Critic: stable + full `run_ux_gate` | pending |

## Out of scope

SCRIPT_VM; MIR Epic 5; LANG_AUTO_CYCLE; frame_snapshot single flatten; edit/save/clipboard stringifies; incremental/diff lexer.
