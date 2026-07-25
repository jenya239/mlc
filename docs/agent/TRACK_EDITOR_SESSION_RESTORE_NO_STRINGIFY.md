# Track: Editor session restore opens without stringify

Parent: [../PLAN.md](../PLAN.md) §88.
Residual of [TRACK_EDITOR_DROP_FILE_NO_STRINGIFY](../archive/tracks/TRACK_EDITOR_DROP_FILE_NO_STRINGIFY.md)
(§87) / #1d. Size **S**.

## Status: **open** — STEP=2 done; next Critic STEP=3

## Next step

**STEP=3** — Critic: stable×2 + related + full `run_ux_gate`.

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `9013de25` | Decision: restore tabs via `tab_set_open_buffer` |
| 1 | `66b06761` | Red: ≥1 `document_to_string` in `session.mlc`; stable stub |
| 2 | (this) | `tab_set_open_buffer` in restore; green stable |

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
| 1 | Red: session restore still stringifies | **done** |
| 2 | `tab_set_open_buffer` in restore; green | **done** |
| 3 | Critic: stable + full `run_ux_gate` | pending |

## Out of scope

SCRIPT_VM; MIR Epic 5; LANG_AUTO_CYCLE; frame_snapshot single flatten; edit/save/clipboard; demo_live dump; tree_hit open_text; incremental/diff lexer.
