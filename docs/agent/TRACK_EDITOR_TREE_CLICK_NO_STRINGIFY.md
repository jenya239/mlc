# Track: Editor tree-click opens without stringify

Parent: [../PLAN.md](../PLAN.md) §91.
Residual of [TRACK_EDITOR_DEMO_OPEN_PATH_NO_STRINGIFY](../archive/tracks/TRACK_EDITOR_DEMO_OPEN_PATH_NO_STRINGIFY.md)
(§90) / #1d. Size **S**.

## Status: **open** — STEP=0 done; next Driver STEP=1

## Next step

**STEP=1** — Red: assert `editor_ux_click_tree` still takes `open_text: string` + `tab_set_open`; stable stub `not implemented`.

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | (this) | Decision: tree click via `tab_set_open_buffer` |

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
| 1 | Red: tree click still stringifies | pending |
| 2 | `tab_set_open_buffer` in click_tree; green | pending |
| 3 | Critic: stable + related + `run_ux_gate` | pending |

<!-- STEP=1 sub-steps: 1) red asserts open_text + tab_set_open in tree_hit; 2) stable stub; 3) docs -->
<!-- STEP=2 sub-steps: 1) signature → OpenBuffer + tab_set_open_buffer; 2) update scenario; 3) green stable -->

## Out of scope

SCRIPT_VM; MIR Epic 5; LANG_AUTO_CYCLE; frame_snapshot; edit/clipboard; incremental/diff lexer.
