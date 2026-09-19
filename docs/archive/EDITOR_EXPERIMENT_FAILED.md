# Native editor (`misc/editor/`) — failed experiment

**Date:** 2026-09-20. User: the GUI editor attempt did not work; mark it failed and restart from the last stable fast OpenGL text point.

## Verdict

PLAN §33–§111 (`misc/editor/`, `demo_live.mlc`, chrome/layout/perf epics) is a **failed product experiment**. Do not reopen numbered editor STEPs. Do not treat `demo_live` as the GUI starting point. Leave the tree in place as history; do not keep extending it.

## Restart point (keep)

The last measured point where **lots of text** rendered **fast and stably** through OpenGL is PLAN **§32** `TRACK_TEXT_GLYPH_CACHE_SCALING`, closed 2026-07-13.

| Item | Value |
|------|--------|
| Commits | `c323556f` (HashMap+FIFO GlyphCache) … `b0ed9c8a` (per-line layout cache in the IDE-panels demo) … `14972c49` (corpus close) |
| Proof demo | `misc/examples/text_ide_panels_demo.mlc` (~2000 visible glyphs/frame: sidebar + editor lines + terminal mock) |
| Smaller sibling | `misc/examples/text_dashboard_demo.mlc` |
| Library | `misc/gui/text_renderer.mlc` (`GlyphCache` `Map<i64,…>` + FIFO), `misc/gui/text_shaping.mlc` (FT/HB face cache) |
| Gate | `scripts/run_text_gl_perf_corpus.rb` |
| Numbers (300 frames, headless) | baseline ~72ms/frame → after cache ~3.2ms/frame; corpus ide 0.98s / dashboard 0.21s |

That stack sits on closed §10 / §10a / §21b / §30 (headless A8/MSDF, windowed MLC text, face/font cache, shim→MLC). §32 is the first time the **volume** path (thousands of glyphs/frame) was both correct and fast.

Editor MVP (§33) started **after** that close. Everything from `misc/editor/` onward is the failed experiment, not the substrate.

## What to build next

New GUI work starts from `misc/gui/` text + GLFW/GL + the §32 demos, not from `misc/editor/demo_live.mlc`. Product shape is not chosen here.

## What not to do

- Do not resume §111 (`TRACK_EDITOR_LAYOUT_INTEGRITY`) or any `TRACK_EDITOR_*` numbered STEP.
- Do not use editor dogfood scripts (`scripts/run_editor_perf_*.sh`, `run_ux_gate.sh` on `demo_live`) as the authority for a new GUI.
- Do not delete `misc/editor/` in this mark — it is dead product code, not the restart surface.
