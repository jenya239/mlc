# Track: Editor minimap strip width × font zoom

Parent: [../PLAN.md](../PLAN.md) §76.
Residual of [TRACK_EDITOR_MINIMAP_ZOOM_SCALE](../archive/tracks/TRACK_EDITOR_MINIMAP_ZOOM_SCALE.md)
(§75) / [TRACK_EDITOR_MINIMAP](../archive/tracks/TRACK_EDITOR_MINIMAP.md)
(§46 #35). Size **S**.

## Status: **open** — STEP=1 done; next Driver STEP=2

## Next step

**STEP=2** — Add `width_for_font` + `layout_for_font`; wire demo; green `minimap_width_zoom_stable`.

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | 46a9c264 | Decision: minimap strip width scales with font zoom |
| 1 | (this) | Red harness + stable stub `not implemented` |

## Decision (STEP=0) — **frozen** 2026-07-25

| Item | Choice |
|------|--------|
| Problem | After §75, glyph `pixel_size` scales with `zoom_font_size`, but strip width stays fixed `editor_ux_minimap_width() = 64` via `editor_ux_minimap_layout`. Honest residual of #35 after §75 |
| Strategy (v1) | Add `editor_ux_minimap_width_for_font(font_size)` → `max(32, 64 * font_size / editor_font_size_default())` (clamp so text band still fits). Add `editor_ux_minimap_layout_for_font(content_band, font_size)` (or pass width into layout). `demo_live` calls zoom-aware layout with `zoom_font_size`. Keep click/indicator/cache semantics |
| Primary gate | Red: no green `run_ux_minimap_width_zoom_stable.sh`. Green: `ux_ok minimap_width_zoom_stable` — `width_for_font` exists; demo uses zoom-aware layout (not bare `minimap_layout(content_band)` only); auto-discovered by `run_ux_gate` |
| Module touch | `ux/minimap.mlc`, `demo_live` layout call, scripts/scenario |
| REG | no |
| Out of scope | Per-frame full rehighlight; ICU; SCRIPT_VM; MIR Epic 5; reopen #35/#75 numbered STEPs |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Red: fixed width = 64 | **done** |
| 2 | Width helper + layout wire; green | pending |
| 3 | Critic: stable + full `run_ux_gate` | pending |

## Out of scope

SCRIPT_VM; MIR Epic 5; LANG_AUTO_CYCLE.
