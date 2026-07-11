# Text rendering (HarfBuzz + FreeType + OpenGL) — дизайн порта на MLC

Parent: [PLAN.md](PLAN.md), [FFI_LAYER.md](FFI_LAYER.md) §7 (порядок биндингов).
Trigger: два референсных C++ прототипа в `~/workspaces/graphics/`
(`gtk-gl-cpp-2025`, `gl-gui-pipeline-2025`) — анализ 2026-07-09, без копирования
кода, только архитектура. Цель — эффективный текстовый рендеринг как основа
будущего media-фреймворка (flash-like) на MLC.

**Зависимость: `TRACK_FFI_LAYER` closed 2026-07-09** (`RawPointer`, `extern fn`/`lib`/`type`+`drop`, C fptr, concurrency attrs).

**Статус headless-пайплайна:** [TRACK_TEXT_RENDERING](archive/tracks/TRACK_TEXT_RENDERING.md) **closed** 2026-07-10 (A8/MSDF FBO golden, MAE ≤ 8.0/255).

**Статус windowed/MLC-оркестрации:** [TRACK_TEXT_RENDERING_NATIVE](archive/tracks/TRACK_TEXT_RENDERING_NATIVE.md) **closed** 2026-07-11 (GLFW окно + `GlRenderer`/`TextRenderer` на MLC) — см. §8.

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

Исторически: `msdfgen` (C++ шаблоны) vs узкий runtime-шим vs чистый MLC.
**2026-07-11 ([TRACK_TEXT_MSDF_TO_MLC](archive/tracks/TRACK_TEXT_MSDF_TO_MLC.md)
closed):** EDT/SDF — `misc/gui/msdf.mlc`; FreeType→mask — thin
`runtime/src/text/msdf_bridge.cpp`. Системный `msdfgen` не используется.
Публичный `msdf_generate` / `msdf_byte_at` сохранён (MLC + RGB cache).

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
`eglGetProcAddress` / `glXGetProcAddress` / loader (glad/epoxy).
**Решение (обновлено TRACK_GL_GLAD_MIGRATION 2026-07-11):** нового языкового
примитива «runtime `extern fn`» нет. GL entry points — **вендоренный GLAD2**
(`runtime/third_party/glad/`); MLC вызывает через `glad_gl.mlc` /
`glad_gl_abi.hpp` (макросы GLAD ломают `&symbol` — тонкий abi). Windowed:
`gladLoadGL(glfwGetProcAddress)` после GLFW context. Headless CI: glad EGL/
GLES2 trees (не ручной `loader_shim`). Smoke: `misc/examples/gl_loader_smoke.mlc`
+ `compiler/tests/run_gl_loader_smoke.sh`.

## 4. Фазы реализации (после закрытия FFI_LAYER)

1. HarfBuzz+FreeType биндинги (§3.1) + `TextShaper` на MLC, без GL —
   headless, проверяется через дамп `ShapedGlyph[]` в тестах.
2. `GlyphRasterizer` (A8 путь через FreeType) + `GlyphAtlas` (shelf packing,
   чистый MLC, без FFI) + `GlyphCache` (LRU, чистый MLC).
3. OpenGL биндинг узкого набора (§3.3) через GLAD2 (§5.2), `TextRenderer` —
   A8-путь end-to-end (glyph на экране / offscreen FBO).
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

### 5.2 GL proc address → MLC — **done** (GLAD2, 2026-07-11)

- **Нет** нового примитива «runtime-полученный `extern fn`» (решение 2026-07-10
  сохраняется).
- **Реализация:** вендоренный **GLAD2** (`runtime/third_party/glad/{gl,egl}/`,
  Core 3.3 + EGL1.5/GLES2). Ручные `glfw_gl_dispatch.*` / `loader_shim.*`
  **удалены** ([TRACK_GL_GLAD_MIGRATION](archive/tracks/TRACK_GL_GLAD_MIGRATION.md)).
- MLC: `lib/mlc/common/stdlib/gl/glad_{gl,egl,gles2}.mlc` + thin
  `runtime/include/mlc/gl/glad_*_abi.hpp` (compat `mlc::gl::gl_*` where needed).
- Window/context/input: thin C++ `glfw_window_gl.*` + MLC `gl_window.mlc`;
  load via `gladLoadGL(glfwGetProcAddress)`.
- Desktop `gl.c` и headless `egl`/`gles2.c` **не** co-link (символ clash
  `glad_gl*`).
- Smoke: `gl_loader_smoke` (GLFW+glad genBuffers); `run_glad_link_smoke.sh`;
  GUI smokes на glad.

### 5.3 MSDF/SDF (FreeType mask + MLC EDT) — **done** (2026-07-11)

**Решение (обновлено TRACK_TEXT_MSDF_TO_MLC):** вариант (б) из старого §3.2 —
чистый MLC EDT/SDF; FreeType mask остаётся тонким FFI.

- Math: `misc/gui/msdf.mlc` (`msdf_sdf_rgb_from_mask` / `msdf_generate`)
- Mask FFI: `runtime/src/text/msdf_bridge.cpp` (+ `msdf_bridge.hpp`);
  `msdf_shim.cpp` **удалён**
- API: `msdf_generate` / `msdf_width` / `msdf_byte_at` / RGB cache для GL helpers
- Golden: `misc/examples/fixtures/msdf_A_32_4.rgb`, MAE gate
  `scripts/run_msdf_mlc_mae_gate.rb` (MAE=0 vs frozen C++ fixture)
- **STEP=7.1 (2026-07-10):** исходный C++ EDT shim (superseded by MLC port)
- **STEP=7.3 (2026-07-10):** `RenderMode` — `0=BitmapA8` если `pixel_size < 28`, иначе `1=MsdfRgb8`

## 6. Критерий приёмки первого milestone (фаза 1-2, headless)

1. Загрузка `.ttf`/`.otf`, shaping строки UTF-8 (включая кириллицу) в
   `ShapedGlyph[]`, сравнение с ожидаемыми glyph indices/advances.
2. Растеризация глифа в A8 bitmap, атлас-паковка, LRU cache hit/miss.
3. Self-host: `mlcc` → `mlcc2` → `diff` идентичен (обычный гейт проекта).

Фаза 3+ (GL offscreen) — §5.2 решён (GLAD2). **Tolerance golden/pixel-diff
(зафиксировано STEP=8, 2026-07-10):** mean absolute error (MAE) по RGB
каналам ≤ **8.0 / 255** между CPU-эталоном и GLES2 readback
(`text_renderer_a8_*`, shaped-string golden exact на CPU-компози; GL vs CPU —
MAE). SSIM не требуется для закрытия трека; при необходимости — отдельный
порог в будущем golden-тесте.

## 7. Тест-стратегия (2026-07-10)

Подробно — в `TRACK_TEXT_RENDERING.md` ("Test strategy"), здесь только
принцип: **GPU нужен только на самом верхнем слое** (шейпинг/растеризация/
паковка/MSDF-генерация — чистые данные, без GL). Для GPU-слоя — headless
через EGL+Mesa llvmpipe (не Xvfb, недетерминированно), golden image с
pixel-diff/SSIM допуском (не байт-в-байт — референс `gl-gui-pipeline-2025`
сам недоделал именно эту часть, не повторять). ASan/UBSan на shim-код
(`freetype_shim.cpp`/`harfbuzz_shim.cpp`/будущий MSDF) с STEP=4, не в конце
трека.

## 8. Windowed path + MLC orchestration (2026-07-11)

Headless golden path: `text_renderer_shim` (где ещё нужен) + glad EGL/GLES2
vendor trees. Windowed path:

```
GLFW window/input (glfw_window_gl) + GLAD2 (gladLoadGL)
  → GlRenderer / TextRenderer на MLC (шейдеры, VBO, atlas batch)
  → main loop на MLC (poll → draw → swap)
```

| Артефакт | Путь |
|----------|------|
| Track (native) | [archive/tracks/TRACK_TEXT_RENDERING_NATIVE.md](archive/tracks/TRACK_TEXT_RENDERING_NATIVE.md) |
| GLAD migration | [archive/tracks/TRACK_GL_GLAD_MIGRATION.md](archive/tracks/TRACK_GL_GLAD_MIGRATION.md) |
| GL entry points | `runtime/include/mlc/gl/glad_gl_abi.hpp` + `runtime/third_party/glad/gl/` |
| Window ABI | `runtime/include/mlc/gl/glfw_window_gl.hpp` |
| Demo | `misc/examples/text_window_demo.mlc` (`scripts/run_text_window_demo.sh`) |
| Smokes | `gl_renderer_smoke`, `text_renderer_native_smoke`, `glad_link` / `gl_loader` |

**Не в этом треке (закрыто отдельно):** layout/widgets/easing v0 —
[GUI.md](GUI.md), [archive/tracks/TRACK_GUI_FRAMEWORK.md](archive/tracks/TRACK_GUI_FRAMEWORK.md) **closed** 2026-07-11.
