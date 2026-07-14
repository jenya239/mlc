# Track: GUI scene Phase B — widgets on retained scene

Parent: [TRACK_GUI_CANVAS_GRAPH.md](TRACK_GUI_CANVAS_GRAPH.md).
Epic §29 / §10c in [../PLAN.md](../PLAN.md).
Phase A (closed): [../archive/tracks/TRACK_GUI_SCENE_PHASE_A.md](../archive/tracks/TRACK_GUI_SCENE_PHASE_A.md).

## Status: **priority / active** (2026-07-14) — queue head after UX_HEADLESS close

Phase A foundation (`scene.mlc` / `scene_draw.mlc`) stays. Phase B adds
widget **kinds** + interaction on the same tree (not a second UI toolkit).

## Next step

**STEP=1** — Decision: widget kinds, local state storage, hit/draw contract.

### STEP=0 done (2026-07-14)

- Planner opened this track; PLAN §10c/§29 → Phase B active.

## Decision (STEP=1) — draft until Driver freezes

| Item | Direction |
|------|-----------|
| Kinds | Extend `SceneNodeKind`: `Label`, `Checkbox`, `Slider`, `TextField` (+ keep `Container`/`RectFill`) |
| State | Per-kind payload on the variant (checked, value, text buffer) — no parallel widget array |
| Hit | Reuse `scene_hit_test`; kind-specific local hit boxes |
| Draw | Batch rect fills via existing `scene_draw`; text via existing text path / stub glyph until MSDF wire |
| Input | Pointer from scene hit; keyboard for TextField via `gui_input` inject (`TRACK_GUI_INPUT_ROBUSTNESS`) |
| Precision | Keep `f64` affine (Phase A amendment) |

## Steps

| Step | Item | Gate |
|------|------|------|
| 1 | Decision: kinds + state + hit/draw/input contract (freeze in this file) | doc + PLAN sync |
| 2 | `Label` kind: add + layout size + draw smoke (string → rect/text stub) | `scene_label_smoke` exit 0 |
| 3 | `Checkbox` kind: hit toggles checked; draw box+mark | `scene_checkbox_smoke` exit 0 |
| 4 | `Slider` kind: hit/drag sets value `[0,1]`; draw track+thumb | `scene_slider_smoke` exit 0 |
| 5 | `TextField` kind: focus + inject keys/text; caret model | `scene_text_field_smoke` exit 0 |
| 6 | Form smoke: one scene with label+checkbox+slider+field; headless inject | `scene_form_smoke` exit 0 |
| 7 | Critic: kinds match Decision; no sleep; v0 demos untouched | close Phase B |

### Sub-steps (Driver)

**STEP=1**
1. Freeze Decision table in this file (amend if scene API forces change).
2. List new `SceneNodeKind` variants + payload fields.
3. Note non-goals: Phase C dirty/quadtree; Phase D camera/bezier.

**STEP=2**
1. Extend `SceneNodeKind` + `scene_add_label` in `misc/gui/scene.mlc`.
2. Draw path in `scene_draw.mlc` (rect + optional text stub).
3. `misc/examples/scene_label_smoke.mlc` + `scripts/run_scene_label_smoke.sh`.

**STEP=3**
1. `Checkbox` payload `{ checked: i32, … }`; toggle on hit.
2. Smoke: click → checked 0→1→0.

**STEP=4**
1. `Slider` payload `{ value: f64, … }`; drag updates value.
2. Smoke: inject drag → value in range.

**STEP=5**
1. `TextField` + focus flag; wire `gui_input` text/backspace inject.
2. Smoke: type → buffer equals expected (no GLFW required).

**STEP=6**
1. Single example composing all four widgets.
2. Script `MLC_GLFW_VISIBLE=0`; stdout token `scene_form_ok`.

**STEP=7**
1. Critic gate: re-run Phase B smokes; archive this track; epic → Phase C Planner later.

## Out of scope

- Phase C dirty-tracking / quadtree.
- Phase D camera pan/zoom, bezier wires, blueprint MVP.
- Deleting v0 `misc/gui/` layout/button demos.
- Full IDE chrome (editor lives under `misc/editor/`).

## Bearing

| File | Role |
|------|------|
| `misc/gui/scene.mlc` | tree + Affine2x3 + hit |
| `misc/gui/scene_draw.mlc` | rect-fill batch |
| `misc/gui/input.mlc` | inject mouse/keys/text |
| `misc/examples/scene_*_smoke.mlc` | Phase A proofs — keep green |
