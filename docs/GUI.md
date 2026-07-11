# GUI framework v0 (immediate-mode on MLC)

Parent: [PLAN.md](PLAN.md) §10b, [TEXT_RENDERING.md](TEXT_RENDERING.md) §8,
[archive/tracks/TRACK_GUI_FRAMEWORK.md](archive/tracks/TRACK_GUI_FRAMEWORK.md).

Built on closed [TRACK_TEXT_RENDERING_NATIVE](archive/tracks/TRACK_TEXT_RENDERING_NATIVE.md)
+ [TRACK_GL_GLAD_MIGRATION](archive/tracks/TRACK_GL_GLAD_MIGRATION.md)
(GLFW window + **GLAD2** GL entry points).

## Model

- **Immediate-mode** layout each frame (`poll → update → layout/draw → swap`).
- Shared MLC modules under `misc/gui/` (not stdlib/compiler).
- Thin C++ only for GLFW window/input (`glfw_window_gl`); GL symbols via
  vendored GLAD2 (`glad_gl_abi` / `glad_gl.mlc`), not a hand GetProcAddress table.

## Modules

| Module | Role |
|--------|------|
| `misc/gui/gl_renderer.mlc` | Shader/VBO triangle helper (glad) |
| `misc/gui/text_renderer.mlc` | Atlas/cache + textured quads (glad) |
| `misc/gui/layout.mlc` | `Rect`/`Point`, row/column cursor, `point_in_rect` |
| `misc/gui/input.mlc` | `GuiInput` poll, hover, click edge |
| `misc/gui/button.mlc` | `gui_button`, `ease_in_out_unit` |
| `lib/mlc/common/stdlib/gl/gl_window.mlc` | GLFW context begin/end + `gladLoadGL` |

## Gates

```bash
scripts/run_gui_layout_smoke.sh    # no GLFW
scripts/run_gui_input_smoke.sh     # needs glfw3
scripts/run_gui_button_demo.sh     # needs glfw3; inject click + easing
scripts/run_gl_renderer_smoke.sh
scripts/run_text_renderer_native_smoke.sh
scripts/run_glad_link_smoke.sh
compiler/tests/run_gl_loader_smoke.sh
```

Interactive: `MLC_GLFW_VISIBLE=1 scripts/run_gui_button_demo.sh`.

## Residuals (not v0)

- Button label text draw (color-only demo)
- `text_window_demo.mlc` still self-contained (FT/MSDF), not on `misc/gui/`
- Full widget set / retained tree / CSS
- Retained affine-transform scene graph + vector primitives — единый
  фундамент для classic/game/Flash-rich/Figma-canvas (один tree+matrix,
  разный контент). v0 (screen-space `Rect`, чистый IM) — прототип одной
  кнопки, не основа. Design-only заметка:
  [TRACK_GUI_CANVAS_GRAPH](agent/TRACK_GUI_CANVAS_GRAPH.md)
