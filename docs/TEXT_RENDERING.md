# Text rendering (HarfBuzz + FreeType + OpenGL) — дизайн порта на MLC

Parent: [PLAN.md](PLAN.md), [FFI_LAYER.md](FFI_LAYER.md) §7 (порядок биндингов).
Trigger: два референсных C++ прототипа в `~/workspaces/graphics/`
(`gtk-gl-cpp-2025`, `gl-gui-pipeline-2025`) — анализ 2026-07-09, без копирования
кода, только архитектура. Цель — эффективный текстовый рендеринг как основа
будущего media-фреймворка (flash-like) на MLC.

**Зависимость: `TRACK_FFI_LAYER` closed 2026-07-09** (`RawPointer`, `extern fn`/`lib`/`type`+`drop`, C fptr, concurrency attrs). Трек text rendering **open**, низкий приоритет.

## 1. Референсы (факты, не код)

### gtk-gl-cpp-2025 — HarfBuzz+FreeType pipeline

```
UTF-8 → TextShaper.shape() → hb_ft_font_create → hb_shape() → ShapedGlyph[]
      → FreeTypeFace.rasterize_bitmap() → A8 bitmap
      → TextureAtlasManager (shelf bin-pack, 2048×2048 страницы, GL_R8)
      → GlyphCache (LRU, 1024 записей)
      → TextRenderer: 2 треугольника/глиф → glDrawArrays
```

Модульная декомпозиция (взять за эталон интерфейсов при порте):
`FreeTypeFace`/`FreeTypeLibrary` (RAII) → `HarfBuzzBuffer`/`HarfBuzzFont`
(RAII) → `TextShaper` → `TextureAtlasManager`/`BinPacker` → `GlyphCache` (LRU)
→ `TextRenderer`.

Известные ограничения оригинала (не наследовать в порте): `hb_shape()` без
`hb_feature_t` (лигатуры/кернинг — заглушка), `flush()` биндит только
страницу атласа 0, нет ICU/bidi/fontconfig.

### gl-gui-pipeline-2025 — MSDF

Архитектурный документ проекта (ICU+HarfBuzz+FreeType+OpenGL+MSDF) — целевая
спека, не отражение кода. Реальный код: MSDF через `msdfgen`
(`loadGlyph` → `edgeColoringSimple` → `generateMSDF`, pxRange=4), без
shaping/ICU, статический атлас на фиксированный набор символов. Шейдер:
`median(r,g,b)` + `fwidth()` для screen-space distance range.

MSDF даёт single-atlas-entry-per-glyph независимо от размера отображения (в
отличие от bitmap A8, который надо перерастеризовывать на каждый pixel size).

## 2. Целевая архитектура для MLC

```
mlc::text::FreeTypeFace     — RAII, extern type FT_Face + drop "FT_Done_Face"
mlc::text::HarfBuzzFont     — RAII, extern type hb_font_t + drop "hb_font_destroy"
mlc::text::TextShaper       — shape(face, text, options) -> Array[ShapedGlyph]
mlc::text::GlyphRasterizer  — bitmap (A8) | msdf (RGB8), режим по pixel size
mlc::text::GlyphAtlas       — shelf packing, Shared[AtlasPage]
mlc::text::GlyphCache       — LRU, ключ (font_id, glyph_index, size_px, mode)
mlc::gl::Renderer           — batched quads, A8/MSDF шейдеры
```

Типы `ShapedGlyph`, `GlyphCacheKey`, `AtlasSlot`, `RenderMode` — калька
интерфейсов из `gtk-gl-cpp-2025` (проверенный дизайн, не переизобретать).

## 3. FFI-поверхность — три независимых по риску группы

### 3.1 HarfBuzz + FreeType (низкий риск, маленькая поверхность)

~15-20 функций суммарно (`FT_Init_FreeType`, `FT_New_Face`,
`FT_Set_Pixel_Sizes`, `FT_Load_Glyph`, `FT_Render_Glyph`, `FT_Done_Face`,
`hb_buffer_create/destroy`, `hb_buffer_set_direction/script/language`,
`hb_buffer_add_utf8`, `hb_shape`, `hb_buffer_get_glyph_infos/positions`,
`hb_ft_font_create`, `hb_font_destroy`). Статическая линковка
(`extern lib "freetype"`, `extern lib "harfbuzz"`), `pkg-config` есть в обеих
референс-системах. Не требует bindgen — ручная разметка укладывается в объём
одного трека (сравнимо с libpq proof-of-concept из `FFI_LAYER.md` §5).

Требует решения не покрытого текущим `FFI_LAYER.md`: `hb_buffer_get_glyph_infos`
возвращает `hb_glyph_info_t*` + `unsigned int* length` (C-массив через
out-параметр, не через `RawPointer[T]` одного значения) — нужен паттерн
"C array view": `RawPointer[T]` + отдельный `length: UInt32`, без copy в
`Array[T]` на каждый вызов (копия допустима для не-hot-path, но шейпинг —
hot path per frame). Зафиксировать как открытый вопрос §5.

### 3.2 MSDF-генерация (средний риск)

`msdfgen` — C++ библиотека (не чистый C ABI) с шаблонным API — прямой FFI
сложнее, чем HarfBuzz/FreeType. Варианты: (а) обернуть в узкий extern "C"
шим на стороне runtime (`runtime/src/text/msdf_shim.cpp`, компилируется
вместе с MLC runtime, экспортирует C-функции) — предпочтительно; (б) писать
MSDF-генерацию на MLC напрямую (алгоритм не сложный: edge coloring + signed
distance per channel) — избегает C++-FFI вообще, но больше кода. Решение —
на этапе реализации, не блокирует дизайн.

### 3.3 OpenGL (высокий риск объёма, НЕ требует bindgen для этой задачи)

Полный OpenGL API — тысячи функций (`FFI_LAYER.md` §7 запрещает биндинг
без генератора). Но `TextRenderer` в референсе использует **фиксированный
малый набор** (~20-30 функций: `glGenTextures`, `glBindTexture`,
`glTexImage2D`, `glTexSubImage2D`, `glGenBuffers`, `glBufferData`,
`glVertexAttribPointer`, `glCreateShader/Program`, `glDrawArrays`, и т.п.) —
объём сопоставим с libpq, ручная разметка допустима **для этого конкретного
рендерера**, не для OpenGL вообще.

Открытый риск: функции после OpenGL 1.1 на Linux не гарантированно
экспортированы напрямую из `libGL.so` — обычно грузятся через
`glXGetProcAddress`/loader (glad/epoxy в референсах). Для MLC это означает
`extern fn`, представляющие собой **не статический символ**, а
function-pointer, полученный runtime-вызовом `glXGetProcAddress("glGenBuffers")`
и приведённый к `extern fn(...)` типу (см. `FFI_LAYER.md` §2.4). Это решается
существующим дизайном C function pointer типа + `RawPointer[Void]` cast, не
требует нового языкового примитива — но нужен smoke-тест перед стартом
реализации фазы 3 (см. §4).

## 4. Фазы реализации (после закрытия FFI_LAYER)

1. HarfBuzz+FreeType биндинги (§3.1) + `TextShaper` на MLC, без GL —
   headless, проверяется через дамп `ShapedGlyph[]` в тестах.
2. `GlyphRasterizer` (A8 путь через FreeType) + `GlyphAtlas` (shelf packing,
   чистый MLC, без FFI) + `GlyphCache` (LRU, чистый MLC).
3. OpenGL биндинг узкого набора функций (§3.3) через `glXGetProcAddress`
   smoke, `TextRenderer` — A8-путь end-to-end (glyph на экране).
4. MSDF-генерация (§3.2) + RGB8 atlas page + MSDF шейдер, `RenderMode`
   переключение по pixel size.
5. Опционально далее (не проектировать сейчас): ICU (bidi/line-break),
   fontconfig (font matching/fallback) — оба отсутствуют в обоих референсах
   как рабочий код, только в документации одного из них.

## 5. Открытые вопросы — решения (2026-07-10, TRACK_TEXT_RENDERING STEP=1)

### 5.1 C array view (`hb_buffer_get_glyph_infos` / `positions`) — **decided**

**Решение:** без нового языкового примитива и без расширения `FFI_LAYER.md`.

- FFI: `extern fn hb_buffer_get_glyph_infos(buf: RawPointer[hb_buffer_t], length_out: RawPointer[u32]) -> RawPointer[hb_glyph_info_t]` (и аналог для positions) — уже покрыто `RawPointer` + out-param.
- Конвенция библиотеки `mlc::text`: пара `(ptr: RawPointer[T], length: u32)` = view на буфер HarfBuzz; **не** копировать в `Array[T]` на каждый доступ к глифу.
- Граница копирования: один раз в `TextShaper.shape()` — собрать `Array[ShapedGlyph]` (owned MLC) из view; дальше pipeline работает только с `Array`/`ShapedGlyph`.
- Hot path per frame = повторный `shape` → снова один copy на вызов; внутри shape — только индексный обход view.

### 5.2 `glXGetProcAddress` → runtime `extern fn(...)` — **deferred to STEP=5**

Не блокирует FreeType/HarfBuzz (STEP=2–4). Решение + smoke (`glGenBuffers` или аналог) — на STEP=5 перед OpenGL-биндингом. Предварительно: существующий C function pointer тип + загрузка адреса достаточны; нового примитива не ждать.

### 5.3 Шим `msdfgen` (C++ → C ABI) — **decided**

**Решение:** вариант (а) из §3.2 — узкий `extern "C"` шим в runtime.

- Файл: `runtime/src/text/msdf_shim.cpp` (+ header), линкуется с runtime; экспортирует C-функции (load glyph outline → generate MSDF bitmap + pxRange).
- MLC: обычный `extern fn` / `extern lib` к шиму (C ABI only; C++ `msdfgen` не торчит в MLC).
- Реализация шима — **STEP=7**, не раньше; STEP=2–4 (A8 FreeType path) не зависят от MSDF.
- Вариант (б) (чистый MLC MSDF) — запасной, только если шим окажется непропорционально тяжёлым; не default.

## 6. Критерий приёмки первого milestone (фаза 1-2, headless)

1. Загрузка `.ttf`/`.otf`, shaping строки UTF-8 (включая кириллицу) в
   `ShapedGlyph[]`, сравнение с ожидаемыми glyph indices/advances.
2. Растеризация глифа в A8 bitmap, атлас-паковка, LRU cache hit/miss.
3. Self-host: `mlcc` → `mlcc2` → `diff` идентичен (обычный гейт проекта).

Фаза 3+ (GL на экране) — критерий приёмки формулируется отдельно перед
стартом фазы (зависит от решения §5.2).

## 7. Тест-стратегия (2026-07-10)

Подробно — в `TRACK_TEXT_RENDERING.md` ("Test strategy"), здесь только
принцип: **GPU нужен только на самом верхнем слое** (шейпинг/растеризация/
паковка/MSDF-генерация — чистые данные, без GL). Для GPU-слоя — headless
через EGL+Mesa llvmpipe (не Xvfb, недетерминированно), golden image с
pixel-diff/SSIM допуском (не байт-в-байт — референс `gl-gui-pipeline-2025`
сам недоделал именно эту часть, не повторять). ASan/UBSan на shim-код
(`freetype_shim.cpp`/`harfbuzz_shim.cpp`/будущий MSDF) с STEP=4, не в конце
трека.
