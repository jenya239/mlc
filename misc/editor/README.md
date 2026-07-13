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
```

Expect stdout containing `[mlc-editor] open` and exit 0.

## Layout (growing)

```text
main.mlc          # entry
ARCHITECTURE.md
document/ layout/ syntax/ workspace/ ui/ commands/ tests/   # later STEPs
```
