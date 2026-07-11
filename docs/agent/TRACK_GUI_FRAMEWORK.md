# Track: GUI framework (native UI on MLC)

Parent: [../PLAN.md](../PLAN.md), [../TEXT_RENDERING.md](../TEXT_RENDERING.md) §8,
[TRACK_TEXT_RENDERING_NATIVE.md](../archive/tracks/TRACK_TEXT_RENDERING_NATIVE.md).

## Status: **open** — STEP=1 next (Decision) — **active**

**Planner 2026-07-11:** promoted after NATIVE Critic OK (`05186e8c`). Queue next
after closed `TRACK_TEXT_RENDERING_NATIVE`. v0 = extract reusable MLC render
modules (Critic residual) + minimal layout/input/widget on GLFW path — not a
full toolkit.

## Problem

`TRACK_TEXT_RENDERING_NATIVE` delivers a GLFW window + MLC text/GL orchestration
(foundation). Critic residuals: `GlRenderer`/`TextRenderer`/`GlyphAtlas` live
only inside example `.mlc` files; `text_window_helpers.cpp` covers UV/blit ABI
gaps. A full GUI stack (layout, widgets, input routing, easing, hit-testing)
was out of scope there.

## Goal (v0)

1. Shared MLC modules for GlRenderer + TextRenderer (dedupe examples).
2. Minimal immediate-mode or retained layout (Decision locks which).
3. Input routing from GLFW (mouse + key) to one hit-tested widget.
4. One animated parameter (easing) on that widget.
5. Smoke/demo gate; headless text golden untouched.

## Decision (STEP=1) — draft for Driver lock

### Layout model (pick one)

| Вариант | Notes |
|---------|-------|
| **A. Immediate-mode** (Dear ImGui-like) | Simpler v0; layout each frame; fits current poll loop |
| **B. Retained tree** | Nodes + dirty flags; heavier; better for complex UI later |

**Planner lean:** **A** for v0 (one frame = one layout pass).

### Event loop

- Keep NATIVE pattern: MLC main loop `poll → update → layout/draw → swap`.
- No new windowing library; reuse `glfw_window_*` / `glfw_gl_*`.

### Module extract (from Critic)

- Move `GlRenderer` / `TextRenderer` (+ atlas/cache helpers used by both) out of
  `misc/examples/*` into importable MLC modules under an agreed path
  (Driver locks: e.g. `misc/gui/` or `stdlib`-style `compiler`/`lib` — prefer
  **examples-adjacent shared `.mlc`** first to avoid stdlib/compiler churn).
- C++ `text_window_helpers` stays thin ABI; do not move scene logic into C++.

### Widget v0

- One `Button` or `Label` + click/hover via framebuffer coords.
- Easing: reuse `glfw_gl_anim_unit` or small MLC `ease_in_out` on color/size.

### Non-goals (v0)

- Full widget set, CSS-like styling, focus ring polish, Wayland-only, Win/macOS
- Replacing headless EGL golden path
- Putting orchestration back into `text_renderer_shim.cpp`

## Steps

| Step | Item | Status |
|------|------|--------|
| 0 | Activate after NATIVE Critic OK. | **done** (2026-07-11 Planner) |
| 1 | Decision: layout model (A/B); module path; widget v0 surface. | pending |
| 2 | Extract shared MLC `GlRenderer` + `TextRenderer` modules; rewire smokes. | pending |
| 3 | Layout pass v0 (box/stack or IM row) + screen hit-test helper. | pending |
| 4 | Input: mouse position/button + key via GLFW → widget hover/click. | pending |
| 5 | Demo: one widget + easing anim; gate script (hidden window OK). | pending |
| 6 | Docs (`TEXT_RENDERING` or short `GUI.md`) + PLAN; verify-gate. | pending |

<!-- sub-steps STEP=1: 1) lock A vs B; 2) lock module directory; 3) lock Button vs Label API sketch -->
<!-- sub-steps STEP=2: 1) shared .mlc module(s); 2) gl_renderer_smoke + text_renderer_native_smoke import; 3) text_window_demo import or thin wrapper -->
<!-- sub-steps STEP=3: 1) Rect/Point types; 2) layout row/column; 3) point-in-rect -->
<!-- sub-steps STEP=4: 1) glfw cursor/button externs if missing; 2) hover state; 3) click edge -->
<!-- sub-steps STEP=5: 1) demo.mlc; 2) run_*_gate.sh; 3) Esc/N-frame -->
<!-- sub-steps STEP=6: 1) docs; 2) self-host+regression if compiler touched else smokes; 3) close -->
