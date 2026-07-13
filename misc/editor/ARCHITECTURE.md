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

**Piece table** — byte offsets; codepoint columns on the current line only;
grapheme/IME out of v1. See STEP=pre response in mlc-support.

## Pipeline

`input → commands → model → invalidate → dirty views → layout → scene
fragments → flatten → render`.

## Stdlib gaps (not in this tree)

- P1: `list_dir` / `is_dir` / `is_file`
- P2: clipboard get/set (+ test inject)

## STEP=0…8 status

- STEP=0–7: window / theme / text / clip / perf / layout / FileTree / panels.
- STEP=8: `workspace/ignore_list.mlc` + filtered expand (`.git` default).
