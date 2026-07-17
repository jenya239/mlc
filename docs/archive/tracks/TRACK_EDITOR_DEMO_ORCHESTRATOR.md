# Track: Editor demo orchestrator (`demo_live` → `ux/*`)

Parent: [../agent/TRACK_EDITOR_UX_BACKLOG.md](../agent/TRACK_EDITOR_UX_BACKLOG.md) §46 **#1**;
origin [TRACK_EDITOR_CLEAN_ARCHITECTURE.md](TRACK_EDITOR_CLEAN_ARCHITECTURE.md).

## Status: **closed** (2026-07-18) — Critic OK

**Critic 2026-07-18 (STEP=5):** Re-ran UX gates + demo_live compile.
Anti-false-done: `0121c330`…`8be56a7c` (STEP=0–4). Mutation clusters
(edit/tab/folder/scroll) call `ux/*`/`app/*` — no inline `edit_insert_*` /
duplicate tab-hit activate / folder resolve / wrap clamp. **reopen: none**.
Honest residual: `main` still ~900 lines (draw + command bus) — out of #1
scope; further thin-orchestrator work is separate backlog if needed.

| Gate | Result |
|------|--------|
| `run_ux_newline_caret.sh` | `ux_ok newline_caret_and_spaces` EXIT=0 |
| `run_ux_tab_strip_click.sh` | `ux_ok tab_strip_click_activates` EXIT=0 |
| `run_ux_tree_click.sh` | `ux_ok tree_click_opens_tab` EXIT=0 |
| `run_ux_wheel_scroll.sh` | `ux_ok wheel_scroll_keeps_caret_visible` EXIT=0 |
| `run_ux_copy_paste.sh` | `ux_ok copy_paste_roundtrip` EXIT=0 |
| `run_editor_demo_live_fs_compile.sh` | `demo_live_fs_compile_ok` EXIT=0 |

## Next step

**closed** — Critic OK. Queue → Planner (§46 `#1b EDITOR_LIVE_SOLARIZED_TEXT`).

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `0121c330` | Decision freeze |
| 1 | `8bd428b6` | edit → `editor_app_*` / `edit_apply` |
| 2 | `756c4a50` | tab strip → `editor_app_click_tab_strip` |
| 3 | `5e90d97d` | folder → `folder_browser_resolve_click` |
| 4 | `8be56a7c` | scroll clamps → `editor_app_clamp_scroll_to_content` |
| 5 | this Critic | close + archive |

## Delivered

Behavior-preserving wire of live chrome mutation paths onto tested
`ux/*`/`app/*` helpers (Standing discipline gates unchanged).

## Out of scope (unchanged)

#1b–#1d, keyboard nav, draw decomposition, `compiler/`.
