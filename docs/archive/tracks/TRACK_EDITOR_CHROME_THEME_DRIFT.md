# Track: Editor Chrome Theme Drift

Parent: [TRACK_EDITOR_UX_BACKLOG.md](../../agent/TRACK_EDITOR_UX_BACKLOG.md) §46 **#33c**
(pulled forward — live-demo audit 2026-07-19). Size **S**.

## Status: **closed** (2026-07-19) — Critic OK

**Critic 2026-07-19 (STEP=4):** Re-ran L2 + demo. Anti-false-done:
`9554b13d`…`64f1f588` (STEP=0–3); no `compiler/` / `lib/mlc/` → REG skip.
Wire: tab/toolbar/breadcrumb base → `theme.panel_*`; hover via
`editor_ux_chrome_hover_rgb_from_panel`; L2 `chrome_panel_rgb_matches_theme`.
**reopen: none**.

Honest residual: dirty-close / context-menu / nav button fills still dark
(out of scope Decision); default `editor_ux_chrome_hover_rgb` still embeds
Solarized Light panel constants as fallback when callers omit panel args.

| Gate | Result |
|------|--------|
| `run_ux_chrome_panel_rgb_matches_theme.sh` | `ux_ok chrome_panel_rgb_matches_theme` EXIT=0 |
| `run_ux_chrome_hover_tint_differs.sh` | `ux_ok chrome_hover_tint_differs` EXIT=0 |
| `run_editor_demo_live_fs_compile.sh` | `demo_live_fs_compile_ok` EXIT=0 |

## Next step

**closed** — Critic OK. Queue → Planner (§46 `#33d EDITOR_TREE_PARENT_DOUBLE_CLICK`).

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `9554b13d` | Decision freeze + open |
| 1 | `1c5c94fd` | Red L2 stub + run scripts |
| 2 | `786470f1` | panel fills + `from_panel` hover; green L2 |
| 3 | `64f1f588` | `demo_live_fs_compile_ok` |
| 4 | this Critic | close + archive |

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
| 3 | `demo_live_fs_compile_ok` | **done** |
| 4 | Critic: gates; archive | **done** (closed) |

<!-- STEP=1: stub fail token; red script -->
<!-- STEP=2: demo_live three rects + chrome_hover from panel; green L2 -->
<!-- STEP=3: compile gate -->

## Out of scope

Overlay/menu/nav button theming; `compiler/`; `lib/mlc/`; `#33d`/`#33e`.
