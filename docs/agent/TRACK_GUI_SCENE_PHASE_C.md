# Track: GUI scene Phase C — dirty-tracking + spatial index

Parent: [TRACK_GUI_CANVAS_GRAPH.md](TRACK_GUI_CANVAS_GRAPH.md).
Epic §29 / §10c in [../PLAN.md](../PLAN.md).
Phase A (closed): [../archive/tracks/TRACK_GUI_SCENE_PHASE_A.md](../archive/tracks/TRACK_GUI_SCENE_PHASE_A.md).
Phase B (closed): [../archive/tracks/TRACK_GUI_SCENE_PHASE_B.md](../archive/tracks/TRACK_GUI_SCENE_PHASE_B.md).

## Status: **priority / active** (2026-07-14) — queue head after Phase B close

Phase A/B stay. Phase C adds **incremental world update** + optional
**AABB spatial index** on the same flat `Scene` (not a second graph).

## Next step

**STEP=4** — World AABB pass.

### STEP=3 done (2026-07-14)

- `scene_update_world_dirty` + `scene_set_node_local`; clears dirty after write.
- `misc/examples/scene_world_dirty_smoke.mlc` + script → `scene_world_dirty_ok`.

### STEP=2 done (2026-07-14)

- `Scene.dirty`; `scene_mark_dirty` / `scene_mark_all_dirty` / `scene_clear_dirty`; add/set hooks.
- `misc/examples/scene_dirty_smoke.mlc` + `scripts/run_scene_dirty_smoke.sh` → `scene_dirty_ok`.

### STEP=1 done (2026-07-14)

- Decision frozen below (dirty/AABB/threshold/API).
- PLAN §10c/§29 → STEP=2 next.

### STEP=0 done (2026-07-14)

- Planner opened this track; PLAN §10c/§29 → Phase C active.

## Decision (STEP=1) — **frozen** 2026-07-14

Grounded in Phase A/B: flat `Scene.nodes` / `world[]` / `camera`,
`scene_update_world` full pass, kind local AABB `[0,w)×[0,h)`, `f64`.

| Item | Choice |
|------|--------|
| Keep | `Scene.nodes` / `world` / `camera` unchanged in meaning |
| Dirty store | On `Scene`: `dirty: [i32]` (0\|1), length == `nodes.length()` |
| Mark | `scene_mark_dirty(scene, node_id)` sets dirty on node **and all descendants** (BFS/DFS via `children`) |
| When mark | `scene_add_*`, local/transform mutators, size-changing payload setters; camera change marks all (or dedicated `scene_mark_all_dirty`) |
| World | Keep `scene_update_world` (full). Add `scene_update_world_dirty`: skip node if `dirty[i]==0` **and** parent world not dirty this pass; clear dirty after write |
| Equivalence | After any mark set, `scene_update_world_dirty` world matrices must match `scene_update_world` |
| AABB type | `SceneAabb { min_x, min_y, max_x, max_y: f64 }` |
| AABB store | On `Scene`: `world_bounds: [SceneAabb]`, parallel to `nodes` |
| AABB source | Local size from kind (`RectFill`/`Label`/… width×height; `Container` → empty/degenerate); transform four corners by `world[i]`; min/max |
| Bounds pass | `scene_update_world_bounds` after world (or folded into dirty world pass for dirty nodes only) |
| Threshold | `SCENE_SPATIAL_THRESHOLD = 64` — quadtree **off** when `nodes.length() < 64` |
| Quadtree | Separate module or section; rebuild/update from `world_bounds`; point query → candidate ids |
| Hit/draw | `< threshold`: existing linear `scene_hit_test` / `scene_draw_*`. `>= threshold`: query candidates, then same per-node logic |
| Precision | `f64` affine + AABB |

### Exact types / constants

```text
SceneAabb = { min_x, min_y, max_x, max_y: f64 }

Scene = {
  nodes: [SceneNode],
  world: [Affine2x3],
  camera: Affine2x3,
  dirty: [i32],           // Phase C
  world_bounds: [SceneAabb] // Phase C
}

SCENE_SPATIAL_THRESHOLD = 64
```

### API surface (STEPs 2–6)

- `scene_mark_dirty` / `scene_mark_all_dirty`
- `scene_update_world_dirty`
- `scene_update_world_bounds` (or combined)
- `scene_quadtree_*` insert/query (STEP=5)
- `scene_hit_test` / draw: threshold branch (STEP=6)
- Smokes: no `sleep`; no GLFW required for STEPs 2–5

### Non-goals (Decision)

Phase D camera/bezier/wires; GPU instancing; MSDF glyphs; deleting v0 demos;
persistent mapped GPU buffers; changing Phase B widget payloads.

## Steps

| Step | Item | Gate |
|------|------|------|
| 1 | Decision: dirty + AABB + threshold + API (freeze in this file) | **done** (2026-07-14): Decision frozen |
| 2 | Dirty flags + `scene_mark_dirty` / mutator hooks + smoke | **done** (2026-07-14): `scene_dirty_ok` |
| 3 | Incremental `scene_update_world_dirty` ≡ full update on dirty set | `scene_world_dirty_smoke` exit 0 |
| 4 | World AABB pass from kind local size | `scene_aabb_smoke` exit 0 |
| 5 | Quadtree insert + point/range query (unit, no GL) | `scene_quadtree_smoke` exit 0 |
| 6 | Threshold integration: cull/query smoke above/below threshold | `scene_spatial_smoke` exit 0 |
| 7 | Critic: Decision match; Phase B smokes still green; no sleep | close Phase C |

### Sub-steps (Driver)

**STEP=1** — **done**
1. Freeze Decision table — done.
2. Exact types / threshold listed — done.
3. Non-goals noted — done.

**STEP=2** — **done**
1. Add `dirty: [i32]` on `Scene`; `scene_mark_dirty` — done.
2. Hook add/set helpers — done.
3. `scene_dirty_smoke` + script — done.

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
