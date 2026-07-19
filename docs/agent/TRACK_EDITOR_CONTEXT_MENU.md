# Track: Editor Context Menu

Parent: [TRACK_EDITOR_UX_BACKLOG.md](TRACK_EDITOR_UX_BACKLOG.md) §46 **#25**.
No right-click menu; Cut/Copy/Paste only via shortcuts/toolbar. Review
gate: `context_menu_opens` (L1). Size **M**.

## Status: **active** (2026-07-19) — STEP=0 done; next STEP=1

## Next step

**STEP=1** — L1 scenario first (`context_menu_opens`) red against stub.

## Decision (STEP=0) — **frozen** 2026-07-19

| Item | Choice |
|------|--------|
| Problem | No context menu; right-click does nothing; Cut/Copy/Paste only via Cmd/toolbar |
| Model | `EditorContextMenu { visible: i32, origin_x: i32, origin_y: i32 }` — hidden when `visible==0` |
| Items | Fixed three rows: **Cut**, **Copy**, **Paste** (order top→bottom). No Undo/Select All this track |
| Open | `editor_ux_context_menu_open_at(menu, x, y) -> EditorContextMenu` — sets visible=1 at point |
| Dismiss | Esc / left-click outside menu / after item action → `editor_ux_context_menu_dismiss(menu)` |
| Hit | `editor_ux_context_menu_item_rect(menu, item_index)` + `editor_ux_context_menu_click(menu, mouse_x, mouse_y) -> ContextMenuClickResult` — miss outside → dismiss; hit item → action + dismiss |
| Actions | `ContextMenuAction = None \| Cut \| Copy \| Paste` — demo_live maps Cut/Copy/Paste to existing `edit_cut` / `edit_copy` / `edit_paste` paths (same as Cmd*) |
| Enabled | Cut/Copy when selection non-empty; Paste always enabled. Disabled item click = dismiss, no edit (L1 may assert enabled Copy path only) |
| Geometry | Menu panel at origin; item height 24; width 120; draw solid panel + labels best-effort |
| Input | Add `glfw_gl_mouse_right_down` (runtime + `gl_window.mlc`) and `GuiInput.mouse_right_down`; demo_live: right-click edge in text rect → open at cursor; left-click routed through menu click helper when visible |
| Live | demo_live holds `let mut context_menu`; draw when visible; item click dispatches edit |
| Scenarios | L1: `context_menu_opens` — open_at → visible; click Copy item center → action=Copy + hidden; token `ux_ok context_menu_opens` |
| Standing discipline | Scenario-first; name tokens in SESSION `done` |
| Module touch | `misc/editor/ux/` (+ scenario/script + thin demo_live); right-button: `runtime/` + `lib/mlc/.../gl_window.mlc` + `misc/gui/input.mlc` |
| REG | if `lib/mlc/` touched → `scripts/regression_gate.sh` before Critic close |

### Gates

| Step | Token |
|------|-------|
| 1–3 | `ux_ok context_menu_opens` (+ run script) |
| every | `bash scripts/run_editor_demo_live_fs_compile.sh` → `demo_live_fs_compile_ok` |
| regression | `bash scripts/run_ux_trailing_ws_visualized.sh` → `ux_ok trailing_ws_visualized` |
| Critic | if `lib/mlc/` changed: `scripts/regression_gate.sh` |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open track / PLAN / backlog | **done** (2026-07-19) |
| 1 | L1 scenario first (`context_menu_opens`) | pending |
| <!-- sub-steps: 1) scenario + run script; 2) stub menu API → red; 3) demo_live_fs_compile + trailing_ws green --> |
| 2 | menu model + open/click/dismiss + item rects | pending |
| <!-- sub-steps: 1) EditorContextMenu + click helper; 2) L1 green for open+Copy; 3) trailing_ws regression --> |
| 3 | right-button ABI + GuiInput + demo_live wire | pending |
| <!-- sub-steps: 1) glfw_gl_mouse_right_down; 2) demo_live open/draw/dispatch; 3) gates + REG if lib/mlc --> |
| 4 | Critic: gates; archive | pending |

## Out of scope

Native OS menu; submenu; Undo/Select All/Find entries; keyboard nav in menu; tree/tab-strip context menus; spellcheck; `compiler/` (except via existing edit cmds).

## Verify discipline

- One STEP per turn; commit + push.
- Leave foreign `compiler/out/**` alone.
- Standing discipline: name scenario tokens in SESSION `done`.
