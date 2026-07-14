# Track: Headless UX test harness (editor + GUI)

Parent: [../GUI_UX_TESTING.md](../GUI_UX_TESTING.md), [../EDITOR.md](../EDITOR.md),
[../GUI_ARCHITECTURE.md](../GUI_ARCHITECTURE.md),
[../archive/tracks/TRACK_EDITOR_MVP.md](../archive/tracks/TRACK_EDITOR_MVP.md).

## Status: **priority / active** (2026-07-14) — голова очереди после EDITOR close

Critic EDITOR residuals (live UX L1–L9) → this harness. Design frozen in
`GUI_UX_TESTING.md`. GUI Phase B eligible but **queued after** UX_HEADLESS
STEP progress (or explicit user override).

## Next step

**STEP=9** — Scenario: tree hit open tab (L4).

### STEP=8 done (2026-07-14)

- `ux/selection_apply.mlc`: click/drag + per-line selection draw report.
- Scenario `multi_line_selection_probe` → `ux_ok` (SelectionRange + 3 highlights).

### STEP=7 done (2026-07-14)

- `ux/scroll.mlc`: wheel + clamp + ensure caret visible.
- Scenario `wheel_scroll_keeps_caret_visible` → `ux_ok` (ScrollY + caret in view after wheel).

### STEP=6 done (2026-07-14)

- `ux/overflow.mlc`: content vs text clip; scissor clears overflows.
- Scenario `overflow_editor_text` → `ux_ok` (overflow without clip; NoOverflow with scissor).

### STEP=5 done (2026-07-14)

- `ux/edit_apply.mlc`: insert text/newline into active tab + selection.
- Scenario `newline_caret_and_spaces` → `ux_ok` (trailing space + Enter + caret).

### STEP=4 done (2026-07-14)

- `ux/tab_strip.mlc`: equal-width hit-test + `editor_ux_click_tab_strip`.
- Scenario `ux_scenarios/tab_strip_click_activates.mlc` → `ux_ok` (L5 model green).

### STEP=3 done (2026-07-14)

- `misc/editor/ux/probe.mlc`: `EditorUxState` → `editor_ux_snapshot`.
- `tests/ux_probe_unit.mlc` + gate (`probe_unit`).

### STEP=2 done (2026-07-14)

- `misc/gui/ux/driver.mlc`: apply/run Move/Click/Type/Keys/WaitFrames/SetClock.
- `tests/ux_driver_unit.mlc` + gate (`driver_unit`).

### STEP=1 done (2026-07-14)

- `TextContains` via `string.contains`; `EditorRectIs` / `RectContainsPoint`.
- `ux_rect_equals` / `ux_rect_contains_point`; expanded `ux_expect_unit`.
- Matcher list in `GUI_UX_TESTING.md`; gate green.

### STEP=0 done (2026-07-14)

- Design: `docs/GUI_UX_TESTING.md`; PLAN §33a.
- Scaffold: `misc/gui/ux/{snapshot,expect}.mlc` (+ actions in expect file).
- `scripts/run_ux_expect_unit.sh` + `run_ux_gate.sh` → `[ux] expect_unit ok`.

## Decisions (frozen)

| # | Decision |
|---|----------|
| D1 | Pyramid L0→L3; **L1 semantic scenarios = primary gate** |
| D2 | No Xvfb/full-window PNG as primary CI |
| D3 | Scenarios in **MLC**; runners **Ruby/bash**; no Python |
| D4 | Fake `UxClock`; inject-only input; fixed size/font |
| D5 | Pixel MAE only L3; reuse TEXT_RENDERING tolerance |
| D6 | Editor exposes `UxSnapshot` probe; demo shares shell builders |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Scaffold `misc/gui/ux/{snapshot,expect}.mlc` + `scripts/run_ux_gate.sh` | **done** (2026-07-14): expect_unit + gate |
| 1 | `UxSnapshot` + `UxExpect` matchers (caret/selection/tab/rects/NoOverflow/TextContains) pure unit | **done** (2026-07-14): TextContains+rects + gate |
| 2 | `UxDriver`: inject actions + WaitFrames + SetClock (no app yet) | **done** (2026-07-14): `driver.mlc` + unit |
| 3 | Editor probe port: build snapshot from shell/tabs/doc/selection/layout | **done** (2026-07-14): `probe.mlc` + unit |
| 4 | First real scenario binary: `tab_strip_click_activates` (may fail red until L5 fixed) | **done** (2026-07-14): scenario green (hit model) |
| 5 | Scenario: `newline_caret_and_spaces` (L9) | **done** (2026-07-14): scenario green |
| 6 | Scenario: `overflow_editor_text` (L1) + clip/scissor wiring | **done** (2026-07-14): scenario green |
| 7 | Scenario: scroll wheel / scroll_offset (L2) | **done** (2026-07-14): scenario green |
| 8 | Scenario: multi-line selection probe (L3) | **done** (2026-07-14): scenario green |
| 9 | Scenario: tree hit open tab (L4) | red→green |
| 10 | Cursor shape probe + divider ew-resize (L6); needs GLFW cursor FFI | red→green |
| 11 | L2 headless GL draw report (optional overflow after draw) | smoke `MLC_GLFW_VISIBLE=0` |
| 12 | L3 optional: one caret/glyph FBO MAE fixture (not full UI) | MAE gate |
| 13 | Clipboard scenario after GLFW P2 | red→green |
| 14 | `run_ux_gate.sh` in CI / Critic checklist; README how to add scenario | docs + gate |
| 15 | Critic: design matches code; no flaky sleep; L1 coverage of L1–L9 | close |

## Out of scope

- Replacing EDITOR product modules.
- Browser/Playwright.
- Full IDE session record/replay v1 (optional later).
- Opening GUI Phase B (separate track when this yields or user overrides).

## Progress

- **Planner** (2026-07-14): activated as queue head post-EDITOR; STEP=0 landed in git.
- **STEP=1 done** (2026-07-14): TextContains + rect matchers.
- **STEP=2 done** (2026-07-14): UxDriver clock/actions.
- **STEP=3 done** (2026-07-14): editor probe port.
- **STEP=4 done** (2026-07-14): tab_strip_click_activates green.
- **STEP=5 done** (2026-07-14): newline_caret_and_spaces green.
- **STEP=6 done** (2026-07-14): overflow_editor_text green.
- **STEP=7 done** (2026-07-14): wheel_scroll_keeps_caret_visible green.
- **STEP=8 done** (2026-07-14): multi_line_selection_probe green; next **STEP=9**.
