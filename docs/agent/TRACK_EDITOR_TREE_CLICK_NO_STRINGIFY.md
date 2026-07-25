# Track: Editor tree-click opens without stringify

Parent: [../PLAN.md](../PLAN.md) §91.
Residual of [TRACK_EDITOR_DEMO_OPEN_PATH_NO_STRINGIFY](../archive/tracks/TRACK_EDITOR_DEMO_OPEN_PATH_NO_STRINGIFY.md)
(§90) / #1d. Size **S**.

## Status: **open** — STEP=2 done; next Critic STEP=3

## Next step

**STEP=3** — Critic: stable + related + `run_ux_gate`.

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `3c4cbf99` | Decision: tree click via `tab_set_open_buffer` |
| 1 | `53314c77` | Red: `open_text` + `tab_set_open`; stable stub |
| 2 | (this) | `OpenBuffer` + `tab_set_open_buffer`; green stable |

## Decision (STEP=0) — **frozen** 2026-07-25

| Item | Choice |
|------|--------|
| Problem | After §90, `editor_ux_click_tree` still takes `open_text: string` and calls `tab_set_open(state.tabs, node.path, open_text)` — same stringify open path as pre-§87 drop/session. Live `demo_live` folder chrome already uses `open_path_into_tabs`; this API remains for L4 `tree_click_opens_tab` and any future UX caller. Honest #1d residual after §90 |
| Strategy (v1) | Change signature to `OpenBuffer` (or equivalent) + `tab_set_open_buffer`. Scenario builds buffer via `open_buffer_from_text`. Keep `tab_set_open` for unrelated test fixtures. Out of scope: frame_snapshot; edit/clipboard; incremental lexer; demo_live folder path (already §90) |
| Primary gate | Red: no green `run_ux_tree_click_no_stringify_stable.sh`. Green: `ux_ok tree_click_no_stringify` — `editor_ux_click_tree` uses `tab_set_open_buffer`; `tree_click_opens_tab` stays green; demo compile stays green |
| Module touch | `ux/tree_hit.mlc`; `ux_scenarios/tree_click_opens_tab.mlc`; scripts |
| REG | no |
| Out of scope | frame_snapshot; edit/clipboard; incremental lexer; SCRIPT_VM; MIR Epic 5 |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Red: tree click still stringifies | **done** |
| 2 | `tab_set_open_buffer` in click_tree; green | **done** |
| 3 | Critic: stable + related + `run_ux_gate` | pending |

<!-- STEP=2: signature OpenBuffer + tab_set_open_buffer; scenario open_buffer_from_text -->

## Out of scope

SCRIPT_VM; MIR Epic 5; LANG_AUTO_CYCLE; frame_snapshot; edit/clipboard; incremental/diff lexer.
