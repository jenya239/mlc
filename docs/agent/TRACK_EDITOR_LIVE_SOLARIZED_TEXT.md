# Track: Editor live Solarized glyph colors

Parent: [TRACK_EDITOR_UX_BACKLOG.md](TRACK_EDITOR_UX_BACKLOG.md) §46 **#1b**.
Found 2026-07-17 screenshot audit (not a regression from #1).
Evidence note in backlog row + stale comment `ui/theme.mlc:122`.

## Status: **active** (2026-07-18) — STEP=0 done; STEP=1 next

## Next step

**STEP=1** — L2 scenario `syntax_glyph_color_matches_theme` (draw-report /
color assert on glyph path, not only highlight-rect presence) — scenario first
per Standing discipline; same commit as minimal failing harness if needed.

### STEP=0 done (2026-07-18)

- Decision frozen below; PLAN §46 + UX_BACKLOG #1b → active.

## Decision (STEP=0) — **frozen** 2026-07-18

| Item | Choice |
|------|--------|
| Problem | Live demo tints Solarized tag colors only as dimmed **background** rects (`rgb*0.35`); glyphs drawn via `static_text_draw_lines_colored` stay default/white. Comment that glyph shader has no color is **stale** — `StaticTextLine` already has `red/green/blue/alpha` |
| Glyph color | Wire per-tag (or default text) color into `StaticTextLine` rgba using `theme_rgb_for_tag` / theme text channels. Prefer splitting draw spans so tagged regions get tag RGB; untagged → theme text RGB |
| Syntax background tint | **Remove** Solarized tag highlight **background** rectangles once glyphs carry color (selection highlight rects stay) |
| Chrome theme | Switch live demo `Theme` to **`theme_solarized_light()`** (single theme). Dark shell was a workaround for white glyphs — obsolete. Delete/update stale `theme.mlc` comment |
| Module touch | Prefer `misc/editor/demo_live.mlc` draw path + `ui/theme.mlc` comment; extend `syntax/` helpers only if needed for span→line color mapping. No `compiler/` / `lib/mlc/` |
| Standing discipline | New L2 scenario named in SESSION `done`, same commit as behavior change |
| REG | no `lib/mlc/`; no `compiler/` |

### Gates

| Step | Token |
|------|-------|
| 1–2 | new `misc/editor/ux_scenarios/syntax_glyph_color_matches_theme.mlc` + run script → `ux_ok syntax_glyph_color_matches_theme` |
| every | `bash scripts/run_editor_demo_live_fs_compile.sh` → `demo_live_fs_compile_ok` |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open track / PLAN / backlog row | **done** (2026-07-18) |
| 1 | L2 scenario first (`syntax_glyph_color_matches_theme`) | scenario fails or harness ready |
| 2 | Wire glyph rgba + remove syntax bg tint; fix stale theme comment; Solarized Light chrome | scenario + compile |
| 3 | Critic: gates; archive | close |

### Sub-steps (Driver)

**STEP=1**
1. Add `ux_scenarios/syntax_glyph_color_matches_theme.mlc` + `scripts/run_ux_syntax_glyph_color.sh` asserting draw-report / StaticTextLine colors match theme tags (L2).
2. Prefer red scenario before STEP=2 wiring.

**STEP=2**
1. `demo_live` editor draw: set `StaticTextLine` colors from tags/theme; drop syntax bg tint rects.
2. `theme = theme_solarized_light()`; scrub stale white-glyph comment.
3. Gates: scenario ok + `demo_live_fs_compile_ok`.

**STEP=3** — Critic; `next` = Planner (§46 #1c).

## Out of scope

Full syntax engine rewrite; tree-sitter; #1c help text; #1d large-file stringify;
keyboard nav; `compiler/`.

## Verify discipline

- One STEP per turn; commit + push.
- Leave foreign `compiler/out/**` alone.
- Standing discipline: name scenario token in SESSION `done`.
