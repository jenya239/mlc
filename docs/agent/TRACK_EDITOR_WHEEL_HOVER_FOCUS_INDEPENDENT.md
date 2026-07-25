# Track: Wheel-scroll-on-hover independent of focus — protective regression scenario

Parent: [../PLAN.md](../PLAN.md) §96. Same report as §94/§95 (user: "колесо
мыши должно работать по ховеру и это параллельно фокусу").

## Status: **open** — STEP=0 (Decision) next, blocked behind §95 in queue order

## Code-read finding (2026-07-25, no fix expected)

`demo_live.mlc` already implements this correctly: the tree wheel-scroll
gate (`tree_hovered`, line ~1059) and the main-content wheel-scroll gate
(`point_in_rect(mouse, editor_rect)`, line ~1085) both check only mouse
position, never `app.editor_focused` or any window-focus state. Requesting
focus/click before scrolling is **not** required by the current code.

## STEP=1

Add exactly one new L1 scenario asserting this stays true (protective
regression test, not a fix): construct an `EditorUxState`/equivalent probe
with `editor_focused = 0` (or the model-level equivalent — check
`app/state.mlc` for the actual field name) and a hover position inside the
content rect, call the wheel-scroll model function, assert `scroll_offset_y`
changes. If the model layer has no `focused` field to test against at all
(scroll model functions in `ux/scroll.mlc` take no focus parameter — likely
the case, since the gate lives only in `demo_live.mlc`), this needs a
`demo_live`-level scenario instead (same offscreen `glfw_gl_input_test_*` +
frame-tick technique as §94/§95) with focus explicitly not granted before
the scroll input. Critic in STEP=2. Low risk, small size — do not expand
scope beyond locking in the already-correct behavior.
