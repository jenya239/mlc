# Track: GUI scene Phase A — retained tree + affine + hit-test + batch

Parent: [TRACK_GUI_CANVAS_GRAPH.md](TRACK_GUI_CANVAS_GRAPH.md). Epic §29 / §10c
in [PLAN.md](../PLAN.md).

## Status: **active** (2026-07-13) — STEP=1 Decision next

Created by Planner after DEBUG_SOURCE_MAP Critic OK. Phase B–D tracks stay
uncreated until this file closes.

## Next step

**STEP=1** — Freeze Design Decision (docs only; no `misc/gui/` code yet).

### STEP=1 sub-steps

1. Tree: flat `[SceneNode]` + parent index (`i32`, root = `-1`); children =
   contiguous range or `[i32]` indices — **not** `Shared<Node>` pointers.
   Cite: cache locality + optional later `Region` arena.
2. Matrix: `Affine2x3` `{ a, b, c, d, tx, ty }` (column/row convention frozen
   in Decision); API `affine_multiply` / `affine_invert` / `affine_transform_point`.
   Refs: Flash `DisplayObject` matrix chain; no perspective.
3. Precision: local/world `f32`; before GPU — camera-relative (subtract camera
   translation from world). Refs: Godot `CanvasItem` large-world practice.
4. Write **Decision (STEP=1)** section in this file; mark STEP=1 done; leave
   STEP=2+ pending. Do not start `scene.mlc` in the same turn.

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | Design Decision: tree indices, Affine2×3, camera-relative f32 | pending |
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
