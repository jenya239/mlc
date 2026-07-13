# Track: GUI scene Phase A — retained tree + affine + hit-test + batch

Parent: [TRACK_GUI_CANVAS_GRAPH.md](TRACK_GUI_CANVAS_GRAPH.md). Epic §29 / §10c
in [PLAN.md](../PLAN.md).

## Status: **active** (2026-07-13) — STEP=7 verify next

Created by Planner after DEBUG_SOURCE_MAP Critic OK. Phase B–D tracks stay
uncreated until this file closes.

## Next step

**STEP=7** — Verify smokes green; close Phase A → Critic.

### STEP=6 done (2026-07-13)

- `misc/examples/scene_button_smoke.mlc`: same `layout_row_item(40,40,160×48)`
  as `gui_button_demo`; `scene_hit_test` click edge; `scene_draw` flush.
- `scripts/run_scene_button_smoke.sh`; v0 `gui_button_demo` unchanged.

### STEP=5 done (2026-07-13)

- Adopted `misc/gui/solid_renderer.mlc` (+ `solid_renderer_add_vertex`).
- `misc/gui/scene_draw.mlc`: `scene_draw_rect_fills` (camera-relative NDC
  triangles, one flush batch); `scene_rect_fill_batch_vertex_count`.
- `misc/examples/scene_batch_smoke.mlc`: count=12 + 3-frame GL flush exit 0.

### STEP=4 done (2026-07-13)

- `scene_hit_test`: inverse(camera) → inverse(world) → local `[0,w)×[0,h)`;
  front-to-back (highest index).
- `misc/examples/scene_hit_smoke.mlc`: parity with `gui_button` 40,40,160×48;
  nested parent; later RectFill wins. Exit 0.

### STEP=3 done (2026-07-13)

- `scene_update_world`: topo index order, `world[i] = parent∘local`, reuse
  `world[]` via `.set`.
- Smoke extended: nested tx (100,200)+(10,20) → child origin (110,220); second
  pass same.

### STEP=2 done (2026-07-13)

- `misc/gui/scene.mlc`: `Affine2x3`, `SceneNode`/`Scene`, add container/rect,
  affine multiply/invert/transform.
- **Precision amendment:** fields are `f64` (mlcc `f32` arith incomplete);
  STEP=5 GPU upload still camera-relative `f32` verts.
- Smoke: `misc/examples/scene_types_smoke.mlc` exit 0.

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
type Affine2x3 = { a: f64, b: f64, c: f64, d: f64, tx: f64, ty: f64 }
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
| Local / world | **`f64` in MLC** (STEP=2 amendment: mlcc `f32` +/`*` incomplete; Decision STEP=1 said f32) |
| GPU upload | still camera-relative **`f32`** verts at draw (STEP=5) |
| v0 boundary | `layout.mlc` `Point`/`Rect` stay `i32` screen-space; convert at smoke edges (STEP=4/6) |
| Phase A camera | `Scene.camera: Affine2x3` (default identity); Phase D may fold pan/zoom into root — same math |

Refs: Godot `CanvasItem` large-world / camera-relative practice.

### Non-goals this Decision

Draw batching, hit-test algorithm details, widget kinds — STEPs 3–5 / Phase B.

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | Design Decision: tree indices, Affine2×3, camera-relative f32 | **done** (2026-07-13; precision → f64 in STEP=2) |
| 2 | `misc/gui/scene.mlc`: `SceneNode` + flat `Scene` | **done** (2026-07-13: + `scene_types_smoke` exit 0) |
| 3 | World-transform pass (topo order, reused buffer) | **done** (2026-07-13: `scene_update_world`) |
| 4 | Hit-test via inverse chain; smoke vs `gui_button` case | **done** (2026-07-13: `scene_hit_test` + `scene_hit_smoke`) |
| 5 | Batched rect-fill draw (reuse dashboard solid-rect trick) | **done** (2026-07-13: `scene_draw` + `solid_renderer` + `scene_batch_smoke`) |
| 6 | Migration smoke: `gui_button_demo` on scene (v0 kept) | **done** (2026-07-13: `scene_button_smoke` + run script) |
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
