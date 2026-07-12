# Track: HarfBuzz/FreeType shim — bookkeeping на MLC, GL-путь perf+baseline

Parent: [../FFI_LAYER.md](../FFI_LAYER.md) §8, [../TEXT_RENDERING.md](../TEXT_RENDERING.md), [../PLAN.md](../PLAN.md).
Trigger: пользователь 2026-07-12 — «разве мы не давали задачу переделать
все такие шимы на чистый MLC?». Верно: §8 покрыл Postgres/Crypto/Tcp
([TRACK_FFI_SHIM_MIGRATION](../archive/tracks/TRACK_FFI_SHIM_MIGRATION.md)),
MSDF ([TRACK_TEXT_MSDF_TO_MLC](../archive/tracks/TRACK_TEXT_MSDF_TO_MLC.md)),
WebSocket, Env/Log — но **не** `harfbuzz_shim.cpp`/`freetype_shim.cpp`. Ранее
предложенный (и удалённый) трек `TRACK_TEXT_GL_PERF_BASELINE` шёл в обратную
сторону — предлагал добавить **больше** C++ (кеш `FT_Library`/`FT_Face`
внутри шима), а не перенести bookkeeping на MLC. Эта версия исправляет это.

## Status: **active** (2026-07-12) — STEP=1 Decision next

**Gates cleared:** [TRACK_TEXT_GL_PERF_BASELINE](../archive/tracks/TRACK_TEXT_GL_PERF_BASELINE.md)
Critic OK; [TRACK_LANG_REGION_ARENA](../archive/tracks/TRACK_LANG_REGION_ARENA.md)
Critic OK. Bearing API already shipped (`glyph_bearing_x`/`glyph_bearing_y` in
`freetype_shim.hpp` — **do not duplicate**). This track: move face/font
handle-кеш + pitch-copy bookkeeping from C++ shims onto MLC per FFI §8.

## Next step

**STEP=1** — Decision: freeze ABI table; verify byte-read pattern for pitch-copy.

### STEP=1 sub-steps (Driver)

1. Confirm current public surface: `freetype_shim.hpp` /
   `harfbuzz_shim.hpp` — which symbols stay thin wrappers vs move to
   `*_abi.hpp` (cite lines). Note `glyph_bearing_*` already present.
2. `RawPointer[Byte]` indexed read from MLC: grep stdlib/examples — if no
   deref/`load_u8`, Decision must pick **ABI** `ft_glyph_byte_at(offset)`
   (or row+col) mirroring existing `glyph_byte_at`, not invent pointer arithmetic.
3. Write Decision block into this TRACK: final ABI function list (keep /
   rename / add); reject list unchanged unless new evidence.
4. Verify gate: docs-only; no `compiler/` edits this step.

## Проверено в коде (2026-07-12, важная поправка к более раннему диагнозу)

- `runtime/src/text/freetype_shim.cpp` / `harfbuzz_shim.cpp` **уже кешируют**
  `FT_Library`/`FT_Face`/`hb_font_t` процесс-локально по `(font_path,
  pixel_size)` (`cached_font_face`/`cached_shaping_font`, static `vector`,
  linear scan). Комментарий в коде помечает это как «STEP=2» — кеш добавлен
  раньше, не является актуальной проблемой производительности. **Более
  ранний диагноз «FreeType переинициализируется на каждый вызов» неверен для
  текущего состояния кода.**
- Реальный источник CPU-нагрузки —
  `misc/examples/text_dashboard_demo.mlc`: каждый кадр вызывает
  `glyph_atlas_new` (новый атлас с нуля), `shape_glyph_count`/
  `glyph_bitmap_by_index` для **каждого глифа каждой строки** (включая
  статичный заголовок/подзаголовок, которые не меняются между кадрами), и
  `text_renderer_upload_atlas` (полная загрузка текстуры на GPU) — 60 раз/сек.
  Существующий LRU `GlyphCache` в `misc/gui/text_renderer.mlc` не используется
  демо вообще (оно работает поверх raw scratch/atlas API, не через
  `text_renderer.mlc`).
- **Update (Planner 2026-07-12):** `glyph_bearing_x`/`glyph_bearing_y` уже
  в `freetype_shim.hpp` и используются демо
  (`text_dashboard_demo`/`text_window_demo`/`gui_text_field_demo`). Старый
  пункт «bearing не экспортируется» устарел после GL_PERF STEP=7.
- `runtime/src/gl/text_renderer_shim.cpp` остаётся golden-MAE oracle
  (не runtime-путь) — out of scope.
- Классификация по [../FFI_LAYER.md](../FFI_LAYER.md) §8:
  вызовы `hb_shape`/`FT_Load_Glyph`/`FT_Render_Glyph` — «FFI-адаптер к
  сторонней C-библиотеке» → bookkeeping-логика (кеш хендлов по ключу)
  переписывается на MLC, direct `extern fn` к библиотечным вызовам остаётся.
  Копирование битмапа по pitch (`render_face_glyph`, `shape_and_rasterize`)
  — чистый data-marshalling loop без библиотечных вызовов внутри —
  кандидат на перенос на MLC через raw pointer доступ.

## Decision (STEP=1) — draft below; Driver freezes in STEP=1 commit

### ABI-граница (по прецеденту `postgres_abi.hpp`/`tcp_abi.hpp`)

| Функция (новый `*_abi.hpp`, один вызов библиотеки, без control flow) | Заменяет |
|---|---|
| `ft_library_get() -> i64` (ленивый singleton, без per-key логики) | `shared_free_type_library()` |
| `ft_face_open(library: i64, path: string) -> i64` (0/negative = error) | часть `cached_font_face` (создание) |
| `ft_face_set_pixel_size(face: i64, size: i32) -> i32` | часть `cached_font_face` |
| `ft_face_close(face: i64) -> i32` | — (сейчас лица не закрываются вообще, утечка допустима как singleton-кеш; закрытие — на MLC, если понадобится вытеснение) |
| `ft_face_load_glyph(face: i64, glyph_index: i32) -> i32` | `FT_Load_Glyph` в `with_face` |
| `ft_face_render_glyph(face: i64) -> i32` (packed width\<\<16\|rows) | `render_face_glyph` (без copy-loop) |
| `glyph_bearing_x()`/`glyph_bearing_y()` | уже добавлены в `TRACK_TEXT_GL_PERF_BASELINE` STEP=7 — переиспользовать, не дублировать |
| `ft_glyph_pitch() -> i32`, `ft_glyph_row_pointer(row: i32) -> RawPointer[Byte]` | заменяет copy-loop — MLC копирует по pitch сама |
| `hb_font_create_from_face(face: i64) -> i64` | часть `cached_shaping_font` |
| `hb_shape_text(font: i64, text: string) -> i32` (glyph count) | `shape_glyph_count` (без cache-lookup внутри) |
| `shape_glyph_id_at(i32)`/`shape_glyph_advance_at(i32)` | без изменений, уже тонкие |

### Что переносится на MLC

1. **Face/font handle-кеш** (`Map<string, i64>`, ключ `font_path + "|" +
   pixel_size.to_string()`) — новый модуль `misc/gui/text_shaping.mlc`
   (или расширение `text_renderer.mlc`). MLC решает: искать в кеше, открыть
   через `ft_face_open`+`ft_face_set_pixel_size`, положить в `let mut` map.
2. **Copy-по-pitch** — вместо C++ цикла, MLC читает `ft_glyph_row_pointer(row)`
   через `RawPointer[Byte]` + существующий byte-read intrinsic (см.
   `RawPointer[T]` в [FFI_LAYER.md §2.1](../FFI_LAYER.md)) построчно в свой
   `Array<i32>`/scratch-буфer.
3. **Bearing + baseline compositing math** (`destination_x = origin_x +
   pen_x + bearing_x`, `destination_y = baseline_y - pen_y - bearing_y`) —
   чистая арифметика, переносится в `append_line`
   (`misc/examples/text_dashboard_demo.mlc` и аналоги) и/или в общую
   MLC-функцию в `text_shaping.mlc`, используемую всеми демо.
4. **Кеш атласа между кадрами** — демо переключается на существующий
   `GlyphCache`/`glyph_atlas_pack` с lookup по `(font_path, glyph_id,
   pixel_size)`: перестраивать/перезагружать только изменившиеся строки, не
   весь атлас каждый кадр.

### Rejected

- Полный перенос `hb_shape`/`FT_Render_Glyph` вызовов на MLC — невозможно
  без FFI, библиотека остаётся C.
- Перенос `text_renderer_shim.cpp` (`text_renderer_a8_glyph_smoke`,
  `text_renderer_a8_string_smoke`) — это golden-MAE test oracle
  (EGL pbuffer + shader + `mean_absolute_error`), не runtime-путь; остаётся
  C++ как тестовая инфраструктура (тот же прецедент — CPU-reference oracle
  для MSDF golden-теста тоже не портировался).

## Scope

1. Decision — зафиксировать ABI-таблицу выше, прочитать `RawPointer[T]`
   byte-read intrinsic (есть ли уже способ читать `RawPointer[Byte]` по
   индексу из MLC — проверить `postgres_abi.hpp`/`crypto_mlc_abi_smoke.mlc`
   на паттерн).
2. `freetype_abi.hpp`/`.cpp` — 1:1 обёртки из таблицы (face handle, load,
   render, bearing, row pointer). Удалить cache-lookup control flow из C++.
3. `harfbuzz_abi.hpp`/`.cpp` — аналогично для font handle + shape.
4. `misc/gui/text_shaping.mlc` — face/font handle-кеш (`let mut` Map),
   glyph-bitmap copy по pitch, bearing-aware composite helper.
5. Golden-регрессия: сравнить байты, отдаваемые новым MLC-путём, с текущим
   `glyph_bitmap_by_index`/`shape_glyph_count` выводом (до удаления старого
   API) — MAE/byte-exact там, где буфер идентичен.
6. Переключить `text_dashboard_demo.mlc` (и любые другие живые демо на
   `glyph_bitmap_by_index`/`shape_glyph_count`) на `text_shaping.mlc`;
   применить bearing-aware baseline formula.
7. Демо: не перестраивать атлас/не грузить текстуру, если ни одна строка не
   изменилась с прошлого кадра (dirty-flag на `Line`/`GlyphCache` lookup
   вместо full rebuild).
8. Удалить старые `glyph_bitmap_packed`/`glyph_bitmap_by_index`/
   `shape_glyph_count` C++ функции (или оставить как deprecated thin wrapper
   над abi, если внешние demo на них ссылаются — решить на STEP=6).
9. Self-host diff (`mlcc`→`mlcc2`) + `regression_gate.sh`; обновить
   `TEXT_RENDERING.md` §3/§5 и `FFI_LAYER.md` §8 (добавить строку про
   HarfBuzz/FreeType, отметить closed).
10. Замерить: CPU% при 60 демо-кадров/сек до/после (перестройка атласа
    только при изменении текста); визуальная проверка baseline на
    `h`/заглавных против `x`-height строки.

## Out of scope

- `text_renderer_shim.cpp` (golden test oracle) — не трогать.
- MSDF-путь (`misc/gui/msdf.mlc`) — уже портирован, не относится.
- Замена алгоритма растеризации/шейпинга — цель языковая граница, не
  качество рендера.

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | Decision — freeze ABI table; byte-read = ABI `*_byte_at` if no MLC `RawPointer` deref | pending |
| 2 | `freetype_abi.hpp`/`.cpp` — thin FT wrappers; strip cache control-flow from shim | pending |
| 3 | `harfbuzz_abi.hpp`/`.cpp` — thin HB wrappers; strip cache control-flow from shim | pending |
| 4 | `misc/gui/text_shaping.mlc` — handle-кеш + pitch-copy + bearing composite helper | pending |
| 5 | Golden-регрессия: new MLC path vs current `glyph_bitmap_*` / shape output | pending |
| 6 | Switch `text_dashboard_demo.mlc` (+ other live demos) to `text_shaping.mlc` | pending |
| 7 | Dirty-flag atlas: skip rebuild/upload when lines unchanged | pending |
| 8 | Remove/deprecate old `freetype_shim`/`harfbuzz_shim` public cache helpers | pending |
| 9 | Self-host diff + `regression_gate.sh`; update `TEXT_RENDERING.md` / `FFI_LAYER.md` §8 | pending |
| 10 | CPU%/visual before/after; close track | pending |
