# Track: Editor demo open-path without stringify

Parent: [../PLAN.md](../PLAN.md) §90.
Residual of [TRACK_EDITOR_DEMO_SAVE_NO_DOUBLE_STRINGIFY](../archive/tracks/TRACK_EDITOR_DEMO_SAVE_NO_DOUBLE_STRINGIFY.md)
(§89) / #1d. Size **S**.

## Status: **open** — STEP=1 done; next Driver STEP=2

## Next step

**STEP=2** — `tab_set_open_buffer` for disk; green stable.

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `a7a0c9ac` | Decision: disk opens via `tab_set_open_buffer` |
| 1 | (this) | Red: ≥2 `tab_set_open(…, text_for_path(…))`; stable stub |

## Decision (STEP=0) — **frozen** 2026-07-25

| Item | Choice |
|------|--------|
| Problem | After §89, `demo_live.mlc` still opens disk files via `tab_set_open(tabs, path, text_for_path(path))` in `try_open_path` and folder `tree_open` — full `file_read_to_string` then rebuild buffer. Same wasteful pattern as pre-§87/§88 drop_file/session. Honest #1d residual after §89 |
| Strategy (v1) | For disk paths: `open_buffer_from_path` + `tab_set_open_buffer` (skip on error). Keep `text_for_path` + `tab_set_open` only for virtual `/demo/` sample paths. Out of scope: `editor_ux_click_tree` `open_text` API; frame_snapshot; edit/clipboard; incremental lexer |
| Primary gate | Red: no green `run_ux_demo_open_path_no_stringify_stable.sh`. Green: `ux_ok demo_open_path_no_stringify` — disk open sites use `tab_set_open_buffer`; `demo_live` compile stays green; related tree/drop gates stay green |
| Module touch | `demo_live.mlc`; scripts |
| REG | no |
| Out of scope | tree_hit open_text API; frame_snapshot; edit/clipboard; incremental lexer; SCRIPT_VM; MIR Epic 5 |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Red: disk opens still stringify | **done** |
| 2 | `tab_set_open_buffer` for disk; green | pending |
| 3 | Critic: stable + demo_live + `run_ux_gate` | pending |

<!-- STEP=2 sub-steps: 1) try_open_path + tree_open disk path; 2) green stable; 3) demo compile -->

## Out of scope

SCRIPT_VM; MIR Epic 5; LANG_AUTO_CYCLE; tree_hit open_text API; frame_snapshot; edit/clipboard; incremental/diff lexer.
