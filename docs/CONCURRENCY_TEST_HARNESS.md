# Concurrency test superstand — дизайн

Источник: `CONCURRENCY_V2.md` §34-37 (deterministic TestRuntime, stress suites,
sanitizers, Clang thread-safety analysis), развёрнуто в конкретный
план — как добиться **полного автоматического покрытия** concurrency-примитивов,
а не единичных smoke-тестов. Статус: **T1–T5 реализованы** (2026-07-09); **T6 active** 2026-07-12
([TRACK_CONCURRENCY_TEST_HARNESS](agent/TRACK_CONCURRENCY_TEST_HARNESS.md));
T7 pending Decision.

## Проблема сегодня (проверено по коду)

- `runtime/test/run_concurrency_smoke.sh` — по одному smoke-тесту на примитив
  (`test_channel`, `test_arc`, `test_mutex`, `test_spawn`) + **один** stress-сценарий
  (`stress_channel.cpp::test_multi_producer_consumer_stress`). Нет close-during-send,
  cancel-during-recv, rapid open/close, sender/receiver destruction, 1M messages.
- `MLC_TSAN=1` — вручную, локально. `.github/workflows/ci.yml` **не запускает TSan
  вообще** (grep не находит ни одного упоминания).
- Нет ASan/UBSan gate для concurrency-кода.
- Нет детерминированного воспроизведения найденного race (сегодня — просто
  "тест иногда падает", без seed/schedule для повтора).
- Line coverage (если появится) **не эквивалентен** покрытию интерливингов —
  race может жить в строке, которая формально покрыта тестом, но не покрыта
  тем порядком исполнения потоков, где race проявляется. Это ключевое
  отличие concurrency-тестирования от обычного, важно не спутать метрики.

## Архитектура (4 слоя)

```text
Layer 1: Deterministic scheduler   — воспроизводимые interleaving'и (seed → schedule)
Layer 2: Stress scenario matrix    — полный список обязательных сценариев на примитив
Layer 3: Sanitizer CI matrix       — TSan/ASan/UBSan как gate, не опция
Layer 4: Fuzz/chaos (позже)        — случайные seed'ы + fault injection в CI nightly
```

### Layer 1 — Deterministic scheduler ("loom для MLC runtime")

Аналог Rust `loom` (не копируем API, только принцип): для набора хэндроут-C++
примитивов в `runtime/include/mlc/concurrency/` завести **параллельную тестовую
реализацию** с тем же публичным API, но исполняющуюся кооперативно в одном
процессе под управлением seed'ированного PRNG-планировщика — **не настоящий
параллелизм**, а сериализация всех "потоков" через turn-baton, где планировщик
на каждой точке синхронизации (`lock`, `send`, `recv`, `wait`, `notify`) решает,
какому virtual-thread передать ход.

```text
runtime/include/mlc/concurrency/testing/
  scheduler.hpp   — TestScheduler(seed): выбирает следующий runnable thread
  mutex.hpp       — TestMutex<T>: тот же API что mutex.hpp, yield в Scheduler
  channel.hpp     — TestChannel<T>: тот же API что channel.hpp, yield в Scheduler
```

Тест пишется один раз, но исполняется в двух режимах:

```text
режим A (сегодня, оставить как есть): реальные std::thread, TSan ловит race runtime'ом
режим B (новый): TestScheduler(seed) — детерминированная реплей, без реальных потоков
```

При падении режима B — точный `seed` в выводе, воспроизводится повторным
запуском с тем же seed (в отличие от TSan, где race либо проявился, либо нет —
без гарантии повтора).

**Почему не сразу exhaustive model checking (как полноценный loom/Concuerror):**
дороже в реализации на порядок, для MVP достаточно seeded randomized exploration
+ TSan как дополняющий слой (TSan детектирует race runtime'ом на реальных потоках,
не заменяет, а дополняет детерминированный слой).

**Область применения на старте:** только hand-written C++ unit/stress-тесты в
`runtime/test/` (тестируют сами примитивы). Пользовательский MLC-уровень
`TestRuntime.new(seed:)` (`CONCURRENCY_V2.md` §34) — отдельная, более поздняя
надстройка, требует `Task`/`Isolate` (Фазы 6-8 в `CONCURRENCY_V2.md`), тонкая
обёртка над этим же `TestScheduler`.

### Layer 2 — Stress scenario matrix (обязательный чек-лист, не опциональный)

| Примитив | Обязательные сценарии | Файл (план) |
|----------|------------------------|--------------|
| `Channel<T>` | single sender/receiver; many senders; many receivers; full queue (send blocks); empty queue (recv blocks); close during send; close during recv; cancel during send/recv (после `StopToken`, TRACK_CONCURRENCY_V2 STEP=5); sender destruction (drop last sender → `Closed`); receiver destruction; rapid open/close (1000 iterations); 1M messages throughput | `stress_channel.cpp` (расширить существующий) |
| `Mutex<T>` | high contention (N threads, tight loop); panic/exception while guarded (lock освобождается корректно); cancellation while waiting (после StopToken) | `stress_mutex.cpp` (новый) |
| `Arc<T>` | concurrent clone/drop под high contention; refcount race stress | `stress_arc.cpp` (новый) |
| `spawn`/`Task` | много одновременных spawn; результат корректно возвращается под нагрузкой; exception внутри spawn не роняет процесс (`std::terminate` не должен случиться — сегодня не проверено явно) | `stress_spawn.cpp` (новый) |
| `TaskScope` (после TRACK_CONCURRENCY_V2 Фаза 6) | child success/error/panic; parent cancellation; nested scopes; simultaneous sibling errors | будущий трек |
| `Isolate` (после Фазы 8) | mailbox overflow policies; serial processing под нагрузкой | будущий трек |

Каждый файл — оба режима (Layer 1 режим A и режим B) через общий тестовый
хелпер, не дублировать логику сценария.

### Layer 3 — Sanitizer CI matrix (gate, не ручной опциональный запуск)

```text
                debug   release   asan   ubsan   tsan
build-mlcc-once   ✓        ✓       —       —      —     (сегодня)
concurrency gate  ✓        —       ✓       ✓      ✓     (план)
```

`scripts/concurrency_sanitize_gate.sh` (новый) — оборачивает
`run_concurrency_smoke.sh` + все `stress_*` в цикл по `{asan, ubsan, tsan}`,
падает при первом non-zero exit. Встроить в `.github/workflows/ci.yml` как
обязательную job (не отдельный опциональный workflow) — сегодня TSan запускается
только если разработчик вручную поставил `MLC_TSAN=1` локально, что означает
регрессия может годами не всплывать в CI.

`mlcc test --sanitize=thread/address/undefined` (CLI-обёртка над тем же gate
для программ, написанных на MLC, не только runtime unit-тестов) — **не
блокирующий** пункт этого трека, отдельный шаг после того, как заработает
сам gate для runtime-тестов.

### Layer 4 — Fuzz/chaos (после Layer 1-3, отдельный трек)

Nightly CI job: N случайных seed'ов через `TestScheduler` на весь набор
stress-сценариев + (после `StopToken`/cancellation) случайная инъекция
cancel/disconnect в случайный момент. При падении — seed сохраняется как
regression corpus (аналогия с `TRACK_FUZZ_DIFF`, closed — та же идея
"seed → repro corpus", другой домен: там differential fuzzing компилятора,
здесь — concurrency schedules).

## Метрика "полное покрытие" — определение (не line coverage)

"Полное покрытие" в этом контексте означает **обе** метрики одновременно, не
одну:

1. **Scenario coverage** — 100% строк таблицы Layer 2 реализованы и зелёные
   (чек-лист, отслеживается в TRACK-степах, не автоматическая метрика).
2. **Code coverage** (`llvm-cov`/`gcov`), но **только** как вспомогательный
   сигнал "эта ветка вообще выполнялась хоть одним тестом" — не как
   самоцель и не как замена п.1. Порог — информационный, не gate (concurrency
   код малого объёма, 100% line coverage легко достижимо и почти ничего не
   говорит о race-покрытии).

## Порядок реализации

| Шаг | Содержание | Блокирует |
|-----|------------|-----------|
| T1 | `TestScheduler` + `TestMutex`/`TestChannel` (Layer 1, режим B) | — |
| T2 | Расширить `stress_channel.cpp` до полной матрицы Layer 2 (без cancel-сценариев — ждут StopToken) | T1 (общий хелпер для двух режимов) |
| T3 | `stress_mutex.cpp`, `stress_arc.cpp`, `stress_spawn.cpp` (новые) | T1 |
| T4 | `scripts/concurrency_sanitize_gate.sh` + wiring в `.github/workflows/ci.yml` | T2, T3 (иначе gate не на чём запускать) |
| T5 | Cancel-сценарии в матрице Layer 2 | TRACK_CONCURRENCY_V2 STEP=5 (`StopToken`) |
| T6 | Fuzz/chaos nightly job (Layer 4) | T1, T4 |
| T7 | `TestRuntime.new(seed:)` на уровне MLC | CONCURRENCY_V2.md Фазы 6-8 (`Task`/`TaskScope`/`Isolate`) |

T1-T4 не зависят от `TRACK_CONCURRENCY_V2` — можно начинать параллельно, тестируют
уже существующий MVP (`Channel`/`Mutex`/`Arc`/`spawn`). T5+ зависят от того трека.

## Ссылки

- [CONCURRENCY_V2.md](CONCURRENCY_V2.md) §34-37 (исходные требования)
- [agent/TRACK_CONCURRENCY_TEST_HARNESS.md](agent/TRACK_CONCURRENCY_TEST_HARNESS.md) (исполняемый трек)
- Существующее: `runtime/test/run_concurrency_smoke.sh`, `runtime/test/stress_channel.cpp`
- CI: `.github/workflows/ci.yml`
