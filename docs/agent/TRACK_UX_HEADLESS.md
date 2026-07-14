# Track: Headless UX test harness (editor + GUI)

Parent: [../GUI_UX_TESTING.md](../GUI_UX_TESTING.md), [../EDITOR.md](../EDITOR.md),
[../GUI_ARCHITECTURE.md](../GUI_ARCHITECTURE.md),
[../archive/tracks/TRACK_EDITOR_MVP.md](../archive/tracks/TRACK_EDITOR_MVP.md).

## Status: **priority / active** (2026-07-14) — голова очереди после EDITOR close

Critic EDITOR residuals (live UX L1–L9) → this harness. Design frozen in
`GUI_UX_TESTING.md`. GUI Phase B eligible but **queued after** UX_HEADLESS
STEP progress (or explicit user override).

## Next step

**STEP=3** — Editor probe port: build snapshot from shell/tabs/doc/selection/layout.

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
| 3 | Editor probe port: build snapshot from shell/tabs/doc/selection/layout | unit with fixture state |
| 4 | First real scenario binary: `tab_strip_click_activates` (may fail red until L5 fixed) | documented red→green |
| 5 | Scenario: `newline_caret_and_spaces` (L9) | red→green with product fix |
| 6 | Scenario: `overflow_editor_text` (L1) + clip/scissor wiring | red→green |
| 7 | Scenario: scroll wheel / scroll_offset (L2) | red→green |
| 8 | Scenario: multi-line selection probe (L3) | red→green |
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
- **STEP=2 done** (2026-07-14): UxDriver clock/actions; next **STEP=3**.
