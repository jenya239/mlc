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

# STEP=6 FileTree lazy expand unit (no GLFW)
scripts/run_editor_file_tree_unit.sh

# STEP=7 shell panels + divider drag
MLC_GLFW_VISIBLE=0 scripts/run_editor_shell_panels_smoke.sh

# STEP=8 ignore list unit (no GLFW)
scripts/run_editor_ignore_list_unit.sh

# STEP=9 piece table unit (no GLFW)
scripts/run_editor_piece_table_unit.sh

# STEP=10 LineIndex unit (no GLFW)
scripts/run_editor_line_index_unit.sh

# STEP=11 visible range unit (no GLFW)
scripts/run_editor_visible_range_unit.sh
```

Expect stdout containing `[mlc-editor] open` and exit 0.
Text smoke also requires `[mlc-editor] glyphs=N` with N>0.
Clip unit requires `[mlc-editor] clip_unit ok`.
Perf smoke requires `perf frames=3 …=0`.
Layout unit requires `[mlc-editor] layout_unit ok`.
FileTree unit requires `[mlc-editor] file_tree_unit ok`.
Shell panels requires `[mlc-editor] shell_panels ok`.
Ignore list requires `[mlc-editor] ignore_list_unit ok`.
Piece table requires `[mlc-editor] piece_table_unit ok`.
LineIndex requires `[mlc-editor] line_index_unit ok`.
Visible range requires `[mlc-editor] visible_range_unit ok`.

## Layout (growing)

```text
main.mlc          # entry
ARCHITECTURE.md
document/         # piece_table, line_index
layout/           # shell, visible_range (STEP=11)
syntax/ workspace/ ui/ commands/ tests/
```
