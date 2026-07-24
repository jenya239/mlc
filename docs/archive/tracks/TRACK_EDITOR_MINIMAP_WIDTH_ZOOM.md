# Track: Editor minimap strip width × font zoom

Parent: [../../PLAN.md](../../PLAN.md) §76.
Residual of [TRACK_EDITOR_MINIMAP_ZOOM_SCALE](TRACK_EDITOR_MINIMAP_ZOOM_SCALE.md)
(§75) / [TRACK_EDITOR_MINIMAP](TRACK_EDITOR_MINIMAP.md)
(§46 #35). Size **S**.

## Status: **closed** (2026-07-25) — Critic OK

**Critic 2026-07-25 (STEP=3):** Re-ran `minimap_width_zoom_stable` ×2 +
`editor_minimap_cache_stable` + `minimap_zoom_scale_stable` +
`minimap_syntax_tags_stable` + `demo_live` compile + HEAD `run_ux_gate`.
Anti-false-done: `46a9c264`…`9403d8f0` (STEP=0–2); `width_for_font` +
`layout_for_font`; `misc/editor/**` + scripts → REG skip; no
`compiler/`/`lib/mlc/`.
**reopen: none**.

Honest residual: `*_red` post-green fails (use stable only); per-frame full
rehighlight still OOS.

| Gate | Result |
|------|--------|
| `run_ux_minimap_width_zoom_stable.sh` | `ux_ok minimap_width_zoom_stable` EXIT=0 (×2) |
| `run_ux_editor_minimap_cache_stable.sh` | `ux_ok editor_minimap_cache_stable` EXIT=0 |
| `run_ux_minimap_zoom_scale_stable.sh` | `ux_ok minimap_zoom_scale_stable` EXIT=0 |
| `run_ux_minimap_syntax_tags_stable.sh` | `ux_ok minimap_syntax_tags_stable` EXIT=0 |
| `run_editor_demo_live_fs_compile.sh` | `demo_live_fs_compile_ok` EXIT=0 |
| HEAD `run_ux_gate.sh` (91 scenarios) | `[ux gate] all ok` EXIT=0 |
| REG / self-host | N/A (editor + scripts) |

## Next step

**closed** — Critic OK. Authorized queue empty → Planner.

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `46a9c264` | Decision: minimap strip width scales with font zoom |
| 1 | `0c9780b2` | Red harness + stable stub `not implemented` |
| 2 | `9403d8f0` | width_for_font + layout_for_font + demo wire; green |
| 3 | (this) | Critic close + archive |

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
| 2 | Width helper + layout wire; green | **done** |
| 3 | Critic: stable + full `run_ux_gate` | **done** |

## Out of scope

SCRIPT_VM; MIR Epic 5; LANG_AUTO_CYCLE.
