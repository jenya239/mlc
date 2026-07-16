# Track: GUI scene — Path match arms in Phase B widget smokes

Parent: [TRACK_GUI_CANVAS_GRAPH.md](TRACK_GUI_CANVAS_GRAPH.md),
[../archive/tracks/TRACK_GUI_SCENE_PHASE_D.md](../archive/tracks/TRACK_GUI_SCENE_PHASE_D.md).
Epic residual after Phase D Critic (`18d90c85`): `Path` on `SceneNodeKind`
made several Phase B smokes non-exhaustive (E084). Form smoke fixed in
STEP=6; label/checkbox/slider/text_field still red.

## Status: **active** (2026-07-16) — queue head after Phase D

## Next step

**STEP=2** — Critic: full `scene_*_smoke` + form/spatial; archive.

### STEP=1 done (2026-07-16)

- `Path(_)` arms in label/checkbox/slider/text_field smokes.
- Four scripts → `*_ok`.

## Decision (frozen) — minimal

| Item | Choice |
|------|--------|
| Scope | Match arms only in smoke helpers; no `scene.mlc` API change |
| Arms | `Path(_) =>` same sentinel as other non-target kinds (empty string / `-1`) |
| Out | `scene_form_live.mlc` (foreign WIP); Path fill; precise stroke hit; SCRIPT_VM |

## Steps

| Step | Item | Gate |
|------|------|------|
| 1 | `Path(_)` in `scene_{label,checkbox,slider,text_field}_smoke.mlc` | **done** (2026-07-16): four `*_ok` |
| 2 | Critic: full `scene_*_smoke` + form/spatial; archive | close |

### Sub-steps (Driver)

**STEP=1** — **done**
1. Add `Path(_)` to every `match … kind` in the four files — done.
2. Four smoke scripts → ok — done.

**STEP=2**
1. Critic: re-run Phase B widget + form/spatial + Phase D blueprint smokes; archive.

## Out of scope

- Absorbing `scene_form_live` / EDITOR.md foreign dirty.
- Path fill / precise stroke hit (Phase D deferred).
- `TRACK_LANG_AUTO_CYCLE` / SCRIPT_VM / MIR Epic 5.
