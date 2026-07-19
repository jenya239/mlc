# Track: Editor Chrome Theme Drift

Parent: [TRACK_EDITOR_UX_BACKLOG.md](TRACK_EDITOR_UX_BACKLOG.md) §46 **#33c**
(pulled forward — live-demo audit 2026-07-19). Size **S**.

## Status: **active** (2026-07-19) — STEP=0–2 done; next STEP=3

## Next step

**STEP=3** — `demo_live_fs_compile_ok`.

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `9554b13d` | Decision freeze + open |
| 1 | `1c5c94fd` | Red L2 stub + run scripts |
| 2 | `786470f1` | panel fills + `from_panel` hover; green L2 |

## Decision (STEP=0) — **frozen** 2026-07-19

| Item | Choice |
|------|--------|
| Problem | Header uses `theme.panel_*` (Solarized Light) but tab-bar (`demo_live` ~1628) and toolbar (~1646) base fills are still hardcoded dark `0.12/0.14/0.18` and `0.13/0.15/0.19`. Breadcrumb strip (~1667) same class (`0.10/0.12/0.16`). Tree panel body already `theme.panel_*`. Light header over dark chrome reads as broken UI |
| Surfaces in scope | (1) tab-bar base rect, (2) toolbar base rect, (3) breadcrumb strip rect. Tree panel already OK — verify only |
| Hover coherence | `#32` `editor_ux_chrome_hover_rgb` idle/hover are still dark-bar constants (`0.13` / `0.23`). After base bars go Light, retarget idle ≈ `theme.panel_*` and hover = panel + small lift (clamp ≤1.0) so tab/toolbar hover fills match the Light chrome. Token must still require idle ≠ hover |
| Out of scope colors | Dirty-close overlay, context-menu panel, nav back/forward button fills — leave this track (separate surfaces; not the sampled tab/toolbar drift) |
| API | No new `Theme` fields. `demo_live` passes `theme.panel_*` into the three base rects. `chrome_hover.mlc` accepts theme panel RGB (or reads via helper args) for idle/hover — prefer `editor_ux_chrome_hover_rgb_from_panel(panel, hovered)` rather than hardcoding Light numbers again |
| Token | `ux_ok chrome_panel_rgb_matches_theme` — L2: with `theme_solarized_light()`, reported tab-bar / toolbar / breadcrumb fill RGB equals `theme.panel_*` within ε (e.g. 0.002); chrome hover idle ≠ hover and idle near panel |
| Standing discipline | Scenario-first; name token in SESSION `done` |
| Module touch | `misc/editor/demo_live.mlc`, `misc/editor/ux/chrome_hover.mlc`, scenario + `scripts/run_ux_chrome_panel_rgb_matches_theme.sh`. No `compiler/`. No `lib/mlc/` |
| REG | no |
| Out of scope | `#33d` parent double-click; `#33e` content scrollbar; syntax tags; changing Solarized palette values themselves |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | L2 red harness + run scripts | **done** — `ux_ok chrome_theme_drift_red` |
| 2 | Wire panel RGB + Light hover; green token | **done** — `ux_ok chrome_panel_rgb_matches_theme` |
| 3 | `demo_live_fs_compile_ok` | pending |
| 4 | Critic: gates; archive | pending |

<!-- STEP=1: stub fail token; red script -->
<!-- STEP=2: demo_live three rects + chrome_hover from panel; green L2 -->
<!-- STEP=3: compile gate -->

## Out of scope

Overlay/menu/nav button theming; `compiler/`; `lib/mlc/`; `#33d`/`#33e`.
