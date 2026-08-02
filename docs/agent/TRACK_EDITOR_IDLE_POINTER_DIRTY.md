# Track: Editor idle/hover CPU — pointer_dirty split (§106)

Parent: [../PLAN.md](../PLAN.md) §106. Residual of §97 (`TRACK_EDITOR_RENDER_ARCHITECTURE`)
and of the 2026-07-31 live-dogfood incident: the editor burned a full core while the
window was merely hovered (or while the cursor sat outside the client area), because
every pointer event drove a full content frame — HarfBuzz pixel-wrap retick included —
and the paint loop ran uncapped (`glfwSwapInterval(0)`).

## Status: **CLOSED** 2026-08-01 — fix shipped in commit `47b4f134`

This track is written **after** the fix landed (incident-driven, not scenario-first).
Its purpose is to record what shipped, what was *not* verified, and where the
residuals went — not to claim a Critic close of unshipped work.

## Decision (as shipped, `47b4f134`)

| Item | Choice |
|------|--------|
| Problem | `frame_input_poll` set a single `content_dirty` flag; any cursor movement (including jitter with the pointer outside the client area) forced the full content frame: wrap retick, visible-row collect, snapshot tick, full chrome/glyph repaint. Combined with `glfwSwapInterval(0)` on visible windows this produced ~100% of one core at idle-with-mouse |
| Split | New `pointer_dirty: i32` on `EditorFrameInput` (`app/frame_input.mlc`), distinct from `content_dirty`. Mouse-move alone raises **only** `pointer_dirty`. Buttons, text, keys, scroll, resize, drop, binding keys, context menu and overlay keep raising `content_dirty` |
| Out-of-window jitter | Mouse-move is ignored entirely unless the new **or** previous position is inside the client area (`0 <= x < width`, `0 <= y < height`) — cursor motion over other windows no longer wakes the editor |
| Idle gate | `demo_live.mlc` returns to `glfw_gl_context_wait_events_timeout(0.05)` **before** any layout/chrome work when both `content_dirty` and `pointer_dirty` are clear |
| layout_skip | When `content_dirty == 0 && pointer_dirty != 0 && cached_visual_rows_valid != 0`, the frame paints from `cached_visual_rows` and skips `frame_layout_tick_pixel` / `collect_visible_visual_rows_pixel_budget` (`skip_full_pixel_wrap_now` also set) — hover repaints chrome/caret without re-shaping the document |
| vsync | `glfw_gl_context_begin` calls `glfwSwapInterval(1)` when `MLC_GLFW_VISIBLE` is set, `0` otherwise (headless/automated paths, perf smokes and pixel gates keep uncapped behaviour) |
| Module touch | `misc/editor/app/frame_input.mlc`, `misc/editor/demo_live.mlc`, `runtime/src/gl/glfw_window_gl.cpp` |
| REG | no (`compiler/**` untouched; `lib/mlc/**` untouched → `regression_gate.sh` not required) |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Incident triage: idle CPU pinned with pointer over the window | **done** (live dogfood, 2026-07-31) |
| 1 | Fix shipped: `pointer_dirty` + out-of-window jitter filter + pre-layout idle wait + `layout_skip` + conditional `SwapInterval(1)` | **done** — `47b4f134` |
| 2 | Regression: existing UX gate unaffected (hover/scroll/caret scenarios), `dev_gate_fast.sh` green | **done** — no new failures observed in the §102f/§102g gate runs that followed on the same tree |
| 3 | Dedicated numeric idle-CPU-under-pointer-load gate | **NOT done** — deliberately deferred, see Residuals |

## What was *not* verified (honest gaps)

- **No dedicated gate exists for this fix.** `scripts/run_ux_idle_cpu_budget_stable.sh`
  measures idle *without* pointer motion, so it would have stayed green through the
  original incident. No numeric before/after CPU figure was recorded at fix time.
- `scripts/run_editor_demo_live_perf_smoke.sh` cannot observe the fix: it runs with
  `MLC_EDITOR_PERF=1`, which forces `skip_full_pixel_wrap_now = 1` and bypasses the
  hover path entirely (this is itself EHA-04 / §107d).
- No Critic audit turn was run for this change; it shipped as an incident fix.

## Residuals — handed to §107 (do not re-open here)

| Residual | Where it went |
|---|---|
| `frame_layout_tick_spans` runs **outside** `layout_skip`, so every hover frame still does a full-text comparison (`state.text == text`) | §107i `EDITOR_SPANS_TICK_UNDER_LAYOUT_SKIP` (EHA-05 hover half) |
| Cache invalidation keyed by full-text compare instead of a document version | §107e `EDITOR_DOCUMENT_VERSION` (EHA-05) |
| Overlay/context-menu open forces `content_dirty = 1` every iteration | §107 P2 backlog `EDITOR_OVERLAY_IDLE_NO_RETICK` (EHA-26) |
| Missing idle-CPU-under-pointer-load gate | §107r `EDITOR_UX_GATE_BEHAVIORAL` (EHA-19) + §107d perf-full smoke |
| `collect_visible_visual_rows_pixel_budget` still called up to 3× per content frame | §107j `EDITOR_VISIBLE_ROWS_SINGLE_COLLECT` (EHA-18) |

## Non-goals

- No move of the editor chrome onto `SceneNode` (the §GUI_ARCHITECTURE Deviation stands).
- No dirty-rect / partial-framebuffer repaint — the fix is "don't recompute layout",
  not "don't repaint"; a real damage model is out of scope and not queued.
- `SwapInterval(1)` is **not** a perf result: it caps frame count, it does not make a
  frame cheaper. Any claim that vsync "closed" editor perf is explicitly rejected here
  (see audit §9).
