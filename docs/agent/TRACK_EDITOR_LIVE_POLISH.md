# Track: Editor / GUI live polish (P0 reality over model)

Parent: [../EDITOR.md](../EDITOR.md), [../GUI_ARCHITECTURE.md](../GUI_ARCHITECTURE.md),
[../GUI_UX_TESTING.md](../GUI_UX_TESTING.md).
Closed predecessors: [../archive/tracks/TRACK_EDITOR_MVP.md](../archive/tracks/TRACK_EDITOR_MVP.md),
[../archive/tracks/TRACK_UX_HEADLESS.md](../archive/tracks/TRACK_UX_HEADLESS.md).
Source review: `mlc-support/responses/gui_editor_architecture_20260715_105621.md`
(+ `mlc-support/steps/gui_editor_architecture_review.md`).

## Status: **PRIORITY OVERRIDE / queue head** (2026-07-15)

User 2026-07-15: live GLFW still spills across panels, divider cursor unchanged,
clipboard OS missing — despite closed MVP/UX_HEADLESS (model gates only).
Inserted **before** parked `TRACK_GUI_SCENE_PHASE_D` STEP=3+.

**Do not** reopen EDITOR_MVP / UX_HEADLESS numbered STEPs. This track owns
**live wiring** + missing Platform bindings.

## Next step

**STEP=0** — Freeze Decision (bindings + live shell scope) + PLAN sync.

## Decision (STEP=0) — draft until Driver freezes

| Item | Direction |
|------|-----------|
| Scope | Live scissor, cursor, clipboard, command-bus wire, frame split seed; not Phase D Path |
| `demo_live.mlc` | **In scope** for this track (was foreign WIP; absorb/commit as product shell) |
| Phase D | **Parked** — leave uncommitted Path WIP untouched; resume after this track Critic close |
| Scene vs editor chrome | Document Decision later (STEP=8); do not migrate tabs/tree to Scene in P0 |
| Fake scissor | Fix `editor_ux_apply_scissor_clip` so overflow depends on real clip math; enable GL scissor in draw |
| Gates | Prefer `MLC_GLFW_VISIBLE=0` smokes + existing `run_ux_gate` still green |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + PLAN/CONTINUITY sync | doc |
| 1 | `gl_scissor` / `GL_SCISSOR_TEST` in glad/gl stdlib | `run_gl_scissor_smoke.sh` → `scissor_ok` (or FBO/clear proof) |
| 2 | Wire scissor in editor/gui draw; fix fake `editor_ux_apply_scissor_clip` | `run_editor_live_scissor_smoke.sh` + `run_ux_gate` still ok |
| 3 | `glfwSetCursor` / standard cursors; wire `ux/cursor.mlc` in live | `run_gui_cursor_shape_smoke.sh` |
| 4 | GLFW clipboard get/set + inject; live uses OS clipboard | `run_gui_clipboard_smoke.sh` |
| 5 | Wire `command_bus_resolve` into live (kbd + toolbar one path) | unit + `run_editor_command_bus_live_smoke.sh` |
| 6 | Split `demo_live` frame: input / apply / layout / draw modules (seed `misc/editor/app/`) | compiles; live still runs |
| 7 | Single `EditorAppState` (or build `EditorUxState` from live); drop duplicate hit/scroll paths where safe | `run_ux_gate` + live smoke |
| 8 | Decision: editor chrome on SceneNode **or** documented split in `GUI_ARCHITECTURE.md` | doc Deviation/Decision |
| 9 | Text glyph color (`u_color` / per-quad) — real highlight, not only underlays | theme smoke / live dark+light readable |
| 10 | Critic: P0 gates green; Phase D unpark note; archive | close |

### Sub-steps (Driver)

**STEP=0**
1. Freeze Decision table.
2. Mark Phase D parked in its TRACK + epic; queue head = this track.
3. Non-goals: Tree-sitter, IME, Flex everywhere, Script VM, full Scene migration.

**STEP=1**
1. ABI + `glad_gl.mlc` scissor enable/rect.
2. Minimal smoke proving clip (headless if possible).

**STEP=2**
1. Apply scissor around tree/editor/tab text draws in live path.
2. Make `editor_ux_apply_scissor_clip` honest (overflow not always `[]`).

**STEP=3–4**
1. Platform cursor + clipboard bindings mirroring existing glfw test-inject style.
2. Live hover divider → ew-resize; text → ibeam.

**STEP=5–7**
1. One command path; start extracting `app/` from `demo_live` god-loop.
2. Prefer reusing `ux/*` helpers over re-implementing.

**STEP=8–9**
1. Architecture Decision only if cheap; glyph color unblock Solarized light.

**STEP=10**
1. Critic; then Planner resumes Phase D STEP=3 (Path tess) — do not discard Path WIP.

## Out of scope

- Phase D Path/wires/blueprint MVP (parked, not cancelled).
- `list_dir` / real FS tree (separate P1).
- Full a11y / DPI (P2/P3 after Critic unless trivial stub).
- Rewriting GUI as Qt/Flutter/GPUI.

## Verify discipline

- One atomic STEP per turn; commit + push.
- Do not mark done without gate script exit 0.
- Foreign: Phase D Path files (`scene_path*.mlc`, path tess smoke) if still uncommitted — do not revert; do not mix into this track's commits unless STEP explicitly needs them (it should not).
