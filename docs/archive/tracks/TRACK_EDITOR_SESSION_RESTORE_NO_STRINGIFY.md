# Track: Editor session restore opens without stringify

Parent: [../../PLAN.md](../../PLAN.md) §88.
Residual of [TRACK_EDITOR_DROP_FILE_NO_STRINGIFY](TRACK_EDITOR_DROP_FILE_NO_STRINGIFY.md)
(§87) / #1d. Size **S**.

## Status: **closed** (2026-07-25) — Critic OK

**Critic 2026-07-25 (STEP=3):** Re-ran `session_restore_no_stringify_stable` ×2 +
`session_unit` + `session_restore_caret` + `demo_live` compile + HEAD `run_ux_gate`.
Anti-false-done: `9013de25`…`4f95fc34` (STEP=0–2); `tab_set_open_buffer` in
`workspace_session_restore_tabs` (no `document_to_string` in `session.mlc`);
`misc/editor/**` + scripts → REG skip; no `compiler/`/`lib/mlc/`.
**reopen: none**.

Honest residual: `*_red` post-green fails (use stable only); `frame_snapshot` /
edit/save/clipboard/demo_live dump/tree_hit stringifies + incremental lexer still OOS.

| Gate | Result |
|------|--------|
| `run_ux_session_restore_no_stringify_stable.sh` | `ux_ok session_restore_no_stringify` EXIT=0 (×2) |
| `run_editor_session_unit.sh` | `session_unit ok` EXIT=0 |
| `run_ux_session_restore_caret.sh` | `ux_ok session_restore_caret` EXIT=0 |
| `run_editor_demo_live_fs_compile.sh` | `demo_live_fs_compile_ok` EXIT=0 |
| HEAD `run_ux_gate.sh` (103 scenarios) | `[ux gate] all ok` EXIT=0 |
| REG / self-host | N/A (editor + scripts) |

## Next step

**closed** — Critic OK. Authorized queue empty → Planner.

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `9013de25` | Decision: restore tabs via `tab_set_open_buffer` |
| 1 | `66b06761` | Red harness + stable stub `not implemented` |
| 2 | `4f95fc34` | `tab_set_open_buffer` in restore; green |
| 3 | (this) | Critic OK; archive |

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
| 3 | Critic: stable + full `run_ux_gate` | **done** |

## Out of scope

SCRIPT_VM; MIR Epic 5; LANG_AUTO_CYCLE; frame_snapshot single flatten; edit/save/clipboard; demo_live dump; tree_hit open_text; incremental/diff lexer.
