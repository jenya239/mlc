# Track: Editor session restore opens without stringify

Parent: [../PLAN.md](../PLAN.md) §88.
Residual of [TRACK_EDITOR_DROP_FILE_NO_STRINGIFY](../archive/tracks/TRACK_EDITOR_DROP_FILE_NO_STRINGIFY.md)
(§87) / #1d. Size **S**.

## Status: **open** — STEP=0 done; next Driver STEP=1

## Next step

**STEP=1** — Red: assert `workspace_session_restore_tabs` still calls `document_to_string`; stable stub `not implemented`.

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | (this) | Decision: restore tabs via `tab_set_open_buffer` |

## Decision (STEP=0) — **frozen** 2026-07-25

| Item | Choice |
|------|--------|
| Problem | After §87, production `ux/**` has zero `document_to_string`, but `workspace_session_restore_tabs` (`workspace/session.mlc`) still does `open_buffer_from_path` then `tab_set_open(…, document_to_string(buffer.document))` — same wasteful flatten+rebuild as pre-§87 drop_file. Honest #1d residual after §87 |
| Strategy (v1) | Use `tab_set_open_buffer(tabs, path, buffer)` in `workspace_session_restore_tabs`; drop `document_to_string` import/use from that path in `session.mlc`. Out of scope: `frame_snapshot` single flatten; edit/save/clipboard; demo_live dump; tree_hit `open_text`; incremental lexer |
| Primary gate | Red: no green `run_ux_session_restore_no_stringify_stable.sh`. Green: `ux_ok session_restore_no_stringify` — restore path uses `tab_set_open_buffer` (no `document_to_string` in restore); `session_unit` / `session_restore_caret` stay green; auto-discovered by `run_ux_gate` |
| Module touch | `workspace/session.mlc`; scripts; small scenario |
| REG | no |
| Out of scope | frame_snapshot; edit/save/clipboard; demo_live dump; tree_hit; incremental lexer; SCRIPT_VM; MIR Epic 5 |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Red: session restore still stringifies | pending |
| 2 | `tab_set_open_buffer` in restore; green | pending |
| 3 | Critic: stable + full `run_ux_gate` | pending |

<!-- STEP=1 sub-steps: 1) red asserts document_to_string in session restore; 2) stable stub; 3) docs -->
<!-- STEP=2 sub-steps: 1) tab_set_open_buffer in restore_tabs; 2) drop import if unused; 3) green stable -->

## Out of scope

SCRIPT_VM; MIR Epic 5; LANG_AUTO_CYCLE; frame_snapshot single flatten; edit/save/clipboard; demo_live dump; tree_hit open_text; incremental/diff lexer.
