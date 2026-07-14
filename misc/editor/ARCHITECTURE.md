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
- P2: clipboard get/set (+ test inject)
- by-value `mlc::file` wrappers for `document_from_file`

## STEP=0…13 status

- STEP=0–12: scaffold through gutter.
- STEP=13: `layout/scroll_layout.mlc` — large-file scroll visits ≤ visible+overscan.
