# Track: Build speed 3 — ccache CI cache, post-closure-escape `-ftime-trace` re-check

Parent: [../PLAN.md](../PLAN.md) §2.9. Продолжение
[TRACK_BUILD_SPEED2](../archive/tracks/TRACK_BUILD_SPEED2.md) (closed,
STEP=5 ninja и STEP=6 `-ftime-trace` были **осознанно deferred** до тех пор,
пока не закроется [TRACK_LANG_CLOSURE_ESCAPE](TRACK_LANG_CLOSURE_ESCAPE.md) —
тот трек закрыт 2026-07-09 (STEP=4, `48534e81`), условие снято.

**Приоритет: пользователь явно попросил поставить этот трек в очередь перед
продолжением `TRACK_CONCURRENCY_V2` STEP=2+** (см. note в
`TRACK_CONCURRENCY_V2.md` — Status). Короткий трек, не трогает checker/codegen
кроме STEP=3 (условно).

## Status: **open** — STEP=1 ready for Driver

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | CI ccache cache: `ci.yml` не кеширует `~/.cache/ccache` между запусками (`actions/cache`) — весь фикс из `TRACK_BUILD_SPEED2` STEP=3 (`CCACHE_SLOPPINESS=pch_defines,time_macros`, cold 416s → warm 2.4s) сейчас работает только локально. Добавить `actions/cache@v4` шаг в `ci.yml` (key на `github.sha`/restore-keys на префикс ветки — ccache сам разберётся по контенту через уже включенный content-hash режим); проверить hit rate через `ccache -s` в логе job. Если есть другие workflow, что собирают `mlcc` (`build-mlcc-once.yml`, `triple-bootstrap.yml`) — тот же шаг туда. | **next** |
| 2 | `-ftime-trace` на текущем самом крупном TU в `compiler/out` (найти через `wc -l compiler/out/*.cpp \| sort -n`) под clang; таблица инстанцирований `InstantiateFunction`/`Backend` конкретно для `Shared<T>`/`Array<T>`/`std::function<...>` — с числами, не на глаз. Сравнить качественно: доминируют ли эти три типа всё ещё, теперь когда часть замыканий (non-escaping top-level fn-параметры) стала `template<class F>` вместо `std::function` (`TRACK_LANG_CLOSURE_ESCAPE`). | pending |
| 3 | Решение по итогам STEP=2 (не начинать реализацию заранее): (a) если инстанцирование `Shared`/`Array`/`std::function` всё ещё доминирует — открыть follow-up на `extern template` явную инстанциацию горячих комбинаций, с таблицей из STEP=2 как обоснованием; (b) если узкое место — сама bash-оркестрация параллельной компиляции, а не время компиляции TU — вернуться к `TRACK_BUILD_SPEED2` STEP=5 (ninja); (c) если ни то ни другое — закрыть трек с выводом "дальше улучшать нечего без нового измеренного узкого места". | pending |
| 4 | Docs: обновить `docs/PLAN.md` §2.9 (эта строка); явно зафиксировать вне-скоуп: C++20 modules — не исследовать (тулинг под modules ещё незрел у clang/gcc для многофайловых проектов, риск выше выигрыша, не приоритет); `TRACK_MIR_VM_FULL` — не пересекается с этим треком (интерпретация без g++, 20-80× медленнее *исполнения*, не про скорость *сборки*), только cross-ref, не путать в SESSION/PLAN. | pending |

## Verify gate

```bash
# STEP=1
# ci.yml валиден (actions lint/dry-run или просто визуальная проверка синтаксиса actions/cache);
# локальный аналог: MLCC_OBJ_CLEAN=1 два раза подряд без правок исходников, ccache -s показывает hits на втором прогоне.

# STEP=2 (только замер, без изменений кода)
clang++ -std=c++20 -ftime-trace -I compiler/out -I runtime/include -c compiler/out/<largest>.cpp -o /dev/null
python3 -c '...' # или scripts/*.rb — разбор .json трейса, таблица по символам

# STEP=3 (если триггерится по решению)
bundle exec rake test_compiler_mlc
compiler/build.sh
scripts/regression_gate.sh
```

## Out of scope

- C++20 modules (см. STEP=4).
- Аренда/апгрейд сервера для сборки — не в этом треке (обсуждалось с
  пользователем отдельно, не оформлено как трек; поднимать заново только
  если после STEP=1-3 остаётся неустранимое узкое место в железе, а не в
  тулчейне).
- `MIR_VM_FULL` — отдельный трек, не про скорость сборки.

## Per-turn template

```
| step | <1-4> |
| done | <one line> |
| verify | <числа/факт> |
| next | ROLE=Driver STEP=<n+1> TRACK_BUILD_SPEED3 (или возврат на TRACK_CONCURRENCY_V2 STEP=2 после STEP=4) |
```
