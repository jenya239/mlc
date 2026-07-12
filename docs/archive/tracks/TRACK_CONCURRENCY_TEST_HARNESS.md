# Track: Concurrency test harness (deterministic scheduler + stress matrix + sanitizer CI)

Parent: [../../PLAN.md](../../PLAN.md) §26 / Фаза 8; спецификация:
[../../CONCURRENCY_TEST_HARNESS.md](../../CONCURRENCY_TEST_HARNESS.md) (архитектура,
4 слоя, полная stress-матрица).
Смежный трек (closed): [TRACK_CONCURRENCY_V2.md](TRACK_CONCURRENCY_V2.md);
[TRACK_CONCURRENCY_TASKSCOPE.md](TRACK_CONCURRENCY_TASKSCOPE.md);
[TRACK_CONCURRENCY_SUPERVISOR.md](TRACK_CONCURRENCY_SUPERVISOR.md)
(Critic OK 2026-07-12 — queue predecessor).

## Status: **closed** (2026-07-12) — T1–T7 **done**; awaiting Critic

**Driver 2026-07-09:** T1–T5 done (scheduler, stress matrix, sanitize CI, cancel).
**Planner 2026-07-12:** activated after SUPERVISOR Critic OK.
**Driver 2026-07-12:** T6 fuzz nightly (`24486b14`); T7 Decision C++-only (this commit).
No `compiler/**` for T6/T7 (regression_gate N/A).

## Next step

**Critic** — `critique-audit` (HARD LIMIT after close).

## Goal

Полное автоматическое покрытие существующих concurrency-примитивов
стресс-тестами, sanitizer-gate и nightly seed fuzz. T7 MLC surface deferred.

## Decision (STEP=7 / T7, 2026-07-12)

### Pipeline

- **C++ harness v1 (shipped):** `mlc::concurrency::testing::TestScheduler` +
  `TestMutex` / `TestChannel`; smoke `test_scheduler.cpp`; sanitize gate;
  seed fuzz `fuzz_scheduler.cpp` + `scripts/concurrency_fuzz_gate.sh` +
  nightly workflow. This **is** the deterministic runtime for concurrency
  tests today.
- **MLC `TestRuntime.new(seed:)`: deferred** — wrapping the C++ scheduler for
  MLC user tests still needs a reliable way to register spawn bodies
  (`std::function` / closures) from MLC into the opaque C++ harness. Same
  class of gap as [TRACK_STDLIB_JOB_QUEUE.md](TRACK_STDLIB_JOB_QUEUE.md) and
  [TRACK_CONCURRENCY_SUPERVISOR.md](TRACK_CONCURRENCY_SUPERVISOR.md)
  (C++-only; MLC deferred). Pattern = **C++-only**, not a half MLC API.
- `INSTRUCTIONS_REV` runtime-stays-cpp reinforces: do not invent an MLC module
  that cannot pass real scenario bodies.
- Follow-up (out of this track): MLC `TestRuntime` once closures/`extern type`
  story supports it; until then §34 sketch in `CONCURRENCY_V2.md` remains
  aspirational syntax.

### Non-goals (locked)

MLC `import … from 'TestRuntime'`, mlcc demos that construct `TestRuntime` in
`.mlc`, registry entries for TestRuntime.

## Verify gate

```bash
runtime/test/run_concurrency_smoke.sh
scripts/concurrency_sanitize_gate.sh
scripts/concurrency_fuzz_gate.sh
```

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 (T1) | `TestScheduler` + `TestMutex` / `TestChannel` | **done** |
| 2 (T2) | `stress_channel` Layer 2 matrix (sans cancel) | **done** |
| 3 (T3) | `stress_mutex` / `stress_arc` / `stress_spawn` | **done** |
| 4 (T4) | `concurrency_sanitize_gate.sh` + CI | **done** |
| 5 (T5) | Cancel-during-send/recv + StopToken | **done** |
| 6 (T6) | Nightly fuzz/chaos + corpus | **done** (2026-07-12) — `fuzz_scheduler.cpp`, `concurrency_fuzz_gate.sh`, `fuzz_corpus/`, `concurrency-fuzz-nightly.yml` |
| 7 (T7) | MLC `TestRuntime` vs C++-only Decision | **done** (2026-07-12) — **C++-only v1; MLC deferred** (see Decision) |

## Progress

- **T1–T5** (2026-07-09): done.
- **Planner** (2026-07-12): activated; T6 next.
- **STEP=6 / T6** (2026-07-12): fuzz + nightly; gate ok.
- **STEP=7 / T7** (2026-07-12): Decision C++-only; track closed; archived.

## Out of scope (этот трек)

- Exhaustive model checking (loom/Concuerror-style).
- `mlcc test --sanitize=X` CLI for user MLC programs.
- Clang Thread Safety Analysis annotations (`CONCURRENCY_V2.md` §37).
- MLC `TestRuntime` module (deferred — Decision).
