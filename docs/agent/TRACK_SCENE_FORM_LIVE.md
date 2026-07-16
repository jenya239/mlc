# Track: Scene Phase B form live demo (absorb foreign WIP)

Parent: [TRACK_GUI_CANVAS_GRAPH.md](TRACK_GUI_CANVAS_GRAPH.md),
[../archive/tracks/TRACK_GUI_SCENE_PHASE_B.md](../archive/tracks/TRACK_GUI_SCENE_PHASE_B.md),
[../archive/tracks/TRACK_GUI_SCENE_PATH_MATCH.md](../archive/tracks/TRACK_GUI_SCENE_PATH_MATCH.md).
Trigger: after §34 Critic — `misc/examples/scene_form_live.mlc` + runner
were foreign untracked WIP and still lack `Path(_)` (E084).

## Status: **active** (2026-07-16) — queue head after §34

## Next step

**STEP=1** — `Path(_)` arms + commit live example; compile/link gate (no interactive hang).

## Decision (frozen) — minimal

| Item | Choice |
|------|--------|
| Absorb | Commit `scene_form_live.mlc` + `run_scene_form_live.sh` into tree |
| Path | Same sentinel arms as form smoke |
| Gate | Compile+link only → stdout `scene_form_live_compile_ok` (do **not** `exec` interactive loop in CI) |
| Interactive | `MLC_GLFW_VISIBLE=1 scripts/run_scene_form_live.sh` remains manual Esc-quit |
| Out | EDITOR.md / demo_live dirty; Path fill; SCRIPT_VM; LANG_AUTO_CYCLE; MIR Epic 5 |

## Steps

| Step | Item | Gate |
|------|------|------|
| 1 | Path arms + commit live + compile script | `scene_form_live_compile_ok` |
| 2 | Critic: compile gate + form smoke still green; archive | close |

### Sub-steps (Driver)

**STEP=1**
1. Add `Path(_)` to all `match … kind` in `scene_form_live.mlc`.
2. Add `scripts/run_scene_form_live_compile.sh` (build, print token, no exec).
3. Commit example + both scripts.

**STEP=2**
1. Critic: re-run compile gate + `run_scene_form_smoke.sh`; archive.

## Out of scope

- Making live demo headless auto-quit / UX harness.
- Absorbing `misc/editor/demo_live.mlc` foreign dirty.
- Path fill / precise stroke hit.
