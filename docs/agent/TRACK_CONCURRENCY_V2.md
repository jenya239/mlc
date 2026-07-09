# Track: Concurrency v2 (Send/Sync, structured concurrency, cancellation)

Parent: [../PLAN.md](../PLAN.md) Фаза 8; спецификация:
[../CONCURRENCY_V2.md](../CONCURRENCY_V2.md) (полные требования, 44 пункта +
дорожная карта Фаза 1-11 + критерий приёмки — читать перед началом работы).
Предыдущий MVP (closed): [../archive/tracks/TRACK_CONCURRENCY.md](../archive/tracks/TRACK_CONCURRENCY.md).

## Status: **open** — не начат

**Перед STEP=1:** Planner/Driver должен решить именование трейта из ⚠️ в
`CONCURRENCY_V2.md` §"Открытый вопрос" (рекомендация — `Sync`, не `Shared`,
из-за коллизии с существующим типом `Shared<T>`). Без решения — STEP=1
не начинать.

## Goal

Довести concurrency-модель MLC до критерия приёмки из `CONCURRENCY_V2.md`
("Итоговый критерий", 17 пунктов). Этот трек покрывает только Фазы 1-4 из
дорожной карты (runtime primitives → `spawn_thread` → `Send` trait → move
capture). Фазы 5-11 (`Sync`, `TaskScope`, cancellation, `Isolate`,
`ThreadPool`, `Supervisor`, async I/O) — отдельные треки, создавать по мере
готовности (Planner), не забегать вперёд.

## Verify gate

```bash
bundle exec rake test_compiler_mlc
compiler/build.sh
compiler/out/mlcc -o .tmp_selfhost/p1 compiler/main.mlc
compiler/build_bin.sh .tmp_selfhost/p1 .tmp_selfhost/mlcc2
.tmp_selfhost/mlcc2 -o .tmp_selfhost/p2 compiler/main.mlc
diff -rq .tmp_selfhost/p1 .tmp_selfhost/p2
runtime/test/run_concurrency_smoke.sh
MLC_TSAN=1 runtime/test/run_concurrency_smoke.sh
```

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | Обобщить `compiler/checker/send_safe.mlc` (`type_is_send_safe`) в переиспользуемый bound `Send[T]`, доступный не только для `Channel.send`, но и для будущих `spawn_thread`/`ThreadPool.submit` capture-проверок. Развести понятия Send/Sync — сейчас предикат конфлирует оба (`Arc<T>` даёт `false`, должен давать `true` для `Send`). | pending |
| 2 | Rendezvous channel: `Channel[T].bounded(0)` в `runtime/include/mlc/concurrency/channel.hpp` — сейчас `capacity == 0` кидает `invalid_argument`, нужен синхронный handoff вместо ошибки. | pending |
| 3 | `Sender[T]`/`Receiver[T]` split + `Sender.clone()` + явная close-семантика (последний `Sender` уничтожен → `Closed` после дочитывания buffer; `tx.close()` будит blocked receivers) — сейчас `Channel<T>` единый handle без разделения ролей. | pending |
| 4 | `spawn_thread(move x) { ... }` — простое move-state tracking (не полный borrow checker): после `move x` использование `x` в исходном scope — ошибка компиляции. Conservative capture checker: захват mutable значения без `move`/явного `Sync`-типа — ошибка (сегодня — тихий COW data race, см. `MEMORY_MODEL.md` §Известные ограничения п.2). | pending |
| 5 | `StopSource`/`StopToken` runtime primitive (backend: C++20 `stop_source`/`stop_token`, не экспортировать C++ API наружу напрямую). | pending |
| 6 | Self-host верификация + `MEMORY_MODEL.md` обновление (заменить "Thread safety (TRACK_CONCURRENCY closed)" на актуальную таблицу с `Send`/`Sync`); close track или передать эстафету следующему (`TaskScope`/cancellation) треку. | pending |

## Out of scope (этот трек)

- `TaskScope`, error propagation policy, `Isolate`, `ThreadPool`, `Supervisor`,
  async I/O — Фазы 5-11 `CONCURRENCY_V2.md`, отдельные треки.
- Полный Rust-style borrow checker — не строим, только 4 категории
  (owned / immutable / local mutable / explicitly shared), см. `CONCURRENCY_V2.md` §2.
- Lock-free channel — mutex+condvar остаётся, пока нет профиля, доказывающего
  необходимость иного.

## Per-turn template

```
| step | <1-6> |
| done | <one line> |
| verify | build_tests N/0; concurrency smoke |
| next | ROLE=Driver STEP=<n+1> |
```
