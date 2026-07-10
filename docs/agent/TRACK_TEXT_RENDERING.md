# Track: Text rendering (HarfBuzz + FreeType + OpenGL) на MLC

Parent: [../PLAN.md](../PLAN.md), [../TEXT_RENDERING.md](../TEXT_RENDERING.md)
(полная спецификация, читать перед началом — референсы, FFI-поверхность по
группам риска, открытые вопросы §5).

Приоритет: **личный проект пользователя, низкий в общей очереди** —
разблокирован закрытием [TRACK_FFI_LAYER](../archive/tracks/TRACK_FFI_LAYER.md)
(STEP=1–8 closed 2026-07-09). Не поднимать выше `TRACK_MIR_VM_FULL` без
явной команды.

## Status: **open** — STEP=0–3 **done**; STEP=4 next (GlyphAtlas + GlyphCache)

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

## Steps

| Step | Item | Status |
|------|------|--------|
| 0 | Предусловие: `TRACK_FFI_LAYER` closed (STEP=1-6, минимум без concurrency-метаданных STEP=7-8). | **done** (FFI closed 2026-07-09) |
| 1 | Решить открытые вопросы `TEXT_RENDERING.md` §5.1 (C array view) и §5.3 (msdfgen шим) — design-turn, не кодогенерация. | **done** (2026-07-10: RawPointer+length view; msdf_shim.cpp at STEP=7; §5.2 → STEP=5) |
| 2 | `extern type`/`extern fn` биндинги FreeType (`TEXT_RENDERING.md` §3.1, FreeType-подмножество). Тест: загрузка `.ttf`, `FT_Load_Glyph`+`FT_Render_Glyph` на 1 символе, сравнение bitmap-размера с ожидаемым. | **done** (shim `glyph_bitmap_packed`; smoke exit 0; DejaVu 'A'@32px) |
| 3 | `extern type`/`extern fn` биндинги HarfBuzz (§3.1, HarfBuzz-подмножество) + `hb_ft_font_create` мост. `TextShaper.shape()` на MLC: строка UTF-8 (включая кириллицу) → `ShapedGlyph[]`. | **done** (shim + `text_shaper_shape`; smoke «Привет»=6; `literals.mlc` byte_size) |
| 4 | `GlyphAtlas` (shelf bin-pack) + `GlyphCache` (LRU) — чистый MLC, без FFI. Юнит-тесты на packing/eviction. | **pending** |
<!-- sub-steps STEP=4:
  1) GlyphAtlas shelf pack in MLC (no FFI)
  2) GlyphCache LRU insert/get/evict
  3) unit tests packing + eviction
-->
| 5 | Решить открытый вопрос §5.2 (`glXGetProcAddress` как runtime-полученный `extern fn(...)`) — design-turn + smoke на 1 функции (`glGenBuffers` или аналог) до начала полного биндинга. | pending |
| 6 | OpenGL биндинг узкого набора (§3.3, ~20-30 функций) + `TextRenderer` A8-путь — end-to-end glyph на экране (offscreen framebuffer + pixel-diff тест, не интерактивное окно). | pending |
| 7 | MSDF-генерация (§3.2) + RGB8 atlas page + MSDF шейдер + `RenderMode` переключение по pixel size. | pending |
| 8 | Verify-gate + close: self-host (`mlcc`→`mlcc2`→`diff`), `regression_gate.sh`, критерий приёмки `TEXT_RENDERING.md` §6 (фаза 1-2 headless). | pending |

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
