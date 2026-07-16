# Track: Editor folder browser (breadcrumbs + listing)

Parent: [../PLAN.md](../PLAN.md) §38; residual polish after
[archive/tracks/TRACK_STDLIB_IO_FS.md](../archive/tracks/TRACK_STDLIB_IO_FS.md)
(disk tree) + [archive/tracks/TRACK_EDITOR_LIVE_POLISH.md](../archive/tracks/TRACK_EDITOR_LIVE_POLISH.md).

## Status: **active** (2026-07-16) — queue head

Foreign WIP already in tree (do not delete): `misc/editor/ux/folder_panel.mlc`,
`demo_live.mlc` folder wire, `tab_strip` close hit, `run_editor_live_demo.sh`
`MLC_EDITOR_ROOT`. Absorb into numbered STEPs; no invent-from-scratch.

## Next step

**STEP=0** — Decision freeze: model + gates + absorb list.

## Decision (STEP=0) — open

| Item | Choice (draft → freeze at STEP=0) |
|------|----------------------------------|
| UX model | Flat listing + breadcrumbs (not expand/collapse tree chrome) |
| Module | `misc/editor/ux/folder_panel.mlc` — rows, breadcrumb hit, scroll |
| Disk | Reuse §36 `dir_entries_from_disk` / `list_dir`; mock `/demo` fallback |
| Tabs | Close affordance hit via `tab_strip_close_rect` / `tab_strip_hit_close_index` |
| Live root | `MLC_EDITOR_ROOT` default repo; `MLC_EDITOR_MOCK=1` → mock |
| REG | Prefer no `lib/mlc/`; if touched → REG before Critic |
| Non-goals | tree-sitter; Fontconfig; SCRIPT_VM; MIR Epic 5; `compiler/` |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + PLAN/CONTINUITY | Decision table frozen |
| 1 | Land `folder_panel.mlc` (+ minimal unit or compile import) | compile/unit token |
| 2 | Land `tab_strip` close hit helpers | compile/unit token |
| 3 | Wire `demo_live` folder browser + disk/mock | `demo_live_fs_compile_ok` |
| 4 | `run_editor_live_demo.sh` root/mock env | script dry-run / comment gate |
| 5 | Critic: gates (+ REG if `lib/mlc/`); archive | close |

### Sub-steps (Driver)

**STEP=0**
1. Freeze Decision table; list exact absorb paths.
2. Confirm non-goals; no `compiler/`.

**STEP=1**
1. Commit `folder_panel.mlc` (or slice if too large).
2. Gate: compile import or tiny unit printing a token.

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
- Foreign dirty listed above is **in scope** — absorb, do not discard.
- Leave `compiler/out/extern_concurrency_lint.*` alone (unrelated).
