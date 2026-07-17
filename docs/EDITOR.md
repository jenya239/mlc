# Native code editor (MLC) — product + placement

Parent: [PLAN.md](PLAN.md) §33, track [archive/tracks/TRACK_EDITOR_MVP.md](archive/tracks/TRACK_EDITOR_MVP.md) (**closed**).
Architecture decisions (frozen): [GUI_ARCHITECTURE.md](GUI_ARCHITECTURE.md).
Depends on: [GUI.md](GUI.md), [TEXT_RENDERING.md](TEXT_RENDERING.md),
scene Phase A ([archive/tracks/TRACK_GUI_SCENE_PHASE_A.md](archive/tracks/TRACK_GUI_SCENE_PHASE_A.md)).
Future (out of scope v1): AI-first IDE, projections, notebooks — see
[GUI_ARCHITECTURE.md](GUI_ARCHITECTURE.md) invariants (multi-tree, View≠DOM).

## North star (from architecture review)

Build a **shared UI runtime** whose first dogfood is a code editor — not a
general-purpose widget toolkit that later hosts an editor. The **same**
runtime must be able to ship a file manager, image viewer, or music player
(domain modules + scene kinds), without a second framework. See
[GUI_ARCHITECTURE.md](GUI_ARCHITECTURE.md) §Product surfaces.

**Product target (2026-07-17, user directive):** feature set comparable to
**Sublime Text** (fast open/edit/save/find on large files, tabs, multi-file
project view, syntax highlight, goto-line, multi-cursor) — **not** VS
Code/LSP/IDE parity (no debugger, no language server, no extension host).
When a backlog item trades feature breadth against startup time, frame
latency, or crash/data-loss safety, **stability and speed win** — see
[TRACK_EDITOR_UX_BACKLOG](agent/TRACK_EDITOR_UX_BACKLOG.md) priority rule.
## Placement decision (frozen)

**Priority (2026-07-13):** PLAN §33 head of queue — ahead of GUI Phase B–D.
Preflight: `mlc-support/steps/editor_mvp_preflight.md` + LARGE_CONTEXT §7.

| Layer | Where | Why |
|-------|-------|-----|
| Editor app + domain (document, workspace, commands, panels) | **`misc/editor/`** | Same convention as `misc/gui/`: dogfood inside this repo, relative imports, not compiler/stdlib |
| Shared window/GL/text/scene primitives | **`misc/gui/`** (reuse + thin extensions) | Already GLFW+GLAD+atlas+scene Phase A; do not fork. PlatformBackend = GLFW v1; SDL3 later per [GUI_ARCHITECTURE.md](GUI_ARCHITECTURE.md) |
| FS/clipboard/dir listing gaps | **`lib/mlc/common/stdlib/` + `runtime/`** | Platform capability, reusable beyond editor |
| External heavy deps later (tree-sitter) | **git package** via [PACKAGE_MANAGER.md](PACKAGE_MANAGER.md) | Not in-tree; `mlc.json` + `.mlc_packages/` when needed |
| Separate top-level git repo / npm-style package for the editor itself | **No (v1)** | Editor is the dogfood of *this* language/repo; extract later if it graduates |

**Not** under `compiler/` (not part of mlcc). **Not** under `lib/mlc/common/stdlib/` as a whole app (stdlib stays small, platform-ish).

Layout sketch:

```text
misc/editor/
  README.md                 # build/run (filled as steps land)
  ARCHITECTURE.md           # module boundaries (filled STEP=0 / kept current)
  main.mlc                  # entry: argv workspace root → app loop
  app.mlc                   # frame loop, wire panels
  document/                 # TextDocument, rope/piece-table, undo, cursor/selection
  layout/                   # visible lines, glyph runs, doc↔visual coords (no wrap v1)
  syntax/                   # SyntaxHighlighter interface + mlc lexer stub
  workspace/                # root, file tree model, open docs, tabs
  ui/                       # panels: tree, editor view, tabs, status, split, theme
  commands/                 # CommandId, bindings, handlers, focus
  tests/                    # pure MLC unit + headless inject integration
```

Imports: `import { ... } from '../gui/scene'` etc. Stdlib: relative to
`lib/mlc/common/stdlib/io/file.mlc` (or bare `Env` where already registered).

## What already exists (reuse, do not reimplement)

| Asset | Use |
|-------|-----|
| `misc/gui/scene.mlc` + `scene_draw.mlc` | Retained affine tree, hit-test, batched rects |
| `misc/gui/text_renderer.mlc` + `text_shaping.mlc` | Glyph atlas/cache, shaping (HashMap cache — see TRACK_TEXT_GLYPH_CACHE_SCALING) |
| `misc/gui/input.mlc` + `gl_window.mlc` | Mouse/keys/text inject; headless `MLC_GLFW_VISIBLE=0` |
| `misc/examples/text_ide_panels_demo.mlc` | Visual mock only — **not** the editor; retire or keep as perf toy |
| `lib/.../io/file.mlc` | Read/write single files (`Result` wrappers) |
| `scripts/run_*_smoke.sh` pattern | TDD gates for new modules |

## Gaps that force MLC/runtime work (planned inside track)

| Gap | Where to fix | Blocks |
|-----|--------------|--------|
| No `list_dir` / `is_dir` / `is_file` | stdlib `io` + runtime — **done** §36 (`TRACK_STDLIB_IO_FS`) | File tree |
| Clipboard | **done** (§33b LIVE_POLISH: GLFW get/set + inject) | Cut/copy/paste |
| No font discovery API | **done** §39 (`TRACK_EDITOR_FONT_CONFIG`) | Theme font |
| tree-sitter | later package + FFI; v1 = incremental lexer behind interface | Syntax (E6) |
| Optional: better UTF-8 grapheme/column rules | editor `document/` — **§40 done** (codepoints; grapheme deferred) | Cursor columns |
| Caret blink phases (UX L8) | editor `ux/` + `demo_live` — **§41 done** (`TRACK_EDITOR_CARET_BLINK`) | Headless clock + live |
| Soft word-wrap (no wrap v1 residual) | editor `layout/word_wrap` — **§42 done** (`TRACK_EDITOR_WORD_WRAP`) | Long lines |
| Folder browse history (back/forward) | editor `ux/folder_panel` — **§43 active** (`TRACK_EDITOR_FOLDER_NAV`; absorb WIP) | Tree nav |

Language/compiler changes only when a vertical slice is blocked (document in
track Decision, one small compiler track if needed — never drive editor from
hypothetical language features).

## Process

- Clean Architecture without ceremony: layers as **modules + data**, not
  Widget OO trees / CSS / DOM.
- TDD: pure tests for document/workspace/commands **before** GL window;
  inject API for integration.
- **UX gate:** semantic headless scenarios ([GUI_UX_TESTING.md](GUI_UX_TESTING.md),
  [TRACK_UX_HEADLESS](agent/TRACK_UX_HEADLESS.md)) — primary for interactions;
  unit-only green ≠ UX done. Pixel/FBO MAE only for text render (rare).
- Measure: frame/layout/shaping counters behind `MLC_EDITOR_PERF=1`.
- Vertical slices E1→E6 (see track); after each stage: builds and runs.

## Criteria (done when)

Open dir → tree → open files → tabs → edit → save → line numbers → basic
MLC highlight → scroll large file → dirty close confirm — usable on this
repo’s sources. See track §Acceptance.
