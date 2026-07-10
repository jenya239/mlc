# Track: Text rendering (HarfBuzz + FreeType + OpenGL) на MLC

Parent: [../PLAN.md](../PLAN.md), [../TEXT_RENDERING.md](../TEXT_RENDERING.md)
(полная спецификация, читать перед началом — референсы, FFI-поверхность по
группам риска, открытые вопросы §5).

Приоритет: **личный проект пользователя, низкий в общей очереди** —
разблокирован закрытием [TRACK_FFI_LAYER](../archive/tracks/TRACK_FFI_LAYER.md)
(STEP=1–8 closed 2026-07-09). Не поднимать выше `TRACK_MIR_VM_FULL` без
явной команды.

## Status: **open** — STEP=0–6 **done**; STEP=7 next (MSDF)

**Planner 2026-07-10:** After `TRACK_MIR_VM_FULL` Epic 4 closed (STEP=12), queue
skips SUPERVISOR (deferred) and LANG_REGION_ARENA (design-blocked). Next
executable track is this one. Epic 5 MIR_VM remains unauthorized.

**Driver 2026-07-10:** STEP=1 — decisions in `TEXT_RENDERING.md` §5.1/§5.3;
§5.2 deferred to STEP=5.

**Driver 2026-07-10:** STEP=2 — `runtime/src/text/freetype_shim.cpp` +
`misc/examples/freetype_glyph_smoke.mlc`; `build_bin.sh` optional freetype2;
gate `run_freetype_glyph_smoke.sh`.

**Driver 2026-07-10:** STEP=3 — `harfbuzz_shim` + `text_shaper_shape` →
`[ShapedGlyph]`; Cyrillic smoke; fix `gen_string_literal` to use `byte_size()`.

**Driver 2026-07-10:** STEP=4.1 — `GlyphAtlas` shelf pack (pure MLC);
`glyph_atlas_smoke.mlc` + `run_glyph_atlas_smoke.sh` (native; shelf wrap,
new page, A8/RGB8 split, oversized reject).

**Driver 2026-07-10:** STEP=4.2 — `GlyphCache` LRU insert/get/evict;
`glyph_cache_smoke.mlc` + `run_glyph_cache_smoke.sh`.

**Driver 2026-07-10:** STEP=4.3 — `MLC_SANITIZE` in `build_bin.sh`;
`run_text_shim_sanitize_gate.sh` (address+undefined × freetype+harfbuzz).

**Driver 2026-07-10:** STEP=5 — §5.2 decided: EGL shim + `eglGetProcAddress`
(no runtime `extern fn` primitive); `loader_shim.cpp`;
`gl_loader_smoke.mlc` + `run_gl_loader_smoke.sh`; CI job `gl-loader-smoke`.

**Driver 2026-07-10:** STEP=6.1 — `text_renderer_shim.cpp`: FT A8 → GLES2 FBO
textured quad → readback; MAE vs CPU blit (tol 8/255); smoke gate.

**Driver 2026-07-10:** STEP=6.2 — HB-shaped «Привет» multi-quad FBO; golden
`text_a8_privet_24.rgba` exact CPU lock + MAE vs GL; STEP=6 closed.

## Test strategy (обязательно к соблюдению, не только для STEP=6)

Пирамида: **GPU нужен только на верхнем слое.** Шейпинг, растеризация,
packing атласа, MSDF-генерация — чистые данные, тестируются без GL вообще
(уже так и делалось в STEP=2-3, продолжать так же в STEP=4/7).

1. **Sanitizer с STEP=4, не откладывать до конца трека.** Shim-код
   (`freetype_shim.cpp`, `harfbuzz_shim.cpp`, будущий atlas/MSDF) — сырые
   указатели, C-массив `hb_glyph_info_t*`+length, копирование bitmap —
   классический источник buffer overflow. ASan/UBSan гонять на
   `misc/examples/*_smoke.mlc` фикстурах каждый STEP начиная с 4, по
   образцу `scripts/concurrency_sanitize_gate.sh` (не ждать отдельного
   финального прогона).
2. **Не рассчитывать на системные шрифты в тестах** — bundled test-font
   (уже верно: DejaVu в STEP=2-3 smoke) — держать так и дальше, шрифт как
   fixture в репозитории, не системная зависимость.
3. **Headless GL для STEP=6+**: EGL surfaceless + Mesa llvmpipe (софтверный
   рендер, детерминированный между машинами) — не Xvfb+GLX (зависит от
   системного драйвера, менее детерминированно), не расчитывать на
   реальный GPU в CI-раннере. Добавить в `.github/workflows/ci.yml` новую
   job по образцу существующей `concurrency-sanitize` (строка 63) —
   `apt-get install` Mesa/EGL headless-пакеты — **до** начала STEP=6, не
   после.
4. **Golden image — pixel-diff/SSIM с допуском, не байт-в-байт.** Даже
   софтверный рендерер даёт микро-отличия между версиями Mesa. Референс
   `gl-gui-pipeline-2025` (см. `TEXT_RENDERING.md` §1) сам признаёт в своей
   архитектурной доке "CI/SSIM described but not connected" — не повторять
   этот пробел, подключить сразу при первом golden-тесте STEP=6, не
   оставлять аспирационным пунктом.
5. **Тест-матрикс** (минимум, по образцу покрытия `gtk-gl-cpp-2025`):
   шейпинг — ASCII/кириллица/пустая строка/unknown-glyph fallback/смешанные
   скрипты; растеризация — граничные pixel size; атлас — заполнение
   страницы → новая страница, LRU eviction, раздельные A8/RGB8(MSDF)
   страницы; GPU — один глиф pixel-diff, целая строка pixel-diff, resize/
   projection-матрица; стресс — тысячи уникальных глифов (давление на кэш).

## Steps

| Step | Item | Status |
|------|------|--------|
| 0 | Предусловие: `TRACK_FFI_LAYER` closed (STEP=1-6, минимум без concurrency-метаданных STEP=7-8). | **done** (FFI closed 2026-07-09) |
| 1 | Решить открытые вопросы `TEXT_RENDERING.md` §5.1 (C array view) и §5.3 (msdfgen шим) — design-turn, не кодогенерация. | **done** (2026-07-10: RawPointer+length view; msdf_shim.cpp at STEP=7; §5.2 → STEP=5) |
| 2 | `extern type`/`extern fn` биндинги FreeType (`TEXT_RENDERING.md` §3.1, FreeType-подмножество). Тест: загрузка `.ttf`, `FT_Load_Glyph`+`FT_Render_Glyph` на 1 символе, сравнение bitmap-размера с ожидаемым. | **done** (shim `glyph_bitmap_packed`; smoke exit 0; DejaVu 'A'@32px) |
| 3 | `extern type`/`extern fn` биндинги HarfBuzz (§3.1, HarfBuzz-подмножество) + `hb_ft_font_create` мост. `TextShaper.shape()` на MLC: строка UTF-8 (включая кириллицу) → `ShapedGlyph[]`. | **done** (shim + `text_shaper_shape`; smoke «Привет»=6; `literals.mlc` byte_size) |
| 4 | `GlyphAtlas` (shelf bin-pack) + `GlyphCache` (LRU) — чистый MLC, без FFI. Юнит-тесты на packing/eviction (тест-матрикс п.5 выше). **Плюс: ASan/UBSan гейт на shim-код с этого шага** (Test strategy п.1). | **done** (2026-07-10: 4.1 atlas; 4.2 LRU; 4.3 `run_text_shim_sanitize_gate.sh` asan+ubsan × FT/HB ok) |
<!-- sub-steps STEP=4:
  1) GlyphAtlas shelf pack in MLC (no FFI)
  2) GlyphCache LRU insert/get/evict
  3) unit tests packing + eviction (full page -> new page, LRU eviction, A8/RGB8 separation)
  4) ASan/UBSan run on freetype_shim/harfbuzz_shim smoke fixtures
-->
| 5 | Решить открытый вопрос §5.2 (`glXGetProcAddress` как runtime-полученный `extern fn(...)`) — design-turn + smoke на 1 функции (`glGenBuffers` или аналог) до начала полного биндинга. **Плюс: до этого шага добавить headless GL CI job** (Test strategy п.3 — EGL/Mesa llvmpipe, не Xvfb). | **done** (2026-07-10: EGL shim `eglGetProcAddress`; smoke `glGenBuffers`; CI `gl-loader-smoke`) |
| 6 | OpenGL биндинг узкого набора (§3.3, ~20-30 функций) + `TextRenderer` A8-путь — end-to-end glyph на экране (offscreen framebuffer + pixel-diff тест, не интерактивное окно). **Golden image с tolerance (pixel-diff/SSIM), не точное совпадение** (Test strategy п.4) — подключить с первого теста, не откладывать. | **done** (2026-07-10: 6.1 glyph MAE; 6.2 shaped «Привет» + golden RGBA) |
<!-- sub-steps STEP=6:
  1) text_renderer_shim: FT A8 → FBO → MAE vs CPU blit (tol 8) + smoke gate — done
  2) string/shaped-glyph path + golden fixture + CI — done
  3) optional narrow GL extern surface — skipped (shim owns GL; §5.2)
-->
| 7 | MSDF-генерация (§3.2) + RGB8 atlas page + MSDF шейдер + `RenderMode` переключение по pixel size. Численное сравнение distance-field с эталоном (без GPU для самой генерации). | pending |
| 8 | Verify-gate + close: self-host (`mlcc`→`mlcc2`→`diff`), `regression_gate.sh`, критерий приёмки `TEXT_RENDERING.md` §6 (фаза 1-2 headless) + tolerance-порог golden-тестов зафиксировать в критерии приёмки явным числом. | pending |

## Out of scope (см. `TEXT_RENDERING.md` §1, §4.5)

- ICU (bidi, line breaking, script segmentation) — не реализовано ни в одном
  референсе как рабочий код, отдельный будущий трек.
- fontconfig (font matching/fallback) — то же.
- Полный OpenGL bindgen-генератор — не нужен для этой задачи (узкий набор
  функций, см. §3.3), но и не создавать вместо него отдельный трек заранее.
- GTK/ffmpeg-интеграция — не относится к этому треку.

## Per-turn template

```
| step | <0-8> |
| done | <one line> |
| verify | <числа/факт> |
| next | ROLE=Driver STEP=<n+1> TRACK_TEXT_RENDERING (или BLOCKED, если STEP=0) |
```
