# Track: Text rendering (HarfBuzz + FreeType + OpenGL) на MLC

Parent: [../PLAN.md](../PLAN.md), [../TEXT_RENDERING.md](../TEXT_RENDERING.md)
(полная спецификация, читать перед началом — референсы, FFI-поверхность по
группам риска, открытые вопросы §5).

Приоритет: **личный проект пользователя, низкий в общей очереди** —
**жёстко блокирован** закрытием [TRACK_FFI_LAYER](TRACK_FFI_LAYER.md) (нужны
STEP=1-6 минимум: RawPointer, extern fn codegen, extern lib, extern type +
drop, C function pointer тип).

## Status: **blocked** — не начинать до `TRACK_FFI_LAYER` close

## Steps

| Step | Item | Status |
|------|------|--------|
| 0 | Предусловие: `TRACK_FFI_LAYER` closed (STEP=1-6, минимум без concurrency-метаданных STEP=7-8). | blocked |
| 1 | Решить открытые вопросы `TEXT_RENDERING.md` §5.1 (C array view для `hb_buffer_get_glyph_infos`) и §5.3 (msdfgen шим) — design-turn, не кодогенерация. | pending |
| 2 | `extern type`/`extern fn` биндинги FreeType (`TEXT_RENDERING.md` §3.1, FreeType-подмножество). Тест: загрузка `.ttf`, `FT_Load_Glyph`+`FT_Render_Glyph` на 1 символе, сравнение bitmap-размера с ожидаемым. | pending |
| 3 | `extern type`/`extern fn` биндинги HarfBuzz (§3.1, HarfBuzz-подмножество) + `hb_ft_font_create` мост. `TextShaper.shape()` на MLC: строка UTF-8 (включая кириллицу) → `ShapedGlyph[]`. | pending |
| 4 | `GlyphAtlas` (shelf bin-pack) + `GlyphCache` (LRU) — чистый MLC, без FFI. Юнит-тесты на packing/eviction. | pending |
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
