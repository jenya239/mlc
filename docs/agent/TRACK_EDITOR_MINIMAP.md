# Track: Editor Minimap

Parent: [TRACK_EDITOR_UX_BACKLOG.md](TRACK_EDITOR_UX_BACKLOG.md) §46 **#35**.
User question 2026-07-19 (Sublime-style feasibility). Size **L**.

## Status: **active** (2026-07-20) — STEP=3 done → Critic

## Next step

**STEP=4** — Critic: gates; archive.

## Decision (STEP=0) — **frozen** 2026-07-19

| Item | Choice |
|------|--------|
| Problem | No minimap. Product ceiling is Sublime Text; backlog gate requires a second small-scale pass + viewport indicator + click-to-scroll, with rebuild cost tied to edits not frames (`#1d` `document_frame_snapshot` discipline) |
| Layout | Reserve fixed `minimap_width = 64` from the **right** of the editor content band. `text_rect` shrinks by that width. Content scrollbar (#33e) stays on the **right edge of `text_rect`** (between text and minimap). Minimap is rightmost chrome. Empty/disabled when editor width would leave `text_rect.width < 80` after reserve — skip draw, no crash |
| Scale | Minimap `pixel_size = 2`, `line_height = 3` (fixed; not tied to font zoom this track). Vertical map: full document content height → minimap strip height (`y = scroll_content_y * strip_height / content_height`). Horizontal: clip glyphs to strip; no horizontal minimap scroll |
| Render | Real reduced-scale **glyph** pass via existing shaping/static_text path (not color-block-only). Source text = current `DocumentFrameSnapshot.text` (same flatten as main pane). Syntax tags optional reuse of `highlight_mlc` + `theme_rgb_for_tag` when already available for the buffer; fallback = theme text color. Do **not** call `document_to_string` a second time per frame |
| Cache | Pure model key = snapshot text identity (byte length + equality / or retained `prev_text` pointer-equivalent string compare). `editor_ux_minimap_needs_rebuild(prev_key, next_key)` → false when text unchanged across frames; true on edit. Live path may keep a cached draw-report / shaped batch rebuilt only when needs_rebuild. Viewport indicator + scroll mapping recompute every frame (cheap math) without reshaping |
| Viewport indicator | Translucent rect in minimap coords: `top = scroll_y * strip_h / content_h`, `height = max(4, viewport_h * strip_h / content_h)`. Theme: use `theme.panel_*` tint at ~0.35 alpha equivalent (or fixed `0.35/0.40/0.45` if alpha unavailable — match existing overlay style in demo) |
| Click | Press in minimap → set `scroll_offset_y` so the mapped document y is near viewport center; clamp via `editor_ux_clamp_scroll_offset`. Drag while button held in minimap updates scroll the same way. No wheel remap inside strip this track (wheel stays on editor_rect hover as today) |
| Token | `ux_ok editor_minimap_cache_stable` — L2: (1) geometry: strip width 64, text shrink; (2) two frames same text → rebuild count stays 0; (3) text edit → rebuild increments; (4) click y maps to clamped scroll in range; (5) viewport indicator height ≥4 when content > viewport |
| Standing discipline | Scenario-first; name token in SESSION `done` |
| Module touch | New `misc/editor/ux/minimap.mlc` (geometry + cache key + scroll map + indicator rect); `demo_live.mlc` layout shrink + draw + click/drag; scenario + scripts. Thin helpers in `ux/scroll.mlc` only if needed. No `compiler/`. No `lib/mlc/` |
| REG | no |
| Out of scope | Color-block-only minimap as final deliverable; per-frame full reshape; font-zoom-linked minimap scale; horizontal minimap; always-on when width too small; `compiler/`; `lib/mlc/`; semantic coloring beyond existing highlighter |

## Steps

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze + open | **done** |
| 1 | L2 red harness + run scripts | **done** — `ux_ok editor_minimap_red` |
| 2 | Model: geometry + cache key + indicator + click→scroll; green token | **done** — `ux_ok editor_minimap_cache_stable` |
| 3 | `demo_live` wire (shrink text, draw glyphs+indicator, click/drag); `demo_live_fs_compile_ok` | **done** |
| 4 | Critic: gates; archive | open |

<!-- STEP=1: stub fail token; red script -->
<!-- STEP=2: ux/minimap.mlc + green L2 -->
<!-- STEP=3: demo_live layout/draw/input + compile -->

## Out of scope

Color-block-only final; per-frame reshape; zoom-linked scale; `compiler/`; `lib/mlc/`.
