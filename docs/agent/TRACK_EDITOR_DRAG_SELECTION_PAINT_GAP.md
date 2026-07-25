# Track: Editor drag-selection paint gap — no visible highlight during click-drag

Parent: [../PLAN.md](../PLAN.md) §95. Same report as
[TRACK_EDITOR_HOVER_SCROLLBAR_PAINT_GAP](TRACK_EDITOR_HOVER_SCROLLBAR_PAINT_GAP.md)
(§94) — pick up after that track closes, same repro technique applies.

## Status: **open** — STEP=0 (Decision) next, blocked behind §94 in queue order

## Evidence (2026-07-25 interactive session, not a Driver turn)

Same live build/run as §94. Drove a multi-line mouse-down + move×3 + mouse-up
sequence (`xdotool mousedown 1` / `mousemove --window ...` / `mouseup 1`)
spanning roughly lines 5-14 of the open file. Status bar caret position
changed between the drag-in-progress screenshot (`Ln 11, Col 49`) and the
post-mouseup screenshot (`Ln 23, Col 1`), confirming the drag was received
and processed by the running binary — but **no selection highlight
rectangle rendered in either screenshot**, despite the drag clearly
spanning multiple lines.

L1 model scenarios for selection rendering already pass
(`selection_highlight_matches_glyph_x.mlc`, `multi_line_selection_probe.mlc`,
`wrap_selection_visual_stable.mlc`, all in `run_ux_gate.sh`'s 107-scenario
green run as of 2026-07-25) — same L1-vs-wiring gap shape as §94: the model
functions are correct in isolation, the live per-frame paint call in
`demo_live.mlc` may not be reached, gated wrong, or drawing with a
now-stale rect during an active drag specifically (as opposed to a
completed, static selection — check whether existing scenarios only cover
the post-drag static state, not the drag-in-progress frame).

**Honest limitation:** same `xdotool` reliability caveat as §94 applies —
do not treat this as confirmed without a headless repro.

## STEP=1

Same technique as §94 STEP=1: offscreen hidden window, `glfw_gl_input_test_*`
hooks to simulate mouse-down at one text position, mouse-move to another
(same or different frame — test both, since "highlight only updates after
mouse-up" vs "never updates" are different bugs), `glReadPixels` at the
expected highlight rect, assert non-background color. If it fails, fix in
STEP=2, Critic in STEP=3, `run_ux_gate.sh` twice per standing discipline.
