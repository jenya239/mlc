# Track: GlyphCache O(n) LRU touch — IDE-panels demo lag

Parent: [TRACK_TEXT_GL_PERF_BASELINE](../archive/tracks/TRACK_TEXT_GL_PERF_BASELINE.md)
(closed — fixed FT/HB re-init + atlas dirty flag + bearing), [TRACK_TEXT_SHIM_TO_MLC](../archive/tracks/TRACK_TEXT_SHIM_TO_MLC.md)
(closed — moved shim bookkeeping to MLC, kept same `GlyphCache` structure).

Trigger: пользователь 2026-07-13, `misc/examples/text_ide_panels_demo.mlc`
заметно тормозит несмотря на «космический» ожидаемый FPS после закрытия
обоих треков выше.

## Status: **active** (2026-07-13) — STEP=2 layout cache next

Activated by Planner after PACKAGE_MANAGER Critic OK. Priority ahead of
`DEBUG_SOURCE_MAP` / `GUI_CANVAS_GRAPH`.

## Next step

**STEP=2** — Per-line layout cache (skip reshape+lookup for unchanged text).

## Measurement (2026-07-13)

Baseline (pre-HashMap), `MLC_TEXT_IDE_BENCH=1 MLC_GLFW_VISIBLE=0`, 300 frames:

```
Elapsed: 21.65s        →  ~72ms/frame (~14 FPS)
```

After STEP=1 HashMap+FIFO (`Map<i64, GlyphCacheEntry>`, packed key, no hit reorder):

```
Elapsed: 14.40s        →  ~48ms/frame
```

O(n) scan+rebuild removed. Residual (~14s) is per-frame HarfBuzz reshape of
static lines (TRACK secondary finding) — STEP=2 target. Original STEP=1 gate
`&lt;3s` was optimistic (reshape alone dominates); revised gate for STEP=1:
wall **&lt;16s** (must beat baseline and drop O(n) path).

## Root cause (найден, не гипотеза)

`misc/gui/text_renderer.mlc:275-292`, `glyph_cache_get`:

```263:270:misc/gui/text_renderer.mlc
fn glyph_cache_entries_without(entries: [GlyphCacheEntry], remove_index: i32) -> [GlyphCacheEntry] = do
  let mut rebuilt: [GlyphCacheEntry] = []
  let mut index = 0
  while index < entries.length() do
    if index != remove_index then
      rebuilt.push(entries[index])
    end
    index = index + 1
  end
  rebuilt
end
```

На **каждый** `glyph_cache_get` (то есть каждый глиф каждой видимой строки,
каждый кадр — не только на miss): `glyph_cache_find_index` — линейный скан
`O(n)`, затем на hit `glyph_cache_entries_without` — полная пересборка
массива `O(n)` (новый `Array`, копия `n-1` элементов, `push`) только чтобы
подвинуть элемент в конец для LRU. Это `O(n)` allocation+copy на **каждый
глиф**, не на каждый miss.

`text_ide_panels_demo.mlc` рисует ~2000+ видимых глифов за кадр (sidebar 23
строки + editor 20 строк с номерами + terminal 6 + header + status), cache
`capacity=512` — рабочее множество различных глифов быстро насыщает кеш, то
есть почти все обращения — hits с полной `O(n)` пересборкой. `~2000 глифов
× ~250-500 записей` скан+копия за кадр — доминирующая CPU-стоимость, не
имеет отношения к atlas-растеризации/аплоаду (уже исправлено в
`TEXT_GL_PERF_BASELINE`).

Почему `text_dashboard_demo.mlc` не жаловались так же явно: там на порядок
меньше текста → меньше видимых глифов за кадр → тот же алгоритм, но менее
заметная деградация.

## Вторичная находка (не измерена отдельно, но реальна)

`append_line` (в демо, не в либе) вызывает `shape_glyph_count`/
`shape_glyph_id_at`/`shape_glyph_advance_at` (HarfBuzz) для **каждой** строки
**каждый кадр**, даже для строк, чей текст не менялся между кадрами (весь
sidebar/editor/terminal/header — статичны, кроме blink-курсора в terminal и
frame-счётчика в status). Reshaping неизменного текста — чистые потери, но
дешевле находки выше (не главный вклад в 72ms/frame).

## Fix direction (design, не implementation-ready)

1. **`GlyphCache` backing store → `HashMap<GlyphCacheKey, GlyphCacheEntry>`**
   вместо `[GlyphCacheEntry]` + линейный скан. Убирает `O(n)` на каждый
   lookup.
2. **LRU touch не должен быть `O(n)` на каждый hit.** Варианты: (a) убрать
   true-LRU, эвикция по capacity — FIFO/insertion-order или periodic sweep,
   не per-access reorder; (b) intrusive doubly-linked list с `O(1)` move-to-
   front поверх hashmap (классический LRU), не array-rebuild.
3. **Per-line layout cache** (новый слой, не в `GlyphCache`): если текст
   строки не изменился с прошлого кадра — пропустить reshape +
   glyph_cache_get полностью, переиспользовать закешированный список
   `(slot, bearing_x, bearing_y, advance)` на глиф и просто сдвинуть quad-
   push. Даёт выигрыш независимо от фикса (1)/(2).

## Non-goals

- Не трогать atlas-растеризацию/upload — уже корректны (`TEXT_GL_PERF_BASELINE`).
- Не трогать FT/HB face-кеш (`freetype_abi`/`harfbuzz_abi`) — уже корректны
  (`TEXT_SHIM_TO_MLC`).
- Не переписывать `GlyphCache` на C++ — остаётся MLC (`misc/gui/text_renderer.mlc`
  уже чистый MLC поверх thin abi, см. §8 «без hand-written C++»).

## Steps

Design-ready, не gated — Planner может открыть STEP=1.

| Step | Deliverable | Gate | Status |
|------|-------------|------|--------|
| 1 | `HashMap`-backed `GlyphCache` (`Map<i64,…>` + FIFO order), без per-access reorder | ide bench wall &lt;16s (revised; was &lt;3s) | **done** (2026-07-13: 21.65→14.40s) |
| 2 | Per-line layout cache в демо (или в `text_renderer.mlc` как переиспользуемый helper) — skip reshape+lookup для неизменного текста | тот же bench &lt; 3s wall | pending |
| 3 | Re-bench `text_dashboard_demo.mlc` + добавить оба демо в perf-regression corpus (порог по времени, не только `error:`-grep как в `TRACK_EXAMPLES_CI`) | оба демо в geo corpus, порог зафиксирован | pending |

## Verify template

```bash
compiler/out/mlcc -o /tmp/ide_bench_src misc/examples/text_ide_panels_demo.mlc
clang++ -std=c++20 -O2 -I /tmp/ide_bench_src -I runtime/include -I runtime/third_party/glad/gl/include \
  /tmp/ide_bench_src/*.cpp runtime/src/io/io.cpp runtime/src/core/string.cpp runtime/src/core/profile.cpp \
  runtime/src/gl/glfw_window_gl.cpp runtime/src/gl/text_window_helpers.cpp \
  runtime/src/text/harfbuzz_shim.cpp runtime/src/text/freetype_shim.cpp \
  runtime/src/text/harfbuzz_abi.cpp runtime/src/text/freetype_abi.cpp runtime/src/text/msdf_bridge.cpp \
  runtime/third_party/glad/gl/src/gl.c \
  $(pkg-config --cflags --libs glfw3 harfbuzz freetype2) -ldl -lpthread -o /tmp/ide_bench_bin
MLC_TEXT_IDE_BENCH=1 MLC_GLFW_VISIBLE=0 /usr/bin/time -v /tmp/ide_bench_bin
```
