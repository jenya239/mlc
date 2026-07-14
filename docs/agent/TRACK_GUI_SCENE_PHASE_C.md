# Track: GUI scene Phase C — dirty-tracking + spatial index

Parent: [TRACK_GUI_CANVAS_GRAPH.md](TRACK_GUI_CANVAS_GRAPH.md).
Epic §29 / §10c in [../PLAN.md](../PLAN.md).
Phase A (closed): [../archive/tracks/TRACK_GUI_SCENE_PHASE_A.md](../archive/tracks/TRACK_GUI_SCENE_PHASE_A.md).
Phase B (closed): [../archive/tracks/TRACK_GUI_SCENE_PHASE_B.md](../archive/tracks/TRACK_GUI_SCENE_PHASE_B.md).

## Status: **priority / active** (2026-07-14) — queue head after Phase B close

Phase A/B stay. Phase C adds **incremental world update** + optional
**AABB spatial index** on the same flat `Scene` (not a second graph).

## Next step

**STEP=1** — Decision: dirty model, AABB, quadtree threshold, API.

### STEP=0 done (2026-07-14)

- Planner opened this track; PLAN §10c/§29 → Phase C active.

## Decision (STEP=1) — draft until Driver freezes

| Item | Direction |
|------|-----------|
| Dirty | Parallel `dirty: [i32]` (0\|1) same length as `nodes`; mark node + descendants when `local`/kind size changes |
| World | Keep full `scene_update_world`; add `scene_update_world_dirty` that skips clean nodes when parent world already fresh |
| Mutators | Existing `scene_*` setters/`scene_add_*` mark dirty (or thin `scene_mark_dirty` + call sites) |
| AABB | Parallel `world_bounds: [SceneAabb]` (min_x/min_y/max_x/max_y f64); from local size × world corners |
| Quadtree | Optional index rebuilt/updated from AABBs; **off** when `nodes.length() < threshold` (draft: 64) |
| Hit/draw | Below threshold: keep linear Phase A paths; above: query candidates then same hit/draw |
| Precision | Keep `f64` affine / AABB |

## Steps

| Step | Item | Gate |
|------|------|------|
| 1 | Decision: dirty + AABB + threshold + API (freeze in this file) | doc + PLAN sync |
| 2 | Dirty flags + `scene_mark_dirty` / mutator hooks + smoke | `scene_dirty_smoke` exit 0 |
| 3 | Incremental `scene_update_world_dirty` ≡ full update on dirty set | `scene_world_dirty_smoke` exit 0 |
| 4 | World AABB pass from kind local size | `scene_aabb_smoke` exit 0 |
| 5 | Quadtree insert + point/range query (unit, no GL) | `scene_quadtree_smoke` exit 0 |
| 6 | Threshold integration: cull/query smoke above/below threshold | `scene_spatial_smoke` exit 0 |
| 7 | Critic: Decision match; Phase B smokes still green; no sleep | close Phase C |

### Sub-steps (Driver)

**STEP=1**
1. Freeze Decision table (amend if scene API forces change).
2. List exact types: `SceneAabb`, dirty array ownership, threshold constant.
3. Non-goals: Phase D camera/bezier/wires; GPU instancing; MSDF glyphs.

**STEP=2**
1. Add `dirty: [i32]` on `Scene` (or parallel store); `scene_mark_dirty(scene, node_id)`.
2. Hook add/set helpers that change transform or size.
3. `misc/examples/scene_dirty_smoke.mlc` + script → `scene_dirty_ok`.

**STEP=3**
1. `scene_update_world_dirty`: recompute only dirty nodes (parent world required).
2. Smoke: mutate one leaf → world matches full `scene_update_world`; clean siblings unchanged identity optional.

**STEP=4**
1. `scene_update_world_bounds` (or fold into dirty world pass).
2. Smoke: known rect under translation → expected AABB.

**STEP=5**
1. `misc/gui/scene_quadtree.mlc` (or section in `scene.mlc`): insert AABB + query point.
2. Smoke: N non-overlapping rects; query hits one id.

**STEP=6**
1. Wire threshold: below → linear; above → query then hit/draw candidates.
2. Smoke token `scene_spatial_ok`; Phase B form still exit 0.

**STEP=7**
1. Critic: re-run Phase C + Phase B form/label smokes; archive; epic → Phase D Planner later.

## Out of scope

- Phase D camera pan/zoom, bezier wires, blueprint MVP.
- Deleting v0 `misc/gui/` demos.
- Full MSDF label rendering.
- GPU instancing / persistent mapped buffers.

## Bearing

| File | Role |
|------|------|
| `misc/gui/scene.mlc` | tree + dirty + world + AABB |
| `misc/gui/scene_draw.mlc` | rect-fill batch (cull filter later) |
| `misc/gui/scene_quadtree.mlc` | optional spatial index (if split out) |
| `misc/examples/scene_*_smoke.mlc` | Phase A/B/C proofs |
