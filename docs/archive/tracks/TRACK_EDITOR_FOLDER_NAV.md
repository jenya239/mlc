# Track: Editor folder nav history (back/forward)

Parent: [../../PLAN.md](../../PLAN.md) §43; polish after
[TRACK_EDITOR_FOLDER_BROWSER.md](TRACK_EDITOR_FOLDER_BROWSER.md).

## Status: **closed** (2026-07-18) — Critic OK

**Critic 2026-07-18 (STEP=3):** Re-ran unit + demo_live compile.
No `lib/mlc/` → REG skipped (Decision). Anti-false-done:
`d082a24e`…`b0a7afbf` (STEP=0–2). Exports match Decision. **reopen: none**.
L1 scenario backfill → §46 `#30 EDITOR_UX_L1_BACKFILL` (not this track).

| Gate | Result |
|------|--------|
| `run_editor_folder_nav_unit.sh` | `folder_nav_unit ok` EXIT=0 |
| `run_editor_demo_live_fs_compile.sh` | `demo_live_fs_compile_ok` EXIT=0 |

## Next step

**closed** — Critic OK. Queue → Planner (§45 STEP=4 Critic).

### STEP=3 done (2026-07-18)

- Critic gates; archive.

### STEPs 0–2 done in git (2026-07-16)

| Step | Commit (abbrev) | Gate |
|------|-----------------|------|
| 0 | `d082a24e` | Decision frozen |
| 1 | `87284d8c` | `folder_nav_unit ok` |
| 2 | `b0a7afbf` | `demo_live_fs_compile_ok` |

## Decision (STEP=0) — **frozen** 2026-07-16

| Item | Choice |
|------|--------|
| Module | `misc/editor/ux/folder_panel.mlc` (no new file) |
| History | `FolderNavHistory { paths: [string], index: i32 }` |
| Semantics | `push` truncates forward; duplicate current no-op; `can_*` → i32 0/1 |
| Hit | `folder_nav_button_hit` → 0 none / 1 back / 2 forward |
| Listing | `folder_tree_browser_rows` |
| Wire | `demo_live` only |
| REG | no `lib/mlc/`; no `compiler/` |

### Exact exports (delivered)

```text
type FolderNavHistory = { paths: [string], index: i32 }
folder_tree_browser_rows / folder_nav_new / folder_nav_current /
folder_nav_can_back / folder_nav_can_forward / folder_nav_push /
folder_nav_back / folder_nav_forward / folder_nav_button_size /
folder_nav_controls_width / folder_nav_back_rect / folder_nav_forward_rect /
folder_nav_crumbs_origin_x / folder_nav_button_hit
```

## Out of scope (unchanged)

File watch; recursive glob; tree-sitter; SCRIPT_VM; LANG_AUTO_CYCLE; MIR Epic 5;
`compiler/` / `lib/mlc/`.
