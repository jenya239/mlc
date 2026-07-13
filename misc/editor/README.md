# mlc editor

Native text editor dogfooding MLC + `misc/gui`.

Track: [docs/agent/TRACK_EDITOR_MVP.md](../../docs/agent/TRACK_EDITOR_MVP.md).
Architecture: [ARCHITECTURE.md](ARCHITECTURE.md).

## Run

```bash
# STEP=0 scaffold
MLC_GLFW_VISIBLE=0 scripts/run_editor_scaffold_smoke.sh

# STEP=1 theme + solid panel
MLC_GLFW_VISIBLE=0 scripts/run_editor_theme_smoke.sh
# STEP=2 static mono text
MLC_GLFW_VISIBLE=0 scripts/run_editor_text_smoke.sh

# STEP=3 clip unit (no GLFW)
scripts/run_editor_clip_unit.sh

# STEP=4 perf stub
MLC_GLFW_VISIBLE=0 scripts/run_editor_perf_smoke.sh

# STEP=5 shell layout unit (no GLFW)
scripts/run_editor_layout_unit.sh
```

Expect stdout containing `[mlc-editor] open` and exit 0.
Text smoke also requires `[mlc-editor] glyphs=N` with N>0.
Clip unit requires `[mlc-editor] clip_unit ok`.
Perf smoke requires `perf frames=3 …=0`.
Layout unit requires `[mlc-editor] layout_unit ok`.

## Layout (growing)

```text
main.mlc          # entry
ARCHITECTURE.md
document/ layout/ syntax/ workspace/ ui/ commands/ tests/   # later STEPs
```
