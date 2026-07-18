# Track: Editor Font Zoom (Ctrl+wheel)

Parent: [../agent/TRACK_EDITOR_UX_BACKLOG.md](../agent/TRACK_EDITOR_UX_BACKLOG.md) §46 **#16**.
Wheel always scrolls; Ctrl+wheel does not change font size / wrap metrics.
Review gate: `ctrl_wheel_font_zoom` (L1). Size **S**.

## Status: **closed** (2026-07-18) — Critic OK

**Critic 2026-07-18 (STEP=3):** Re-ran L1 + wheel regression + demo compile.
Anti-false-done: `2b0d3523`…`897c5cc9` (STEP=0–2). Wire present:
`editor_ux_font_zoom` (±1, clamp 8..48) + chrome-ratio metrics; demo_live
mutable `zoom_font_size`/`metrics`; Ctrl+editor-hover wheel zooms without
scroll; plain wheel unchanged. **reopen: none**.

Honest residual: `char_width` integer steps flat for some sizes (L1 loops);
zoom not persisted in session; Shift+wheel out of scope (#17); one-frame wrap
lag after zoom (wrap computed earlier in frame).

| Gate | Result |
|------|--------|
| `run_ux_ctrl_wheel_font_zoom.sh` | `ux_ok ctrl_wheel_font_zoom` EXIT=0 |
| `run_ux_wheel_scroll.sh` | `ux_ok wheel_scroll_keeps_caret_visible` EXIT=0 |
| `run_editor_demo_live_fs_compile.sh` | `demo_live_fs_compile_ok` EXIT=0 |

## Next step

**closed** — Critic OK. Queue → Planner (§46 `#17 EDITOR_HORIZONTAL_SCROLL`).

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `2b0d3523` | Decision freeze + open |
| 1 | `33682bc7` | L1 red harness + stubs |
| 2 | `897c5cc9` | zoom API + demo_live Ctrl+wheel |
| 3 | this Critic | close + archive |

## Decision (STEP=0) — **frozen** 2026-07-18

| Item | Choice |
|------|--------|
| Problem | Ctrl+wheel scrolls like plain wheel; font size / wrap metrics fixed to chrome constants |
| State | Mutable `font_size: i32` (default `editor_app_font_size()` = 11). Clamp **8..48** inclusive |
| Metrics derive | From `font_size` using chrome ratios: `line_height = font_size + 3`, `char_width = max(1, (font_size * 7) / 11)` → `EditorTextMetrics` |
| Ctrl+wheel | Editor pane hover + `mod_ctrl` + scroll ticks ≠ 0 → change `font_size` by **sign(ticks)** (±1 per call aggregation ok); **do not** change `scroll_offset_y` |
| Plain wheel | Unchanged vertical scroll (`editor_app_wheel_scroll` / existing path) |
| Shift+wheel | **Out of scope** (`#17 EDITOR_HORIZONTAL_SCROLL`) |
| Reflow | Callers recompute wrap via `wrap_max_columns(..., char_width)` after metrics change (demo_live already derives wrap each frame from `char_width()`) |
| API | `editor_ux_font_zoom(font_size, wheel_ticks) -> FontZoomResult { font_size, metrics }` (or equivalent); L1 calls API without GLFW |
| Live | `demo_live`: before plain scroll branch, if ctrl+editor-hover+ticks → apply zoom + refresh local line_height/char_width/font_size helpers; tree hover still scrolls tree only |
| Scenarios | L1: `ctrl_wheel_font_zoom` — default size; apply positive ticks → `font_size`/`line_height`/`char_width` increase; wrap columns for fixed width decrease; token `ux_ok ctrl_wheel_font_zoom` |
| Standing discipline | Scenario-first; name tokens in SESSION `done` |
| Module touch | `misc/editor` ux (+ thin helper), `demo_live` (+ scenario/script). No `compiler/` / `lib/mlc/` |
| REG | no `lib/mlc/`; no `compiler/` |

### Gates

| Step | Token |
|------|-------|
| 1–2 | `ux_ok ctrl_wheel_font_zoom` (+ run script) |
| every | `bash scripts/run_editor_demo_live_fs_compile.sh` → `demo_live_fs_compile_ok` |
| regression | `bash scripts/run_ux_wheel_scroll.sh` → `ux_ok wheel_scroll_keeps_caret_visible` |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open track / PLAN / backlog | **done** (2026-07-18) |
| 1 | L1 scenario first (`ctrl_wheel_font_zoom`) | **done** (2026-07-18) — stub red: `ux_fail font_zoom size not increased` |
| 2 | font-zoom API + demo_live Ctrl+wheel wire | **done** (2026-07-18) — `ux_ok ctrl_wheel_font_zoom` |
| 3 | Critic: gates; archive | **done** (closed) |

## Out of scope

Font picker UI; persist zoom in session; Shift+wheel horizontal (#17); per-buffer zoom; `compiler/`.

## Verify discipline

- One STEP per turn; commit + push.
- Leave foreign `compiler/out/**` alone.
- Standing discipline: name scenario tokens in SESSION `done`.
