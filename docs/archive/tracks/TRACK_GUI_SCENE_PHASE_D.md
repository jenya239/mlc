# Track: GUI scene Phase D — camera + vector path + blueprint MVP

Parent: [../../agent/TRACK_GUI_CANVAS_GRAPH.md](../../agent/TRACK_GUI_CANVAS_GRAPH.md).
Epic §29 / §10c in [../../PLAN.md](../../PLAN.md).
Phase A (closed): [TRACK_GUI_SCENE_PHASE_A.md](TRACK_GUI_SCENE_PHASE_A.md).
Phase B (closed): [TRACK_GUI_SCENE_PHASE_B.md](TRACK_GUI_SCENE_PHASE_B.md).
Phase C (closed): [TRACK_GUI_SCENE_PHASE_C.md](TRACK_GUI_SCENE_PHASE_C.md).

## Status: **closed** (2026-07-16) — Critic OK

**Critic 2026-07-16 (STEP=7):** Decision match — `Scene.camera` pan/zoom;
`Path(ScenePath)` + `SCENE_PATH_CUBIC_STEPS=8` tess; stroke ribbon draw;
`scene_wire_rebuild`; blueprint MVP (≥2 RectFill + wire + pan/zoom).
Re-ran camera/path_tess/path_draw/wire/blueprint + form/spatial → all ok.
No `sleep` in Phase D/form/spatial smokes. Anti-false-done:
`d9cc60de`…`9e3eaebf` (STEP=1–6). **reopen: none**. Residuals: Path fill
deferred; precise stroke hit deferred; widget smokes
`scene_{checkbox,slider,text_field}_smoke` still lack `Path(_)` match arms
(form smoke fixed in STEP=6).

## Next step

**closed** — Critic OK. Queue → Planner (epic §10c/§29 Phase A–D done).

### STEP=7 done (2026-07-16)

- Critic: re-ran Phase D + form/spatial; Decision audit; archive; epic → Planner.

### STEPs 1–6 done in git (2026-07-15…16)

| Step | Commit (abbrev) | Gate |
|------|-----------------|------|
| 1 | `d9cc60de` | Decision freeze |
| 2 | `c23459e9` | `scene_camera_ok` |
| 3 | `9f0c9b2e` | `scene_path_tess_ok` |
| 4 | `ec968e68` | `scene_path_draw_ok` |
| 5 | `e66e62cb` | `scene_wire_ok` |
| 6 | `9e3eaebf` | `scene_blueprint_ok` (+ form/spatial) |

### STEP=0 done (2026-07-15)

- Planner opened Phase D (`c943151a`).

## Decision (STEP=1) — frozen summary

Camera = scene→screen via `Scene.camera`; Path = cubics in flat `points`
(8 floats each), stroke MVP; wire = Path + `scene_wire_rebuild`; blueprint
MVP = ≥2 RectFill + wire + pan/zoom; `f64`; no product chrome.

## Out of scope (unchanged)

Full Figma product; GPU instancing; deleting v0 demos; MSDF on paths;
changing Phase B widget payloads; reopening Phase C spatial contract.
