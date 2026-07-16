# Track: Editor folder nav history (back/forward)

Parent: [../PLAN.md](../PLAN.md) §43; polish after
[archive/tracks/TRACK_EDITOR_FOLDER_BROWSER.md](../archive/tracks/TRACK_EDITOR_FOLDER_BROWSER.md).
Uncommitted WIP in `misc/editor/ux/folder_panel.mlc` (`FolderNavHistory`,
`folder_nav_*`, `folder_tree_browser_rows`) — absorb, do not discard.

## Status: **active** (2026-07-16) — queue head

After §42 word-wrap closed. Next editor gap: directory browse history
(back/forward + current) for tree/breadcrumb navigation.

## Next step

**STEP=0** — Decision freeze: history API, UI controls, absorb WIP scope.

## Decision (STEP=0) — open

| Item | Choice (draft → freeze at STEP=0) |
|------|----------------------------------|
| Scope v1 | `FolderNavHistory` push/back/forward/can_* + current path |
| Module | Absorb into `misc/editor/ux/folder_panel.mlc` (or sibling `folder_nav.mlc`) |
| UI | Back/forward hit targets in tree chrome (WIP rects) |
| Wire | `demo_live` uses history on browse/breadcrumb/parent |
| Unit | Pure history + hit helpers; no GLFW |
| REG | Prefer no `lib/mlc/`; no `compiler/` |
| Non-goals | Recursive glob; file watch; tree-sitter; SCRIPT_VM; MIR Epic 5 |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + PLAN/CONTINUITY | Decision table frozen |
| 1 | Absorb nav module + unit | unit token |
| 2 | Wire `demo_live` (+ compile) | `demo_live_fs_compile_ok` |
| 3 | Critic: gates (+ REG if `lib/mlc/`); archive | close |

### Sub-steps (Driver)

**STEP=0**
1. Freeze exact exports from WIP audit; file placement.
2. List in-scope WIP files (exclude unrelated `runtime/io` unless needed).

**STEP=1**
1. Land/absorb `folder_nav_*` + unit script.
2. Gate: `scripts/run_editor_folder_nav_unit.sh` (name TBD at STEP=0).

**STEP=2**
1. Wire `demo_live` to history (explicit `git add`; stash unrelated foreign).
2. `run_editor_demo_live_fs_compile.sh`.

**STEP=3** — Critic; `next` = Planner.

## Out of scope

- File watch / recursive search
- tree-sitter / SCRIPT_VM / LANG_AUTO_CYCLE / MIR Epic 5
- `compiler/` changes
- Unrelated foreign WIP (`runtime/io`, static_text API) unless required for compile

## Verify discipline

- One STEP per turn; commit + push.
- Leave `compiler/out/extern_concurrency_lint.*` alone.
- Do not `git add` foreign files outside TRACK scope.
