# mlc editor — architecture (STEP=0+)

Product: [docs/EDITOR.md](../../docs/EDITOR.md).
Frozen runtime decisions: [docs/GUI_ARCHITECTURE.md](../../docs/GUI_ARCHITECTURE.md).
Track: [docs/agent/TRACK_EDITOR_MVP.md](../../docs/agent/TRACK_EDITOR_MVP.md).

## Module layout

```text
misc/editor/
  main.mlc          # entry (STEP=0 scaffold)
  app.mlc           # frame loop wire-up (later)
  document/         # TextDocument piece table, cursor, selection, undo
  layout/           # visible lines, gutter, horizontal scroll (no wrap v1)
  syntax/           # SyntaxHighlighter + MLC lexer stub
  workspace/        # root, file tree, open docs, tabs
  ui/               # panels, theme, split
  commands/         # CommandId, bindings, focus
  tests/            # pure unit + inject integration
```

Editor owns **no** raw GL. Draw via `misc/gui/scene*` + `text_renderer` /
`text_shaping`. Domain models stay out of `misc/gui/`.

## Buffer (Decision E)

**Piece table** (`document/piece_table.mlc`) — byte offsets; `Original`|
`Added` pieces; inserts append to `added`, never rewrite `original`. Open
path today: `document_from_string` (path load deferred — FFI). Codepoint
columns on the current line only; grapheme/IME out of v1.

## Pipeline

`input → commands → model → invalidate → dirty views → layout → scene
fragments → flatten → render`.

## Stdlib gaps (not in this tree)

- P1: `list_dir` / `is_dir` / `is_file`
- P2: clipboard get/set — editor inject done (STEP=17); GLFW `glfw_gl_clipboard_*` deferred
- by-value `mlc::file` wrappers: `runtime/include/mlc/io/file_abi.hpp` (STEP=19)

## STEP=0…28 status

- STEP=0–27: scaffold through Solarized theme.
- STEP=28: `ui/perf_report.mlc` — large-file scroll/highlight counts.

## Perf report (STEP=28, 2026-07-14)

Headless unit `scripts/run_editor_perf_report.sh` (no GLFW screenshot infra):

| Metric | Value |
|--------|-------|
| file_lines (scroll model) | 100000 |
| scroll_bound (viewport 400/20 + overscan 2×2) | 24 |
| scroll_max_touched (40 scroll samples) | 24 |
| highlight_window_bytes | 58 |
| highlight_spans | 6 |
| line_index_lines (compact fixture) | 5 |

GPU frame timings remain stub zeros (`MLC_EDITOR_PERF` smoke). Pixel/FBO capture not added.
