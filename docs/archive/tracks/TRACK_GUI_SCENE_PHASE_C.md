# Track: GUI scene Phase C — dirty-tracking + spatial index

Parent: [../../agent/TRACK_GUI_CANVAS_GRAPH.md](../../agent/TRACK_GUI_CANVAS_GRAPH.md).
Epic §29 / §10c in [../../PLAN.md](../../PLAN.md).
Phase A (closed): [TRACK_GUI_SCENE_PHASE_A.md](TRACK_GUI_SCENE_PHASE_A.md).
Phase B (closed): [TRACK_GUI_SCENE_PHASE_B.md](TRACK_GUI_SCENE_PHASE_B.md).

## Status: **closed** (2026-07-15) — Critic OK

**Critic 2026-07-15 (STEP=7):** Decision match — `Scene.dirty` + `world_bounds`/`SceneAabb`;
`scene_mark_dirty` / `scene_update_world_dirty` / `scene_update_world_bounds`;
quadtree module + `scene_spatial_threshold()==64`; hit/draw candidates above threshold.
Re-ran dirty/world_dirty/aabb/quadtree/spatial + form/label → all ok. No `sleep` in
Phase C smokes. `misc/examples/gui_button_demo.mlc` + `layout.mlc` present.
Anti-false-done: `d0a3996f`…`64014467`. **reopen: none**. Residuals: nested
if/else codegen can truncate `build_from_scene` (mitigated with fixed 800×600 root);
`scene_hit_test` in `scene.mlc` stays linear compat — spatial path is
`scene_hit_test_spatial`.

## Next step

**closed** — Critic OK. Queue → Planner (Phase D eligible).

### STEP=7 done (2026-07-15)

- Critic: re-ran Phase C + form/label smokes; Decision audit; archive; epic → Phase D Planner.

### STEPs 1–6 done in git (2026-07-14)

| Step | Commit (abbrev) | Gate |
|------|-----------------|------|
| 1 | `d0a3996f` | Decision freeze |
| 2 | `fd22298b` | `scene_dirty_ok` |
| 3 | `20fb095e` | `scene_world_dirty_ok` |
| 4 | `ba472f44` | `scene_aabb_ok` |
| 5 | `74b924fc` | `scene_quadtree_ok` |
| 6 | `64014467` | `scene_spatial_ok` |

### STEP=0 done (2026-07-14)

- Planner opened this track; PLAN §10c/§29 → Phase C active.

## Decision (STEP=1) — frozen

| Item | Direction |
|------|-----------|
| Dirty | Parallel `dirty: [i32]` (0\|1) same length as `nodes`; mark node + descendants |
| World | Keep full `scene_update_world`; add `scene_update_world_dirty` |
| Mutators | `scene_add_*` / setters mark dirty |
| AABB | Parallel `world_bounds: [SceneAabb]`; from local size × world corners |
| Quadtree | Optional; **off** when `nodes.length() < 64` |
| Hit/draw | Below threshold: linear; above: query candidates then same hit/draw |
| Precision | `f64` affine / AABB |

## Steps

| Step | Item | Gate |
|------|------|------|
| 1 | Decision: dirty + AABB + threshold + API | **done** |
| 2 | Dirty flags + `scene_mark_dirty` + smoke | **done**: `scene_dirty_ok` |
| 3 | Incremental `scene_update_world_dirty` | **done**: `scene_world_dirty_ok` |
| 4 | World AABB pass | **done**: `scene_aabb_ok` |
| 5 | Quadtree insert + point/AABB query | **done**: `scene_quadtree_ok` |
| 6 | Threshold integration hit/draw | **done**: `scene_spatial_ok` |
| 7 | Critic | **done** (2026-07-15): closed |

## Out of scope

- Phase D camera pan/zoom, bezier wires, blueprint MVP.
- Deleting v0 `misc/gui/` demos.
- Full MSDF label rendering.
- GPU instancing / persistent mapped buffers.

## Bearing

| File | Role |
|------|------|
| `misc/gui/scene.mlc` | tree + dirty + world + AABB |
| `misc/gui/scene_draw.mlc` | rect-fill batch + spatial candidates |
| `misc/gui/scene_quadtree.mlc` | AABB spatial index |
| `misc/gui/scene_spatial.mlc` | threshold + spatial hit/draw helpers |
| `misc/examples/scene_*_smoke.mlc` | Phase A/B/C proofs |
