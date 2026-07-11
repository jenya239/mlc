# GUI framework v0 (immediate-mode on MLC)

Parent: [PLAN.md](PLAN.md) §10b, [TEXT_RENDERING.md](TEXT_RENDERING.md) §8,
[archive/tracks/TRACK_GUI_FRAMEWORK.md](archive/tracks/TRACK_GUI_FRAMEWORK.md).

Built on closed [TRACK_TEXT_RENDERING_NATIVE](archive/tracks/TRACK_TEXT_RENDERING_NATIVE.md)
(GLFW window + thin GL dispatch).

## Model

- **Immediate-mode** layout each frame (`poll → update → layout/draw → swap`).
- Shared MLC modules under `misc/gui/` (not stdlib/compiler).
- Thin C++ only for GL dispatch / input poll ABI (`mlc::gl::*`).

## Modules

| Module | Role |
|--------|------|
| `misc/gui/gl_renderer.mlc` | Shader/VBO triangle helper |
| `misc/gui/text_renderer.mlc` | Atlas/cache + textured quads |
| `misc/gui/layout.mlc` | `Rect`/`Point`, row/column cursor, `point_in_rect` |
| `misc/gui/input.mlc` | `GuiInput` poll, hover, click edge |
| `misc/gui/button.mlc` | `gui_button`, `ease_in_out_unit` |

## Gates

```bash
scripts/run_gui_layout_smoke.sh    # no GLFW
scripts/run_gui_input_smoke.sh     # needs glfw3
scripts/run_gui_button_demo.sh     # needs glfw3; inject click + easing
scripts/run_gl_renderer_smoke.sh
scripts/run_text_renderer_native_smoke.sh
```

Interactive: `MLC_GLFW_VISIBLE=1 scripts/run_gui_button_demo.sh`.

## Residuals (not v0)

- Button label text draw (color-only demo)
- `text_window_demo.mlc` still self-contained (FT/MSDF), not on `misc/gui/`
- Full widget set / retained tree / CSS
- Масштабируемое поле со связями (Figma/blueprint-подобное) — другая
  архитектура, не расширение v0: camera/zoom, spatial culling, instancing,
  retained scene graph. Design-only заметка:
  [TRACK_GUI_CANVAS_GRAPH](agent/TRACK_GUI_CANVAS_GRAPH.md)
