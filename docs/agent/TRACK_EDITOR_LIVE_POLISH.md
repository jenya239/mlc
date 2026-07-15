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

**STEP=6** — Split `demo_live` frame → `misc/editor/app/` seed.

### STEP=5 done (2026-07-15)

- GLFW chord mods + `take_binding_key` (+ test inject).
- `demo_live`: toolbar + keys → one `command_bus_resolve` / dispatch path.
- Unit + smoke `editor_command_bus_live_ok`.

### STEP=4 done (2026-07-15)

- `glfw_gl_clipboard_get`/`set`/`test_set`; `clipboard_os` pull/push.
- `demo_live` copy/cut → OS; paste ← OS.
- Smoke `gui_clipboard_ok`.

### STEP=3 done (2026-07-15)

- `glfw_gl_cursor_set` / `glfw_gl_cursor_shape_get`; standard arrow/ew/ibeam.
- `demo_live` calls `editor_ux_cursor_shape_at` each frame.
- Smoke `gui_cursor_shape_ok`.

### STEP=2 done (2026-07-15)

- `gl_disable`; `editor_gl_scissor_*` helpers; honest `apply_scissor_clip` + clear after draw.
- `demo_live` scissor around editor solids/text; `flush_over` / `draw_over` compose.
- Smoke `editor_live_scissor_ok`; `run_ux_gate` green.

### STEP=1 done (2026-07-15)

- `gl_scissor` / `gl_is_enabled` / `gl_get_integer_at` + `gl_scissor_test`/`gl_scissor_box` in glad ABI + `glad_gl.mlc`.
- `misc/examples/gl_scissor_smoke.mlc` + script → `scissor_ok`.

### STEP=0 done (2026-07-15)

- Decision frozen below (bindings + live shell scope).
- Phase D remains parked; PLAN §33b → STEP=1 next.

## Decision (STEP=0) — **frozen** 2026-07-15

Grounded in: `misc/editor/demo_live.mlc` (god-loop), `editor/ux/overflow.mlc`
(`editor_ux_apply_scissor_clip` always clears overflows), `editor/ux/cursor.mlc`
(shape ids only), `editor/clipboard.mlc` (in-memory), `command_bus_resolve`,
`glad_gl.mlc` (`gl_enable` present; **no** scissor), `gl_window.mlc` (no cursor/
clipboard GLFW).

| Item | Choice |
|------|--------|
| Scope | Live scissor, cursor, OS clipboard, command-bus wire, `demo_live` frame split seed; **not** Phase D Path |
| `demo_live.mlc` + `run_editor_live_demo.sh` | **In scope** — absorb as product shell; commit when a STEP first needs them |
| Phase D | **Parked** — leave Path WIP (`scene_path*`, tess smoke, related `scene.mlc`/`scene_draw.mlc` dirty) untouched; resume STEP=3 after this track Critic |
| Scene vs chrome | Deferred to STEP=8; P0 keeps shell panels / `EditorUxState` paths |
| Fake scissor | Today `editor_ux_apply_scissor_clip` always `state_with_overflows([],…)` when clip nonempty — **must** report real overflows when content exceeds clip **without** GL; with GL scissor, draw uses `glEnable(GL_SCISSOR_TEST)` + `glScissor` |
| Scissor ABI | Add `gl_scissor(x,y,width,height)` + `GL_SCISSOR_TEST` constant in `glad_gl.mlc` / shim (mirror existing `gl_enable`/`gl_viewport` style) |
| Cursor ABI | `glfw_gl_cursor_set(shape: i32)` mapping `ux_cursor_arrow/ew_resize/ibeam` → GLFW standard cursors; live calls after `editor_ux_cursor_shape_at` |
| Clipboard ABI | `glfw_gl_clipboard_get` / `glfw_gl_clipboard_set`; live: OS sync on copy/cut/paste; keep in-memory `Clipboard` as cache; headless inject via test setters if needed |
| Command bus | Live key path calls `command_bus_resolve` (same bindings as unit); toolbar buttons dispatch same command ids |
| Frame split | Seed `misc/editor/app/` — extract input/apply/layout/draw from `demo_live` without behavior change (STEP=6) |
| Gates | `MLC_GLFW_VISIBLE=0` where GL; `scripts/run_ux_gate.sh` must stay green after STEPs that touch ux |

### Exact API surface (STEPs 1–4)

```text
# STEP=1
gl_scissor(x: i32, y: i32, width: i32, height: i32) -> unit
GL_SCISSOR_TEST : i32   # use with existing gl_enable

# STEP=3
glfw_gl_cursor_set(shape: i32) -> unit   # 0 arrow, 1 ew-resize, 2 ibeam

# STEP=4
glfw_gl_clipboard_get() -> string
glfw_gl_clipboard_set(text: string) -> unit
# optional test inject parallel to glfw_gl_text_test_push
```

### Non-goals (Decision)

Tree-sitter; IME; Flex everywhere; Script VM; full Scene migration of tabs/tree;
Phase D Path/wires; `list_dir` FS tree; full a11y/DPI; Qt/Flutter rewrite.

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + PLAN/CONTINUITY sync | **done** (2026-07-15) |
| 1 | `gl_scissor` / `GL_SCISSOR_TEST` in glad/gl stdlib | **done** (2026-07-15): `scissor_ok` |
| 2 | Wire scissor in editor/gui draw; fix fake `editor_ux_apply_scissor_clip` | **done** (2026-07-15): `editor_live_scissor_ok` + ux_gate |
| 3 | `glfw_gl_cursor_set`; wire `ux/cursor.mlc` in live | **done** (2026-07-15): `gui_cursor_shape_ok` |
| 4 | GLFW clipboard get/set; live uses OS clipboard | **done** (2026-07-15): `gui_clipboard_ok` |
| 5 | Wire `command_bus_resolve` into live | **done** (2026-07-15): unit + `editor_command_bus_live_ok` |
| 6 | Split `demo_live` frame → `misc/editor/app/` seed | compiles; live still runs |
| 7 | Single app state; drop duplicate hit/scroll where safe | `run_ux_gate` + live smoke |
| 8 | Decision: chrome on SceneNode **or** documented split | doc in `GUI_ARCHITECTURE.md` |
| 9 | Text glyph color (`u_color` / per-quad) | theme / live readable |
| 10 | Critic: P0 gates green; Phase D unpark note; archive | close |

### Sub-steps (Driver)

**STEP=0** — **done**
1. Freeze Decision table — done.
2. Phase D parked (Planner) — confirmed.
3. Non-goals noted — done.

**STEP=1** — **done**
1. ABI + `glad_gl.mlc` scissor enable/rect — done.
2. Minimal smoke proving clip — done (`scissor_ok`).

**STEP=2** — **done**
1. Apply scissor around editor solids/text in `demo_live` — done.
2. Make `editor_ux_apply_scissor_clip` honest (overflow not always `[]`) — done.

**STEP=3** — **done**
1. Platform cursor binding + live hover divider/text — done.

**STEP=4** — **done**
1. Platform clipboard get/set; live OS sync on copy/cut/paste — done.

**STEP=5** — **done**
1. Live key path + toolbar share `command_bus_resolve` / same CommandId dispatch — done.

**STEP=6–7**
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
- Foreign: Phase D Path files (`scene_path*.mlc`, path tess smoke, related scene/draw dirty) — do not revert; do not mix into this track's commits unless STEP explicitly needs them (it should not).
