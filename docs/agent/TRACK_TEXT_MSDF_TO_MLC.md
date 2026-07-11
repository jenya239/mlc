# Track: MSDF (EDT/SDF) алгоритм — на MLC, не C++

Parent: [../FFI_LAYER.md](../FFI_LAYER.md) §8, [../TEXT_RENDERING.md](../TEXT_RENDERING.md), [../PLAN.md](../PLAN.md).
Trigger: пользователь 2026-07-11 — весь функционал должен собираться без
ручного C++. `msdf_shim.cpp` — не биндинг к библиотеке (проект намеренно
не взял системный `msdfgen`), это собственный EDT/SDF-алгоритм, написанный
на C++ вместо MLC.

## Status: **active** — STEP=5 next (retarget callers; delete msdf_shim)

**Driver 2026-07-11:** STEP=4 — `run_msdf_mlc_mae_gate.rb` PASS **MAE=0.0000**
(limit 8.0); fixtures `msdf_A_32_4.mask.txt` + `_mask.mlc`; gate
`msdf_mlc_mae_gate.mlc`.

**Driver 2026-07-11:** STEP=3 — `misc/gui/msdf.mlc` (`msdf_sdf_rgb_from_mask`,
Felzenszwalb 1D/2D, SDF pack R=G=B); cast helpers `i32_to_f64`/`f64_to_i32`/
`f64_infinity` in `math.hpp`; smoke `msdf_mlc_math_smoke.mlc` EXIT 0.

**Driver 2026-07-11:** STEP=2 — golden fixture
`misc/examples/fixtures/msdf_A_32_4.rgb` (3072 bytes, DejaVu 'A' 32/px_range=4)
+ `msdf_A_32_4.meta.txt`; regenerate/check via `scripts/dump_msdf_golden.rb`
(`--check` matches live C++). STEP=2 **done**.

**Driver 2026-07-11:** STEP=1 — Decision **locked**. Read
`msdf_shim.cpp` (253) + `msdf_shim.hpp` (28) end-to-end. Algorithm =
Felzenszwalb–Huttenlocher squared EDT (1D→2D columns then rows); dual
outside/inside fields; R=G=B single-channel SDF (not true multi-channel
msdfgen). FreeType mask stays FFI; port is math only.

**Planner 2026-07-11:** activated after SPAWN_DETACH Critic OK (`87ee1b0a`).
First remaining «без C++» queue item (§20b). Verified on disk:
`msdf_shim.cpp` 253 lines + `msdf_shim.hpp` 28; smokes
`msdf_glyph_smoke.mlc` / `msdf_renderer_smoke.mlc` present.

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

## Decision (STEP=1, 2026-07-11) — **locked**

Verified by reading `runtime/src/text/msdf_shim.cpp` in full.

### Algorithm (1:1 port target)

| Stage | Behavior in shim |
|-------|------------------|
| Mask | FreeType render → binary mask (`coverage >= 128` → 1); **stays FFI** |
| Seed | outside=0 / inside=1e20 on ink; outside=1e20 / inside=0 on empty |
| EDT | `distance_transform_1d` = Felzenszwalb & Huttenlocher squared EDT; `distance_transform_2d` = all columns then all rows |
| SDF | `signed = sqrt(outside) - sqrt(inside)`; `mapped = clamp(signed/px_range*0.5+0.5, 0, 1)`; byte = `round(mapped*255)` |
| RGB | R=G=B = same byte (comment: true multi-channel needs msdfgen; median shaders still work) |
| Cache | process-global `g_msdf_cache` (width/height/px_range/rgb); public getters |

### Locked choices

| Вопрос | Locked |
|--------|--------|
| Intermediate squared-distance buffers | `Array<f64>` row-major `field_size²` (C++ uses `float`; f64 OK) |
| Infinity seed for mask EDT | `1.0e20` (same as shim seed; 1D envelope still uses ±inf locally) |
| Channel model | Keep **single-channel SDF replicated to RGB** — do not invent true MSDF |
| Port boundary | MLC owns EDT+SDF+pack; FreeType glyph→mask stays thin FFI (or existing generate wrapper until STEP=5) |
| MLC core API (STEP=3) | `msdf_sdf_rgb_from_mask(mask: [i32], field_size: i32, px_range: i32) -> [i32]` — RGB interleaved bytes as i32 0..255 |
| Public surface | Keep `msdf_generate`/`msdf_width`/… names for demos until STEP=5; then MLC impl or thin FreeType+MLC glue |
| Perf budget | Accept ~2–3× vs C++ (atlas build, not per-frame) |
| Golden test | Dump C++ RGB for fixed glyph (e.g. codepoint 65, field_size=32, px_range=4) before delete; MAE ≤ **8.0/255** ([TEXT_RENDERING.md](../TEXT_RENDERING.md) precedent) |
| Rejected | Replacing EDT with another algo; true multi-channel msdfgen; Shared-per-element buffers |

## Scope

1. Decision (буфер, тест-стратегия).
2. Портировать EDT (Felzenszwalb 1D + 2D columns/rows — **as in shim**, not
   reinvent) на MLC-функцию, вход/выход через `Array<i32>` / `Array<f64>`.
3. Golden-тест: тот же вход, MAE ≤ 8.0/255 vs C++ fixture до удаления C++.
4. Заменить вызов в demos/`text_renderer` на новую MLC-функцию.
5. Удалить `msdf_shim.cpp`/`.hpp`, ссылки в `build_bin.sh`.
6. Self-host diff + `regression_gate.sh`; обновить `TEXT_RENDERING.md`.

## Out of scope

- Замена EDT-алгоритма на другой (не цель — цель язык реализации, не
  качество SDF).
- True multi-channel MSDF (msdfgen) — shim already R=G=B; keep that.

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | Decision + прочитать `msdf_shim.cpp` целиком, зафиксировать алгоритм 1:1. | **done** (2026-07-11: Felzenszwalb EDT; R=G=B; API locked) |
| 2 | Golden fixture из текущего C++ (до порта, для последующего сравнения). | **done** (2026-07-11: `msdf_A_32_4.rgb` + `dump_msdf_golden.rb --check`) |
| 3 | Порт EDT/SDF на MLC-функцию. | **done** (2026-07-11: `msdf.mlc` + math smoke EXIT 0) |
| 4 | Golden-сравнение MLC vs C++ (MAE-порог как в `TEXT_RENDERING`). | **done** (2026-07-11: MAE=0.0000 ≤8.0) |
| 5 | Переключить вызывающий код; удалить `msdf_shim.*`. | pending |
| 6 | Self-host diff + regression_gate; docs; close. | pending |

<!-- STEP=1 sub-steps: 1) read msdf_shim.cpp end-to-end; 2) lock Decision table (Array<f64> buffer, MAE golden, 1:1 algo); 3) note entry/exit fn signatures for STEP=2–3; PLAN→STEP=2 -->
<!-- STEP=2 sub-steps: 1) dump C++ reference bytes for one glyph fixture; 2) store under specs/ or misc/fixtures; 3) script to regenerate from msdf_shim until STEP=5 -->
<!-- STEP=3 sub-steps: 1) MLC module (e.g. misc/gui/msdf.mlc or stdlib text); 2) port EDT passes 1:1; 3) RGB packing matching shim -->
<!-- STEP=4 sub-steps: 1) gate MLC vs fixture MAE; 2) keep C++ shim until green; 3) document threshold -->
<!-- STEP=5 sub-steps: 1) retarget text_renderer/smokes to MLC; 2) delete msdf_shim.*; 3) drop build_bin refs -->
<!-- STEP=6 sub-steps: 1) regression_gate; 2) self-host diff; 3) TEXT_RENDERING.md + close → Critic -->
