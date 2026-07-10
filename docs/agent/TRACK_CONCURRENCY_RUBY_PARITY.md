# Track: concurrency (`spawn`/`Mutex`/`Channel`/`Task`) отсутствует в Ruby-пайплайне; `Tcp` stdlib отсутствует в self-hosted

Parent: [../CONCURRENCY_V2.md](../CONCURRENCY_V2.md),
[../archive/tracks/TRACK_STDLIB_NET_SERVER.md](../archive/tracks/TRACK_STDLIB_NET_SERVER.md).
Найдено 2026-07-10 при попытке собрать многопоточный TCP-сервер: два
языковых feature-набора реализованы в двух непересекающихся компиляторах.

## Status: **open** (архитектурная находка; после SPAWN_DOUBLE_EXEC)

**Planner 2026-07-10:** не стартуем — сначала критический
[TRACK_LANG_SPAWN_DOUBLE_EXEC](../archive/tracks/TRACK_LANG_SPAWN_DOUBLE_EXEC.md). NET_SERVER
STEP=7 уже обошёл разрыв через C++ `serve_http_with_thread_pool`.

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
   способом, который проходит checker, — нельзя.

## Практический обход (уже сделан в NET_SERVER)

`TRACK_STDLIB_NET_SERVER` STEP=7 закрыт через чистый C++
`serve_http_with_thread_pool` (не языковой `spawn`) — TCP+пул достижим
без Ruby/`spawn` паритета. Этот трек остаётся для design-решения
разрыва пайплайнов.

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | Design-решение: портировать `spawn`/`Mutex`/`Channel`/`Task` в `lib/mlc/` (Ruby), **или** портировать stdlib-скан (`common/stdlib/`) в self-hosted, **или** осознанно принять разделение (Ruby = backend/IO stdlib, self-hosted = язык/concurrency) как временную границу с явной документацией — записать решение в этот трек. | open |
| 2 | `block_on`/`is_ready` как identifiers в self-hosted checker (registry) + codegen (уже есть маппинг) + regression. | open |
| 3 | В зависимости от решения STEP=1 — либо портировать один набор фич в другой пайплайн, либо задокументировать границу в `MLC.md`/`README.md` («какие фичи доступны только через Ruby-сборку, какие только через `mlcc`»). | open |

## Out of scope

- Полное слияние двух пайплайнов в один — не в этом треке, слишком
  дорого; см. общий roadmap self-hosting в `PLAN.md`.
- Дублирование statement/tail-expression в `spawn`-лямбде — отдельный,
  критический баг, см. [TRACK_LANG_SPAWN_DOUBLE_EXEC](../archive/tracks/TRACK_LANG_SPAWN_DOUBLE_EXEC.md).
