# GUI / Editor UX headless testing

Parent: [GUI_ARCHITECTURE.md](GUI_ARCHITECTURE.md), [EDITOR.md](EDITOR.md),
[TEXT_RENDERING.md](TEXT_RENDERING.md). Track:
[agent/TRACK_UX_HEADLESS.md](agent/TRACK_UX_HEADLESS.md).

## Goal

Автоматически ловить UX-регрессии редактора/GUI **без** ручного окна,
**без** хрупких full-window PNG как основного gate, с детерминизмом CI.

Live demo (`demo_live.mlc`) — для глаз. Gate — harness.

## Pyramid (frozen)

```text
L3  Pixel / FBO golden (MAE)     — редко; только рендер текста/атласа
L2  Headless GL draw + probes    — 1–N кадров; scissor/overflow after draw
L1  Semantic UX scenarios ★      — основной слой; inject + state asserts
L0  Pure model/unit              — уже есть (piece table, nav, tabs, …)
```

**★ L1 — source of truth for UX.** Если сценарий можно проверить по
модели/layout probe без GPU — так и делать. Pixel не заменяет L1.

## Non-goals

- Xvfb + screenshot как primary CI gate (недетерминированно).
- Full-window visual golden на каждый клик.
- Playwright/браузерный стек.
- Python-раннеры (скрипты — Ruby/JS; сценарии — MLC).

## Reliability rules

| Rule | Why |
|------|-----|
| Fixed window size + fixed font path in scenarios | DPI/font drift |
| Fake clock (`UxClock`) for caret blink / animations | no `sleep` flaky |
| All input via inject (`gui_input_test_*`, text push, keys) | no real WM |
| `MLC_GLFW_VISIBLE=0` for GL steps | CI / agents |
| Assert **probes**, not stringified debug prints | stable API |
| One scenario = one exit code + stdout token `ux_ok <name>` | gate scripts |
| Flaky → quarantine + fix clock/probe; never «retry 3×» as green | honesty |

Reuse text golden MAE policy from TEXT_RENDERING (≤ 8/255) only at L3.

## Architecture

```text
Scenario (MLC)  →  UxDriver (actions + clock)
                      ↓
                 App under test (editor shell / widget)
                      ↓ probe each frame
                 UxSnapshot (semantic)  →  expect matchers
                      ↓ optional
                 GL frame + UxDrawReport (clipped? overflow rects?)
                      ↓ rare
                 FBO RGBA + MAE vs fixture
```

### Placement

| Piece | Path |
|-------|------|
| Shared harness types + driver | `misc/gui/ux/` |
| Editor scenarios | `misc/editor/ux_scenarios/` |
| Fixtures (optional pixels) | `misc/editor/fixtures/ux/` |
| Gate scripts | `scripts/run_ux_*.sh` + `scripts/run_ux_gate.sh` |
| Design (this file) | `docs/GUI_UX_TESTING.md` |

Editor app must expose a **probe port** (function returning `UxSnapshot`),
not a second UI toolkit. Demo and harness share the same shell state builders.

### UxSnapshot (minimum v1)

Semantic, serializable enough to assert:

- window: width, height
- focus: `tree` \| `editor` \| `tabs` \| …
- tabs: paths, active_index, dirty flags
- tree: expanded ids, selected path (when wired)
- editor: path, scroll_x/y, caret line/col/byte, selection start/end
- layout rects: tree, divider, editor, gutter, text, tab_strip, status
- document_text hash or full text for small fixtures
- overflow: list of `{widget_id, child_rect}` outside clip (L1/L2)

### UxAction / UxExpect (v1)

Actions: `Move`, `Click`, `DragTo`, `Type`, `Key` (Escape/Enter/Backspace + later
arrows/modifiers), `WaitFrames`, `SetClock`.

Expects: `FocusIs`, `ActiveTabIndex`, `CaretAt`, `SelectionRange`, `ScrollY`,
`TextEquals` / `TextContains`, `DirtyIs`, `NoOverflow`, `CursorShape`,
`EditorRectIs(x,y,w,h)`, `RectContainsPoint(x,y,w,h,px,py)` (half-open hit).

Helpers: `ux_rect_equals`, `ux_rect_contains_point`.

Driver (STEP=2): `ux_driver_new` / `ux_driver_apply` / `ux_driver_run` — fake
clock + pointer/typed/last_key; no GLFW until probe port (STEP=3+).

Probe (STEP=3): `misc/editor/ux/probe.mlc` — `EditorUxState` + `editor_ux_snapshot`
from tabs/selection/shell_panels (fixture; no window).

Scenario (STEP=4): `ux_scenarios/tab_strip_click_activates.mlc` +
`ux/tab_strip.mlc` hit-test; stdout `ux_ok tab_strip_click_activates`.

Scenario (STEP=5): `newline_caret_and_spaces` — trailing space + Enter + caret
cols via `ux/edit_apply.mlc`; `ux_ok newline_caret_and_spaces`.

Later: clipboard expects after P2 GLFW clipboard.

## Mapping live backlog → scenarios

| Backlog | First scenario (must fail until fixed) |
|---------|----------------------------------------|
| L1 clip | `overflow_editor_text` — long line / many lines; `NoOverflow` |
| L2 scroll | `wheel_scroll_keeps_caret_visible` + `scroll_offset_y` assert |
| L3 selection | `multi_line_selection_probe` — start/end on different lines + draw report |
| L4 tree | `tree_click_opens_tab` — real hit-test, not Y-hack |
| L5 tabs | `tab_strip_click_activates` — hit on title rect |
| L6 cursor | `divider_hover_cursor_ew` — probe cursor shape id |
| L7 clipboard | after P2: `copy_paste_roundtrip` |
| L8 caret | `caret_blink_phases` with `UxClock` |
| L9 whitespace | `newline_caret_and_spaces` — Enter, trailing space, caret col |

## CI

- `scripts/run_ux_gate.sh` — all L0-linked UX scenarios (fast, no display).
- Optional job with Mesa/EGL for L2/L3 (reuse glad EGL path from text rendering).
- EDITOR_MVP Critic close requires UX gate green for scenarios covering
  acceptance interactions (edit, tab, scroll, dirty) — see track STEP close.

## Relation to EDITOR_MVP

TRACK_EDITOR_MVP keeps product steps. This track owns **harness + scenarios**.
Product agents add a scenario when fixing L1–L9 or shipping STEP≥21 UX.
Do not mark UX «done» on unit-only green.
