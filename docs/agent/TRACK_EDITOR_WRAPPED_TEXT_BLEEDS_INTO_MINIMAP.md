# Track: Editor wrapped text bleeds into minimap

Parent: [TRACK_EDITOR_UX_BACKLOG.md](TRACK_EDITOR_UX_BACKLOG.md) §46 **#37**
(pulled forward 2026-07-22 — live screenshot). Size **M**.

## Status: **active** (2026-07-22) — STEP=3 done → Critic

## Next step

**STEP=4** — Critic: gates; archive.

## Decision (STEP=0) — **frozen** 2026-07-22

| Item | Choice |
|------|--------|
| Problem | Full-size content glyphs from a long wrapped row appear **inside** `minimap_rect` (README line 12, mixed script; substrings "VS Code"/"Replace" matched from screenshot). `editor_ux_minimap_layout` yields non-overlapping `text_rect`/`minimap_rect`; `wrap_columns = wrap_max_columns(text_rect.width - 8, metrics.char_width)` (`demo_live.mlc` ~922). Content glyph batch is drawn with `editor_gl_scissor_enable_rect(height, editor_rect)` (`demo_live.mlc` ~2360) — **`editor_rect` includes the minimap strip**, so overflow (and wrap mis-budget) is not clipped at `minimap_rect.x`. Uniform `metrics.char_width` in `wrap_max_columns` remains a secondary overrun source |
| Strategy (v1) | **Primary:** clip the editor glyph batch (gutter labels + content rows) to the non-minimap strip: left `editor_rect.x`, width `minimap_rect.x - editor_rect.x` when minimap enabled (else full `editor_rect`). Prefer a small layout helper next to `editor_ux_minimap_layout` over inline arithmetic only. **Secondary only if L2 still red after clip:** tighten wrap budget (padding / advance-aware wrap) — do not start there |
| Primary gate | L2/unit: fixture with long wrap line + minimap geometry — content clip rect's right edge equals `minimap_rect.x` (no content scissor covering minimap), and/or max laid-out pen_x for wrap segments stays `< minimap_rect.x`. Optional draw-report later; pixel gate not required in sandbox |
| Module touch | `misc/editor/**` only |
| REG | no (`lib/mlc/` out) |
| Out of scope | idle-CPU work (`#38` closed); viewport-only wrap rewrite; minimap feature redesign; `compiler/` |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | Red: clip / bleed probe | **done** — `ux_ok minimap_content_clip_red` (stub; demo_live gap) |
| 2 | Clip (and secondary wrap only if needed) | **done** — `editor_ux_content_clip_rect` clips at `minimap_rect.x`; `ux_ok minimap_content_clip_stable` |
| 3 | `demo_live` wire; compile | **done** — glyph batch scissors `content_clip`; `demo_live_fs_compile_ok` |
| 4 | Critic: gates; archive | open |

<!-- STEP=1: red harness — assert current content scissor is editor_rect (includes minimap) or glyph pen past minimap_rect.x -->
<!-- STEP=2: content clip rect excludes minimap; green L2 -->
<!-- STEP=3: demo_live_fs_compile_ok; Critic archive -->

## Out of scope

`#38` reopen; full HarfBuzz wrap rewrite unless clip alone fails L2; `lib/mlc/`.
