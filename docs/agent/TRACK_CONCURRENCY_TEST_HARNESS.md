# Track: Concurrency test harness (deterministic scheduler + stress matrix + sanitizer CI)

Parent: [../PLAN.md](../PLAN.md) §26 / Фаза 8; спецификация:
[../CONCURRENCY_TEST_HARNESS.md](../CONCURRENCY_TEST_HARNESS.md) (архитектура,
4 слоя, полная stress-матрица, читать перед началом).
Смежный трек (closed): [../archive/tracks/TRACK_CONCURRENCY_V2.md](../archive/tracks/TRACK_CONCURRENCY_V2.md);
[../archive/tracks/TRACK_CONCURRENCY_TASKSCOPE.md](../archive/tracks/TRACK_CONCURRENCY_TASKSCOPE.md);
[../archive/tracks/TRACK_CONCURRENCY_SUPERVISOR.md](../archive/tracks/TRACK_CONCURRENCY_SUPERVISOR.md)
(Critic OK 2026-07-12 — queue predecessor).

## Status: **active** (Planner 2026-07-12) — очередь §26; T1–T6 done; T7 next

**Driver 2026-07-09:** T1–T5 done (scheduler, stress matrix, sanitize CI, cancel).
**Planner 2026-07-12:** activated after SUPERVISOR Critic OK; T6/T7 remaining.

## Next step

**STEP=7 (T7)** — Decision: MLC `TestRuntime.new(seed:)` vs C++-only docs (JobQueue/Supervisor pattern).

## Goal

Полное автоматическое покрытие существующих concurrency-примитивов
(`Channel`/`Mutex`/`Arc`/`spawn`, TRACK_CONCURRENCY MVP) стресс-тестами и
sanitizer-gate в CI. T1-T4 не зависят от `TRACK_CONCURRENCY_V2` — тестируют то,
что уже есть в коде сегодня. T5+ зависят от `StopToken`/`Task`/`Isolate` из
того трека. **This activation:** finish Layer 4 (T6) + decide T7 surface.

## Verify gate

```bash
runtime/test/run_concurrency_smoke.sh
MLC_TSAN=1 runtime/test/run_concurrency_smoke.sh
# после T4:
scripts/concurrency_sanitize_gate.sh
# после T6:
scripts/concurrency_fuzz_gate.sh
```

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 (T1) | `runtime/include/mlc/concurrency/testing/scheduler.hpp` — seeded turn-baton планировщик; `testing/mutex.hpp`, `testing/channel.hpp` — тот же публичный API что production, yield в scheduler на lock/unlock/send/recv/wait. Общий тестовый хелпер, чтобы один сценарий гонялся в обоих режимах (реальные потоки vs `TestScheduler`). | **done** |
| 2 (T2) | Расширить `runtime/test/stress_channel.cpp` до полной матрицы из `CONCURRENCY_TEST_HARNESS.md` Layer 2 (кроме cancel-сценариев): full/empty queue, close during send/recv, sender/receiver destruction, rapid open/close (1000×), 1M messages. | **done** |
| 3 (T3) | Новые `runtime/test/stress_mutex.cpp` (high contention, exception-safety под lock), `stress_arc.cpp` (concurrent clone/drop), `stress_spawn.cpp` (много одновременных spawn, exception внутри spawn не роняет процесс). Добавить в `run_concurrency_smoke.sh`. | **done** |
| 4 (T4) | `scripts/concurrency_sanitize_gate.sh` (asan/ubsan/tsan матрица по всем stress-тестам) + wiring в `.github/workflows/ci.yml` как обязательная job (не опциональный ручной `MLC_TSAN=1`). | **done** |
| 5 (T5) | Cancel-during-send/recv в матрице Layer 2 (`stress_channel` + StopToken). | **done** |
| 6 (T6) | Nightly fuzz/chaos job (Layer 4): N случайных seed через `TestScheduler`, regression corpus при падении. Wire как отдельная (не блокирующая обычный PR) CI job — nightly cron, не на каждый push. | **done** (2026-07-12) — `fuzz_scheduler.cpp`, `concurrency_fuzz_gate.sh`, `fuzz_corpus/scheduler_seeds.txt`, `.github/workflows/concurrency-fuzz-nightly.yml` |
| 7 (T7) | `TestRuntime.new(seed:)` на уровне MLC (тонкая обёртка над `TestScheduler`) — решить MLC-reachable или C++-only тем же способом что `STDLIB_JOB_QUEUE`/`CONCURRENCY_SUPERVISOR` Step 4 (не предполагать MLC-reachable по умолчанию). | pending |

### STEP=6 (T6) sub-steps (Driver)

1. Add `runtime/test/fuzz_scheduler.cpp`: one fixed `TestScheduler` scenario (e.g. 2–3 virtual threads + `TestMutex` and/or `TestChannel`); accept seed via argv/`MLC_SCHEDULER_SEED`; on failure print seed and exit non-zero; deterministic for a given seed.
2. Add `scripts/concurrency_fuzz_gate.sh`: first re-run seeds from `runtime/test/fuzz_corpus/scheduler_seeds.txt` (create file with header comment if empty); then N random seeds (default ~64–128, overridable); on fail append seed to corpus and fail the gate.
3. Add `.github/workflows/concurrency-fuzz-nightly.yml` with `on.schedule` (cron) + `workflow_dispatch`; call the fuzz gate only — **do not** add to PR-blocking `ci.yml`.
4. Smoke: run gate locally once (small N) exit 0; note corpus path in TRACK Progress. Do **not** implement T7 / MLC `TestRuntime` here.

### STEP=7 (T7) note (after T6)

Decision-first (like Supervisor STEP=4 / JobQueue): MLC `TestRuntime.new(seed:)` vs C++-only harness docs. Prefer C++-only if closures/`extern` gap blocks a real MLC surface — document in TRACK Decision; no half MLC API.

## Progress

- **T1–T5** (2026-07-09): done (scheduler, stress, sanitize CI, cancel).
- **Planner** (2026-07-12): activated after SUPERVISOR Critic OK; STEP=6 (T6) next.
- **STEP=6 / T6** (2026-07-12): `fuzz_scheduler` + `scripts/concurrency_fuzz_gate.sh` + nightly workflow; corpus `runtime/test/fuzz_corpus/scheduler_seeds.txt`; gate ok (32 random, seed0=1).

## Out of scope (этот трек)

- Exhaustive model checking (полный loom/Concuerror-style — дороже, не MVP).
- `mlcc test --sanitize=X` CLI-обёртка для пользовательских MLC-программ — после
  того как gate для runtime-тестов заработает (T4), отдельный шаг, не блокер.
- Clang Thread Safety Analysis annotations в generated C++ (`CONCURRENCY_V2.md`
  §37) — второй эшелон, отдельный трек позже.
- Fault-injection cancel at random mid-schedule (nice-to-have inside T6 if cheap;
  not a blocker if seed exploration alone lands).

## Per-turn template

```
| step | <6-7> |
| done | <one line> |
| verify | concurrency_fuzz_gate / Decision doc |
| next | ROLE=Driver STEP=<n+1> |
```
