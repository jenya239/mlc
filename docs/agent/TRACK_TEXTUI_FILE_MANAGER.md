# Track: Text UI kit + file manager (`misc/textui/`)

Parent: PLAN §112, [TEXT_RENDERING.md](../TEXT_RENDERING.md),
[archive/EDITOR_EXPERIMENT_FAILED.md](../archive/EDITOR_EXPERIMENT_FAILED.md),
PLAN §32 [TRACK_TEXT_GLYPH_CACHE_SCALING](../archive/tracks/TRACK_TEXT_GLYPH_CACHE_SCALING.md)
(closed — last stable fast OpenGL text).

Trigger: 2026-09-20 — native editor product failed; restart GUI from §32 in a
**new folder**, following the Cairo/Pango widget experiment in
`ruby-playground-2026` (`experiments/cairo_ui`, commits `52d1000` →
`4e5a4e2` → `8ea5d93` → `ae37b8f` → `28f5115` as of 2026-09-20). First product: **file manager**, not an editor.

mlc-support: `bundle_textui_fm_step0.sh` + `review_textui_fm_step0.rb`
+ `steps/textui_fm_step0.md` + `implement_textui_fm_step1.rb`.

## Status: **open** — STEP=2 done; next = 02-selection

Compiler queue (§104-6) is independent. Slice 02 needs HB cluster.

## Next step

**02-selection** (not numbered as STEP=3 until Planner writes it): single-line
`index_at` / caret / copy on HB cluster ABI. No GLFW required for hit tests.

```bash
bash scripts/run_textui_slice0_smoke.sh
bash scripts/run_textui_slice01_smoke.sh
ruby scripts/run_textui_file_size.rb
```

STEP=2 (2026-09-20): Sonnet `implement_textui_fm_step2.rb` ($1.01, 62 607
output tokens, `impl_textui_fm_step2_20260920_142545.md`). Stream glue —
do not `apply_response.rb`. Driver compiled-fixed. `wrap_width` is
`WrapWidth` (sum), not `Option<i32>`: three mlcc bugs (fixed 2026-09-20
after STEP=2): (1) `substitute_type` reapplied `T := T` / `T := [T]` and
stack-overflowed on `fn f<T>(x: Option<T>)` match — this is why importing
`option.mlc` segfaulted; (2) `sem_type_to_cpp` remapped `Option` to
`std::optional` while `type_to_cpp` (record fields) emitted `Option<int>`
with no C++ type unless an ADT was declared; (3) sum/variant HashMap
builders passed the map by value into `.set` helpers — COW `detach`
dropped the insert, so the “user ADT Option” gate never fired. After the
fix, a declared `type Option<T> = Some(T) | None` is a variant; undeclared
`Option<T>` still maps to `std::optional`. Stdlib `option.mlc` also used
Ruby `fn(T) -> U` in type position; mlcc only parsed `(T) -> U` and treated
`fn(...)` as `unit` (`void f` / empty `map`). `parse_fn_type` accepts the
Ruby form. Call unify also looped on `T := T` when a generic with two `T`
parameters called another (`unwrap_or` → `unwrap` in `option.mlc`). STEP=2
keeps `WrapWidth`. Residual: `map`/`and_then` `None` vs `Some(f(x))` still
E020 (`Option` vs `Option`).

Source Decision: `mlc-support/responses/textui_fm_step0_20260920_103642.md`
(Opus 5, 131 643 cache-write + 15 868 output, $1.22). Streaming had a few
glued tokens; types below are the cleaned freeze.

## Poles already frozen (do not reopen)

Taken from Cairo `00-strategy.md` plus MLC memory facts. The 2026-08-27
`gui_v2_fork_map` **immediate-mode** recommendation is **not** the spine.

| Topic | Choice |
|-------|--------|
| Product | File manager. Toolkit stays in `misc/textui/` until a live FM exists. |
| Frame | Retained widget tree. Not ImGui. Not `misc/gui/scene*`. |
| Layout | Box row/column + grow (Cairo `Box`), integer top-left rects. |
| Clip | One `overflow` (`visible` / `hidden`) for hit **and** paint. |
| List | Data rows + painter. Never a widget per row. |
| Focus | Keyboard focus ≠ window text-selection owner (FM path field). |
| Paint | Widgets emit `DrawOp`. One module talks to OpenGL (`solid_renderer` + `text_renderer`). |
| Identity | Explicit `WidgetId`. Not `object_id`, not ImGui call-stack ids. |
| Store | Flat `let mut` node array; children are ids. Not nested `[Widget]` records (COW detach). |
| Events | `UiEvent` returned from `update`. Not mutating closures. |
| Text | One `TextLayout` type for Label, path field, and a future editor. Widgets never call HB/Pango/GL. First backend = HarfBuzz + existing atlas. Pango is a later *backend*, not the document model. |
| Language | 100% MLC for widgets/FM. Thin FFI: GLFW/GL/FT/HB. New C++ only for ABI holes named by a STEP. No GTK/Cairo paint. |
| Tests | Layout/hit/dump **without** GLFW first. Same live tree as the window. No parallel L1 probe. |
| Idle | No live frame-counter text on a Label (Cairo slice 03). Corpus `scripts/run_text_gl_perf_corpus.rb` stays the text-stack gate. |
| Files | One concern per `.mlc`. Hard max **400** lines (`scripts/run_textui_file_size.rb`). Split in the same STEP — no allowlist. `app.mlc` only wires poll→update→layout→paint→submit. New control = `widget_*.mlc`, not a dump into store/app. Historical `main.mlc` (§32 demo, 637) must not grow. |

## TextLayout (architecture for FM and a later editor)

The failed editor (`misc/editor/`) already shaped with HarfBuzz per line.
It died on a god-loop and O(document) wrap every frame, not on missing Pango.
A future multiline editor reuses this type, not `demo_live`.

Invariant:

- Input: UTF-8, font, size, optional `wrap_width` (`none` = one line, `\n` still splits).
- Output: pixel size, `index_at(x, y)`, caret, selection rects **per visual line**, glyph runs for `text_renderer`.
- Unit of layout for long documents: **line or paragraph**, never one layout object for the whole file (a single `PangoLayout` on 100k lines is the wrong editor architecture; GTK TextView is per-paragraph for a reason).
- First implementation: single-line, one `.ttf`, extend `harfbuzz_abi` with cluster + positions when hit/caret is needed.
- Multiline wrap is a later fill of the same type (Cairo slice 04), not a second API.
- Pango/`pangoft2` only if a backend must do in-line itemization (fallback, bidi) that HB+one-face cannot. Swap the backend behind `TextLayout`; do not paint via `show_layout`.

## Decision (STEP=0, 2026-09-20)

Host `let mut` owners: `store`, `state`, `theme`. Per-frame geometry is
`LayoutResult` (new arrays), never a write into `nodes[i]`.

| Type | Fields / variants | Invariant |
|------|-------------------|-----------|
| `Rect` | `x, y, width, height: i32` | top-left, exclusive right/bottom, `>= 0` |
| `ColorRgba` | `red, green, blue, alpha: f64` | 0…1, straight alpha |
| `Theme` | `font_path`, `font_size_pixels`, `padding`, `gap`, `background`, `foreground`, `muted`, `selection_background` | parameter to measure/layout/paint, not copied into nodes |
| `WidgetId` | `index: i32` | `-1` = none; equals `nodes` index |
| `Overflow` | `OverflowVisible \| OverflowHidden` | one policy for hit **and** paint |
| `WidgetKind` | `KindBox` / `KindLabel` first | new control = new variant + functions, not a new tree |
| `WidgetNode` | `id, parent, children: [WidgetId], kind, overflow, grow, visible` | children are ids only |
| `WidgetStore` | `nodes, root` | append-only while building a screen |
| `LayoutResult` | `rects, clips` indexed by id | length = node count; clips = intersected hidden ancestors |
| `UiState` | `focus, hover, pointer, selection, scroll_offsets, list_selection, inspected, pick_enabled` | focus ≠ `selection.owner` |
| `DrawOp` | `OpFillRect`, `OpStrokeRect`, `OpText`, `OpPushClip`, `OpPopClip` | no GL handles; order = paint order; `OpPopClip` is a unit ctor (raw Opus glued this as `OpP`) |
| `UiEvent` | `EventButtonClicked`, `EventRowSelected`, `EventRowActivated`, `EventFocusChanged`, `EventTextChanged`, `EventTextSubmitted`, `EventSelectionChanged`, `EventScrollChanged`, `EventContextRequested` | returned from `ui_update`, no closures |
| `TextLayout` | `text, font_id, pixel_size, wrap_width: Option<i32>` (`None` = no wrap), `pixel_width/height`, `lines`, `clusters` | does not paint; submit takes glyph runs; `'\n'` still splits |
| `FrameDump` | text: theme, nodes, ops, state, fingerprint, needed | no FPS/time/addresses |

Modules: `misc/textui/{geometry,theme,store,draw_op,dump,submit,app}.mlc`.
Do not import `misc/gui/layout.mlc` (IM cursor). Hit: hidden overflow clips
subtree; visible overflow still hits overflowing children (Cairo 05).

ABI named, not in STEP=1: `ft_face_line_height` (slice 01),
`hb_shape_glyph_cluster_at` + offsets (slice 02). PRIMARY optional (CLIPBOARD
only until then). IME out of FM. Pango not linked.

Slice-01 tests (STEP=2 gate; names from Opus §6, glue unglued here):

1. `test_three_labels_stack_in_document_order` — three Labels in a column; `rects[1].y >= rects[0].y + rects[0].height` (same for 2 vs 1).
2. `test_gap_between_labels_follows_theme` — `theme.gap = 20` → gap between boxes ≥ 19 and < 22.
3. `test_label_height_grows_with_font_size` — `font_size_pixels` 14 → 28 grows first Label height.
4. `test_label_box_includes_padding_twice` — `rect.width == layout.pixel_width + padding*2` (same for height).
5. `test_label_height_uses_font_metrics_not_bitmap_rows` — `"Text"` and `"Typo"` same box height (`ft_face_line_height`).
6. `test_hit_test_returns_second_label` — point in second box → that `WidgetId`.
7. `test_hit_test_outside_returns_none` — point below root → none.
8. `test_draw_ops_are_background_plus_three_texts` — one `OpFillRect` background + three `OpText`.
9. `test_dump_lists_every_node_with_id_and_rect` — dump has `node <i>`, `rect=`, `overflow=` for every node.
10. `test_dump_is_byte_identical_for_two_identical_frames`.
11. `test_second_frame_reports_needed_false` — fingerprint match → `needed=no`.
12. `test_second_frame_rebuilds_zero_text_layouts` — `TextLayoutCache` rebuilds on frame 2 = 0.

### STEP=0 re-audit (2026-09-20)

STEP=0 is a **Decision**, not a kit. At re-audit time `misc/textui/` had only
`main.mlc` (§32 demo). Kit files landed in **STEP=1**, not by re-running STEP=0.
Do **not** re-run Opus or Sonnet on STEP=0: seven required sections exist in
`textui_fm_step0_20260920_103642.md`; glue is listed below and unglued in the
Decision table.

Checked against tree:

- `harfbuzz_abi.cpp` `ShapedGlyphEntry` stores only `glyph_id` + `x_advance`.
  `infos[].cluster` and `positions[].x_offset` are computed and discarded.
- `freetype_abi` has no ascender/line-height. Glyph `bearing_y` is per-bitmap.
- `glfw_gl_clipboard_*` exists. No PRIMARY.
- `gl_scissor`, `solid_renderer_flush_over_stream`,
  `text_renderer_draw_over_colored_range` exist as named in §5.
- Cairo tests cited exist: `test_labels_stack_and_boxes_follow_theme`,
  `test_second_frame_reuses_pango_and_reports_idle_in_params`.
- Response did not reopen IM / GTK paint / Pango-as-document / `misc/editor/`.

Residuals for Driver (do **not** re-run Opus):

- Raw response has stream glue (`OpP`, «ip-операций»). Trust the table in
  this file: five `DrawOp` variants including `OpPopClip`.
- `wrap_width`: use `Option<i32>` (`None` = no wrap), not `-1`. Stdlib has
  `Option`. `'\n'` still splits.
- `TextLayoutHandle` / `GlyphPlacement` / `ColorRole` / `FontWeight` named in
  prose, not in the table — introduce when Label lands, not in STEP=1.
- Fingerprint algorithm (Cairo `Ui#frame_fingerprint` hashes fields, not the
  dump string). Slice 0 gate = dump text identical on two idle frames.
- Slice-01 test 5 (`"Text"` vs `"Typo"`) is **new**, not in `cairo_ui_test.rb`.
  Keep it: it is the `ft_face_line_height` load-bearing check.
- `BoxData.padding/gap` vs `Theme.padding/gap`: widget fields override;
  Label without its own gap uses theme.
- Do not `import` `misc/gui/layout.mlc` (IM cursor). Duplicate `Rect` in
  `geometry.mlc` (same exclusive edges as `layout.mlc`).

## Restart numbers (this machine, 2026-09-20)

Headless `misc/textui` / ide-panels path, `MLC_GLFW_VISIBLE=0`, 300 frames,
30 warmup: ~2.8 ms/frame (~350 FPS uncapped), first present ~190–240 ms,
RSS ~84 MiB. Visible window uses vsync (~60). Status line still changes
every frame (known idle anti-pattern for slice 03).

`freetype_shim` / `harfbuzz_shim` **deleted** 2026-09-20; live path is
`text_shaping.mlc` + thin abi. FBO MAE oracles (`text_renderer_shim`,
`msdf_renderer_shim`) remain for CI, not for the widget kit.

## Host allowlist (import)

Allowed from existing stack:

- `misc/gui/text_renderer.mlc`, `text_shaping.mlc`, `solid_renderer.mlc`
- `lib/mlc/common/stdlib/gl/gl_window.mlc`, `glad_gl.mlc`
- `lib/mlc/common/stdlib/io/file.mlc` (`list_dir`)
- `glfw_gl_clipboard_*` when the field slice needs CLIPBOARD

Forbidden:

- `misc/gui/scene*.mlc`, IM `button.mlc` as the kit
- `misc/editor/**`
- widgets calling `text_renderer_draw` / any `gl_*` directly

## Slice spine (Cairo order; STEP=0 may rename files, not the order)

| After STEP=0 | Cairo doc | MLC deliverable |
|--------------|-----------|-----------------|
| 1 | *(no Cairo analogue)* | Slice 0: `Rect`, `Theme`, `WidgetId`, store, `DrawOp`, `UiEvent`, `FrameDump`, host loop |
| 2 | 01-label | Theme + column + Label |
| 3 | 02-selection | Single-line `TextLayout` (`index_at`, caret, copy) on HB; API already has `wrap_width` |
| 4 | 03-invalidation | Idle fingerprint; metrics off the widget tree |
| 5 | 05-button | Button, click vs text |
| 6 | 06-box | Row/column, grow |
| 7 | 07–08 | ScrollView + overflow=hidden = hit∩scissor |
| 8 | 09-devtools | Dump + pick overlay from the **same** `DrawOp` list |
| 9 | 10–11 | ListView + Splitter |
| 10 | 12 | TextField skeleton; focus ≠ selection |
| 11 | 13–18 | Field model/keys/clipboard as needed for a path bar |
| 12 | — | Live FM: `list_dir`, two panes, path field |
| later | overlay/menu | After live FM, as in Cairo «Не делаем» |

04-multiline wrap is **not** on the FM critical path; do it when a slice
needs it, not before ListView.

## STEPs

| STEP | What | Gate | Status |
|------|------|------|--------|
| 0 | Opus map: slice-0 types + Cairo→MLC + TextLayout + dump + slice-01 tests | `responses/textui_fm_step0_20260920_103642.md`; Decision in this file | **done** (map; not code). Glue unglued into this file (`OpPopClip`, 12 slice-01 tests, `wrap_width: Option<i32>`). Do not re-run Opus/Sonnet |
| 1 | Slice 0: geometry/theme/store/draw_op/dump (+ host without Label). Hit + dump tests, no GLFW. No per-frame node mutation. Each file ≤400 | `misc/textui/test/` exit 0; `ruby scripts/run_textui_file_size.rb` | **done** 2026-09-20. Gate: `bash scripts/run_textui_slice0_smoke.sh` + file-size. `WidgetKind = KindBox(BoxData) or KindLabel` (Label is a reserved unit ctor: mlcc lowers a one-ctor sum to `struct WidgetKind { field0 }`, so `KindBox{data}` does not compile). Clips use `[i32]` flags, not `[bool]` (`std::vector<bool>` proxies). |
| 2 | 01-label: `text_layout` measure + column + Label. `ft_face_line_height` ABI | `bash scripts/run_textui_slice01_smoke.sh`; file-size | **done** 2026-09-20. 12 tests in `slice01_labels.mlc`. ABI `ft_face_line_height`. `KindLabel(LabelData)`. `WrapWidth` not `Option<i32>` (kept; Option checker/codegen bugs: substitute loop, `type_to_cpp` vs `sem_type_to_cpp`, COW HashMap by-value index builders) |

## Non-goals

- A second `demo_live`: one file that owns input, layout, paint, and commands.
- Port Cairo/GTK as the window or paint backend (`show_layout`, ImageSurface per string).
- Two glyph owners: Pango paint texture **and** `text_renderer` atlas for the same label.
- Per-widget FBO / ImageSurface.
- Icon grid, DnD, copy-progress, tabs — after live FM.
- Promoting the kit to `lib/` or `misc/gui/` before a live FM.
- Compiler language features for GUI (macros, signals).
- Rewriting FreeType/HarfBuzz/GLFW in MLC.

## Verify (track, not STEP=0)

- Widgets: `misc/textui/test/*.mlc` compiled like `run_glyph_cache_smoke.sh` (no window).
- File size: `ruby scripts/run_textui_file_size.rb` (kit ≤400; `main.mlc` must not grow).
- Text stack regression: `ruby scripts/run_text_gl_perf_corpus.rb`.
- No `compiler/` → no `regression_gate.sh` / self-host unless an ABI file lands.
