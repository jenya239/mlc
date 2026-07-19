# Track: Editor Tree Parent Double-Click

Parent: [TRACK_EDITOR_UX_BACKLOG.md](TRACK_EDITOR_UX_BACKLOG.md) §46 **#33d**
(pulled forward — live-demo audit 2026-07-19). Size **S**.
Revises `#33` Decision residual: parent `..` still single-click navigate.

## Status: **active** (2026-07-19) — STEP=0 done; next STEP=1

## Next step

**STEP=1** — L1 red harness + `run_ux_tree_parent_double_click.sh`.

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | *(pending hash)* | Decision freeze + open |

## Decision (STEP=0) — **frozen** 2026-07-19

| Item | Choice |
|------|--------|
| Problem | `folder_browser_resolve_click` (`folder_panel.mlc` ~617–622) fires `folder_click_parent()` + `folder_nav_push` on the **first** click of the `..` row. Sibling directories (post-`#33`) gate navigate on `is_double` (same path, ≤25 frames) and use single-click for toggle. Parent remains inconsistent with the frozen click contract |
| Single-click `..` | **Arm only** — no browse, no `folder_nav_push`. Return `handled=1`, action = reuse `folder_click_arm_dir()` (already exists; parent has no expand). Set `last_tree_click_path` / `last_tree_click_frame` to this row. `demo_live` already ignores unknown non-browse actions except toggle — arm falls through without `browse_panels_to` |
| Double-click `..` | Same gate as dir enter: path == last path && frame delta in `[0, 25]` → `folder_click_parent()` + `folder_nav_push` + scroll reset (current parent branch body). Clear/reset arm frame like enter_dir does (`0 - 1000`) so a third click does not immediately re-fire |
| Apply | `demo_live`: no new branch required if arm uses `folder_click_arm_dir` and parent keep existing `folder_click_parent` browse list. Confirm parent still in browse `if` |
| Token | `ux_ok tree_parent_double_click_navigates` — L1: listing with parent row; click 1 → action ≠ `folder_click_parent`, nav current unchanged; click 2 within window → action == `folder_click_parent`, nav current == parent path |
| Standing discipline | Scenario-first; name token in SESSION `done` |
| Module touch | `misc/editor/ux/folder_panel.mlc`; scenario + `scripts/run_ux_tree_parent_double_click.sh`; `demo_live` only if arm apply needs a no-op acknowledge. No `compiler/`. No `lib/mlc/` |
| REG | no |
| Out of scope | `#33e` content scrollbar; changing dir toggle/enter contract; breadcrumb/nav buttons; FS watch |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | L1 red harness + run scripts | pending — `ux_ok tree_parent_double_click_red` |
| 2 | Gate parent on `is_double`; green token | pending — `ux_ok tree_parent_double_click_navigates` |
| 3 | `demo_live` verify/wire if needed; `demo_live_fs_compile_ok` | pending |
| 4 | Critic: gates; archive | pending |

<!-- STEP=1: stub fail token; red script -->
<!-- STEP=2: parent branch mirrors dir is_double; green L1 -->
<!-- STEP=3: compile gate (+ demo only if apply gap) -->

## Out of scope

`#33e`; breadcrumb chrome; `compiler/`; `lib/mlc/`.
