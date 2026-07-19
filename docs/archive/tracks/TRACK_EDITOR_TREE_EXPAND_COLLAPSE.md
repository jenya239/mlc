# Track: Editor Tree Expand / Collapse

Parent: [TRACK_EDITOR_UX_BACKLOG.md](../../agent/TRACK_EDITOR_UX_BACKLOG.md) §46 **#33**.
User directive 2026-07-19 — **revises** §38 flat breadcrumb-only listing
([TRACK_EDITOR_FOLDER_BROWSER.md](TRACK_EDITOR_FOLDER_BROWSER.md)).
Expandable tree model already exists unused. Size **M**.

## Status: **closed** (2026-07-19) — Critic OK

**Critic 2026-07-19 (STEP=4):** Re-ran L1 + opens_tab + demo. Anti-false-done:
`144e20b9`…`a24edf9f` (STEP=0–3); no `compiler/` / `lib/mlc/` → REG skip.
Wire: `folder_click_toggle_expand` + `folder_tree_toggle_node`;
`demo_live` hit/draw → `folder_tree_browser_rows`; indent + ▸/▾;
dir single-click toggle, double-click enter_dir kept.
**reopen: none**.

Honest residual: `..` parent still single-click navigate (Decision: parent
unchanged) → backlog `#33d`; first click of a double still toggles once
(Decision: acceptable); flat `folder_listing_rows` remains for older L1
nav/breadcrumb scenarios only.

| Gate | Result |
|------|--------|
| `run_ux_tree_row_click_toggles_expand.sh` | `ux_ok tree_row_click_toggles_expand` EXIT=0 |
| `run_ux_tree_click.sh` | `ux_ok tree_click_opens_tab` EXIT=0 |
| `run_editor_demo_live_fs_compile.sh` | `demo_live_fs_compile_ok` EXIT=0 |

## Next step

**closed** — Critic OK. Queue → Planner (§46 `#33b EDITOR_ROW_BYTE_RANGE_BLEED`).

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `144e20b9` | Decision freeze + open |
| 1 | `1ecfddb3` | Red L1 stub + run scripts |
| 2 | `ea6596b9` | `folder_click_toggle_expand` + `folder_tree_toggle_node`; green L1 |
| 3 | `a24edf9f` | `demo_live` tree rows + toggle apply; indent/▸▾ |
| 4 | this Critic | close + archive |

## Decision (STEP=0) — **frozen** 2026-07-19

| Item | Choice |
|------|--------|
| Problem | Live tree uses `folder_listing_rows` (flat children of browse root, depth 0). Dir single-click is silent `folder_click_arm_dir` (arm only); enter requires 2nd click within 25 frames — feels unstable. Nested expand UI unused despite `file_tree_expand` / `file_tree_collapse` / `folder_tree_browser_rows` |
| Listing | Switch `demo_live` draw + hit rows to `folder_tree_browser_rows` (nested, `FolderRow.depth`). Indent draw by `depth *` fixed px (e.g. 10). Optional dir prefix `▸`/`▾` from `file_tree_is_expanded` — preferred for clarity |
| Single-click dir | **Toggle expand/collapse in place** immediately: if expanded → `file_tree_collapse`; else load children if needed (`file_tree_expand_from_disk` / existing browse expand) then expand. New click action `folder_click_toggle_expand` (or reuse arm code path with real tree mutation). Replaces silent arm-only feedback |
| Double-click dir | Keep: 2nd click same path within ≤25 frames → `folder_click_enter_dir` (re-root breadcrumbs / `browse_panels_to`). First click of a double still toggles once — acceptable; do not require disclosure-only hit target this track |
| File / parent / crumbs / nav | Unchanged: file → open tab; `..` / crumb / back-forward → browse |
| Apply | `demo_live`: on toggle action, update `panels.tree` (no path browse). On enter/nav/crumb/parent — existing browse. Draw uses tree rows after mutation same frame |
| Tokens | `ux_ok tree_row_click_toggles_expand` (L1: dir click → `expanded` flips 0↔1 and nested child appears/disappears in row list). Keep `ux_ok tree_click_opens_tab` green |
| Standing discipline | Scenario-first; name tokens in SESSION `done` |
| Module touch | `misc/editor/ux/folder_panel.mlc`, `demo_live`, possibly thin `file_tree` helpers; scenario/script. No `compiler/`. No `lib/mlc/` |
| REG | no |
| Out of scope | Drag-drop reorder; multi-select; sticky scroll; lazy FS watch; changing §43 breadcrumb chrome; `#34` syntax |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | L1 red harness + `run_ux_tree_row_click_toggles_expand.sh` | **done** — `ux_ok tree_expand_red` |
| 2 | Click resolve toggle + tree rows helpers; green token | **done** — `ux_ok tree_row_click_toggles_expand` |
| 3 | `demo_live` wire listing+toggle; keep `tree_click_opens_tab`; compile | **done** — `demo_live_fs_compile_ok` + `ux_ok tree_click_opens_tab` |
| 4 | Critic: gates; archive | **done** (closed) |

## Out of scope

FS watch; multi-root workspaces; `compiler/`; `lib/mlc/`; syntax highlight (#34).
