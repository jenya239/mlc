# Track: Editor Horizontal Scroll (Shift+wheel)

Parent: [../agent/TRACK_EDITOR_UX_BACKLOG.md](../agent/TRACK_EDITOR_UX_BACKLOG.md) §46 **#17**.
Shift+wheel scrolls vertically like plain wheel; `scroll_offset_x` stays 0 on
the wheel path; live `view_metrics` hardcodes horizontal offset 0.
Review gate: `shift_wheel_scrolls_horizontal` (L1). Size **S**.

## Status: **closed** (2026-07-19) — Critic OK

**Critic 2026-07-19 (STEP=3):** Re-ran L1 + wheel/font regressions + demo compile.
Anti-false-done: `e17a2f93`…`1396621f` (STEP=0–2). Wire present:
`editor_ux_wheel_scroll_horizontal` (−ticks×char_width + clamp);
`editor_app_wheel_scroll_horizontal`; demo_live Shift+editor-hover wheel
(Ctrl checked first) updates `scroll_offset_x`; `view_metrics` passes
`app.ux.scroll_offset_x` (no longer hardcoded 0); plain/Ctrl wheel unchanged.
**reopen: none**.

Honest residual: live wrap click (`wrap_offset_at_point`) ignores scroll_x;
glyph draw under wrap may not shift with scroll_x; no scrollbar UI;
horizontal drag autoscroll out of scope; L1 uses negative ticks (formula −ticks×char_width).

| Gate | Result |
|------|--------|
| `run_ux_shift_wheel_scrolls_horizontal.sh` | `ux_ok shift_wheel_scrolls_horizontal` EXIT=0 |
| `run_ux_wheel_scroll.sh` | `ux_ok wheel_scroll_keeps_caret_visible` EXIT=0 |
| `run_ux_ctrl_wheel_font_zoom.sh` | `ux_ok ctrl_wheel_font_zoom` EXIT=0 |
| `run_editor_demo_live_fs_compile.sh` | `demo_live_fs_compile_ok` EXIT=0 |

## Next step

**closed** — Critic OK. Queue → Planner (§46 `#18 EDITOR_BRACKET_AUTOCLOSE`).

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `e17a2f93` | Decision freeze + open |
| 1 | `ca2f1b0e` | L1 red harness + stubs |
| 2 | `1396621f` | horizontal API + demo_live Shift+wheel |
| 3 | this Critic | close + archive |

## Decision (STEP=0) — **frozen** 2026-07-19

| Item | Choice |
|------|--------|
| Problem | Shift+wheel uses vertical scroll; `scroll_offset_x` never changes via wheel; demo_live passes `scroll_offset_x=0` into `editor_view_metrics_new` |
| State | Existing `EditorUxState.scroll_offset_x` (already in probe/nav metrics) |
| Shift+wheel | Editor pane hover + `mod_shift` (and **not** `mod_ctrl`) + scroll ticks ≠ 0 → change `scroll_offset_x` by **−ticks × char_width** (same sign as vertical wheel); **do not** change `scroll_offset_y` |
| Ctrl+wheel | Unchanged font zoom (`#16`); checked **before** Shift |
| Plain wheel | Unchanged vertical (`editor_ux_wheel_scroll` / app path) |
| Clamp | `scroll_offset_x` ∈ `[0, max(0, content_width − viewport_width)]` via existing `horizontal_visible_window` / thin clamp helper; L1 may pass explicit content/viewport widths |
| Hit-test | Already adds `metrics.scroll_offset_x` in `nav_*` (`navigation.mlc`); L1 asserts same pixel → different column when `scroll_offset_x` changes |
| Draw/live | `demo_live`: Shift branch before plain vertical; pass `app.ux.scroll_offset_x` into `editor_view_metrics_new` (stop hardcoding 0) |
| API | `editor_ux_wheel_scroll_horizontal(state, metrics, wheel_ticks, content_width, viewport_width) -> EditorUxState` (or equivalent); L1 calls API without GLFW |
| Scenarios | L1: `shift_wheel_scrolls_horizontal` — start offset 0; apply positive ticks → `scroll_offset_x` increases; hit-test column shifts; `scroll_offset_y` unchanged; token `ux_ok shift_wheel_scrolls_horizontal` |
| Standing discipline | Scenario-first; name tokens in SESSION `done` |
| Module touch | `misc/editor` ux/scroll (+ thin helper), `demo_live` (+ scenario/script). No `compiler/` / `lib/mlc/` |
| REG | no `lib/mlc/`; no `compiler/` |

### Gates

| Step | Token |
|------|-------|
| 1–2 | `ux_ok shift_wheel_scrolls_horizontal` (+ run script) |
| every | `bash scripts/run_editor_demo_live_fs_compile.sh` → `demo_live_fs_compile_ok` |
| regression | `bash scripts/run_ux_wheel_scroll.sh` → `ux_ok wheel_scroll_keeps_caret_visible`; `bash scripts/run_ux_ctrl_wheel_font_zoom.sh` → `ux_ok ctrl_wheel_font_zoom` |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open track / PLAN / backlog | **done** (2026-07-19) |
| 1 | L1 scenario first (`shift_wheel_scrolls_horizontal`) | **done** (2026-07-19) — stub red: `ux_fail shift_wheel scroll_offset_x not increased` |
| 2 | horizontal wheel API + demo_live Shift+wheel wire | **done** (2026-07-19) — `ux_ok shift_wheel_scrolls_horizontal` |
| 3 | Critic: gates; archive | **done** (closed) |

## Out of scope

Horizontal drag autoscroll; scrollbar UI; disable-under-wrap policy; Ctrl+Shift chord; `compiler/`.

## Verify discipline

- One STEP per turn; commit + push.
- Leave foreign `compiler/out/**` alone.
- Standing discipline: name scenario tokens in SESSION `done`.
