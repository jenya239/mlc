# Track: GL text pipeline — per-frame FreeType/HarfBuzz re-init (CPU load) + missing baseline bearing

Parent: [../TEXT_RENDERING.md](../TEXT_RENDERING.md), [../archive/tracks/TRACK_TEXT_RENDERING_NATIVE.md](../archive/tracks/TRACK_TEXT_RENDERING_NATIVE.md).
Trigger: 2026-07-12, user observation on `misc/examples/text_dashboard_demo.mlc`
(2 issues): (1) demo feels laggy/loads the CPU despite vsync being off and
FPS "should be huge"; (2) glyph baseline misaligned — letters with
ascenders/descenders/caps (`h`, capitals) don't sit on one visual line,
even though a C++ reference project the user looked at has this solved.
Both root-caused below **before** handing to Driver — no design-step
needed, go straight to implementation.

## Status: **active** (Planner 2026-07-12) — queue head after REGION STEP=1

## Next step

**STEP=13** — Self-host verify N/A check (stdlib signature / compiler untouched).

### STEP=1 sub-steps (Driver)

1. Read `with_face` / `shape_glyph_count` — confirm every call path still does
   Init+New_Face (cite lines).
2. Add process-local cache keyed by `(font_path, pixel_size)`; hit = reuse
   face/font; miss = create once.
3. Header comment: single-threaded render-loop only (STEP=2 documents mutex
   constraint fully — STEP=1 may leave a one-line note).
4. Verify: at least one existing text smoke still passes
   (`run_text_renderer_native_smoke.sh` or equivalent); optional quick
   `FT_Init` count drop not required until STEP=5.
5. Do **not** wire GlyphCache / bearing in STEP=1 (STEPs 3–4 / 7–8).

## Root cause A — CPU load (confirmed, read the code, not guessed)

`runtime/src/text/harfbuzz_shim.cpp` (`shape_glyph_count`) and
`runtime/src/text/freetype_shim.cpp` (`with_face`, called from
`glyph_bitmap_by_index`) each do, **on every single call**:

```
FT_Init_FreeType(&library)
FT_New_Face(library, font_path, 0, &face)   // re-parses the whole .ttf from disk
FT_Set_Pixel_Sizes(...)
hb_ft_font_create(...) / FT_Load_Glyph + FT_Render_Glyph
FT_Done_Face / FT_Done_FreeType                // tear down again
```

`text_dashboard_demo.mlc` calls `shape_glyph_count` once per `Line` (6/frame)
and `glyph_bitmap_by_index` once per **glyph** (~90+/frame), at up to 60
frames/sec → thousands of full FreeType-library-init + font-file-reparse
cycles per second. This is the dominant cost, not vsync/GPU — GPU work
(1024×1024 atlas re-zero + re-upload every frame, also wasteful, see below)
is comparatively cheap next to re-parsing a TTF file from disk thousands of
times a second.

Compounding, but secondary: `misc/gui/text_renderer.mlc` already ships a
working LRU `GlyphCache` (`glyph_cache_new`/`glyph_cache_get`/
`glyph_cache_insert`, keyed by font/glyph/size → atlas slot) — but
`text_dashboard_demo.mlc` never uses it. It calls `glyph_atlas_new` fresh
every frame and unconditionally re-packs/re-rasterizes every glyph, then
`text_scratch_u8_resize_zero(atlas_size*atlas_size)` (1 MiB memset) and
`text_renderer_upload_atlas` (full GPU texture upload) every frame — even
though most on-screen text (title, description) is identical frame to
frame; only 3 of 6 lines actually change (mouse coords/click count/frame
number).

## Root cause B — baseline misalignment (confirmed, root cause found in-repo)

`runtime/include/mlc/text/freetype_shim.hpp`/`.cpp` (the GL live-render
path used by `text_dashboard_demo.mlc`, `text_window_demo.mlc`,
`gui_text_field_demo.mlc`, `freetype_glyph_smoke.mlc`,
`harfbuzz_shape_smoke.mlc`, `render_mode_smoke.mlc`) exposes only
`glyph_width()`/`glyph_rows()` — **no FreeType bearing**
(`face->glyph->bitmap_top`/`bitmap_left`) at all. Call sites
(`append_line` in the demos, via `gl_scratch_push_screen_quad`) place every
glyph's bitmap top-left corner at the **same** `line.pen_y`, regardless of
each glyph's individual bitmap height — so a tall ascender/cap and a short
x-height glyph get their *tops* aligned instead of their *baselines*.

The fix already exists in this codebase, just not reused:
`runtime/src/gl/text_renderer_shim.cpp` (`shape_string_to_bitmaps` +
`composite_shaped_string`, the **CPU-reference path** behind the
`text_renderer_a8_string_smoke.mlc` golden-image test) already captures
`shaped.top = face->glyph->bitmap_top + y_offset/64` and computes
`destination_y = baseline_y - glyph.pen_y - glyph.top` correctly. Port this
exact formula into the GL live path — do not re-derive it.

## Steps

### Part A — stop re-initializing FreeType/HarfBuzz per call

| Step | Item | Status |
|------|------|--------|
| 1 | `runtime/src/text/freetype_shim.cpp` + `harfbuzz_shim.cpp`: introduce a small process-local cache keyed by `(font_path, pixel_size)` holding `FT_Library` + `FT_Face` (+ `hb_font_t*` for the harfbuzz side) — create once on first use, reuse on subsequent calls, never torn down until process exit (or an explicit `unload` if one already exists elsewhere — check first). This alone removes the dominant cost (disk font-file re-parse thousands of times/sec) | **done** (2026-07-12) |
| 2 | Concurrency note: these caches are process-global mutable state — if any GL demo ever moves shaping/rasterization off the main/GL thread, this needs a mutex; for now (single-threaded render loop in every current demo) document the constraint in the header comment, do not over-engineer a lock-free cache speculatively | **done** (2026-07-12) |
| 3 | Wire `text_dashboard_demo.mlc` (and, if trivial, `text_window_demo.mlc`) to use the existing `misc/gui/text_renderer.mlc` `GlyphCache` (`glyph_cache_new` once at startup, `glyph_cache_get`/`glyph_cache_insert` per glyph per frame) instead of calling `glyph_atlas_new`/unconditional `glyph_atlas_pack` fresh every frame — persist `GlyphCache` + `GlyphAtlas` across frames in `main()`'s loop state | **done** (2026-07-12; dashboard only — `text_window_demo` has inline atlas, not trivial) |
| 4 | Skip `text_scratch_u8_resize_zero`/`text_renderer_upload_atlas` on frames where the glyph cache had no misses (nothing new packed) — track a simple "atlas dirty" bool for the frame | **done** (2026-07-12; `resize_zero` already once at startup; upload gated on glyph miss or box-brightness change) |
| 5 | Benchmark before/after: wall-clock CPU time (`/usr/bin/time -v` or `perf stat`) for 300 frames of `text_dashboard_demo.mlc`, headless (no `MLC_GLFW_VISIBLE`) if possible via existing smoke harness, or visible with a fixed frame count — record both numbers in this track, not just "feels faster" | **done** (2026-07-12) |
| 6 | Verify: existing text-rendering smokes (`run_text_renderer_native_smoke.sh`, `run_gui_text_field_demo.sh`, golden `text_a8_privet_24.rgba` MAE check) still pass after the cache changes — no regression in correctness while fixing perf | **done** (2026-07-12) |

### Part B — baseline bearing in the GL live path

| Step | Item | Status |
|------|------|--------|
| 7 | `freetype_shim.hpp`/`.cpp`: expose bearing — add `glyph_bearing_x()`/`glyph_bearing_y()` (mirroring the existing `glyph_width()`/`glyph_rows()` pattern) returning `face->glyph->bitmap_left`/`bitmap_top` from the last rendered glyph, same caching slot as `glyph_width`/`glyph_rows` | **done** (2026-07-12) |
| 8 | Update every call site in the 6 affected `.mlc` files (`append_line`-shaped functions) to treat `line.pen_y` as the **baseline**, not the bitmap top: `dest_y = line.pen_y - glyph_bearing_y()` (and `dest_x = cursor_x + glyph_bearing_x()` for correctness, matching `text_renderer_shim.cpp`'s `destination_x = origin_x + glyph.pen_x + glyph.left` formula) | **done** (2026-07-12; 3 GL demos wired; smokes without quad placement unchanged) |
| 9 | Regression fixture: a line mixing ascenders/descenders/caps/x-height (e.g. `"Hxpjy Agq"`) rendered through both the GL path and the existing CPU reference (`text_renderer_shim.cpp` via `text_renderer_a8_string_smoke.mlc`'s harness) — compare, MAE should now match the same tight tolerance already used for STEP 6.2 (`≤8/255`), not just visually "look right" | **done** (2026-07-12; `text_a8_hxpjy_24.rgba`; MAE≤8 via same harness) |
| 10 | Re-run the golden `text_a8_privet_24.rgba` comparison — confirm it still passes (or, if the old golden was itself generated pre-bearing-fix and MAE was low only because "Привет" happens to have uniform-height Cyrillic glyphs, regenerate the golden and note why in this track — do not silently keep a stale golden) | **done** (2026-07-12; no regenerate — blob `d9e1cfbb` matches HEAD; smoke exact + MAE ok) |
| 11 | Visual re-check: re-run `text_dashboard_demo.mlc`/`text_window_demo.mlc` visibly (`MLC_GLFW_VISIBLE=1`), screenshot, confirm mixed-case text now sits on one visible baseline | **done** (2026-07-12; screenshots in `docs/agent/fixtures/text_*_baseline_step11.png`; line `Hxpjy Agq`) |
| 12 | Docs: `TEXT_RENDERING.md` — note the bearing fix + which files were affected; `GUI.md` if it references glyph positioning assumptions | **done** (2026-07-12; TEXT_RENDERING §9; GUI.md text-field note) |
| 13 | Self-host verify: N/A (this track only touches `runtime/` C++ + `misc/`, not `compiler/`) unless a `.mlc` stdlib module under `lib/mlc/common/stdlib/` needs a signature change — check before closing | pending |
| 14 | `scripts/regression_gate.sh` green; close track | pending |

## Progress

- **Planner** (2026-07-12): activated after REGION STEP=1 committed (`3b4df688`);
  Next=STEP=1 face/font cache; Part A then Part B; REGION STEP=2 remains paused.
- **Driver STEP=1** (2026-07-12): `CachedFontFace` / `CachedShapingFont` process-local
  caches; no per-call Init/New_Face/Done; freetype+harfbuzz smokes ok.
- **Driver STEP=2** (2026-07-12): expanded header + `.cpp` comments — process-global
  mutable cache, main/GL only, mutex required before off-thread use; no mutex added.
- **Driver STEP=3** (2026-07-12): `text_dashboard_demo.mlc` — persistent `GlyphCache`/
  `GlyphAtlas`; `append_line` get/insert; fixed box slot; atlas zero once at startup.
- **Driver STEP=4** (2026-07-12): `atlas_dirty` — skip `text_renderer_upload_atlas` unless
  glyph miss or box brightness change.
- **Driver STEP=5** (2026-07-12): bench `MLC_TEXT_DASHBOARD_BENCH=1` × 300 frames,
  headless `/usr/bin/time -v`:
  - **before** (pre-STEP1 shims + pre-STEP3 dashboard): User **8.97s**, Elapsed **11.42s**, RSS 84224 KB
  - **after** (STEPS 1–4): User **0.19s**, Elapsed **0.36s**, RSS 86544 KB
  - ≈**47×** user-CPU, ≈**32×** wall.
- **Driver STEP=6** (2026-07-12): smokes green — native, gui text field, A8 string
  golden (`text_a8_privet_24.rgba`), freetype glyph, harfbuzz shape.
- **Driver STEP=7** (2026-07-12): `glyph_bearing_x`/`glyph_bearing_y` on
  `GlyphPixelBuffer` from `bitmap_left`/`bitmap_top`; no call-site wiring yet.
- **Driver STEP=8** (2026-07-12): `append_line`/`append_shaped_a8` use
  `dest = (cursor_x + bearing_x, pen_y - bearing_y)`; GlyphCache stores bearings
  for dashboard hits; wired: dashboard, text_window, gui_text_field.
- **Driver STEP=9** (2026-07-12): `"Hxpjy Agq"` via `text_renderer_a8_string_smoke`
  + golden `text_a8_hxpjy_24.rgba`; GL FBO vs CPU composite MAE≤8/255.
- **Driver STEP=10** (2026-07-12): `text_a8_privet_24.rgba` still valid — git blob
  `d9e1cfbb` unchanged vs HEAD; harness exact memcmp + MAE≤8 without
  `MLC_WRITE_GOLDEN`. No regenerate: CPU golden path already used `bitmap_top`
  before Part B; STEP=7/8 only exposed that formula to GL demos via freetype_shim.
- **Driver STEP=11** (2026-07-12): `MLC_GLFW_VISIBLE=1` screenshots —
  `docs/agent/fixtures/text_dashboard_baseline_step11.png`,
  `text_window_baseline_step11.png`; demos show `Hxpjy Agq` on one baseline;
  window demo sleeps 50ms/frame only when visible.
- **Driver STEP=12** (2026-07-12): `TEXT_RENDERING.md` §9 (face cache + bearing
  formula + affected files); `GUI.md` text-field bearing note.

## Out of scope

- Subpixel positioning / hinting quality tuning — bearing (integer pixel
  offset) is the correctness bug here, not sub-pixel rendering quality.
- Rewriting the naive shim into the retained scene-graph
  ([TRACK_GUI_CANVAS_GRAPH](TRACK_GUI_CANVAS_GRAPH.md)) — that track's own
  Phase A/B will consume whatever text primitive exists at the time; this
  track only fixes the primitive itself (perf + correctness), does not
  restructure the GUI architecture.
- **Перенос bookkeeping-логики (`CachedFontFace`/`CachedShapingFont`
  lookup) на MLC** per [../FFI_LAYER.md](../FFI_LAYER.md) §8
  («без hand-written C++») — STEP=1/2 добавили C++ кеш как residual
  pragmatic fix (тот же паттерн, что допущенный residual в
  [TRACK_FFI_SHIM_MIGRATION](../archive/tracks/TRACK_FFI_SHIM_MIGRATION.md)
  — «abi+bridge C++ remains»). Полная миграция bookkeeping + pitch-copy на
  MLC + экспозиция bearing через ABI (не хардкод в GL-путь) —
  [TRACK_TEXT_SHIM_TO_MLC](TRACK_TEXT_SHIM_TO_MLC.md), отдельный трек,
  идёт **после** закрытия этого (не блокирует STEP=3+ здесь).
