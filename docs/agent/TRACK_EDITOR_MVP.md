# Track: Native editor MVP (text-centric, Linux)

Parent: [../EDITOR.md](../EDITOR.md), [../PLAN.md](../PLAN.md) §33,
[../GUI.md](../GUI.md). Trigger: пользователь 2026-07-13 — минимальный
используемый редактор на наших GUI/text наработках; задел под будущую
AI-first IDE **без** реализации будущих фич сейчас.

## Status: **priority / active** (2026-07-13) — голова очереди PLAN §33

User override 2026-07-13: editor + mlc-support Opus preflight ahead of GUI
Phase B–D. Phase A scene is done (dependency satisfied).

## Next step

**STEP=7** — Wire tree panel + editor placeholder + divider drag (inject smoke).

### STEP=6 done (2026-07-14)

- `misc/editor/workspace/file_tree.mlc`: lazy expand via inject `DirEntry[]`
  (no recursive scan; P1 `list_dir` still deferred).
- `tests/file_tree_unit.mlc` + `scripts/run_editor_file_tree_unit.sh` exit 0.

### STEP=5 done (2026-07-13)

- `misc/editor/layout/shell.mlc`: Fixed/Row/Col/Flex/Scroll/Split pure layout.
- `tests/layout_unit.mlc` + `scripts/run_editor_layout_unit.sh` exit 0.

### STEP=4 done (2026-07-13)

- `misc/editor/ui/perf.mlc`: `EditorPerfCounters` stub (zeros).
- `main.mlc`: `MLC_EDITOR_PERF` → print frames + zero timings.
- `scripts/run_editor_perf_smoke.sh` — exit 0; `perf frames=3 …=0`.

### STEP=3 done (2026-07-13)

- `misc/editor/ui/clip.mlc`: intersect / empty / contains / clip_child (half-open).
- `misc/editor/tests/clip_unit.mlc` + `scripts/run_editor_clip_unit.sh` exit 0.
- GPU `SceneNodeKind::Clip` deferred until draw needs scissor (ARCHITECTURE).

### STEP=2 done (2026-07-13)

- `misc/editor/ui/static_text.mlc`: GlyphCache/atlas line draw (baseline bearing).
- `main.mlc`: DejaVuSansMono `"mlc-editor"`; stdout `glyphs=N`.
- `scripts/run_editor_text_smoke.sh` — exit 0, open + glyphs>0.

### STEP=1 done (2026-07-13)

- `misc/editor/ui/theme.mlc`: `Theme` + `theme_solarized_light()`.
- `main.mlc`: theme bg clear + panel rect via `solid_renderer_*`, 3 frames.
- `scripts/run_editor_theme_smoke.sh` — exit 0, `[mlc-editor] open`.

### STEP=0 done (2026-07-13)

- `misc/editor/main.mlc`: empty GLFW window, `println("[mlc-editor] open")`,
  3 headless frames.
- `misc/editor/ARCHITECTURE.md` + README run cmds.
- `scripts/run_editor_scaffold_smoke.sh` — exit 0, stdout gate.

### STEP=pre done (2026-07-13)

mlc-support preflight (LARGE_CONTEXT §7):

| Item | Result |
|------|--------|
| Bundle | `mlc-support/bundle_editor_mvp_sources.sh` → `/tmp/mlc_editor_bundle.txt` (4812 lines, 176K) |
| Tokens | dry-run `count_tokens.rb`: **85,214** input (<<1M); paid Sonnet review: **82,187** in / **6,197** out (~$0.23) |
| Response | `mlc-support/responses/editor_mvp_preflight_20260713_185418.md` |
| Buffer Decision E | **closed: piece table** (not rope); byte offsets; codepoint columns on current line only |
| Deviations | **none** vs `GUI_ARCHITECTURE.md`; STEP=3 clip → prefer `SceneNodeKind::Clip` in `misc/gui` |
| Leaf STEPs | refined 0a/0b + E1–E6 gates in response §3 — Driver follows those gates |
| Stdlib gaps | P1 `list_dir`/`is_dir`/`is_file` + `DirEntry`; P2 clipboard get/set (+ test inject) |

**Не путать с** `misc/examples/text_ide_panels_demo.mlc` (статичный макет) и
с [TRACK_MLC_SCRIPT_VM](TRACK_MLC_SCRIPT_VM.md) (design-only, другой продукт).

**Отношение к [TRACK_GUI_CANVAS_GRAPH](TRACK_GUI_CANVAS_GRAPH.md):** Phase A
готово. Phase B–D **deferred** (PLAN §29) until this track closes or user
resumes canvas explicitly.

## Placement (см. EDITOR.md)

- Код приложения: **`misc/editor/`**
- Платформа/рендер: расширять **`misc/gui/`** + stdlib/runtime только для
  clipboard / directory listing
- Не отдельный git-пакет v1; tree-sitter позже — package

## Out of scope (v1)

VM/`mlc-script`, plugins, agent chat, hypergraph, notebooks, CRDT, collab,
LSP, debugger, terminal, minimap, inline widgets in text, heavy animation,
multi-platform, Electron/WebView/Qt Widgets/ready-made editor component.

## Architecture (Clean, minimal) — [GUI_ARCHITECTURE.md](../GUI_ARCHITECTURE.md)

**Editor substrate first.** Hybrid immediate/retained. Multi-tree (entity /
view / layout / scene / hit / focus / semantic stub), not one DOM. No raw GL
from components.

```text
Platform (GLFW = PlatformBackend v1; SDL3 later)
Graphics (scene fragments → flatten → batches + text_renderer)
Text (document/ rope|piece-table + snapshots)
Text layout (visible lines; mono; no wrap v1)
Syntax (interface; MLC lexer stub → Tree-sitter later)
Workspace / UI / Commands (entities + ephemeral views + dirty layout)
```

Pipeline: `input → commands → model → invalidate → dirty views → layout →
scene fragments → flatten → render`.

### Future-safe invariants (do not implement extras)

1. One `TextDocument` → many independent views (view owns cursor/selection/scroll).
2. Renderer does not assume one style per whole line.
3. Flow layout can later accept embedded scenes (no assumption “text only forever”).
4. Scene can show paths/images later (Phase D canvas).
5. Commands address entities/contexts, not concrete buttons.
6. File tree + editor are two views of workspace model.
7. Layout tree is rebuildable, not source of truth.
8. App state serializable: root, open tabs, active file, panel sizes.
9. `ProjectionView` is a `View`, not the base of chrome widgets.
10. Semantic tree stub (`role/label/bounds`) even if a11y unused in v1.
11. Code glyphs = hinted raster atlas; MSDF only for zoom/spatial text.
12. No Widget inheritance tower; no CSS; no global signal mesh.
13. Runtime must not assume “only TextDocument apps”: lists/scene/commands
    stay usable for file manager / image viewer / music player (see
    [GUI_ARCHITECTURE.md](../GUI_ARCHITECTURE.md) §Product surfaces). Those
    apps are **not** EDITOR_MVP deliverables — only architectural room.### Theme

Single `Theme` record (Solarized Light–like: dense, calm, no card chrome).
All colors/sizes/fonts from there.

## Platform gaps → stdlib/runtime (do early)

| STEP | Deliverable | Gate |
|------|-------------|------|
| P1 | `list_dir` / `is_dir` / `is_file` (+ `Result`, `DirEntry={name,is_dir}`) in `io/file` + runtime | unit: temp dir listing; ignore `.git` helper in workspace |
| P2 | Clipboard get/set via GLFW (`glfw_gl_clipboard_*` + `*_test_*` inject) | inject or headless smoke if possible; else visible one-shot |
| P3 | Optional DPI scale read | document; default 1.0 if hard |

## Epics = user stages E1–E6

Each epic: TDD first (pure), then wire GL; app must **build and run** after each.

### E0 — Scaffold (STEP=0)

| Step | Item | Gate |
|------|------|------|
| 0 | Create `misc/editor/{README,ARCHITECTURE}.md` stubs + `main.mlc` empty window reuse GLFW/scene; PLAN queue sync | **done** (2026-07-13): `run_editor_scaffold_smoke.sh` exit 0 |

### E1 — Window / scene / static text / clip / DPI

| Step | Item | Gate |
|------|------|------|
| 1 | Frame loop + theme colors + solid panels via scene | **done** (2026-07-13): `theme.mlc` + `run_editor_theme_smoke.sh` |
| 2 | Static mono text via text_renderer (reuse GlyphCache) | **done** (2026-07-13): `static_text.mlc` + `run_editor_text_smoke.sh` glyphs=60 |
| 3 | Clip rect helper + no full-file work yet | **done** (2026-07-13): `ui/clip.mlc` + `run_editor_clip_unit.sh` |
| 4 | Perf counters stub (`MLC_EDITOR_PERF`) | **done** (2026-07-13): `ui/perf.mlc` + `run_editor_perf_smoke.sh` |

### E2 — Layout shell: tree + editor + split

| Step | Item | Gate |
|------|------|------|
| 5 | UI primitives: row/col/fixed/flex/scroll/split (value types) | **done** (2026-07-14): `layout/shell.mlc` + `run_editor_layout_unit.sh` |
| 6 | FileTree model + lazy expand (needs P1) | **done** (2026-07-14): mock DirEntry expand; `run_editor_file_tree_unit.sh` |
| 7 | Wire tree panel + editor placeholder + divider drag | interactive smoke |
| 8 | Exclude `.git` + extensible ignore list | unit |

### E3 — Open file / line index / visible lines / scroll / gutter

| Step | Item | Gate |
|------|------|------|
| 9 | `TextDocument` piece-table from file (Decision E closed STEP=pre) | unit: load 1–5MB without multi-second block on open path |
| 10 | Line index + offset↔line/col | unit |
| 11 | Visible range + overscan; horizontal scroll OK; no wrap | unit + smoke |
| 12 | Line numbers gutter | visual/smoke |
| 13 | Scroll does not layout whole file | perf: large file scroll frame bound |

### E4 — Edit / selection / clipboard / undo / save

| Step | Item | Gate |
|------|------|------|
| 14 | Cursor + selection (model not in renderer) | unit |
| 15 | Insert/delete/Enter/Backspace/Delete | unit |
| 16 | Arrows/Home/End/Page/Shift+arrows; mouse click+drag | inject integration |
| 17 | Clipboard (P2) Ctrl+C/X/V; Ctrl+A | integration |
| 18 | Undo/redo (Ctrl+Z / Ctrl+Shift+Z or Y) | unit |
| 19 | Dirty + save Ctrl+S; errors shown to user | integration: temp project |

### E5 — Tabs / commands / focus / dirty close

| Step | Item | Gate |
|------|------|------|
| 20 | Tabs: name, dirty, active, close; one path ↔ one document | unit |
| 21 | Ctrl+W / middle-click close; dirty overlay confirm | smoke |
| 22 | Ctrl+Tab next/prev predictable order | unit |
| 23 | Command bus: CommandId, KeyBinding, focus (tree vs editor) | unit all listed commands |
| 24 | Optional: restore workspace root + open files on restart | if cheap |

### E6 — Syntax / theme / status / perf numbers

| Step | Item | Gate |
|------|------|------|
| 25 | `SyntaxHighlighter` interface; MLC lexer stub (visible range / incremental) | unit spans; replaceable by tree-sitter later |
| 26 | Status bar: path, Ln/Col, dirty, language, encoding if known | smoke |
| 27 | Theme finalize Solarized Light–like | visual |
| 28 | Perf report on large file + screenshot/headless capture | numbers in ARCHITECTURE/README |
| 29 | Critic close: acceptance checklist; REG if touched compiler; docs |

## Decisions (closed / open)

| # | Topic | Status |
|---|-------|--------|
| A | Own editor-first runtime; no GPUI/Qt/… dependency | **closed** — [GUI_ARCHITECTURE.md](../GUI_ARCHITECTURE.md) |
| B | Hybrid IM/retained; multi-tree; explicit invalidation | **closed** |
| C | PlatformBackend: GLFW v1, SDL3 later | **closed** |
| D | Code glyphs = raster atlas; MSDF not for body text | **closed** |
| E | Buffer: piece table vs rope | **closed** (STEP=pre): **piece table**; byte offsets; UTF-8 codepoint columns on current line; grapheme/IME out of v1 |
| F | Syntax v1: hand lexer; Tree-sitter later package | **closed** (direction) |
| G | Wrap off; horizontal scroll | **closed** |
| I | Same runtime → file manager / image viewer / music player | **closed** (capability); apps themselves out of EDITOR_MVP scope |

Buffer structure (E): **piece table** (STEP=pre). STEP=9 implements it;
forbid full-string rewrite per keystroke.

## Testing strategy

| Kind | Where | Examples |
|------|-------|----------|
| Unit (no GLFW) | `misc/editor/tests/*.mlc` + `scripts/run_editor_*_smoke.sh` | insert/delete, undo, tabs, tree lazy, visible range, hit-test cursor, highlight spans, commands |
| Integration (inject) | temp project under `.tmp/editor_fixture/` | open → edit → dirty → save → reread |
| Perf | `MLC_EDITOR_PERF=1`, large fixture | frame/layout/shaping/line/primitive counts |
| Snapshot | only if existing headless path cheap | one layout shot — do not invent infra |

Mocks: filesystem via fixture dirs; input via `gui_input_test_set` /
`glfw_gl_*_test_*`; no GPU in document tests.

## Acceptance (track done)

1–13 from user brief (open dir, tree, multi-file tabs, edit, clipboard,
undo/redo, save, line numbers, basic highlight, scroll, dirty close, optional
session restore). Artifacts: code, tests, README run cmds, ARCHITECTURE.md,
known limits, screenshot or headless render, perf numbers, next three steps.

## Estimate

~30 numbered steps + Critic/Planner turns ≈ **40–80 agent commits**; with
stdlib P1–P2 and polish, up to **~100**. Larger reservoir if tree-sitter
and session restore expand — still smaller than inventing Script VM.

## Progress

- **Planner** (2026-07-13): design + placement written.
- **Priority** (2026-07-13 user): ahead of GUI Phase B.
- **Architecture review** (2026-07-13): frozen in
  [GUI_ARCHITECTURE.md](../GUI_ARCHITECTURE.md).
- **STEP=pre done** (2026-07-13): bundle 85K tokens; Sonnet preflight;
  Decision E = piece table.
- **STEP=0 done** (2026-07-13): scaffold GLFW + `[mlc-editor] open`.
- **STEP=1 done** (2026-07-13): Theme + solid_renderer frame.
- **STEP=2 done** (2026-07-13): static mono text.
- **STEP=3 done** (2026-07-13): clip math unit.
- **STEP=4 done** (2026-07-13): perf stub.
- **STEP=5 done** (2026-07-13): shell layout unit.
- **STEP=6 done** (2026-07-14): FileTree lazy expand; next **STEP=7**.