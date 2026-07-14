# Track: GUI scene Phase D — camera + vector path + blueprint MVP

Parent: [TRACK_GUI_CANVAS_GRAPH.md](TRACK_GUI_CANVAS_GRAPH.md).
Epic §29 / §10c in [../PLAN.md](../PLAN.md).
Phase A (closed): [../archive/tracks/TRACK_GUI_SCENE_PHASE_A.md](../archive/tracks/TRACK_GUI_SCENE_PHASE_A.md).
Phase B (closed): [../archive/tracks/TRACK_GUI_SCENE_PHASE_B.md](../archive/tracks/TRACK_GUI_SCENE_PHASE_B.md).
Phase C (closed): [../archive/tracks/TRACK_GUI_SCENE_PHASE_C.md](../archive/tracks/TRACK_GUI_SCENE_PHASE_C.md).

## Status: **priority / active** (2026-07-15) — queue head after Phase C close

Phase A/B/C stay. Phase D adds **camera pan/zoom**, one **vector path**
(bezier fill+stroke) primitive, and a minimal **node/wire canvas** smoke
on the same flat `Scene` (not a second toolkit).

## Next step

**STEP=1** — Decision: camera API, Path kind, wire model, MVP scope.

### STEP=0 done (2026-07-15)

- Planner opened this track; PLAN §10c/§29 → Phase D active.

## Decision (STEP=1) — draft until Driver freezes

| Item | Direction |
|------|-----------|
| Camera | Keep `Scene.camera: Affine2x3`; pan/zoom = mutate camera (or dedicated root); screen↔scene via existing invert chain |
| Path kind | Extend `SceneNodeKind` with `Path(ScenePath)` — cubic segments in **local** space; tessellate to triangles/lines in draw |
| Path payload | `points: [f64]` flat (x,y pairs) or segment list; `stroke_width`, rgba; optional closed/fill flag |
| Wires | Blueprint edge = `Path` whose control points track two node anchors (world→local); rebuild on dirty of endpoints |
| Hit | Path: fat AABB / distance-to-polyline stub OK for MVP; nodes reuse Phase A/B hit |
| Draw | Path → batch into solid/line path (reuse `solid_renderer` or thin stroke tris); rect widgets unchanged |
| MVP smoke | N≥2 drag-able rect nodes + ≥1 cubic wire; pan/zoom changes hit in screen space; no Figma product chrome |
| Precision | Keep `f64` affine / path coords |

## Steps

| Step | Item | Gate |
|------|------|------|
| 1 | Decision: camera + Path + wire + MVP contract (freeze in this file) | doc + PLAN sync |
| 2 | Camera pan/zoom helpers + smoke (screen hit moves with camera) | `scene_camera_smoke` exit 0 |
| 3 | `Path` kind: add + local cubic tessellation unit (no GL required) | `scene_path_tess_smoke` exit 0 |
| 4 | Path draw (+ optional stroke hit) smoke | `scene_path_draw_smoke` exit 0 |
| 5 | Wire helper: path between two node anchors; updates when endpoints move | `scene_wire_smoke` exit 0 |
| 6 | Blueprint MVP smoke: ≥2 nodes + wire + pan/zoom; headless token | `scene_blueprint_smoke` exit 0 |
| 7 | Critic: Decision match; Phase C spatial + Phase B form still green; no sleep | close Phase D |

### Sub-steps (Driver)

**STEP=1**
1. Freeze Decision table (amend if scene API forces change).
2. Exact `ScenePath` fields + camera helper names.
3. Non-goals: full Figma product; GPU instancing; deleting v0 demos; MSDF on paths.

**STEP=2**
1. `scene_camera_pan` / `scene_camera_zoom_at` (or equivalent) on `Scene.camera`.
2. Smoke: place rect; after pan, same screen point misses / new point hits.

**STEP=3**
1. `ScenePath` + `Path(...)` on `SceneNodeKind`; `scene_add_path`.
2. Tessellate one cubic → polyline/tris counts deterministic; token `scene_path_tess_ok`.

**STEP=4**
1. Draw path via batch (tris or line strip → solid renderer).
2. Smoke: vertex count / token `scene_path_draw_ok` (`MLC_GLFW_VISIBLE=0` if GL).

**STEP=5**
1. `scene_wire_set` / rebuild path from two node ids + local anchors.
2. Smoke: move endpoint → wire control points change; hit/draw still coherent.

**STEP=6**
1. Compose MVP example + script; stdout `scene_blueprint_ok`.
2. Phase B form + Phase C spatial still exit 0.

**STEP=7**
1. Critic: re-run Phase D + form/spatial smokes; archive; epic → next PLAN item.

## Out of scope

- Full design tool (layers, multi-page, collaboration).
- Deleting v0 `misc/gui/` demos.
- GPU instancing / persistent mapped buffers.
- Replacing Phase B widgets with vector-only UI.

## Bearing

| File | Role |
|------|------|
| `misc/gui/scene.mlc` | camera helpers; `Path` kind |
| `misc/gui/scene_path.mlc` | tessellation / wire rebuild (if split) |
| `misc/gui/scene_draw.mlc` | path batch |
| `misc/examples/scene_*_smoke.mlc` | Phase D proofs |
