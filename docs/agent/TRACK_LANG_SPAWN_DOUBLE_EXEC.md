# Track: `spawn do <tail-call> end` выполняет тело дважды

Parent: [../CONCURRENCY_V2.md](../CONCURRENCY_V2.md),
[../archive/tracks/TRACK_CONCURRENCY.md](../archive/tracks/TRACK_CONCURRENCY.md).
Найдено 2026-07-10 при попытке собрать многопоточный TCP-сервер (два
`spawn`, разное время выполнения) — оба задания напечатали свой лог
дважды.

## Status: **open** — STEP=1 next (критический correctness-баг)

**Planner 2026-07-10:** выбран следующим после closed NET_SERVER (выше
Postgres/crypto в очереди — повторный побочный эффект на `spawn`).

## Проблема

Сгенерированный C++ для

```mlc
fn slow_task() -> i32 = do
  sleep_ms(2000)
  println("A done")
  1
end

fn main() -> i32 = do
  let task_a = spawn do slow_task() end
  0
end
```

(`compiler/out/mlcc`, файл `spawn_test.cpp`):

```cpp
auto task_a = mlc::concurrency::spawn_task([]() mutable { return [&]() {
slow_task();
return slow_task();
}(); });
```

Тело блока `do slow_task() end` лоуэрится в лямбду, где единственный
statement (который одновременно tail-expression блока) эмитится **два
раза**: один раз как обычный statement, второй раз как `return <tail>`.
Реально запущенный бинарник печатает `"A done"` два раза и выполняет
`slow_task()` дважды — не просто лишний код, реальный повторный побочный
эффект и повторная работа на фоновом потоке.

Репро (воспроизведено `mlcc` → clang++ → запуск бинарника):

```mlc
extern fn sleep_ms(milliseconds: i32) -> unit = "mlc::graphics::sleep_ms" from "mlc/graphics/graphics.hpp"

fn slow_task() -> i32 = do
  sleep_ms(2000)
  println("A done (slow, 2000ms)")
  1
end

fn main() -> i32 = do
  let task_a = spawn do slow_task() end
  0
end
```

Вывод: `A done (slow, 2000ms)` печатается **два раза**.

## Гипотеза причины

Общий паттерн лоуэринга блока-как-выражения (`do ... end` → immediately
invoked lambda `[&]() { ...; return <tail>; }()`), судя по всему,
переиспользует ту же функцию генерации statements для тела блока, что и
для генерации tail-return, без де-дупликации в случае, когда блок состоит
из ровно одного statement, который сам является tail-expression. Похоже
специфично для пути генерации `spawn`-лямбды (обычные функции с
однострочным телом `do X end` — проверить, воспроизводится ли тот же баг
без `spawn`, это сузит область поиска до конкретного codegen-файла).

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | Найти точное место в codegen (`compiler/codegen/`), где генерируется лямбда для `spawn do ... end`; сравнить с генерацией обычного `do ... end` блока-выражения без `spawn` — воспроизводится ли дублирование там же. | **pending** |
| 2 | Fix: убрать дублирование statement/tail-expression в теле блока-как-lambda. | pending |
| 3 | Regression: репро выше (лог печатается **один** раз); проверить существующий concurrency-корпус (`compiler/tests/test_spawn.mlc` и т.п.) — эти тесты проверяют только checker-уровень (`check_error_count`), не ловят этот баг, так как не компилируют+запускают бинарник. Добавить хотя бы один e2e `--run`/`g++`-gate, который реально выполняет `spawn`-программу и проверяет побочный эффект по count, не только по exit-коду. | pending |
| 4 | Verify-gate: self-host (`mlcc`→`mlcc2`→`diff -rq`), `regression_gate.sh`. | pending |

<!-- sub-steps STEP=1: 1) `rg spawn` / `spawn_task` in `compiler/codegen/` + expr/block emit; 2) minimal `do f() end` as expr (no spawn) → inspect C++; 3) SESSION: file:line of double emit -->
<!-- sub-steps STEP=2: 1) patch emit so single-stmt block is only `return <expr>`; 2) recompile repro; 3) assert one side-effect -->
<!-- sub-steps STEP=3: 1) e2e gate script or compiler/tests e2e; 2) count println/side-effect; 3) keep checker corpus green -->
<!-- sub-steps STEP=4: 1) build.sh; 2) mlcc→mlcc2 diff; 3) regression_gate 20/0 -->

## Out of scope

- `block_on`/`is_ready` не зарегистрированы в checker как identifiers
  (`error: undefined: block_on` при попытке забрать результат `Task`) —
  отдельная находка, см. [TRACK_CONCURRENCY_RUBY_PARITY](TRACK_CONCURRENCY_RUBY_PARITY.md).
