# Track: Concurrency test harness (deterministic scheduler + stress matrix + sanitizer CI)

Parent: [../PLAN.md](../PLAN.md) Фаза 8; спецификация:
[../CONCURRENCY_TEST_HARNESS.md](../CONCURRENCY_TEST_HARNESS.md) (архитектура,
4 слоя, полная stress-матрица, читать перед началом).
Смежный трек (не блокер для T1-T4): [TRACK_CONCURRENCY_V2.md](TRACK_CONCURRENCY_V2.md).

## Status: **open** — не начат

## Goal

Полное автоматическое покрытие существующих concurrency-примитивов
(`Channel`/`Mutex`/`Arc`/`spawn`, TRACK_CONCURRENCY MVP) стресс-тестами и
sanitizer-gate в CI. T1-T4 не зависят от `TRACK_CONCURRENCY_V2` — тестируют то,
что уже есть в коде сегодня. T5+ зависят от `StopToken`/`Task`/`Isolate` из
того трека.

## Verify gate

```bash
runtime/test/run_concurrency_smoke.sh
MLC_TSAN=1 runtime/test/run_concurrency_smoke.sh
# после T4:
scripts/concurrency_sanitize_gate.sh
```

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 (T1) | `runtime/include/mlc/concurrency/testing/scheduler.hpp` — seeded turn-baton планировщик; `testing/mutex.hpp`, `testing/channel.hpp` — тот же публичный API что production, yield в scheduler на lock/unlock/send/recv/wait. Общий тестовый хелпер, чтобы один сценарий гонялся в обоих режимах (реальные потоки vs `TestScheduler`). | pending |
| 2 (T2) | Расширить `runtime/test/stress_channel.cpp` до полной матрицы из `CONCURRENCY_TEST_HARNESS.md` Layer 2 (кроме cancel-сценариев): full/empty queue, close during send/recv, sender/receiver destruction, rapid open/close (1000×), 1M messages. | pending |
| 3 (T3) | Новые `runtime/test/stress_mutex.cpp` (high contention, exception-safety под lock), `stress_arc.cpp` (concurrent clone/drop), `stress_spawn.cpp` (много одновременных spawn, exception внутри spawn не роняет процесс). Добавить в `run_concurrency_smoke.sh`. | pending |
| 4 (T4) | `scripts/concurrency_sanitize_gate.sh` (asan/ubsan/tsan матрица по всем stress-тестам) + wiring в `.github/workflows/ci.yml` как обязательная job (не опциональный ручной `MLC_TSAN=1`). | pending |
| 5 (T5) | После `TRACK_CONCURRENCY_V2` STEP=5 (`StopToken`): добавить cancel-during-send/recv в матрицу Layer 2. | TRACK_CONCURRENCY_V2 STEP=5 |
| 6 (T6) | Nightly fuzz/chaos job (Layer 4): N случайных seed через `TestScheduler`, regression corpus при падении. | T1, T4 |
| 7 (T7) | `TestRuntime.new(seed:)` на уровне MLC (тонкая обёртка над `TestScheduler`). | CONCURRENCY_V2.md Фазы 6-8 (Task/TaskScope/Isolate) |

## Out of scope (этот трек)

- Exhaustive model checking (полный loom/Concuerror-style — дороже, не MVP).
- `mlcc test --sanitize=X` CLI-обёртка для пользовательских MLC-программ — после
  того как gate для runtime-тестов заработает (T4), отдельный шаг, не блокер.
- Clang Thread Safety Analysis annotations в generated C++ (`CONCURRENCY_V2.md`
  §37) — второй эшелон, отдельный трек позже.

## Per-turn template

```
| step | <1-7> |
| done | <one line> |
| verify | run_concurrency_smoke.sh ok; MLC_TSAN=1 ok |
| next | ROLE=Driver STEP=<n+1> |
```
