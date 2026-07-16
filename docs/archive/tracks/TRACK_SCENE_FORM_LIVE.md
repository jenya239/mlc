# Track: Scene Phase B form live demo (absorb foreign WIP)

Parent: [../../agent/TRACK_GUI_CANVAS_GRAPH.md](../../agent/TRACK_GUI_CANVAS_GRAPH.md),
[TRACK_GUI_SCENE_PHASE_B.md](TRACK_GUI_SCENE_PHASE_B.md),
[TRACK_GUI_SCENE_PATH_MATCH.md](TRACK_GUI_SCENE_PATH_MATCH.md).

## Status: **closed** (2026-07-16) — Critic OK

**Critic 2026-07-16 (STEP=2):** Decision match — absorb live example; `Path(_)`
sentinels; compile/link gate without interactive `exec`. Re-ran
`run_scene_form_live_compile.sh` → `scene_form_live_compile_ok`;
`run_scene_form_smoke.sh` → `scene_form_ok`. Anti-false-done:
`7969cd73`…`47839f7f`. **reopen: none**. Residuals: interactive Esc-quit
only (no headless auto-quit); EDITOR.md / demo_live foreign dirty left.

## Next step

**closed** — Critic OK. Queue → Planner.

### STEP=2 done (2026-07-16)

- Critic: compile gate + form smoke; archive.

### STEPs open–1 done in git (2026-07-16)

| Step | Commit (abbrev) | Gate |
|------|-----------------|------|
| open | `7969cd73` | PLAN §35 |
| 1 | `47839f7f` | `scene_form_live_compile_ok` |

## Decision (frozen) — summary

Absorb `scene_form_live.mlc` + runners; Path arms; CI = compile/link only;
interactive = manual Esc.

## Out of scope (unchanged)

demo_live/EDITOR dirty; Path fill; UX harness auto-quit; SCRIPT_VM;
LANG_AUTO_CYCLE; MIR Epic 5.
