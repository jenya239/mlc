# Track: Editor minimap zoom-linked scale

Parent: [../../PLAN.md](../../PLAN.md) §75.
Residual of [TRACK_EDITOR_MINIMAP_SYNTAX_TAGS](TRACK_EDITOR_MINIMAP_SYNTAX_TAGS.md)
(§74) / [TRACK_EDITOR_MINIMAP](TRACK_EDITOR_MINIMAP.md)
(§46 #35) / [TRACK_EDITOR_FONT_ZOOM](TRACK_EDITOR_FONT_ZOOM.md)
(§46 #16). Size **S**.

## Status: **closed** (2026-07-25) — Critic OK

**Critic 2026-07-25 (STEP=3):** Re-ran `minimap_zoom_scale_stable` ×2 +
`editor_minimap_cache_stable` + `minimap_syntax_tags_stable` + `demo_live` compile +
HEAD `run_ux_gate`.
Anti-false-done: `1707178f`…`47385f14` (STEP=0–2); `pixel_size_for_font` +
`minimap_cached_font_size` sentinel; `misc/editor/**` + scripts → REG skip; no
`compiler/`/`lib/mlc/`.
**reopen: none**.

Honest residual: `*_red` post-green fails (use stable only); strip width still fixed;
per-frame full rehighlight still OOS.

| Gate | Result |
|------|--------|
| `run_ux_minimap_zoom_scale_stable.sh` | `ux_ok minimap_zoom_scale_stable` EXIT=0 (×2) |
| `run_ux_editor_minimap_cache_stable.sh` | `ux_ok editor_minimap_cache_stable` EXIT=0 |
| `run_ux_minimap_syntax_tags_stable.sh` | `ux_ok minimap_syntax_tags_stable` EXIT=0 |
| `run_editor_demo_live_fs_compile.sh` | `demo_live_fs_compile_ok` EXIT=0 |
| HEAD `run_ux_gate.sh` (90 scenarios) | `[ux gate] all ok` EXIT=0 |
| REG / self-host | N/A (editor + scripts) |

## Next step

**closed** — Critic OK. Authorized queue empty → Planner.

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `1707178f` | Decision: minimap pixel size scales with font zoom |
| 1 | `aab993ea` | Red/stable stub |
| 2 | `47385f14` | for_font + demo wire + L0 green |
| 3 | (this) | Critic close + archive |

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
| 1 | Red: fixed pixel_size = 2 | **done** |
| 2 | Scale helper + demo wire; green | **done** |
| 3 | Critic: stable + full `run_ux_gate` | **done** — Critic OK |

## Out of scope

SCRIPT_VM; MIR Epic 5; LANG_AUTO_CYCLE.
