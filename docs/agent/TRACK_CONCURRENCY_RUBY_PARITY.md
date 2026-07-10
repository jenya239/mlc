# Track: concurrency (`spawn`/`Mutex`/`Channel`/`Task`) отсутствует в Ruby-пайплайне; `Tcp` stdlib отсутствует в self-hosted

Parent: [../CONCURRENCY_V2.md](../CONCURRENCY_V2.md),
[../archive/tracks/TRACK_STDLIB_NET_SERVER.md](../archive/tracks/TRACK_STDLIB_NET_SERVER.md).
Найдено 2026-07-10 при попытке собрать многопоточный TCP-сервер: два
языковых feature-набора реализованы в двух непересекающихся компиляторах.

## Status: **open** — STEP=1 next (design)

**Planner 2026-07-10:** выбран после closed SPAWN_DOUBLE_EXEC (выше
Postgres — `block_on` residual + pipeline split blocks language-level
TCP+spawn). Postgres track not opened this turn.

## Проблема

1. **`spawn`/`Mutex`/`Channel`/`Task` — только self-hosted.** `grep -r
   spawn lib/mlc/` — **0** совпадений. Весь `CONCURRENCY_V2`/`TASKSCOPE`/
   `ISOLATE` (закрыты 2026-07-09) реализован исключительно в
   `compiler/checker/` + `compiler/codegen/` (self-hosted). Попытка
   собрать источник со `spawn` через Ruby `MLC.build_project`:

   ```
   Pass lower_declarations failed: Unknown identifier 'spawn'
   ```

2. **`Tcp` stdlib — только Ruby.** `lib/mlc/common/stdlib/net/tcp.mlc`
   сканируется Ruby stdlib-registry (`@module`/`@namespace` манифест).
   `compiler/` (self-hosted) **не читает** `lib/mlc/common/stdlib/` вообще
   (`grep -r "common/stdlib" compiler/` — 0 совпадений). Попытка собрать
   `import Tcp::{bind}` через self-hosted `mlcc`:

   ```
   error: file not found: /tmp/.mlc
   ```
   (резолвер модулей пытается трактовать `Tcp` как относительный путь —
   специального стдлиб-механизма для него в self-hosted нет).

3. **Следствие:** сегодня нет **одного** компилятора, который понимает и
   TCP, и `spawn`/`Mutex`/`Channel` одновременно — TCP-сервер с потоками
   на языковом `spawn` собрать нельзя ни через Ruby (не знает `spawn`),
   ни через `mlcc` (не знает `Tcp`).

4. **`block_on`/`is_ready` не зарегистрированы в self-hosted checker** —
   `error[E001]: undefined: block_on`, хотя codegen-маппинг на
   `mlc::block_on` уже есть (`compiler/codegen/cpp_naming.mlc:79`). Значит
   `Task` через `spawn` создать можно, а забрать результат штатным
   способом, который проходит checker, — нельзя. Critic residual on
   SPAWN e2e: gate waits via Task/future destructor, not `block_on`.

## Практический обход (уже сделан в NET_SERVER)

`TRACK_STDLIB_NET_SERVER` STEP=7 закрыт через чистый C++
`serve_http_with_thread_pool` (не языковой `spawn`) — TCP+пул достижим
без Ruby/`spawn` паритета. Этот трек остаётся для design-решения
разрыва пайплайнов.

## Decision (STEP=1 — fill here)

_Pending Driver STEP=1._ Options:

| Option | Pros | Cons |
|--------|------|------|
| **A** Port `spawn`/`Mutex`/`Channel`/`Task` into Ruby `lib/mlc/` | One pipeline for backend+concurrency via Ruby | Large port; duplicates self-hosted work |
| **B** Port `common/stdlib/` scan into self-hosted `mlcc` | One pipeline via mlcc; Tcp+spawn together | Stdlib loader design; many modules |
| **C** Document temporary split (Ruby=IO stdlib, mlcc=language/concurrency) | Cheap; honest | No single-compiler TCP+spawn until later |

Prefer recording **one** chosen option + what STEP=2–3 do under it.
`block_on`/`is_ready` (STEP=2) is valuable under any option.

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | Design: choose A/B/C above; write Decision; list non-goals. | **pending** |
| 2 | `block_on`/`is_ready` as identifiers in self-hosted checker (registry) + existing codegen map + regression (spawn e2e can `block_on`). | pending |
| 3 | Per Decision: port one feature set **or** document split in `MLC.md`/`README.md`. | pending |

<!-- sub-steps STEP=1: 1) re-check `rg spawn lib/mlc` + `rg common/stdlib compiler`; 2) pick A/B/C with 3–5 line rationale; 3) Decision table in this TRACK; 4) PLAN §8b sync -->
<!-- sub-steps STEP=2: 1) registry/names for block_on/is_ready; 2) smoke `spawn`+`block_on` checks; 3) optional tighten spawn_side_effect_gate to block_on -->
<!-- sub-steps STEP=3: 1) implement Decision path or MLC.md matrix; 2) verify-gate; 3) close -->

## Out of scope

- Полное слияние двух пайплайнов в один — не в этом треке, слишком
  дорого; см. общий roadmap self-hosting в `PLAN.md`.
- Дублирование statement/tail-expression в `spawn`-лямбде — **closed**,
  см. [TRACK_LANG_SPAWN_DOUBLE_EXEC](../archive/tracks/TRACK_LANG_SPAWN_DOUBLE_EXEC.md).
- Opening Postgres/crypto tracks — after this design (or deferred if C).
