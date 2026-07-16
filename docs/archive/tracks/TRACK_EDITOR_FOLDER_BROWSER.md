# Track: Editor folder browser (breadcrumbs + listing)

Parent: [../../PLAN.md](../../PLAN.md) §38; residual polish after
[TRACK_STDLIB_IO_FS.md](TRACK_STDLIB_IO_FS.md) (disk tree) +
[TRACK_EDITOR_LIVE_POLISH.md](TRACK_EDITOR_LIVE_POLISH.md).

## Status: **closed** (2026-07-16) — Critic OK

**Critic 2026-07-16 (STEP=5):** Re-ran track gates. No `lib/mlc/` → REG skipped
(Decision). Anti-false-done: `9dcb6bfb`…`d33a90eb` (STEP=0–4). **reopen: none**.

| Gate | Result |
|------|--------|
| `run_editor_folder_panel_unit.sh` | `folder_panel_unit ok` |
| `run_editor_tab_strip_close_unit.sh` | `tab_strip_close_unit ok` |
| `run_editor_demo_live_fs_compile.sh` | `demo_live_fs_compile_ok` |
| `run_editor_live_demo_env_gate.sh` | `editor_live_env_ok` |

## Next step

**closed** — Critic OK. Queue → Planner.

### STEP=5 done (2026-07-16)

- Critic gates; archive.

### STEPs 0–4 done in git (2026-07-16)

| Step | Commit (abbrev) | Gate |
|------|-----------------|------|
| 0 | `9dcb6bfb` | Decision frozen |
| 1 | `f455035d` | `folder_panel_unit ok` |
| 2 | `c2badf52` | `tab_strip_close_unit ok` |
| 3 | `4a0476f1` | `demo_live_fs_compile_ok` |
| 4 | `d33a90eb` | `editor_live_env_ok` |

## Decision (frozen) — summary

Flat listing + breadcrumbs; `folder_panel.mlc`; §36 disk APIs; tab close hit;
`MLC_EDITOR_ROOT` / `MLC_EDITOR_MOCK`; no `compiler/` / no `lib/mlc/`.

## Out of scope (unchanged)

tree-sitter; Fontconfig; SCRIPT_VM; MIR Epic 5; recursive glob / file watch.
