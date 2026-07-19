# Track: Editor Chrome Hover State

Parent: [TRACK_EDITOR_UX_BACKLOG.md](TRACK_EDITOR_UX_BACKLOG.md) §46 **#32**.
Found 2026-07-19: `gui_is_hovered` / `GuiButton` exist, but `demo_live`
toolbar / tab-strip / tree-row chrome draws flat (no per-item fill, never
calls hover). Size **M**.

## Status: **active** (2026-07-19) — STEP=0–2 done; next STEP=3

## Next step

**STEP=3** — `demo_live` wire toolbar + tabs + tree rows; compile.

### STEPs done in git

| Step | Commit (abbrev) | Notes |
|------|-----------------|-------|
| 0 | `f371158a` | Decision freeze + open |
| 1 | `1c047859` | Stub equal RGB + red L2 harness + run scripts |
| 2 | `d5cab6ab` | idle≠hover RGB + `gui_is_hovered` draw report; green L2 |

## Decision (STEP=0) — **frozen** 2026-07-19

| Item | Choice |
|------|--------|
| Problem | Live chrome has zero hover feedback: toolbar buttons, tab slots, and tree rows are labels on a flat bar; `gui_is_hovered` unused in those draw paths |
| Surfaces | Exactly three: (1) toolbar button rects (`editor_app_toolbar_button_rect`), (2) tab slots from classic layout (`tab_strip_title_rect_from_widths`), (3) tree list rows (`folder_row_rect`). Not nav back/forward, crumbs, scrollbar, divider, editor text, context menu |
| Hit | Reuse `gui_is_hovered(GuiInput, Rect)` from `misc/gui/input.mlc` (convert `UxRect` → `Rect` where needed). No new framework widget; `GuiButton` optional for click, not required |
| Draw | Each surface item gets a solid background rect every frame: idle fill or hover fill. Hover fill RGB must differ from idle (same channel set). Active tab may keep a distinct active tint; hover still must differ from that item's non-hover state |
| Colors | Freeze idle/hover constants in `ux/chrome_hover.mlc` (Solarized-dark chrome lift, e.g. idle ≈ bar color, hover ≈ +0.08…0.12 per channel, clamp ≤1.0). No new `Theme` fields this track |
| API | Pure helpers + draw report: e.g. `editor_ux_chrome_hover_rgb(hovered)`, `editor_ux_chrome_hover_draw_report(...)` emitting ≥1 hovered and ≥1 idle entry with `red/green/blue` in the same frame. `demo_live` draws from report or calls the same fill picker |
| Token | `ux_ok chrome_hover_tint_differs` — L2: two items same frame, one hovered one not; assert RGB tuples unequal; rects non-empty |
| Standing discipline | Scenario-first; name token in SESSION `done` |
| Module touch | `misc/editor/ux/chrome_hover.mlc` (new), `demo_live`, scenario/script. May thin-wrap Rect/`gui_is_hovered`. No `compiler/`. No `lib/mlc/` |
| REG | no |
| Out of scope | Animated fade; CSS-like cascade; hover on text glyphs only; `#33` tree expand; changing click contracts |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | L2 red harness + `run_ux_chrome_hover_tint_differs.sh` | **done** — `ux_ok chrome_hover_red` |
| 2 | `chrome_hover` helpers + green token | **done** — `ux_ok chrome_hover_tint_differs` |
| 3 | `demo_live` wire toolbar + tabs + tree rows; compile | pending — `demo_live_fs_compile_ok` |
| 4 | Critic: gates; archive | pending |

<!-- STEP=1 sub-steps: 1) stub module returning equal idle/hover RGB; 2) scenario asserts unequal; 3) run script -->
<!-- STEP=2 sub-steps: 1) idle≠hover RGB + GuiInput hover pick; 2) draw report with ≥2 entries; 3) green scenario -->
<!-- STEP=3 sub-steps: 1) toolbar per-button fill; 2) tab slot fills from classic widths; 3) tree row fills via folder_row_rect; 4) demo compile -->

## Out of scope

Nav/crumb hover; animated transitions; `compiler/`; `lib/mlc/`; tree expand (#33).
