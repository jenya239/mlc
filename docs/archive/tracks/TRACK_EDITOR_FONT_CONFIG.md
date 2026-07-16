# Track: Editor font config path (DejaVuMono default)

Parent: [../../PLAN.md](../../PLAN.md) §39; gap from [../../EDITOR.md](../../EDITOR.md)
(«No font discovery API»).

## Status: **closed** (2026-07-16) — Critic OK

**Critic 2026-07-16 (STEP=3):** Re-ran unit + demo_live compile. No `lib/mlc/` →
REG skipped (Decision). Anti-false-done: `279025c9`…`8176d8c5` (STEP=0–2).
**reopen: none**.

| Gate | Result |
|------|--------|
| `run_editor_font_config_unit.sh` | `font_config_unit ok` (default + env) |
| `run_editor_demo_live_fs_compile.sh` | `demo_live_fs_compile_ok` |

## Next step

**closed** — Critic OK. Queue → Planner.

### STEP=3 done (2026-07-16)

- Critic gates; archive.

### STEPs 0–2 done in git (2026-07-16)

| Step | Commit (abbrev) | Gate |
|------|-----------------|------|
| 0 | `279025c9` | Decision frozen |
| 1 | `70574d0e` | `font_config_unit ok` |
| 2 | `8176d8c5` | `demo_live_fs_compile_ok` |

## Decision (frozen) — summary

`editor_resolve_font_path` / `editor_default_font_path`; order explicit →
`MLC_EDITOR_FONT` → well-known DejaVu paths → primary default; `demo_live` wired;
no Fontconfig; no `compiler/` / no `lib/mlc/`.

## Out of scope (unchanged)

Fontconfig; font picker UI; tree-sitter; SCRIPT_VM; MIR Epic 5.
