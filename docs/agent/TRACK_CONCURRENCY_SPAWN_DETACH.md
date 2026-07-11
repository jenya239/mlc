# Track: `spawn` fire-and-forget блокирует (Task-деструктор ждёт `std::future`)

Parent: [../CONCURRENCY_V2.md](../CONCURRENCY_V2.md) §5-6,
[../archive/tracks/TRACK_CONCURRENCY_TASKSCOPE.md](../archive/tracks/TRACK_CONCURRENCY_TASKSCOPE.md),
[../archive/tracks/TRACK_STDLIB_HTTP_MLC.md](../archive/tracks/TRACK_STDLIB_HTTP_MLC.md).
Trigger: пользователь 2026-07-11 — «STDLIB_HTTP_MLC уже готов? можем сделать
мини многопоточный http сервер?». Проверка на бинаре обнаружила блокирующий
баг в `spawn`, не задокументированный ни в одном треке.

## Status: open — STEP=1 next

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

## Decision (STEP=1)

| Вопрос | Вариант | Рекомендация |
|--------|---------|--------------|
| Модель | (a) `std::thread::detach()`-семантика — результат теряется, не блокирует; (b) `TaskGroup`/`TaskScope` — явная коллекция, reap в конце scope (уже есть `task_scope.hpp` в C++, не в MLC surface); (c) checker требует явный `.detach()`/discard на `spawn`, иначе ошибка компиляции | (b) — соответствует `CONCURRENCY_V2.md` §5-6 (structured concurrency, "не `Thread.spawn` без handle"); (a) даёт data race на panics/ошибках без обработки |
| Where | MLC-уровень: `scope |s| { s.spawn { ... } }` (уже спроектировано в `CONCURRENCY_V2.md` §6, `TaskScope` C++ уже есть — не экспонирован в checker/codegen) | реализовать MLC-синтаксис поверх существующего `task_scope.hpp`, не с нуля |
| Совместимость | Текущий `spawn do ... end -> Task<T>` с явным `block_on`/`await` — оставить (валиден, просто нужно знать про блокировку без reap) | не менять существующую семантику, добавить `scope`-форму как основной способ "spawn много, жди все" |
| Минимальный фикс без нового синтаксиса | Задокументировать существующее поведение как есть (не потерять данные — деструктор блокирует, это memory-safe, просто не параллельно) + добавить чек в checker/lint: `spawn` как expression-statement (результат не привязан к `let`) — warning/error | обязательно как shim до реализации `scope`, чтобы не плодить тихо-серийный код |

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

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | Decision: синтаксис `scope`, checker-диагностика на bare `spawn`. | pending |
| 2 | Checker: диагностика bare `spawn do...end` statement. | pending |
| 3 | `scope |s| { s.spawn {...} }` MLC-синтаксис поверх `task_scope.hpp`. | pending |
| 4 | Демо: параллельный accept-loop HTTP-сервер + замер конкурентности. | pending |
| 5 | Self-host diff + `regression_gate.sh`; close. | pending |
