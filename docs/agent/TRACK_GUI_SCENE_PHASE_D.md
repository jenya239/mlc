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

**STEP=3** — Path kind + cubic tessellation.

### STEP=2 done (2026-07-15)

- `affine_scale`; `scene_camera_pan` / `scene_camera_zoom_at` (+ `scene_mark_all_dirty`).
- `misc/examples/scene_camera_smoke.mlc` + script → `scene_camera_ok`.

### STEP=1 done (2026-07-15)

- Decision frozen below (camera/Path/wire/MVP).
- PLAN §10c/§29 → STEP=2 next.

### STEP=0 done (2026-07-15)

- Planner opened this track; PLAN §10c/§29 → Phase D active.

## Decision (STEP=1) — **frozen** 2026-07-15

Grounded in Phase A–C: `Scene.camera: Affine2x3`, hit =
`inverse(camera)` then `inverse(world)` → local; `SceneNodeKind` sum;
`f64` affine; dirty/AABB/spatial unchanged.

| Item | Choice |
|------|--------|
| Keep | `Scene.nodes` / `world` / `camera` / `dirty` / `world_bounds` meaning |
| Camera map | `camera` maps **scene → screen** (same as Phase A hit: screen→`invert(camera)`→scene) |
| Pan | `scene_camera_pan(scene, dx, dy)`: `camera' = affine_multiply(affine_translation(dx, dy), camera)` |
| Zoom | `scene_camera_zoom_at(scene, screen_x, screen_y, factor)`: zoom about screen point (translate→scale→translate); `factor>0` |
| Camera dirty | Pan/zoom call `scene_mark_all_dirty` (world matrices unchanged; bounds/hit use new camera without world rebuild — optional no-op mark OK if hit only uses camera) |
| Path kind | Add `Path(ScenePath)` on `SceneNodeKind`; keep all Phase B kinds |
| Path geometry | Flat `points: [f64]` — **one or more cubics**, each **8** floats: `p0x,p0y, c1x,c1y, c2x,c2y, p1x,p1y` (local space) |
| Path style | `stroke_width: f64`; `filled: i32` (0\|1); rgba; MVP STEPs 3–4 may implement **stroke only** (`filled` reserved) |
| Tessellation | Fixed `SCENE_PATH_CUBIC_STEPS = 8` samples per cubic → polyline; stroke ribbon = 2 tris per polyline edge (draw) |
| Path hit | Fat AABB of control points expanded by `stroke_width/2` in local→world; precise polyline distance **not** required for MVP |
| Path AABB | `scene_kind_local_*` for Path = AABB of control points (+ stroke pad); feeds Phase C `world_bounds` |
| Wire | Blueprint edge **is** a `Path` node; `scene_wire_rebuild(scene, path_node_id, from_id, to_id, from_ax, from_ay, to_ax, to_ay)` writes one cubic: p0=from anchor in path-local, p1=to anchor, c1/c2 = horizontal offsets `(dx/3)` style |
| Wire endpoints | Stored only as rebuild args each frame/smoke (no new Scene fields); optional later cache on `ScenePath` if needed |
| Draw | Path stroke → `solid_renderer` tris via `scene_draw`; widgets unchanged |
| MVP | ≥2 `RectFill` nodes + ≥1 wire `Path` + pan/zoom; headless tokens; no product chrome |
| Precision | `f64` throughout |

### Exact types / constants

```text
ScenePath = {
  points: [f64],       // length % 8 == 0
  stroke_width: f64,
  filled: i32,         // 0|1; fill deferred OK
  red, green, blue, alpha: f64
}

SceneNodeKind += Path(ScenePath)

SCENE_PATH_CUBIC_STEPS = 8
```

### API surface (STEPs 2–6)

- `scene_camera_pan` / `scene_camera_zoom_at`
- `scene_add_path` / `scene_path_set_points` (mark dirty)
- `scene_path_polyline` / tess count helper (STEP=3)
- Path arm in `scene_draw` + optional hit (STEP=4)
- `scene_wire_rebuild` (STEP=5)
- Smokes: no `sleep`; GLFW only if draw flush requires it (`MLC_GLFW_VISIBLE=0`)

### Non-goals (Decision)

Full Figma product; GPU instancing; deleting v0 demos; MSDF on paths;
changing Phase B widget payloads; reopening Phase C spatial contract.

## Steps

| Step | Item | Gate |
|------|------|------|
| 1 | Decision: camera + Path + wire + MVP contract (freeze in this file) | **done** (2026-07-15): Decision frozen |
| 2 | Camera pan/zoom helpers + smoke (screen hit moves with camera) | `scene_camera_smoke` exit 0 |
| 3 | `Path` kind: add + local cubic tessellation unit (no GL required) | `scene_path_tess_smoke` exit 0 |
| 4 | Path draw (+ optional stroke hit) smoke | `scene_path_draw_smoke` exit 0 |
| 5 | Wire helper: path between two node anchors; updates when endpoints move | `scene_wire_smoke` exit 0 |
| 6 | Blueprint MVP smoke: ≥2 nodes + wire + pan/zoom; headless token | `scene_blueprint_smoke` exit 0 |
| 7 | Critic: Decision match; Phase C spatial + Phase B form still green; no sleep | close Phase D |

### Sub-steps (Driver)

**STEP=1** — **done**
1. Freeze Decision table — done.
2. Exact `ScenePath` fields + camera helper names — done.
3. Non-goals noted — done.

**STEP=2**
1. `scene_camera_pan` / `scene_camera_zoom_at` on `Scene.camera`.
2. Smoke: place rect; after pan, same screen point misses / new point hits.

**STEP=3**
1. `ScenePath` + `Path(...)` on `SceneNodeKind`; `scene_add_path`.
2. Tessellate one cubic → polyline/tris counts deterministic; token `scene_path_tess_ok`.

**STEP=4**
1. Draw path via batch (tris → solid renderer).
2. Smoke: vertex count / token `scene_path_draw_ok` (`MLC_GLFW_VISIBLE=0` if GL).

**STEP=5**
1. `scene_wire_rebuild` from two node ids + local anchors.
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
