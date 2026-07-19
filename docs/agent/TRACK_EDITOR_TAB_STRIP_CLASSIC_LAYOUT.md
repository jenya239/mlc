# Track: Editor Tab Strip Classic Layout

Parent: [TRACK_EDITOR_UX_BACKLOG.md](TRACK_EDITOR_UX_BACKLOG.md) §46 **#31**.
Live-demo audit 2026-07-19: equal-width slots + unclamped title draw →
overlap with many tabs. Size **M**.

## Status: **active** (2026-07-19) — STEP=0–1 done; next STEP=2

## Next step

**STEP=2** — Layout + ellipsis helpers; green both tokens.

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `91c3f9eb` | Decision freeze + open |
| 1 | *(this turn)* | Two red stubs + run scripts + `run_ux_tab_strip_classic_red.sh` |

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
| 2 | Layout + ellipsis helpers in `tab_strip`; green both tokens | pending — two `ux_ok` |
| 3 | `demo_live` draw uses laid-out rects + truncate; demo compile | pending — `demo_live_fs_compile_ok` + regression tab click/reorder |
| 4 | Critic: gates; archive | pending |

<!-- STEP=1 sub-steps: 1) stubs + scripts for both tokens; 2) optional `run_ux_tab_strip_classic_red.sh` -->
<!-- STEP=2 sub-steps: 1) preferred/scale width layout; 2) truncate helper; 3) hit/title/close use layout; 4) green scenarios -->
<!-- STEP=3 sub-steps: 1) demo_live title draw from layout+truncate; 2) keep close/hit; 3) `tab_strip_click` / `drag_reorder` stay green -->

## Out of scope

Strip scroll chrome; pin/unpin; animated reorder; `compiler/`; `lib/mlc/`.
