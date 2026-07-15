# MLC GUI / editor runtime — architecture decisions

Status: **frozen 2026-07-13** (external large-context review + project choice).
Product track: [agent/TRACK_EDITOR_MVP.md](agent/TRACK_EDITOR_MVP.md).
Placement: [EDITOR.md](EDITOR.md). Scene substrate: [GUI.md](GUI.md).

## One-line verdict

> Ready-made GUI framework as foundation — **no**. Assemble ideas.
> Build a **spatial/text/media-capable UI runtime** whose *first dogfood* is a
> code editor — not a general Widget kit that later hosts an editor.
> The same substrate must be able to host a **file manager**, **image
> viewer**, and **music player** without a second framework.

## Product surfaces (same runtime)

| App | What it needs from the runtime | v1 EDITOR_MVP |
|-----|--------------------------------|---------------|
| Code editor | documents, visible lines, glyphs, tabs, tree, commands | **in scope** |
| File manager | lazy tree/list, icons/thumbs, selection, DnD later, FS ops | architecture yes; not built in EDITOR track |
| Image viewer | texture/image scene nodes, pan/zoom camera, decode pipeline | scene/image *slot* now; decode later |
| Music player | transport UI, lists, timeline scrub, audio backend hook | commands + layout + list; audio = Platform/Audio backend later |

Shared requirements across all four: `PlatformBackend`, entity/view/layout/
scene/hit/focus/command buses, dirty invalidation, theme, testing without GPU
where possible. Differences are **domain modules + scene primitive kinds**,
not a different GUI stack.

## What we are not

Not Qt/GTK Widgets, not QML runtime, not Flutter/Dart, not Slint-as-core,
not GPUI/Floem/Xilem/Makepad as a dependency, not pure Dear ImGui every-frame
rebuild for the whole IDE, not DOM/CSS/`contenteditable`.

## Reference map (ideas only)

| Source | Take | Reject as dependency |
|--------|------|----------------------|
| **Zed / GPUI** | Hybrid immediate/retained; editor as custom element; entity ownership; flat GPU batches; rope/snapshots; SumTree-like summaries; focus handles | Rust stack; Zed coupling |
| **Xilem / Masonry** | Lightweight view ≠ retained physical node; stable identity; separate event/layout/paint; semantics pass | Experimental; general-purpose first |
| **Qt Quick / GSK** | UI description → independent scene; render thread; render-to-texture; clip/transform layers | QML language/runtime; Qt ownership |
| **Floem / Lapce** | Virtual lists; fine-grained *invalidation*; renderer abstraction; frontend↔workspace proxy | Hidden signal graphs everywhere |
| **Makepad** | GPU-first dogfood; IDE inside own runtime | Entire DSL/runtime bundle |
| **Iced** | Typed commands/messages; update≠view; testable updates | Global Elm as only model |
| **ImGui / egui** | UI as ordinary code; app owns state; no widget lifecycle hell | Pure IM for million-line text |
| **Flutter** | Constraints-down/size-up; dirty flags; layers; render-object split | Dart; mobile-first; rebuild-heavy |
| **HarfBuzz + FreeType + fontconfig** | Shaping + raster for code | Cosmic Text/Parley as required core |
| **Tree-sitter** | Incremental syntax; not document SoT | AST as authority |
| **Taffy** | Later, inside generic form subtrees only | Global Flex/Grid for editor lines |
| **AccessKit** | Later; reserve `SemanticNode` now | Skip forever |
| **Skia / Vello** | Optional later backends | First-stage dependency |

## Architectural forks (decided)

### 1. Own specialized runtime

Low-level libs yes; full foreign toolkit no. MLC dogfoods the runtime.

### 2. Hybrid immediate / retained

```text
retained:  model, entities, component state, layout cache, GPU resources
ephemeral: view descriptions, commands, selectively regenerated scene fragments
```

API may *feel* immediate; implementation is incremental/retained.

### 3. Many trees, one ID space — not one DOM

```text
App/Entity graph     ownership, state
View tree            logical presentation
Layout tree          geometry
Scene fragments      rendering
Hit-test tree        pointer
Focus graph          keyboard
Semantic tree        a11y / search / agents (stub early)
```

Same stable IDs; shapes may differ. Do not force one hierarchy for all roles.

### 4. Projection later, View now

```text
View { build(context) -> ViewDescription }
```

Future `ProjectionView` is *a* View, not the base of Button/Scrollbar.
Do not hard-wire “everything is a Widget class”.

### 5. Specialized layout for IDE chrome

Own: `Row | Column | Stack | Fixed | Flex | Scroll | Split | Overlay`.
Not full CSS Flex/Grid for editor lines / gutters / satellites.
Taffy optional later inside generic widget islands.

### 6. Reactivity

```text
commands/transactions  →  explicit invalidation
small derived UI       →  optional computed values (narrow)
layout/scene           →  dirty flags
background work        →  messages onto UI thread
```

No arbitrary global signal mesh; no full-tree rebuild every keystroke.

### 7. Scene + flatten

```text
retained logical scene fragments
        ↓ flatten
sorted/batched primitive list
        ↓ GPU
```

Components never call OpenGL directly.

### 8. Platform

Abstraction: `PlatformBackend` (window, input, clipboard, IME hooks, DPI, timers).

| Stage | Backend |
|-------|---------|
| **v1 (now)** | **GLFW** — already in-tree (`gl_window`, inject tests, demos). Do not rewrite before editor MVP ships. |
| **next** | **SDL3** (review preference) behind same interface when IME/portals justify it |
| later | Wayland-native / Win32 as additional backends |

Review recommended SDL3 first; we **accept the abstraction**, keep GLFW as first concrete backend to preserve existing dogfood.

## Recommended stack (v1)

```text
Platform:     GLFW (PlatformBackend)
Graphics:     OpenGL + GLAD2 + own shaders (already)
Text:         FreeType + HarfBuzz (+ fontconfig discovery when needed)
Document:     piece table or rope + immutable snapshots
Syntax:       Tree-sitter later; MLC lexer stub behind SyntaxHighlighter first
Layout:       own Row/Column/Split/Scroll/Overlay
UI model:     entities + ephemeral views + layout cache + scene fragments
Rendering:    rect / glyph-sprite / line batches + atlas
Glyphs:       hinted raster atlas for code size; MSDF for zoom/spatial only
Testing:      L0 model unit; L1 semantic UX scenarios (primary);
              inject input; L2/L3 GL/FBO rare — see GUI_UX_TESTING.md
```

## Frame loop

```text
input → commands → model transaction
  → invalidate entities
  → rebuild dirty views
  → measure/layout dirty nodes
  → rebuild dirty scene fragments
  → flatten + batch → render
```

No global UI rebuild.

## Layering (runtime names)

```text
Application Model     app-specific (workspace OR library OR playlist…)
Entity Runtime        identity, lifetime, async bound to entity
View Runtime          lightweight descriptions
Layout Runtime        retained nodes, dirty measure/layout
Domain modules (pluggable; not all required for every app):
  Editor Runtime      rope/snapshots, selection, visible lines
  FsBrowser Runtime   lazy directory model, multi-select (file manager)
  Media Runtime       decode/display images; audio clock/transport hooks
Text Runtime          shaping, glyph runs (reuse misc/gui text_*)
Scene Runtime         fragments, clips, layers, image quads, paths
Render Backend        OpenGL batches, glyph atlas, textures
Interaction Runtime   hit-test, focus, drag, commands
Semantic Runtime      stub roles/bounds (a11y later)
Platform Backend      GLFW now (+ future Audio/Clipboard extensions)
```

Scene primitive kinds (v1 grow-set, not one-shot):

```text
RectFill | GlyphRun | Line | Clip | ImageQuad(texture_id) | Path(stub)
```

`ImageQuad` may be a thin stub until an image-viewer dogfood; must not assume
“only glyphs exist”. Virtualized lists must accept arbitrary row views (file
row, playlist row, editor line) — do not hard-wire “row = shaped code line”.

## Explicit non-goals (architecture)

- DOM / CSS cascade / contenteditable
- QML-like dynamic object model in v1
- Universal style property bag
- `Widget → Control → TextControl → Editor` inheritance
- Full Flexbox as editor surface geometry
- Global reactive rerender / full IM every frame
- Direct GL from UI components
- One tree for layout+render+focus+semantics
- Child OS window per complex surface
- Separate GUI stacks per app type (editor vs file manager vs media)

## North-star order

```text
Shared UI runtime (platform, entities, views, layout, scene, commands)
    first dogfood: Text/editor substrate + workspace chrome
    same runtime must also support: file manager, image viewer, music player
Notebook/projection flow
    following level (editor-adjacent)
General form widgets
    inclusions only — not the foundation
```

**Editor-first** = dogfood order and priority of TRACK_EDITOR_MVP, **not**
“runtime is monospace-only”. Do not encode assumptions that block image
textures, non-text lists, or an audio transport UI.

## Live editor chrome vs Scene (TRACK_EDITOR_LIVE_POLISH STEP=8)

**Frozen 2026-07-15 — documented split (not “chrome = SceneNode tree”).**

| Layer | Owns | Live path today |
|-------|------|-----------------|
| **IDE chrome** | Header, tab strip, toolbar, status, file-tree + editor split, focus/cursor/scroll probes | `misc/editor/ui/shell_panels.mlc`, `EditorUxState`, `EditorAppState` (`misc/editor/app/`), `demo_live.mlc` |
| **Document surface** | Piece table, selection, history, command bus, gutter/visible lines, GL text/solids + scissor | `misc/editor/document/*`, `layout/*`, draw in `demo_live` |
| **Scene graph** | Affine nodes, camera, hit spatial, Path/wires (Phase D) | `misc/gui/scene*.mlc` — **canvas/spatial substrate**, not the IDE chrome host |

**Choice:** keep chrome on shell panels + ux/app state through LIVE_POLISH P0/P1.
Do **not** reparent tabs/tree/toolbar into `SceneNode` in this track.

**Rationale:** product dogfood already runs on shell panels; Scene Phase D Path WIP is parked;
migrating chrome mid-polish would mix unrelated trees and block scissor/cursor/clipboard gates.

**Later (out of this track):** optional Scene-backed chrome only after Path/wires Critic and an
explicit new Decision — not implied by north-star “scene fragments” wording above.

## Relation to existing code
| Existing | Role under this architecture |
|----------|------------------------------|
| `misc/gui/scene.mlc` | Scene/Layout affine substrate (Phases A–C done; D Path parked) |
| `misc/gui/text_renderer.mlc` | Text Runtime glyph atlas path |
| `misc/gui/input.mlc` + `gl_window` | Platform GLFW + Interaction inject |
| `misc/editor/` | Application + Editor + View/Command runtimes |
| `misc/editor/demo_live.mlc` + `app/` | Live product shell (chrome split above) |
| `text_ide_panels_demo.mlc` | Visual mock only — not the architecture |

GUI Canvas Phase D (Path/wires) remains **parked** until LIVE_POLISH Critic; do not contradict
the chrome/Scene split above.
