# Track: MSDF (EDT/SDF) алгоритм — на MLC, не C++

Parent: [../FFI_LAYER.md](../FFI_LAYER.md) §8, [../TEXT_RENDERING.md](../TEXT_RENDERING.md), [../PLAN.md](../PLAN.md).
Trigger: пользователь 2026-07-11 — весь функционал должен собираться без
ручного C++. `msdf_shim.cpp` — не биндинг к библиотеке (проект намеренно
не взял системный `msdfgen`), это собственный EDT/SDF-алгоритм, написанный
на C++ вместо MLC.

## Status: open — STEP=1 next

## Проблема (проверено в коде)

`runtime/src/text/msdf_shim.cpp` (253 строки) + `runtime/include/mlc/text/msdf_shim.hpp`
(28 строк) — чистая математика: Euclidean Distance Transform по битмапу
глифа → signed distance field → упаковка в RGB8. Ни одного `#include`
внешней библиотеки (кроме FreeType-битмапа на входе, который уже приходит
как байтовый буфер из `freetype_shim`). Публичный интерфейс — 6 функций
(`msdf_generate`, `msdf_width/height/px_range`, `msdf_byte_at/count`,
`msdf_rgb_data`) поверх внутреннего C++ `std::vector<float>`-массива.

Нет причины писать EDT/SDF на C++ — это numeric array algorithm,
MLC `Array<i32>`/`Array<f64>` достаточно (сравнимо с `points_mem.mlc`
benchmark'ом, который уже показал компактную работу с массивами записей).

## Цель

`msdf_generate`-эквивалент — чистая MLC-функция: вход — байтовый A8-битмап
(из FreeType, уже FFI, не трогать), выход — `Array<i32>` RGB8-байтов SDF.
Никакого C++ в этом слое; `msdf_shim.cpp`/`.hpp` удаляются.

## Decision (STEP=1)

| Вопрос | Вариант | Рекомендация |
|--------|---------|--------------|
| Представление промежуточного float-буфера EDT | `Array<f64>` (ширина×высота, row-major) | да, обычный MLC array, без `Shared`-обёртки на элемент (int/float элементы уже плоские в текущем codegen) |
| Производительность vs текущий C++ (перф-бюджет один прогон на глиф в build/runtime-time атлас, не hot-path per-frame) | принять regression до ~2-3x, если функционально корректно | атлас строится один раз при первом использовании глифа (LRU cache), не per-frame — 2-3x на одном глифе незаметно |
| Тест | побайтовое сравнение выхода нового MLC-кода со старым C++ на golden входе (`text_a8_privet_24.rgba` или отдельный glyph-битмап) до удаления C++ | да — та же golden-стратегия, что `TRACK_TEXT_RENDERING` (MAE-порог) |

## Scope

1. Decision (буфер, тест-стратегия).
2. Портировать EDT (8-point/Felzenszwalb-подобный проход или текущий
   алгоритм построчно — прочитать `msdf_shim.cpp` целиком перед портом,
   не переизобретать другой алгоритм) на MLC-функцию, вход/выход через
   `Array<i32>`.
3. Golden-тест: тот же вход, побайтовое сравнение (или MAE-порог как в
   `TEXT_RENDERING`) между старым C++ выходом (сохранить как reference
   fixture перед удалением) и новым MLC.
4. Заменить вызов в `misc/gui/text_renderer.mlc` (или где используется
   MSDF-режим) на новую MLC-функцию.
5. Удалить `msdf_shim.cpp`/`.hpp`, ссылки в `build_bin.sh`.
6. Self-host diff + `regression_gate.sh`; обновить `TEXT_RENDERING.md`.

## Out of scope

- Замена EDT-алгоритма на другой (не цель — цель язык реализации, не
  качество SDF).
- MSDF (multi-channel) — если текущий `msdf_shim` уже делает multi-channel,
  сохранить тот же алгоритм 1:1, не упрощать до single-channel SDF.

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | Decision + прочитать `msdf_shim.cpp` целиком, зафиксировать алгоритм 1:1. | pending |
| 2 | Golden fixture из текущего C++ (до порта, для последующего сравнения). | pending |
| 3 | Порт EDT/SDF на MLC-функцию. | pending |
| 4 | Golden-сравнение MLC vs C++ (MAE-порог как в `TEXT_RENDERING`). | pending |
| 5 | Переключить вызывающий код; удалить `msdf_shim.*`. | pending |
| 6 | Self-host diff + regression_gate; docs; close. | pending |
