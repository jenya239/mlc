# GUI / Editor UX headless testing

Parent: [GUI_ARCHITECTURE.md](GUI_ARCHITECTURE.md), [EDITOR.md](EDITOR.md),
[TEXT_RENDERING.md](TEXT_RENDERING.md). Track:
[archive/tracks/TRACK_UX_HEADLESS.md](archive/tracks/TRACK_UX_HEADLESS.md).

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

Scenario (STEP=6): `overflow_editor_text` — long line/tall stack overflows without
scissor; `editor_ux_apply_scissor_clip` → `NoOverflow`; `ux_ok overflow_editor_text`.

Scenario (STEP=7): `wheel_scroll_keeps_caret_visible` — ScrollY + caret stays in
viewport after wheel via `ux/scroll.mlc`; `ux_ok wheel_scroll_keeps_caret_visible`.

Scenario (STEP=8): `multi_line_selection_probe` — click/drag across lines;
SelectionRange + draw report highlights; `ux_ok multi_line_selection_probe`.

Scenario (STEP=9): `tree_click_opens_tab` — visible-row hit-test opens tab by path
(not Y-hack); `ux_ok tree_click_opens_tab`.

Scenario (STEP=10): `divider_hover_cursor_ew` — divider → ew-resize, text → ibeam,
tree → arrow; live GLFW SetCursor deferred; `ux_ok divider_hover_cursor_ew`.

Scenario (STEP=11): `draw_report_headless` — UxDrawReport frames; overflow without
scissor, NoOverflow with scissor; `MLC_GLFW_VISIBLE=0`; `ux_ok draw_report_headless`.

Scenario (STEP=12): `caret_glyph_mae` — 4×16 caret A8 vs golden; MAE≤8/255
(TEXT_RENDERING); live FBO deferred; `ux_ok caret_glyph_mae`.

Scenario (STEP=13): `copy_paste_roundtrip` — select_all/copy/cut/paste inject;
GLFW clipboard deferred; `ux_ok copy_paste_roundtrip`.

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
| L8 caret | `caret_blink_phases` with `UxClock` — **§41 done** ([archive/tracks/TRACK_EDITOR_CARET_BLINK](../archive/tracks/TRACK_EDITOR_CARET_BLINK.md)) |
| L9 whitespace | `newline_caret_and_spaces` — Enter, trailing space, caret col |

## CI

- **Gate:** `scripts/run_ux_gate.sh` — all registered UX scenarios (no display).
- **Workflow:** `.github/workflows/ci.yml` job `ux-headless-gate`.
- Optional Mesa/EGL for true GL L2/L3 (reuse `gl-loader-smoke` path); CPU MAE
  stand-in is enough for caret fixture.

### Critic checklist (UX_HEADLESS closed 2026-07-14)

- [x] `scripts/run_ux_gate.sh` exit 0 on clean tree
- [x] No `sleep` in scenarios; clock via `UxDriver`
- [x] L1–L9: L8 blink deferred (documented); rest gated
- [x] README how-to: `misc/editor/ux/README.md`
- [x] CI job `ux-headless-gate` present
- [x] Known limits listed in TRACK (GLFW clipboard/cursor/FBO; L8)

## Relation to EDITOR_MVP

TRACK_EDITOR_MVP keeps product steps. This track owns **harness + scenarios**.
Product agents add a scenario when fixing L1–L9 or shipping STEP≥21 UX.
Do not mark UX «done» on unit-only green.

## Standing discipline (all `misc/editor/**` tracks, since 2026-07-17)

Frozen by user request 2026-07-17 — applies to every future editor TRACK,
not just this one. See [PLAN.md](PLAN.md) §45 for the first concrete
Clean-Architecture debt item (`demo_live.mlc` decomposition).

1. **TDD, scenario-first.** A Driver STEP that changes editor behavior
   visible to the user (not pure refactor) must add or extend one of
   `tests/*_unit.mlc` (L0) or `ux_scenarios/*.mlc` (L1, prefer this for
   anything input/behavior — see pyramid above) **in the same commit** as
   the implementation, and the STEP's `done` in SESSION.md must name the
   scenario. Prefer writing the scenario/assert first so it fails, then
   make it pass — same spirit as `run_tests` in `compiler/`. A STEP without
   a scenario/unit reference in `done` is not verified UX, only a unit/model
   claim — Critic must reject it per the anti-false-done rule in
   [agent/CONTINUITY.md](agent/CONTINUITY.md).
2. **Clean Architecture — no growing god-files.** `misc/editor/demo_live.mlc`
   is the one sanctioned integration point (live GLFW wiring) but its
   `main()` must stay an orchestrator that calls named phase functions
   (`*_input_phase`, `*_layout_phase`, `*_draw_phase`, …), not accumulate
   inline logic. Budget: no single `fn` body in `misc/editor/**` should
   exceed ~150 lines; a module file exceeding ~500 lines is a signal to
   split by concern (mirrors existing `document/`, `layout/`, `ui/`,
   `workspace/`, `ux/` granularity — each file there is one concern, under
   10 KB). Exceeding budget is not itself a hard-stop, but the next Planner
   turn must open/extend a decomposition track rather than let it grow
   further.
3. **Clean Code.** Full-word identifiers (repo-wide rule, `no-abbreviations`)
   applies here as everywhere. One function = one responsibility; extract
   before adding a third concern to an existing function rather than
   growing it. Prefer pure functions over `EditorAppState`-mutating helpers
   where the existing module pattern already does (e.g. `document/*.mlc`
   returns new records, does not mutate in place).
4. **UX completeness backlog.** "Million small details" (multi-cursor,
   word/line select gestures, drag & drop, IME, find/replace, session
   restore, crash-safety, encoding, …) are tracked as an explicit backlog,
   not folklore — see `docs/agent/TRACK_EDITOR_UX_BACKLOG.md` once opened by
   Planner (source: `mlc-support/responses/editor_tdd_ux_*.md` review) and
   `PLAN.md` §45+ queue. Do not hand-wave "UX polish" as one track — split
   into atomic, independently-gated items like §36-43 already do.
