# Track: Editor drop-file opens without stringify

Parent: [../../PLAN.md](../../PLAN.md) §87.
Residual of [TRACK_EDITOR_PROBE_REUSE_LINE_INDEX_TEXT](TRACK_EDITOR_PROBE_REUSE_LINE_INDEX_TEXT.md)
(§86) / #1d. Size **S**.

## Status: **closed** (2026-07-25) — Critic OK

**Critic 2026-07-25 (STEP=3):** Re-ran `drop_file_no_stringify_stable` ×2 +
`drop_file_opens_tab` + `large_file_no_full_stringify` + `demo_live` compile +
HEAD `run_ux_gate`.
Anti-false-done: `cca9e565`…`cd5a0a61` (STEP=0–2); `tab_set_open_buffer` in
`ux/drop_file.mlc` (no `document_to_string`); production `misc/editor/ux/**` has
zero `document_to_string`; `misc/editor/**` + scripts → REG skip; no `compiler/`/`lib/mlc/`.
**reopen: none**.

Honest residual: `*_red` post-green fails (use stable only); `frame_snapshot` /
edit/save/clipboard stringifies + incremental/diff lexer still OOS.

| Gate | Result |
|------|--------|
| `run_ux_drop_file_no_stringify_stable.sh` | `ux_ok drop_file_no_stringify` EXIT=0 (×2) |
| `run_ux_drop_file_opens_tab.sh` | `ux_ok drop_file_opens_tab` EXIT=0 |
| `run_ux_large_file_no_full_stringify.sh` | `ux_ok large_file_no_full_stringify` EXIT=0 |
| `run_editor_demo_live_fs_compile.sh` | `demo_live_fs_compile_ok` EXIT=0 |
| HEAD `run_ux_gate.sh` (102 scenarios) | `[ux gate] all ok` EXIT=0 |
| REG / self-host | N/A (editor + scripts) |

## Next step

**closed** — Critic OK. Authorized queue empty → Planner.

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `cca9e565` | Decision: open dropped path without `document_to_string` |
| 1 | `f41c931e` | Red harness + stable stub `not implemented` |
| 2 | `cd5a0a61` | `tab_set_open_buffer`; drop stringify; green |
| 3 | (this) | Critic OK; archive |

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
| 3 | Critic: stable + full `run_ux_gate` | **done** |

## Out of scope

SCRIPT_VM; MIR Epic 5; LANG_AUTO_CYCLE; frame_snapshot single flatten; edit/save/clipboard stringifies; incremental/diff lexer.
