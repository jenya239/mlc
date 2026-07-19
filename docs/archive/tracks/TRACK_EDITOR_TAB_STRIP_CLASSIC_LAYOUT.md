# Track: Editor Tab Strip Classic Layout

Parent: [TRACK_EDITOR_UX_BACKLOG.md](../../agent/TRACK_EDITOR_UX_BACKLOG.md) §46 **#31**.
Live-demo audit 2026-07-19: equal-width slots + unclamped title draw →
overlap with many tabs. Size **M**.

## Status: **closed** (2026-07-19) — Critic OK

**Critic 2026-07-19 (STEP=4):** Re-ran both L1 + click + reorder + close unit + demo.
Anti-false-done: `91c3f9eb`…`6471df95` (STEP=0–3); no `compiler/` / `lib/mlc/` → REG skip.
Wire: `tab_strip_preferred_tab_width` / `layout_widths` / `layout_from_titles`;
`*_from_widths` title/close/hit; `truncate_title`; `demo_live` +
`editor_app_click_tab_strip(..., widths)` share one layout. **reopen: none**.

Honest residual: legacy equal-width `tab_strip_title_rect` / hit kept for old
units/scenarios; live chrome uses classic path only. No strip horizontal scroll.

| Gate | Result |
|------|--------|
| `run_ux_tab_strip_rects_no_overlap.sh` | `ux_ok tab_strip_rects_no_overlap` EXIT=0 |
| `run_ux_tab_strip_long_title_ellipsis.sh` | `ux_ok tab_strip_long_title_ellipsis` EXIT=0 |
| `run_ux_tab_strip_click.sh` | `ux_ok tab_strip_click_activates` EXIT=0 |
| `run_ux_drag_reorder_tabs.sh` | `ux_ok drag_reorder_tabs` EXIT=0 |
| `run_editor_tab_strip_close_unit.sh` | `tab_strip_close_unit ok` EXIT=0 |
| `run_editor_demo_live_fs_compile.sh` | `demo_live_fs_compile_ok` EXIT=0 |

## Next step

**closed** — Critic OK. Queue → Planner (§46 `#32 EDITOR_CHROME_HOVER_STATE`).

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `91c3f9eb` | Decision freeze + open |
| 1 | `6c1a71bb` | Two red stubs + run scripts + `run_ux_tab_strip_classic_red.sh` |
| 2 | `8f2ccfb3` | preferred/scale layout + truncate; both L1 green |
| 3 | `6471df95` | `demo_live` + `editor_app_click_tab_strip` use classic widths; truncate draw |
| 4 | this Critic | close + archive |

## Decision (STEP=0) — **frozen** 2026-07-19

| Item | Choice |
|------|--------|
| Problem | `tab_strip_title_rect` = `strip.width / tab_count`; `demo_live` `push_line` title unclamped → with >5 tabs labels overflow into neighbors |
| Layout | Drop equal stretch-to-fill. Per tab: preferred width = clamp(`close_width` + padding + `title_cols * char_width`, `min_width`, `max_width`). Pack left. If `sum(preferred) > strip.width`, scale widths proportionally so sum == strip.width (rects abut, never overlap, stay inside strip) |
| Constants | Freeze `tab_strip_min_width` / `tab_strip_max_width` / title padding in `ux/tab_strip.mlc` (e.g. min≈72, max≈180, padding≈8). `char_width` from caller metrics (mono cell, post-#31a) |
| Ellipsis | If display columns of label (plus dirty/`[]` chrome) exceed slot title area (slot − close − padding), truncate with `"…"` so remaining fits |
| API | Prefer pure helpers in `ux/tab_strip.mlc`: width array from titles+metrics; `tab_strip_title_rect` / close / hit use that layout (or new `tab_strip_title_rect_laid_out`). Hit-test and draw must share one layout |
| Draw | `demo_live`: clamp drawn title string via truncate helper; pen x = title_rect (no unclamped overflow). Existing click/close/reorder keep working |
| Tokens | `ux_ok tab_strip_rects_no_overlap` (N≥6 tabs: consecutive title rects no overlap, each inside strip); `ux_ok tab_strip_long_title_ellipsis` (long path/name → label contains `"…"` and truncated width ≤ title slot minus close) |
| Standing discipline | Scenario-first; name tokens in SESSION `done` |
| Module touch | `misc/editor/ux/tab_strip.mlc`, `demo_live`, scenarios/scripts; thin units if needed. No `compiler/`. No `lib/mlc/` |
| REG | no |
| Out of scope | Horizontal strip scroll/overflow UI; animated slide; pin; proportional fonts; `#32` hover |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Two red L1 harnesses + `run_ux_*.sh` | **done** — `ux_ok tab_strip_classic_red` |
| 2 | Layout + ellipsis helpers in `tab_strip`; green both tokens | **done** — two `ux_ok` |
| 3 | `demo_live` draw uses laid-out rects + truncate; demo compile | **done** — `demo_live_fs_compile_ok` + click/reorder green |
| 4 | Critic: gates; archive | **done** (closed) |

## Out of scope

Strip scroll chrome; pin/unpin; animated reorder; `compiler/`; `lib/mlc/`.
