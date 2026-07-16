# Track: GUI scene — Path match arms in Phase B widget smokes

Parent: [../../agent/TRACK_GUI_CANVAS_GRAPH.md](../../agent/TRACK_GUI_CANVAS_GRAPH.md),
[TRACK_GUI_SCENE_PHASE_D.md](TRACK_GUI_SCENE_PHASE_D.md).
Epic residual after Phase D Critic (`18d90c85`): `Path` on `SceneNodeKind`
made Phase B widget smokes non-exhaustive (E084).

## Status: **closed** (2026-07-16) — Critic OK

**Critic 2026-07-16 (STEP=2):** Decision match — match arms only; `Path(_) =>`
sentinel. Re-ran label/checkbox/slider/text_field + form/spatial + Phase D
(camera/tess/draw/wire/blueprint) → all `*_ok`. No `sleep`. Anti-false-done:
`57015273`…`227a82c4`. **reopen: none**. Residuals: `scene_form_live` still
lacks `Path(_)` (foreign WIP, out of scope); Path fill / precise stroke hit
deferred.

## Next step

**closed** — Critic OK. Queue → Planner.

### STEP=2 done (2026-07-16)

- Critic: re-ran widget + form/spatial + Phase D smokes; archive.

### STEP=1 done in git (2026-07-16)

| Step | Commit (abbrev) | Gate |
|------|-----------------|------|
| open | `57015273` | PLAN §34 |
| 1 | `227a82c4` | four widget `*_ok` |

## Decision (frozen) — summary

Scope = smoke match arms only; no `scene.mlc` API change; leave form_live.

## Out of scope (unchanged)

`scene_form_live`; Path fill; precise stroke hit; SCRIPT_VM; MIR Epic 5;
LANG_AUTO_CYCLE.
