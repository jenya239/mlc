# Track: Editor folder browser (breadcrumbs + listing)

Parent: [../PLAN.md](../PLAN.md) §38; residual polish after
[archive/tracks/TRACK_STDLIB_IO_FS.md](../archive/tracks/TRACK_STDLIB_IO_FS.md)
(disk tree) + [archive/tracks/TRACK_EDITOR_LIVE_POLISH.md](../archive/tracks/TRACK_EDITOR_LIVE_POLISH.md).

## Status: **active** (2026-07-16) — queue head

## Next step

**STEP=3** — Wire `demo_live` folder browser + disk/mock.

### STEP=2 done (2026-07-16)

- Landed `tab_strip` close helpers (`close_width` / `close_rect` / `hit_close_index`).
- Gate: `bash scripts/run_editor_tab_strip_close_unit.sh` → `tab_strip_close_unit ok`.

### STEP=1 done (2026-07-16)

- Landed `folder_panel.mlc` (fix: rename `end` → `end_index`, keyword truncate).
- Gate: `bash scripts/run_editor_folder_panel_unit.sh` → `folder_panel_unit ok`.

### STEP=0 done (2026-07-16)

- Decision frozen below; PLAN §38 → STEP=1.
- Absorb paths listed; WIP stays uncommitted until STEPs 1–4.

## Decision (STEP=0) — **frozen** 2026-07-16

Grounded in existing §36 disk APIs (`dir_entries_from_disk`, `list_dir`) and
foreign WIP already in the working tree.

| Item | Choice |
|------|--------|
| UX model | Flat listing + breadcrumbs (not expand/collapse tree chrome) |
| Module | `misc/editor/ux/folder_panel.mlc` — rows, breadcrumb hit, scroll |
| Disk | Reuse `dir_entries_from_disk` / §36 `list_dir`; mock `/demo` + `/demo/…` → no disk |
| Tabs | Close affordance: `tab_strip_close_width`, `tab_strip_close_rect`, `tab_strip_hit_close_index` |
| Live root | `MLC_EDITOR_ROOT` default repo root; `MLC_EDITOR_MOCK=1` unsets root → mock |
| REG | Prefer **no** `lib/mlc/`; if touched → `scripts/regression_gate.sh` before Critic |
| Namespace | Editor-only under `misc/editor/`; **no** `compiler/` |

### Absorb paths (exact)

| Path | STEP |
|------|------|
| `misc/editor/ux/folder_panel.mlc` (untracked) | 1 |
| `misc/editor/ux/tab_strip.mlc` (close hit) | 2 |
| `misc/editor/demo_live.mlc` (folder wire) | 3 |
| `scripts/run_editor_live_demo.sh` (`MLC_EDITOR_ROOT` / mock) | 4 |

Leave alone: `compiler/out/extern_concurrency_lint.*`.

### Key exports (from WIP — land as-is unless compile forces trim)

```text
# folder_panel.mlc
folder_browser_at / folder_current_path / folder_listing_rows
folder_breadcrumb_segments / folder_breadcrumb_hit
folder_list_rect / folder_clamp_scroll / folder_hit_row_index / folder_scrollbar_thumb
path_leaf_name / path_parent
folder_row_parent | folder_row_directory | folder_row_file

# tab_strip.mlc
tab_strip_close_width / tab_strip_close_rect / tab_strip_hit_close_index
```

### Non-goals (Decision)

tree-sitter; Fontconfig; full HiDPI layout rewrite; SCRIPT_VM; LANG_AUTO_CYCLE;
MIR Epic 5; `compiler/` changes; recursive glob / file watch.

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + PLAN/CONTINUITY | **done** (2026-07-16) |
| 1 | Land `folder_panel.mlc` (+ minimal unit or compile import) | **done** (2026-07-16) `folder_panel_unit ok` |
| 2 | Land `tab_strip` close hit helpers | compile/unit token |
| 3 | Wire `demo_live` folder browser + disk/mock | `demo_live_fs_compile_ok` |
| 4 | `run_editor_live_demo.sh` root/mock env | script dry-run / comment gate |
| 5 | Critic: gates (+ REG if `lib/mlc/`); archive | close |

### Sub-steps (Driver)

**STEP=0** — **done**
1. Freeze Decision; list absorb paths — done.
2. Non-goals; no `compiler/` — done.

**STEP=1** — **done**
1. Commit `folder_panel.mlc` — done (`end`→`end_index`).
2. Gate: `run_editor_folder_panel_unit.sh` — done.

**STEP=2**
1. Commit tab close rect/hit exports.
2. Gate: compile or unit.

**STEP=3**
1. Absorb `demo_live.mlc` folder wire; keep mock `/demo` path.
2. `bash scripts/run_editor_demo_live_fs_compile.sh` → `demo_live_fs_compile_ok`.

**STEP=4**
1. Absorb live demo script env defaults.
2. Verify script exports root when not mock (no need to open GLFW in CI).

**STEP=5** — Critic; `next` = Planner.

## Out of scope

- Syntax highlight / tree-sitter
- Full HiDPI layout redesign (content_scale already §37)
- SCRIPT_VM / LANG_AUTO_CYCLE / MIR Epic 5
- `compiler/` changes

## Verify discipline

- One STEP per turn; commit + push.
- Absorb paths above are **in scope** — absorb, do not discard.
- Leave `compiler/out/extern_concurrency_lint.*` alone (unrelated).
