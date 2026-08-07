# §111a1 `EDITOR_SCROLL_GLYPH_X` — **CLOSED** 2026-08-07

Authorized **2026-08-07** by Opus UX-stuck review
(`mlc-support/responses/editor_ux_stuck_20260807_233045.md`) + local confirm.
Preempted §104-6 until CLOSED.

Parent: §111 / §111a0 closed. Non-goals: S2 wrap-aware caret, S4 shell, S5 minimap.

## Status: **closed** (Critic OK) 2026-08-07

### Decision (frozen 2026-08-07)

| Choice | Freeze |
|--------|--------|
| S1 | Every live `editor_app_clamp_scroll_to_content` / `editor_app_ensure_caret_visible` in `demo_live` calls `editor_live_note_scroll_changed` |
| S3 | Retained editor glyphs: `static_text_glyph_batch_adjust_x` + `buffer_adjust_ndc_x`; `editor_glyph_scroll_x`; fp excludes scroll |
| Gate S1 | `check_scroll_note_neighbors.rb` via `run_editor_scroll_content_dirty.sh` |
| Gate S3 | `run_editor_horizontal_scroll_glyphs.sh` + `horizontal_scroll_moves_glyphs` |
| Sabotage | S1 strip ensure note → fail; S3 strip adjust_x call → fail |
| Out of scope | S2/S4/S5; SceneNode; rename demo_live; §104-6 |

### Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze | **done** |
| 1 | Red S1 | **done** — clamp/ensure missing note |
| 2 | Green S1 | **done** — early/late clamp + ensure wired |
| 3 | Red S3 | **done** — already present after green |
| 4 | Green S3 | **done** — adjust_x + scenario |
| 5 | Critic | **done** — sabotage fail; restore ok |
