# Track: Editor / GUI live polish (P0 reality over model)

Parent: [../../EDITOR.md](../../EDITOR.md), [../../GUI_ARCHITECTURE.md](../../GUI_ARCHITECTURE.md),
[../../GUI_UX_TESTING.md](../../GUI_UX_TESTING.md).
Closed predecessors: [TRACK_EDITOR_MVP.md](TRACK_EDITOR_MVP.md),
[TRACK_UX_HEADLESS.md](TRACK_UX_HEADLESS.md).

## Status: **closed** (2026-07-15) — Critic OK

**Critic 2026-07-15 (STEP=10):** Decision match — live scissor/cursor/OS clipboard/
command-bus/`app/` seed/`EditorAppState`/chrome↔Scene documented split/`u_color`.
Re-ran P0: `scissor_ok`, `editor_live_scissor_ok`, `gui_cursor_shape_ok`,
`gui_clipboard_ok`, `editor_command_bus_live_ok`, `editor_app_seed_ok`,
`text_glyph_color_ok`, `run_ux_gate` → all ok. Anti-false-done:
`011cdbb1`…`f3a8cdbc`. **reopen: none**. Residuals: glyph tint is batch
`u_color` (not per-glyph vertex color); Phase D Path WIP left uncommitted.

## Next step

**closed** — Critic OK. Queue → Phase D STEP=3 (Path tess); leave Path WIP.

### STEP=10 done (2026-07-15)

- Critic: P0 gates; archive; Phase D unpark note.

### STEPs 0–9 done in git (2026-07-15)

| Step | Commit (abbrev) | Gate |
|------|-----------------|------|
| 0 | `011cdbb1` | Decision freeze |
| 1 | `3833924c` | `scissor_ok` |
| 2 | `521810e3` | `editor_live_scissor_ok` + ux_gate |
| 3 | `de71488f` | `gui_cursor_shape_ok` |
| 4 | `b5d8ec14` | `gui_clipboard_ok` |
| 5 | `8c48794f` | `editor_command_bus_live_ok` |
| 6 | `2894ed32` | `editor_app_seed_ok` |
| 7 | `4e121b1d` | ux_gate + seed |
| 8 | `4c3e2706` | chrome/Scene doc split |
| 9 | `f3a8cdbc` | `text_glyph_color_ok` |

## Decision (STEP=0) — frozen (summary)

Live product shell (`demo_live` in scope): GL scissor, GLFW cursor/clipboard,
`command_bus_resolve`, `misc/editor/app/`, theme glyph `u_color`. Chrome stays
shell/`EditorAppState`; Scene = canvas/spatial only. Phase D Path was parked
until this Critic.

## Out of scope (unchanged)

Tree-sitter; IME; full Scene chrome migration; `list_dir` FS tree; full a11y/DPI.
