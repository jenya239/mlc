# Track: GUI scene Phase A — retained tree + affine + hit-test + batch

Parent: [TRACK_GUI_CANVAS_GRAPH.md](TRACK_GUI_CANVAS_GRAPH.md). Epic §29 / §10c
in [PLAN.md](../PLAN.md).

## Status: **active** (2026-07-13) — STEP=2 `scene.mlc` next

Created by Planner after DEBUG_SOURCE_MAP Critic OK. Phase B–D tracks stay
uncreated until this file closes.

## Next step

**STEP=2** — `misc/gui/scene.mlc`: `Affine2x3`, `SceneNode`, flat `Scene`
per Decision below (types + constructors only; world pass = STEP=3).

## Decision (STEP=1) — frozen 2026-07-13

### Tree

| Item | Choice |
|------|--------|
| Storage | `Scene.nodes: [SceneNode]` — flat array, index = `NodeId` (`i32`) |
| Parent | `SceneNode.parent: i32` — root = `-1`; **invariant:** parent index `<` child index (append-only topo order) |
| Children | `SceneNode.children: [i32]` — indices into `nodes` (not `Shared<Node>`) |
| World cache | `Scene.world: [Affine2x3]` — parallel to `nodes`, reused each frame (STEP=3) |
| Kinds (Phase A) | `Container` \| `RectFill` (more kinds in Phase B) |

No pointer graph. Compatible with a later region arena over the flat arrays.

### Matrix (`Affine2x3`)

```
type Affine2x3 = { a: f32, b: f32, c: f32, d: f32, tx: f32, ty: f32 }
// (x, y) → (a*x + c*y + tx,  b*x + d*y + ty)   // Flash / DOMMatrix 2D
```

| API | Meaning |
|-----|---------|
| `affine_identity()` | `{1,0,0,1,0,0}` |
| `affine_multiply(parent, local)` | world = parent ∘ local (apply local first, then parent) |
| `affine_invert(matrix)` | for hit-test; fail/identity if det ≈ 0 |
| `affine_transform_point(matrix, x, y)` | apply forward map |

No perspective. Refs: Flash `DisplayObject` matrix chain.

### Precision / camera

| Item | Choice |
|------|--------|
| Local / world | `f32` on `Affine2x3` and scene points |
| v0 boundary | `layout.mlc` `Point`/`Rect` stay `i32` screen-space; convert at smoke edges (STEP=4/6) |
| GPU upload | camera-relative: subtract camera translation from world translation (or multiply by inverse camera) **before** writing vertex `f32`s — do not rely on large absolute world `f32` in clip space |
| Phase A camera | `Scene.camera: Affine2x3` (default identity); Phase D may fold pan/zoom into root — same math |

Refs: Godot `CanvasItem` large-world / camera-relative practice.

### Non-goals this Decision

Draw batching, hit-test algorithm details, widget kinds — STEPs 3–5 / Phase B.

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | Design Decision: tree indices, Affine2×3, camera-relative f32 | **done** (2026-07-13) |
| 2 | `misc/gui/scene.mlc`: `SceneNode` + flat `Scene` | pending |
| 3 | World-transform pass (topo order, reused buffer) | pending |
| 4 | Hit-test via inverse chain; smoke vs `gui_button` case | pending |
| 5 | Batched rect-fill draw (reuse dashboard solid-rect trick) | pending |
| 6 | Migration smoke: `gui_button_demo` on scene (v0 kept) | pending |
| 7 | Verify smoke green (no self-host; misc/gui only) | pending |

## Bearing (existing v0 — do not patch into foundation)

| File | Role |
|------|------|
| `misc/gui/layout.mlc` | IM `Point`/`Rect` `i32`, `point_in_rect` — screen-space, no hierarchy |
| `misc/examples/gui_button_demo.mlc` | v0 button smoke — keep until STEP=6 parallel proof |
| `misc/examples/text_dashboard_demo.mlc` | solid rect as packed atlas glyph — reuse pattern in STEP=5 |

## Out of scope (Phase A)

Widgets (Phase B), dirty/quadtree (Phase C), camera+bezier+blueprint MVP
(Phase D). Do not delete v0 `misc/gui/` in this phase.
