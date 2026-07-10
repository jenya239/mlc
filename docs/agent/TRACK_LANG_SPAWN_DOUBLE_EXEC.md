# Track: `spawn do <tail-call> end` выполняет тело дважды

Parent: [../CONCURRENCY_V2.md](../CONCURRENCY_V2.md),
[../archive/tracks/TRACK_CONCURRENCY.md](../archive/tracks/TRACK_CONCURRENCY.md).
Найдено 2026-07-10 при попытке собрать многопоточный TCP-сервер (два
`spawn`, разное время выполнения) — оба задания напечатали свой лог
дважды.

## Status: **open** — STEP=1–3 **done**; STEP=4 next (verify-gate)

**Planner 2026-07-10:** выбран следующим после closed NET_SERVER (выше
Postgres/crypto в очереди — повторный побочный эффект на `spawn`).

**Driver 2026-07-10:** STEP=1 — root cause in transform (not codegen emit):
`TransformPass.visit_spawn` double-includes tail.

**Driver 2026-07-10:** STEP=2 — `expr_spawn_body_statements` in `ast.mlc`;
`visit_spawn` + `infer_expr_spawn` use prefix-only stmts; rebuild mlcc.
Verify: `spawn do side() end` → `return side();` (no prior `side();`).

**Driver 2026-07-10:** STEP=3 — `compiler/tests/e2e/spawn_side_effect.mlc` +
`run_spawn_side_effect_gate.sh` (codegen bare-`side();` ban + runtime
`SIDE_ONCE` count=1); `run_tests` 1471/0.

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

## Гипотеза причины — **подтверждена (уточнена)**

Баг не в codegen IIFE-emit, а в **transform** `visit_spawn`: в
`SemanticExpressionBlock` попадают и все stmts, и отдельно tail из
`expr_spawn_body_result`. Обычный `ExprBlock` парсер уже режет хвост
(`block_body` / `block_result`); `ExprSpawn` — нет. Plain `do side() end`
не дублирует.

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | Найти точное место, где `spawn do ... end` даёт double emit; сравнить с обычным `do ... end`. | **done** (2026-07-10: `transform.mlc:1431-1435` `visit_spawn`; plain OK) |
| 2 | Fix: убрать дублирование statement/tail в `visit_spawn` (split как `block_body`/`block_result`). | **done** (2026-07-10: `expr_spawn_body_statements`; codegen single `return side();`) |
| 3 | Regression: репро (лог **один** раз); e2e gate с подсчётом побочного эффекта (checker-only `test_spawn` не ловит). | **done** (2026-07-10: `run_spawn_side_effect_gate.sh` PASS; run_tests 1471/0) |
| 4 | Verify-gate: self-host (`mlcc`→`mlcc2`→`diff -rq`), `regression_gate.sh`. | pending |

<!-- sub-steps STEP=2: 1) split stmts in visit_spawn like block_body/block_result; 2) rebuild mlcc; 3) spawn_locate → single `return side();` -->
<!-- sub-steps STEP=3: 1) e2e gate; 2) count side-effect; 3) checker corpus green -->
<!-- sub-steps STEP=4: 1) build.sh; 2) mlcc→mlcc2 diff; 3) regression_gate 20/0 -->

## Out of scope

- `block_on`/`is_ready` не зарегистрированы в checker как identifiers
  (`error: undefined: block_on` при попытке забрать результат `Task`) —
  отдельная находка, см. [TRACK_CONCURRENCY_RUBY_PARITY](TRACK_CONCURRENCY_RUBY_PARITY.md).
