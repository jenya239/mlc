# Track: Editor folder nav history (back/forward)

Parent: [../PLAN.md](../PLAN.md) §43; polish after
[archive/tracks/TRACK_EDITOR_FOLDER_BROWSER.md](../archive/tracks/TRACK_EDITOR_FOLDER_BROWSER.md).
Uncommitted WIP in `misc/editor/ux/folder_panel.mlc` (`FolderNavHistory`,
`folder_nav_*`, `folder_tree_browser_rows`) — absorb, do not discard.

## Status: **active** (2026-07-16) — queue head

## Next step

**STEP=1** — Absorb nav API + unit.

### STEP=0 done (2026-07-16)

- Decision frozen below; PLAN §43 → STEP=1.

## Decision (STEP=0) — **frozen** 2026-07-16

Grounded in uncommitted `misc/editor/ux/folder_panel.mlc` nav block and
§38 folder browser (breadcrumbs/listing already shipped).

| Item | Choice |
|------|--------|
| Module | Keep in `misc/editor/ux/folder_panel.mlc` (no new file) |
| History | `FolderNavHistory { paths: [string], index: i32 }` |
| Semantics | `push` truncates forward stack; duplicate current path is no-op; `can_*` → `i32` 0/1 |
| Hit | `folder_nav_button_hit` → 0 none / 1 back / 2 forward |
| Listing helper | Absorb `folder_tree_browser_rows` (`..` + expanded children of browse root) |
| UI metrics | `folder_nav_button_size` / `*_rect` / `folder_nav_crumbs_origin_x` |
| Wire (STEP=2) | `demo_live` only; do **not** commit unrelated `runtime/io`, `static_text`, chrome unless required for compile |
| Unit (STEP=1) | Pure: new → push → back/forward → can_* ; hit on rects; no GLFW |
| REG | Prefer **no** `lib/mlc/`; no `compiler/` |

### Exact exports (frozen from WIP)

```text
type FolderNavHistory = { paths: [string], index: i32 }

folder_tree_browser_rows(tree: FileTree) -> [FolderRow]

folder_nav_new(path: string) -> FolderNavHistory
folder_nav_current(history: FolderNavHistory) -> string
folder_nav_can_back(history: FolderNavHistory) -> i32
folder_nav_can_forward(history: FolderNavHistory) -> i32
folder_nav_push(history: FolderNavHistory, path: string) -> FolderNavHistory
folder_nav_back(history: FolderNavHistory) -> FolderNavHistory
folder_nav_forward(history: FolderNavHistory) -> FolderNavHistory

folder_nav_button_size() -> i32
folder_nav_controls_width() -> i32
folder_nav_back_rect(tree_rect: Rect) -> Rect
folder_nav_forward_rect(tree_rect: Rect) -> Rect
folder_nav_crumbs_origin_x(tree_rect: Rect) -> i32
folder_nav_button_hit(tree_rect: Rect, point_x: i32, point_y: i32) -> i32
```

### Gate names

| Step | Script / token |
|------|----------------|
| 1 | `scripts/run_editor_folder_nav_unit.sh` → `folder_nav_unit ok` |
| 2 | `scripts/run_editor_demo_live_fs_compile.sh` → `demo_live_fs_compile_ok` |

### In-scope WIP files (STEP=1–2)

- `misc/editor/ux/folder_panel.mlc` (nav + `folder_tree_browser_rows`; STEP=1)
- `misc/editor/tests/folder_nav_unit.mlc` (new) and/or extend `folder_panel_unit.mlc`
- `misc/editor/demo_live.mlc` (STEP=2 wire only)

### Non-goals (Decision)

File watch; recursive glob; tree-sitter; SCRIPT_VM; LANG_AUTO_CYCLE; MIR Epic 5;
`compiler/` / `lib/mlc/`; committing foreign `runtime/include/mlc/io/**` /
`static_text` / chrome without need.

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + PLAN/CONTINUITY | **done** (2026-07-16) |
| 1 | Absorb nav API + unit | `folder_nav_unit ok` |
| 2 | Wire `demo_live` (+ compile) | `demo_live_fs_compile_ok` |
| 3 | Critic: gates (+ REG if `lib/mlc/`); archive | close |

### Sub-steps (Driver)

**STEP=0** — **done**
1. Freeze exports + placement — done.
2. In-scope WIP list — done.

**STEP=1**
1. Commit nav portion of `folder_panel.mlc` + unit.
2. Gate: `bash scripts/run_editor_folder_nav_unit.sh`.

**STEP=2**
1. Wire `demo_live` history/back/forward (stash unrelated foreign).
2. `bash scripts/run_editor_demo_live_fs_compile.sh`.

**STEP=3** — Critic; `next` = Planner.

## Out of scope

- File watch / recursive search
- tree-sitter / SCRIPT_VM / LANG_AUTO_CYCLE / MIR Epic 5
- `compiler/` changes
- Unrelated foreign WIP unless required for compile

## Verify discipline

- One STEP per turn; commit + push.
- Leave `compiler/out/extern_concurrency_lint.*` alone.
- Do not `git add` foreign files outside TRACK scope.
