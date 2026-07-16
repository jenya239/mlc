# Track: Editor soft word-wrap (layout)

Parent: [../PLAN.md](../PLAN.md) §42; residual «no wrap v1» in
[../EDITOR.md](../EDITOR.md) / `misc/editor/ARCHITECTURE.md` /
`layout/visible_range.mlc`. Uncommitted WIP:
`misc/editor/layout/word_wrap.mlc` + `demo_live` imports (absorb, do not
discard).

## Status: **active** (2026-07-16) — queue head

## Next step

**STEP=1** — Absorb/stabilize `word_wrap.mlc` + unit.

### STEP=0 done (2026-07-16)

- Decision frozen below; PLAN §42 → STEP=1.

## Decision (STEP=0) — **frozen** 2026-07-16

Grounded in uncommitted `misc/editor/layout/word_wrap.mlc` + «no wrap v1»
notes in ARCHITECTURE / `visible_range.mlc`, and §40 codepoint helpers.

| Item | Choice |
|------|--------|
| Module | `misc/editor/layout/word_wrap.mlc` (absorb WIP as-is API) |
| Columns | UTF-8 **codepoints** via `utf8_count_codepoints` / `utf8_byte_offset_for_codepoint_column` |
| Break | Prefer last `" "` in the current visual segment before `max_columns`; else hard cut (≥1 codepoint) |
| Empty line | One `VisualRow` with zero-width span |
| Viewport cols | `wrap_max_columns(viewport_width, char_width)` — floor division, clamp ≥1 |
| Wire (STEP=2) | `demo_live` (+ scroll/hit already in WIP); do **not** commit unrelated `runtime/io`, folder_panel, chrome unless required for compile |
| Unit (STEP=1) | Pure: short line (1 row), long no-space (hard cut), long with spaces (soft break), `wrap_offset_at_point` spot |
| REG | Prefer **no** `lib/mlc/`; no `compiler/` |

### Exact exports (frozen from WIP)

```text
type VisualRow = {
  doc_line: i32,
  byte_start: i32,
  byte_end: i32,
  column_start: i32,
  column_end: i32,
  is_wrap_continue: i32
}

wrap_max_columns(viewport_width: i32, char_width: i32) -> i32
visual_rows_for_line(text, line_index, doc_line, max_columns) -> [VisualRow]
document_visual_row_count(text, line_index, max_columns) -> i32
collect_visible_visual_rows(text, line_index, max_columns, first_visual_row, max_rows) -> [VisualRow]
visual_row_index_for_caret(text, line_index, max_columns, caret_line, caret_column) -> i32
wrap_offset_at_point(text, line_index, max_columns, text_origin_x, text_origin_y,
  scroll_offset_y, line_height, char_width, pixel_x, pixel_y) -> i32
editor_scrollbar_thumb(track_x, track_y, track_height, thumb_width,
  content_height, scroll_y) -> Rect
```

### Gate names

| Step | Script / token |
|------|----------------|
| 1 | `scripts/run_editor_word_wrap_unit.sh` → `word_wrap_unit ok` |
| 2 | `scripts/run_editor_demo_live_fs_compile.sh` → `demo_live_fs_compile_ok` |

### In-scope WIP files (STEP=1–2)

- `misc/editor/layout/word_wrap.mlc` (required STEP=1)
- `misc/editor/demo_live.mlc` wrap call sites (STEP=2; only wrap-related hunks if possible)
- `misc/editor/ux/scroll.mlc` only if wrap scroll needs it
- `misc/editor/tests/word_wrap_unit.mlc` (new)

### Non-goals (Decision)

Hyphenation; proportional fonts; grapheme/IME; tree-sitter; SCRIPT_VM;
LANG_AUTO_CYCLE; MIR Epic 5; `compiler/` / `lib/mlc/`; committing foreign
`runtime/include/mlc/io/**` or folder_panel extras without need.

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + PLAN/CONTINUITY | **done** (2026-07-16) |
| 1 | Absorb/stabilize `word_wrap.mlc` + unit | `word_wrap_unit ok` |
| 2 | Wire scroll/hit/`demo_live`; compile green | `demo_live_fs_compile_ok` |
| 3 | Critic: gates (+ REG if `lib/mlc/`); archive | close |

### Sub-steps (Driver)

**STEP=0** — **done**
1. Freeze break rules + exact exports — done.
2. In-scope WIP file list — done.

**STEP=1**
1. Commit `word_wrap.mlc` + `tests/word_wrap_unit.mlc`.
2. Gate: `bash scripts/run_editor_word_wrap_unit.sh`.

**STEP=2**
1. Finish `demo_live` wrap wire from WIP (explicit `git add` only wrap-needed files).
2. `bash scripts/run_editor_demo_live_fs_compile.sh` (stash unrelated foreign if needed).

**STEP=3** — Critic; `next` = Planner.

## Out of scope

- Grapheme / IME / proportional metrics
- tree-sitter / SCRIPT_VM / LANG_AUTO_CYCLE / MIR Epic 5
- `compiler/` changes
- Unrelated foreign WIP unless required for compile

## Verify discipline

- One STEP per turn; commit + push.
- Leave `compiler/out/extern_concurrency_lint.*` alone.
- Do not `git add` foreign files outside TRACK scope.
