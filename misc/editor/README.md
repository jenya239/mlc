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

# STEP=12 gutter unit + visual smoke
scripts/run_editor_gutter_unit.sh
MLC_GLFW_VISIBLE=0 scripts/run_editor_gutter_smoke.sh

# STEP=13 scroll layout bound (no GLFW)
scripts/run_editor_scroll_layout_unit.sh

# STEP=14 selection model unit (no GLFW)
scripts/run_editor_selection_unit.sh

# STEP=15 edit insert/delete unit (no GLFW)
scripts/run_editor_edit_unit.sh

# STEP=16 navigation unit + mouse inject
scripts/run_editor_navigation_unit.sh
MLC_GLFW_VISIBLE=0 scripts/run_editor_navigation_inject.sh

# STEP=17 clipboard unit (no GLFW)
scripts/run_editor_clipboard_unit.sh

# STEP=18 undo/redo history unit (no GLFW)
scripts/run_editor_history_unit.sh

# STEP=19 dirty + save (temp fixture)
scripts/run_editor_save_unit.sh

# STEP=20 tabs unit (no GLFW)
scripts/run_editor_tabs_unit.sh

# STEP=21 dirty close confirm (no GLFW)
scripts/run_editor_tab_close_unit.sh

# STEP=22 Ctrl+Tab next/prev (no GLFW)
scripts/run_editor_tab_cycle_unit.sh

# STEP=23 command bus (no GLFW)
scripts/run_editor_command_bus_unit.sh

# STEP=24 session restore (temp fixture)
scripts/run_editor_session_unit.sh

# STEP=25 syntax highlight spans (no GLFW)
scripts/run_editor_syntax_unit.sh

# STEP=26 status bar (no GLFW)
scripts/run_editor_status_bar_unit.sh

# STEP=27 Solarized Light theme unit (no GLFW)
scripts/run_editor_theme_unit.sh

# STEP=28 large-file perf report (no GLFW)
scripts/run_editor_perf_report.sh

# Interactive live preview (keeps window open; Esc to quit)
MLC_GLFW_VISIBLE=1 scripts/run_editor_live_demo.sh
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
Gutter unit requires `[mlc-editor] gutter_unit ok`.
Gutter smoke requires open + `gutter ok glyphs=N`.
Scroll layout requires `scroll_layout_unit ok` with `file_lines=100000` and `bound=24`.
Selection requires `[mlc-editor] selection_unit ok`.
Edit unit requires `[mlc-editor] edit_unit ok`.
Navigation unit requires `navigation_unit ok`.
Navigation inject requires `navigation_inject ok`.
Clipboard requires `[mlc-editor] clipboard_unit ok`.
History requires `[mlc-editor] history_unit ok`.
Save requires `[mlc-editor] save_unit ok`.
Tabs requires `[mlc-editor] tabs_unit ok`.
Tab close requires `[mlc-editor] tab_close_unit ok`.
Tab cycle requires `[mlc-editor] tab_cycle_unit ok`.
Command bus requires `[mlc-editor] command_bus_unit ok`.
Session requires `[mlc-editor] session_unit ok`.
Syntax requires `[mlc-editor] syntax_unit ok`.
Status bar requires `[mlc-editor] status_bar_unit ok`.
Theme requires `[mlc-editor] theme_unit ok`.
Perf report requires `perf_report file_lines=100000 scroll_bound=24` + `perf_report_unit ok`.
Track **closed** 2026-07-14 (Critic OK) — see `docs/archive/tracks/TRACK_EDITOR_MVP.md`.

## Layout (growing)

```text
main.mlc          # entry
ARCHITECTURE.md
document/         # … + navigation (STEP=16)
layout/           # shell, visible_range, gutter, scroll_layout (STEP=13)
syntax/ workspace/ ui/ commands/ tests/
```
