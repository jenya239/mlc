# Track: Editor Tree Expand / Collapse

Parent: [TRACK_EDITOR_UX_BACKLOG.md](TRACK_EDITOR_UX_BACKLOG.md) §46 **#33**.
User directive 2026-07-19 — **revises** §38 flat breadcrumb-only listing
([archive/tracks/TRACK_EDITOR_FOLDER_BROWSER.md](../archive/tracks/TRACK_EDITOR_FOLDER_BROWSER.md)).
Expandable tree model already exists unused. Size **M**.

## Status: **active** (2026-07-19) — STEP=0 done; next STEP=1

## Next step

**STEP=1** — Red L1 harness `tree_row_click_toggles_expand` (+ `run_ux_*.sh`).

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | *(this turn)* | Decision freeze + open |

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
| 1 | L1 red harness + `run_ux_tree_row_click_toggles_expand.sh` | pending — red / stub fail |
| 2 | Click resolve toggle + tree rows helpers; green token | pending — `ux_ok tree_row_click_toggles_expand` |
| 3 | `demo_live` wire listing+toggle; keep `tree_click_opens_tab`; compile | pending — `demo_live_fs_compile_ok` + `ux_ok tree_click_opens_tab` |
| 4 | Critic: gates; archive | pending |

<!-- STEP=1 sub-steps: 1) stub scenario fail token; 2) run script + optional red harness -->
<!-- STEP=2 sub-steps: 1) folder_browser_resolve_click: arm→toggle_expand; 2) expand/collapse apply helper; 3) green L1 on folder_tree_browser_rows -->
<!-- STEP=3 sub-steps: 1) demo_live use folder_tree_browser_rows; 2) apply toggle on panels.tree; 3) indent/▸▾ draw; 4) regression opens_tab + compile -->

## Out of scope

FS watch; multi-root workspaces; `compiler/`; `lib/mlc/`; syntax highlight (#34).
