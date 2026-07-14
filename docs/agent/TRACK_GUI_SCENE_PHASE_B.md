# Track: GUI scene Phase B — widgets on retained scene

Parent: [TRACK_GUI_CANVAS_GRAPH.md](TRACK_GUI_CANVAS_GRAPH.md).
Epic §29 / §10c in [../PLAN.md](../PLAN.md).
Phase A (closed): [../archive/tracks/TRACK_GUI_SCENE_PHASE_A.md](../archive/tracks/TRACK_GUI_SCENE_PHASE_A.md).

## Status: **priority / active** (2026-07-14) — queue head after UX_HEADLESS close

Phase A foundation (`scene.mlc` / `scene_draw.mlc`) stays. Phase B adds
widget **kinds** + interaction on the same tree (not a second UI toolkit).

## Next step

**STEP=3** — `Checkbox` kind + smoke.

### STEP=2 done (2026-07-14)

- `SceneLabel` + `Label(SceneLabel)` on `SceneNodeKind`; `scene_add_label` / hit / draw-as-rect.
- `misc/examples/scene_label_smoke.mlc` + `scripts/run_scene_label_smoke.sh` → `scene_label_ok`.
- Phase A types+hit still exit 0 after kind extend.

### STEP=1 done (2026-07-14)

- Decision frozen below (payload fields + hit/draw/input).
- PLAN §10c/§29 → STEP=2 next.

### STEP=0 done (2026-07-14)

- Planner opened this track; PLAN §10c/§29 → Phase B active.

## Decision (STEP=1) — **frozen** 2026-07-14

Grounded in Phase A: flat `Scene.nodes`, `SceneNodeKind` sum type, `scene_hit_test`
front-to-back, `scene_draw` rect-fill batch, `f64` affine.

| Item | Choice |
|------|--------|
| Kinds | Extend `SceneNodeKind` only — no parallel widget store |
| Keep | `Container` \| `RectFill(SceneRectFill)` unchanged |
| Add | `Label(SceneLabel)` \| `Checkbox(SceneCheckbox)` \| `Slider(SceneSlider)` \| `TextField(SceneTextField)` |
| Hit | Extend `scene_hit_test` match arms; local AABB `[0,w)×[0,h)` like `RectFill` |
| Draw | Widgets emit rect fills via `scene_draw` batch; **text glyphs deferred** (Label/TextField Phase B = colored rect + `string` in payload for model/smoke; MSDF draw = follow-up, not STEP=2–6 blocker) |
| Pointer | Click/drag on hit node id; mutate payload via `scene_set_*` helpers returning new `Scene` |
| Keyboard | `TextField` only: apply `GuiInput.text_buffer` / backspace when `focused==1`; inject via `gui_input_*_test_*` |
| Focus | At most one `TextField` with `focused=1` per scene (clear others on focus) |
| Precision | `f64` affine + sizes; slider `value` in `[0.0, 1.0]` |

### Payload fields (exact)

```text
SceneLabel     { width, height: f64; text: string; red, green, blue, alpha: f64 }
SceneCheckbox  { width, height: f64; checked: i32; /* 0|1 */ red, green, blue, alpha: f64 }
SceneSlider    { width, height: f64; value: f64; /* clamp 0..1 */ red, green, blue, alpha: f64 }
SceneTextField { width, height: f64; text: string; focused: i32; caret: i32; red, green, blue, alpha: f64 }
```

### API surface (STEPs 2–5)

- `scene_add_label` / `scene_add_checkbox` / `scene_add_slider` / `scene_add_text_field`
- `scene_checkbox_toggle(scene, node_id)`, `scene_slider_set_value`, `scene_text_field_apply_input`
- Smokes: no `sleep`; `MLC_GLFW_VISIBLE=0` when GL flush used

### Non-goals (Decision)

Phase C dirty/quadtree; Phase D camera/bezier/wires; deleting v0 demos; full MSDF
label rendering in Phase B smokes.

## Steps

| Step | Item | Gate |
|------|------|------|
| 1 | Decision: kinds + state + hit/draw/input contract (freeze in this file) | **done** (2026-07-14): Decision frozen |
| 2 | `Label` kind: add + layout size + draw smoke (string → rect/text stub) | **done** (2026-07-14): `scene_label_ok` |
| 3 | `Checkbox` kind: hit toggles checked; draw box+mark | `scene_checkbox_smoke` exit 0 |
| 4 | `Slider` kind: hit/drag sets value `[0,1]`; draw track+thumb | `scene_slider_smoke` exit 0 |
| 5 | `TextField` kind: focus + inject keys/text; caret model | `scene_text_field_smoke` exit 0 |
| 6 | Form smoke: one scene with label+checkbox+slider+field; headless inject | `scene_form_smoke` exit 0 |
| 7 | Critic: kinds match Decision; no sleep; v0 demos untouched | close Phase B |

### Sub-steps (Driver)

**STEP=1** — **done**
1. Freeze Decision table — done.
2. Payload fields listed — done.
3. Non-goals noted — done.

**STEP=2** — **done**
1. Extend `SceneNodeKind` + `scene_add_label` in `misc/gui/scene.mlc` — done.
2. Draw path in `scene_draw.mlc` (rect stub) — done.
3. `misc/examples/scene_label_smoke.mlc` + `scripts/run_scene_label_smoke.sh` — done.

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
