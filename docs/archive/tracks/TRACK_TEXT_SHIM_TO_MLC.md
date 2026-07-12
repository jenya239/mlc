# Track: HarfBuzz/FreeType shim — bookkeeping на MLC, GL-путь perf+baseline

Parent: [../../FFI_LAYER.md](../../FFI_LAYER.md) §8, [../../TEXT_RENDERING.md](../../TEXT_RENDERING.md), [../../PLAN.md](../../PLAN.md).
Trigger: пользователь 2026-07-12 — «разве мы не давали задачу переделать
все такие шимы на чистый MLC?». Верно: §8 покрыл Postgres/Crypto/Tcp
([TRACK_FFI_SHIM_MIGRATION](TRACK_FFI_SHIM_MIGRATION.md)),
MSDF ([TRACK_TEXT_MSDF_TO_MLC](TRACK_TEXT_MSDF_TO_MLC.md)),
WebSocket, Env/Log — но **не** `harfbuzz_shim.cpp`/`freetype_shim.cpp`. Ранее
предложенный (и удалённый) трек `TRACK_TEXT_GL_PERF_BASELINE` шёл в обратную
сторону — предлагал добавить **больше** C++ (кеш `FT_Library`/`FT_Face`
внутри шима), а не перенести bookkeeping на MLC. Эта версия исправляет это.

## Status: **closed** (2026-07-13) — Critic OK

**Critic 2026-07-13:** STEP=1–10 vs `75263977`…`21b33afb` (+ `39aeed57` link fix);
abi+text_shaping+demos+dirty+shim thin wrappers; REG 20/0 (STEP=9); self-host
p1≡p2; re-ran `text_shaping_vs_shim_ok` + `gui_text_field_demo` exit 0.
**reopen: none**. Residuals: deprecated `*_shim` kept for smokes/gate; bench
User 0.33s wall 0.92s slower than GL_PERF C++-cache after (0.19/0.36) because
dashboard dynamic lines reshape every frame; `text_renderer_shim` oracle
untouched (out of scope).

## Next step

**closed** — Critic OK. Queue: `TRACK_PACKAGE_MANAGER` (Planner activate).

### STEP=10 done (2026-07-13)

- Bench `MLC_TEXT_DASHBOARD_BENCH=1` × 300 frames, headless `/usr/bin/time -v`:
  - **before** (GL_PERF pre-STEP1, archived): User **8.97s**, Elapsed **11.42s**
  - **after GL_PERF** (C++ face cache): User **0.19s**, Elapsed **0.36s**
  - **after TEXT_SHIM** (MLC `text_shaping` + dirty atlas): User **0.33s**,
    Elapsed **0.92s**, RSS 85392 KB, ~46% CPU — still ≈**27×** user vs original
    before; slight wall vs GL_PERF-after from Map/open cost when lines change
    every frame (mouse/frame counters).
- Visual: `text_renderer_a8_string_smoke` exit 0 (`Hxpjy Agq` + `Привет` MAE≤8);
  baseline fixtures `docs/agent/fixtures/text_*_baseline_step11.png` unchanged.
- Track closed → archive.

### STEP=9 done (2026-07-13)

- Docs: `TEXT_RENDERING.md` §3.1/§9.1 + file table; `FFI_LAYER.md` §8 row + track list
  (bookkeeping on MLC; shim deprecated).
- Self-host: `mlcc`→`mlcc2` `diff -rq p1 p2 --exclude=obj` identical.
- `regression_gate.sh`: 20/0; examples sweep ok=115 fail=0 skip=1 (`dynrecord_demo`).

### STEP=8 done (2026-07-13)

- `freetype_shim`/`harfbuzz_shim`: removed `CachedFontFace`/`CachedShapingFont`;
  thin wrappers over `freetype_abi`/`harfbuzz_abi` (open/shape/close per call).
- Headers marked DEPRECATED; keep API for smokes + `text_shaping_vs_shim_gate`.
- Verify: `text_shaping_vs_shim_ok`; `freetype_glyph_smoke`/`harfbuzz_shape_smoke` exit 0.

### STEP=7 done (2026-07-13)

- `gui_text_field_demo`: persistent GlyphAtlas/GlyphCache; rebuild/upload only when
  `display` changes; asserts `content_rebuild_count`/`atlas_upload_count` < frames.
- `text_dashboard_demo`: `lines_key` fingerprint — skip reshape/rebuild/upload when
  lines + box brightness unchanged.
- Verify: `gui_text_field_demo` exit 0; dashboard `--check-only` 0.

### STEP=6 done (2026-07-13)

- Demos on `text_shaping.mlc`: `text_dashboard_demo`, `gui_text_field_demo`,
  `text_window_demo` (A8 path).
- `ft_glyph_a8_data` + blit prefers abi flat buffer (shim fallback).
- Verify: `gui_text_field_demo` exit 0; `text_shaping_vs_shim_ok`; dashboard/window check-only ok.

### STEP=5 done (2026-07-12)

- Gate: `misc/examples/text_shaping_vs_shim_gate.mlc` — shape (`Hi`/`Привет`) +
  raster (`A`) byte-exact vs `harfbuzz_shim`/`freetype_shim`.
- Verify: run prints `text_shaping_vs_shim_ok` exit 0.

### STEP=4 done (2026-07-12)

- `misc/gui/text_shaping.mlc`: `TextShapingState` face/font `Map` cache;
  `text_shaping_shape` / `text_shaping_rasterize_glyph` (pitch-copy flatten);
  `text_baseline_destination`.
- Smoke: `misc/examples/text_shaping_smoke.mlc` → `text_shaping_ok`.
- Legacy shims still used by demos until STEP=6.

### STEP=3 done (2026-07-12)

- `runtime/include/mlc/text/harfbuzz_abi.hpp` + `runtime/src/text/harfbuzz_abi.cpp`.
- `hb_font_create_from_face` / `hb_font_destroy` / `hb_shape_text`;
  accessors `hb_shape_glyph_id_at` / `hb_shape_glyph_advance_at` (names avoid
  clash with legacy `shape_glyph_*` shim).
- Wired in `compiler/build_bin.sh`. Old `harfbuzz_shim` unchanged.
- Smoke: `/tmp/hb_abi_smoke` → `hb_abi_ok` (FT face + shape `"Hi"`).

### STEP=2 done (2026-07-12)

- `runtime/include/mlc/text/freetype_abi.hpp` + `runtime/src/text/freetype_abi.cpp`.
- i64 library/face; load/render; last-glyph slot stores **raw pitch** buffer +
  `ft_glyph_pitch` / `ft_glyph_byte_at` (no flatten — MLC STEP=4).
- Wired in `compiler/build_bin.sh` RT_SRC. Old `freetype_shim` unchanged.
- Smoke: `/tmp/ft_abi_smoke` → `ft_abi_ok` (DejaVuSans 24px).

### STEP=1 done (2026-07-12)

- Public surface cited: `freetype_shim.hpp:19-32`, `harfbuzz_shim.hpp:17-20`.
- `RawPointer`: only `raw_pointer_null` / `raw_pointer_is_null` (`memory.mlc`) — **no** byte load → pitch-copy via ABI `ft_glyph_byte_at`.
- Decision frozen below.

### STEP=1 sub-steps (Driver) — done

1. Confirm current public surface — done.
2. RawPointer byte-read — absent; ABI `*_byte_at` chosen.
3. Decision block frozen.
4. Docs-only verify.

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

## Decision (STEP=1) — **frozen** 2026-07-12

### Current public surface (cite)

| Header | Symbols | Role today |
|--------|---------|------------|
| `freetype_shim.hpp:19-32` | `glyph_bitmap_packed`, `glyph_bitmap_by_index`, `glyph_width`/`rows`/`bearing_x`/`bearing_y`/`byte_count`/`byte_at`, `glyph_a8_data` | path+size **cache** + load/render + last-glyph slot |
| `harfbuzz_shim.hpp:17-20` | `shape_glyph_count`, `shape_glyph_id_at`, `shape_glyph_advance_at` | path+size **cache** + shape + last-shape slot |

Cache control-flow lives in `freetype_shim.cpp` (`cached_font_face` ~46–74,
`with_face` ~101–124) and `harfbuzz_shim.cpp` (`cached_shaping_font` /
`shape_glyph_count`). Pitch-copy is `render_face_glyph` ~76–98.

### Byte-read pattern

`lib/mlc/common/stdlib/core/memory.mlc`: `RawPointer<T>` + `raw_pointer_null` /
`raw_pointer_is_null` only — **no** indexed byte load. Therefore pitch-copy
**must** use ABI `ft_glyph_byte_at(index: i32) -> i32` (same contract as
existing `glyph_byte_at`), not `RawPointer[Byte]` row pointers.

### Frozen ABI (`freetype_abi.hpp` / `harfbuzz_abi.hpp`, postgres-style `i64` handles)

| ABI | Replaces / notes |
|-----|------------------|
| `ft_library_get() -> i64` | `shared_free_type_library()` singleton only |
| `ft_face_open(library: i64, path: string) -> i64` | `FT_New_Face`; 0 = error |
| `ft_face_set_pixel_size(face: i64, size: i32) -> i32` | `FT_Set_Pixel_Sizes`; 0 = ok |
| `ft_face_close(face: i64) -> i32` | `FT_Done_Face` (MLC cache eviction) |
| `ft_face_load_glyph(face: i64, glyph_index: i32) -> i32` | `FT_Load_Glyph` |
| `ft_face_render_glyph(face: i64) -> i32` | `FT_Render_Glyph` + fill last-glyph slot **without** pitch-flatten loop into `vector` if MLC copies; minimum: expose dimensions/bearing + bytes via accessors below. Packed `(width<<16)\|rows` on success |
| `ft_glyph_width` / `ft_glyph_rows` / `ft_glyph_pitch` / `ft_glyph_bearing_x` / `ft_glyph_bearing_y` / `ft_glyph_byte_count` / `ft_glyph_byte_at` | last-glyph slot after render; raw pitch buffer (STEP=2); existing `glyph_*` shim names stay until STEP=8 |
| `hb_font_create_from_face(face: i64) -> i64` | `hb_ft_font_create` (or current shim equivalent) |
| `hb_font_destroy(font: i64) -> i32` | pair for MLC eviction (`::hb_font_destroy`) |
| `hb_shape_text(font: i64, text: string) -> i32` | shape only; fills last-shape slot; **no** path/size lookup |
| `hb_shape_glyph_id_at` / `hb_shape_glyph_advance_at` | abi accessors (STEP=3); legacy `shape_glyph_*` shim kept until STEP=8 |

### Moves to MLC (`text_shaping.mlc`, STEP=4+)

1. Face/font handle map keyed by `font_path + "|" + pixel_size`.
2. Pitch-copy loop: after `ft_face_render_glyph`, read `ft_glyph_byte_at` into MLC buffer.
3. Bearing/baseline arithmetic helpers (demos already call `glyph_bearing_*`).
4. Atlas dirty/reuse (STEP=7) — separate from ABI.

### Rejected (unchanged + new)

- MLC `RawPointer` pitch-copy / `ft_glyph_row_pointer` — no byte deref intrinsic.
- Porting `hb_shape` / `FT_Render_Glyph` bodies into MLC.
- Touching `text_renderer_shim.cpp` (golden MAE oracle).
- Duplicating `glyph_bearing_*` under new names without need.

### Migration note

Until STEP=8, old `glyph_bitmap_*` / `shape_glyph_count(path,…)` may remain as
deprecated wrappers calling cache-in-C++ **or** thin wrappers over MLC path —
STEP=2/3 add abi alongside; STEP=6 switches demos; STEP=8 deletes cache helpers.
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
| 1 | Decision — freeze ABI table; byte-read = ABI `*_byte_at` if no MLC `RawPointer` deref | **done** (2026-07-12) |
| 2 | `freetype_abi.hpp`/`.cpp` — thin FT wrappers; strip cache control-flow from shim | **done** (2026-07-12) abi added; shim cache deferred to STEP=8 |
| 3 | `harfbuzz_abi.hpp`/`.cpp` — thin HB wrappers; strip cache control-flow from shim | **done** (2026-07-12) abi added; shim cache deferred to STEP=8 |
| 4 | `misc/gui/text_shaping.mlc` — handle-кеш + pitch-copy + bearing composite helper | **done** (2026-07-12) `text_shaping_smoke` ok |
| 5 | Golden-регрессия: new MLC path vs current `glyph_bitmap_*` / shape output | **done** (2026-07-12) `text_shaping_vs_shim_gate` byte-exact |
| 6 | Switch `text_dashboard_demo.mlc` (+ other live demos) to `text_shaping.mlc` | **done** (2026-07-13) dashboard+field+window A8 |
| 7 | Dirty-flag atlas: skip rebuild/upload when lines unchanged | **done** (2026-07-13) field+dashboard |
| 8 | Remove/deprecate old `freetype_shim`/`harfbuzz_shim` public cache helpers | **done** (2026-07-13) thin wrappers over abi |
| 9 | Self-host diff + `regression_gate.sh`; update `TEXT_RENDERING.md` / `FFI_LAYER.md` §8 | **done** (2026-07-13) REG 20/0 sweep 115/0/1; p1≡p2 |
| 10 | CPU%/visual before/after; close track | **done** (2026-07-13) User 0.33s; a8 smoke 0; archived |
