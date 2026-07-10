# Track: concurrency (`spawn`/`Mutex`/`Channel`/`Task`) отсутствует в Ruby-пайплайне; `Tcp` stdlib отсутствует в self-hosted

Parent: [../CONCURRENCY_V2.md](../CONCURRENCY_V2.md),
[../archive/tracks/TRACK_STDLIB_NET_SERVER.md](../archive/tracks/TRACK_STDLIB_NET_SERVER.md).
Найдено 2026-07-10 при попытке собрать многопоточный TCP-сервер: два
языковых feature-набора реализованы в двух непересекающихся компиляторах.

## Status: **open** — STEP=1 **done**; STEP=2 next (`block_on`)

**Planner 2026-07-10:** выбран после closed SPAWN_DOUBLE_EXEC (выше
Postgres — `block_on` residual + pipeline split blocks language-level
TCP+spawn). Postgres track not opened this turn.

**Driver 2026-07-10:** STEP=1 — Decision **C** (document temporary split);
re-checked `rg spawn lib/mlc` → 0; `rg common/stdlib compiler` → 0;
`block_on`/`is_ready` mapped in `cpp_naming.mlc` only.

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

## Decision (STEP=1, 2026-07-10) — **C**

**Chosen: C — document temporary pipeline split** (this track STEP=3).

| Option | Verdict |
|--------|---------|
| **A** Port concurrency into Ruby `lib/mlc/` | **Reject.** Duplicates self-hosted CONCURRENCY_V2; Ruby is bootstrap/reference, not the destination. |
| **B** Port `common/stdlib/` into self-hosted `mlcc` | **Defer** to a future dedicated track. Correct long-term (self-host north star) but too large for STEP=3 here (loader + many modules). |
| **C** Document split: Ruby = IO/backend stdlib (`Tcp`, …); `mlcc` = language + concurrency | **Accept for now.** Matches reality; cheap; NET_SERVER already uses C++ `ThreadPool` without language `spawn`. |

### Rationale

1. Re-check 2026-07-10: `rg spawn lib/mlc` → **0 files**; `rg common/stdlib compiler` → **0 files**.
2. Full parity (A or B) is a multi-track project; this track’s job is an explicit Decision + cheap `block_on` fix + docs.
3. Long-term intent remains **B** (stdlib on `mlcc`); do not start B implementation in STEP=3 — only name it as follow-up in `MLC.md` when documenting C.

### What STEP=2–3 do under C

- **STEP=2:** register `block_on`/`is_ready` in self-hosted checker (codegen map already exists) so `mlcc` spawn programs can join without relying on Task dtor.
- **STEP=3:** matrix in `MLC.md` (and short README pointer): which features are Ruby-only vs `mlcc`-only; note follow-up = stdlib-on-mlcc (option B).

### Non-goals (this track)

- Implementing A or B.
- Opening Postgres/crypto (unblocked after STEP=1 design; queue still prefers finishing STEP=2–3 first).

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | Design: choose A/B/C above; write Decision; list non-goals. | **done** (2026-07-10: **C**; B deferred follow-up) |
| 2 | `block_on`/`is_ready` as identifiers in self-hosted checker (registry) + existing codegen map + regression (spawn e2e can `block_on`). | pending |
| 3 | Document split in `MLC.md`/`README.md` (Decision C); name B as follow-up. | pending |

<!-- sub-steps STEP=1: 1) re-check `rg spawn lib/mlc` + `rg common/stdlib compiler`; 2) pick A/B/C with 3–5 line rationale; 3) Decision table in this TRACK; 4) PLAN §8b sync -->
<!-- sub-steps STEP=2: 1) registry/names for block_on/is_ready; 2) smoke `spawn`+`block_on` checks; 3) optional tighten spawn_side_effect_gate to block_on -->
<!-- sub-steps STEP=3: 1) implement Decision path or MLC.md matrix; 2) verify-gate; 3) close -->

## Out of scope

- Полное слияние двух пайплайнов в один — не в этом треке, слишком
  дорого; см. общий roadmap self-hosting в `PLAN.md`.
- Дублирование statement/tail-expression в `spawn`-лямбде — **closed**,
  см. [TRACK_LANG_SPAWN_DOUBLE_EXEC](../archive/tracks/TRACK_LANG_SPAWN_DOUBLE_EXEC.md).
- Opening Postgres/crypto tracks — after this design (or deferred if C).
