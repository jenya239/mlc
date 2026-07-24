# Track: Editor minimap zoom-linked scale

Parent: [../PLAN.md](../PLAN.md) §75.
Residual of [TRACK_EDITOR_MINIMAP_SYNTAX_TAGS](../archive/tracks/TRACK_EDITOR_MINIMAP_SYNTAX_TAGS.md)
(§74) / [TRACK_EDITOR_MINIMAP](../archive/tracks/TRACK_EDITOR_MINIMAP.md)
(§46 #35) / [TRACK_EDITOR_FONT_ZOOM](../archive/tracks/TRACK_EDITOR_FONT_ZOOM.md)
(§46 #16). Size **S**.

## Status: **open** — STEP=0 done; next Driver STEP=1

## Next step

**STEP=1** — Red: assert minimap rebuild always uses constant `editor_ux_minimap_pixel_size()` (no zoom arg); stable stub `not implemented`.

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | (this) | Decision: minimap pixel size scales with font zoom |

## Decision (STEP=0) — **frozen** 2026-07-25

| Item | Choice |
|------|--------|
| Problem | Minimap glyph `pixel_size` is fixed `editor_ux_minimap_pixel_size() = 2` while editor font zooms via Ctrl+wheel (`zoom_font_size`). Strip scale ignores zoom. Honest residual of #35 after §74 |
| Strategy (v1) | Add `editor_ux_minimap_pixel_size_for_font(font_size)` (or extend existing helper) → `max(1, 2 * font_size / editor_font_size_default())`. `demo_live` rebuild uses that with `zoom_font_size`. Force strip rebuild when zoom changes (cache key / local last-zoom sentinel). Keep width/layout/click unchanged |
| Primary gate | Red: no green `run_ux_minimap_zoom_scale_stable.sh`. Green: `ux_ok minimap_zoom_scale_stable` — helper exists; demo rebuild calls it with `zoom_font_size` (not bare constant-only path); auto-discovered by `run_ux_gate` |
| Module touch | `ux/minimap.mlc`, `demo_live` rebuild, scripts/scenario |
| REG | no |
| Out of scope | Per-frame full rehighlight; strip width zoom; ICU; SCRIPT_VM; MIR Epic 5; reopen #16/#35/#74 numbered STEPs |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Red: fixed pixel_size = 2 | pending |
| 2 | Scale helper + demo wire; green | pending |
| 3 | Critic: stable + full `run_ux_gate` | pending |

## Out of scope

SCRIPT_VM; MIR Epic 5; LANG_AUTO_CYCLE.
