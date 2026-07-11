# Track: `spawn` fire-and-forget блокирует (Task-деструктор ждёт `std::future`)

Parent: [../CONCURRENCY_V2.md](../CONCURRENCY_V2.md) §5-6,
[../archive/tracks/TRACK_CONCURRENCY_TASKSCOPE.md](../archive/tracks/TRACK_CONCURRENCY_TASKSCOPE.md),
[../archive/tracks/TRACK_STDLIB_HTTP_MLC.md](../archive/tracks/TRACK_STDLIB_HTTP_MLC.md),
[../PLAN.md](../PLAN.md) §11b.
Trigger: пользователь 2026-07-11 — «STDLIB_HTTP_MLC уже готов? можем сделать
мини многопоточный http сервер?». Проверка на бинаре обнаружила блокирующий
баг в `spawn`, не задокументированный ни в одном треке.

## Status: **active** — STEP=2 next (bare-spawn checker)

**Driver 2026-07-11:** STEP=1 — Decision **locked**. Verified
`runtime/include/mlc/concurrency/task_scope.hpp` (`TaskScope::spawn` /
`join` / `~TaskScope` waits). CONCURRENCY_V2 §6 surface matches. Bare
`spawn` statement → **error** (STEP=2); MLC `scope` → STEP=3 over existing
C++ TaskScope (syntax deferred from closed TASKSCOPE track).

**Planner 2026-07-11:** activated after WEBSOCKET_TO_MLC Critic OK (`2a53fa0c`).
CONTINUITY hard limit: SPAWN_DETACH before remaining «без C++» queue (§20b/d/e).

## Проблема (эмпирически проверено 2026-07-11)

```mlc
extern fn sleep_ms_probe(milliseconds: i32) -> i32 = "..." from "..."

fn slow(id: i32) -> i32 = do sleep_ms_probe(500) id end

fn main() -> i32 = do
  spawn do slow(1) end   // Task отброшен, не сохранён
  spawn do slow(2) end   // Task отброшен, не сохранён
  println("after spawns")
  0
end
```

`time ./probe` → **1.009s** (не ~0.5s). Каждый `spawn do ... end` как
statement блокирует **на месте**, ждёт завершения, прежде чем перейти к
следующей строке — т.е. **полностью серийно**, несмотря на `spawn`.

**Причина** (`runtime/include/mlc/concurrency/spawn.hpp` +
`core/task.hpp`): `spawn_task` = `std::async(std::launch::async, ...)` →
`std::future<T>`, обёрнутый в `mlc::Task<T>` — ленивый C++20-корутин
(`initial_suspend` = `suspend_always`). Параметр корутины (`future`)
копируется/перемещается в frame **в момент вызова**, до первой
suspend-точки. Если `Task` уничтожается без `resume()`/`block_on()`/
`co_await` (как временный объект в конце statement), деструктор
`~Task() { handle_.destroy(); }` разрушает frame — включая `future`.
`std::future`, полученный из `std::async(std::launch::async, ...)`, **по
стандарту C++ блокируется в своём деструкторе**, если результат не
забран через `.get()`. Итог: "fire-and-forget" `spawn` физически
невозможен сегодня — тихо превращается в блокирующий вызов.

## Impact

Блокирует ровно тот паттерн, который нужен для многопоточного сервера:

```mlc
while true do
  let stream = accept(listener)
  spawn do handle_one(stream) end   // ← блокирует до завершения handle_one
end
```

Это **не параллельный accept-loop**, а последовательный обработчик,
маскирующийся под конкурентный (`spawn` есть в тексте, конкурентности нет
в рантайме). `STDLIB_HTTP_MLC`/`PIPELINE_MERGE_TCP_SPAWN` демо не поймали
это, потому что оба held `Task` в переменной и делали `block_on` сразу же
после двух параллельных `accept()` (2 соединения, оба spawn до первого
`block_on` — валидный частный случай, не accept-loop).

**Итог: реальный многопоточный HTTP-сервер (accept-loop, N параллельных
соединений) сегодня писать на MLC нельзя** — не из-за отсутствия
Tcp/HTTP/spawn по отдельности (все три готовы и стыкуются), а из-за этого
рантайм-бага в `spawn`.

## Decision (STEP=1, 2026-07-11) — **locked**

Verified on disk: `runtime/include/mlc/concurrency/task_scope.hpp` —
`TaskScope::spawn` / `spawn_with_token` / `join` / destructor waits for
children + cancel. CONCURRENCY_V2 §6 sketches `scope |s| { s.spawn {…} }`;
closed TASKSCOPE deferred MLC syntax (C++-only today).

| Вопрос | Locked |
|--------|--------|
| Модель | **TaskScope** (structured concurrency). Not detach-only. |
| Where | MLC `scope \|s\| { s.spawn { … } }` codegen → existing `task_scope.hpp` (STEP=3) |
| Совместимость | Keep `spawn do … end -> Task<T>` + explicit `block_on`/`await` |
| Shim (STEP=2) | Bare `spawn` as expression-statement (result not bound by `let`) → **error** `E089` |
| Diagnostic text (STEP=2) | `bare spawn discards Task and blocks in destructor; bind with let or use scope` |
| Child errors / cancel (STEP=3+) | Scope join waits; cancel via `StopToken` already on C++ TaskScope; MLC error aggregation deferred if not in V2 MVP |

### Rejected

| Вариант | Почему |
|---------|--------|
| `std::thread::detach` as primary fix | no reap/error aggregation; against CONCURRENCY_V2 §5-6 |
| Change `~Task` to not wait on `std::future` | drops running work / UB risk; still no structured join |
| Docs-only / warning-only | silent serial accept-loop stays a trap |
| New runtime from scratch | `task_scope.hpp` already implements the join invariant |

## Scope

1. Decision (таблица) — синтаксис `scope`, семантика reap/error propagation.
2. Checker: диагностика на `spawn do ... end` как statement без привязки к
   `let`/массиву `Task` — не даёт молча собрать псевдо-конкурентный код.
3. MLC `scope |s| { s.spawn { ... } }` поверх `task_scope.hpp` (STEP детали
   после Decision).
4. Демо: реальный accept-loop HTTP-сервер (`while true` + `scope`/N spawn),
   замер — параллельная обработка N одновременных соединений (`curl` × N
   параллельно, суммарное время ≈ время одного запроса, не N×).
5. Self-host + `regression_gate.sh`.

## Out of scope

- Полный `CONCURRENCY_V2.md` (`Send`/`Sync` checker, cancellation) — уже
  частично закрыт предыдущими треками, не переоткрывать.
- Async/non-blocking I/O — остаётся thread-per-connection, только реально
  параллельный, не серийный под видом параллельного.
- «без C++» stdlib ports (§20b/d/e) — после этого трека или параллельно
  только если SPAWN не блокирует.

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | Decision: синтаксис `scope`, checker-диагностика на bare `spawn`. | **done** (2026-07-11: locked TaskScope+E089; task_scope.hpp verified) |
| 2 | Checker: диагностика bare `spawn do...end` statement. | pending |
| 3 | `scope |s| { s.spawn {...} }` MLC-синтаксис поверх `task_scope.hpp`. | pending |
| 4 | Демо: параллельный accept-loop HTTP-сервер + замер конкурентности. | pending |
| 5 | Self-host diff + `regression_gate.sh`; close. | pending |

<!-- STEP=1 sub-steps: 1) lock Decision table (scope vs detach vs lint-only); 2) cite task_scope.hpp + CONCURRENCY_V2 §6 surface; 3) list STEP=2 diagnostic code/message; PLAN→STEP=2 -->
<!-- STEP=2 sub-steps: 1) find spawn stmt in checker; 2) error if result unused; 3) unit test + mlcc --check-only repro -->
<!-- STEP=3 sub-steps: 1) parse/check scope syntax; 2) codegen → task_scope.hpp; 3) smoke parallel sleep < serial -->
<!-- STEP=4 sub-steps: 1) HttpServer accept-loop demo; 2) N parallel curl gate; 3) wall-time assert -->
<!-- STEP=5 sub-steps: 1) regression_gate; 2) self-host diff; 3) close → Critic -->
