# Track: VM MIR lowering — unary operators, non-tail `if`

Parent: [../PLAN.md](../PLAN.md), [TRACK_MIR_VM_FULL](TRACK_MIR_VM_FULL.md).
Found while writing example scripts for `mlcc --run` 2026-07-10. Distinct
From [TRACK_VM_BLOCK_ID_COLLISION](../archive/tracks/TRACK_VM_BLOCK_ID_COLLISION.md) (that one
is a correctness bug producing wrong CFG; these two are missing coverage —
compile-time error, not silent wrong behaviour, lower severity).

## Status: **open, высокий приоритет** (блокирует обычные языковые
конструкции в VM, но не корректность/hang) — после closed
`TRACK_VM_BLOCK_ID_COLLISION`; можно параллельно с `TRACK_CLI_STDIN`.

## Gap 1 — унарные операторы не lowering'уются

`compiler/mir/lower_fn.mlc`: ни в `mir_lower_operand_from_expression`
(match на `SemanticExpression*`, строки 560-643), ни в
`mir_lower_rvalue_from_expression` (647-761) нет кейса
`SemanticExpressionUn` — унарные `!`/`-`/`~`/`+` (см.
`compiler/frontend/parser/exprs.mlc:597-608`, `parse_unary`) вообще не
обрабатываются в этих двух функциях.

Repro:

```mlc
fn main() -> i32 = do
  const flag = true
  if !flag then 1 else 0 end
end
```

```
mlcc --run repro.mlc
error: mir lower: unsupported expression in rvalue context
```

(Ошибка возникает именно на `!flag`, не на `if` — подтверждено убиранием
`!` из выражения.)

### Fix approach

Добавить кейс `SemanticExpressionUn(operation, inner, _, _)` в обе функции:
вычислить `inner` как operand, эмитить `MirRvalueUnary(operation, operand)`
(нужен новый вариант `MirRvalue` в `compiler/mir/mir_types.mlc`, если такого
ещё нет — проверить перед реализацией) + соответствующий case в
`compiler/vm/mir_eval.mlc` (`vm_eval_rvalue`) для `!`/`-`/`~`. Небольшой,
изолированный, аналогичный по объёму уже существующим арифметическим
операциям (`%`, `mir_eval.mlc:46-47`).

## Gap 2 — `if` как statement в середине блока не lowering'уется

`compiler/mir/lower_fn.mlc:1549-1550` (`mir_lower_statement`, кейс
`SemanticStatementExpr` → `SemanticExpressionIf`):

```mlc
SemanticExpressionIf(_, _, _, _, _) =>
  Err(['mir lower: if expression statement requires return context']),
```

`if`-выражение поддержано **только** когда оно tail/return-position всей
функции (через `mir_lower_return_expression`). Как обычный statement в
середине `do`-блока (не последний, или используемое как rvalue для
`const`/аргумента вызова) — падает.

Repro:

```mlc
fn main() -> i32 = do
  let mut x = 0
  if true then x = 1 else x = 2 end   // <-- не последний statement
  x
end
```

```
error: mir lower: if expression statement requires return context
```

Также падает как rvalue: `const y = if cond then a else b end` внутри
`do`-блока, если это не сам tail этого блока — `mir_lower_rvalue_from_expression`
не имеет кейса `SemanticExpressionIf` (только `mir_lower_return_expression`
имеет).

### Fix approach

Два отдельных случая, чуть разной сложности:

1. **`if` как statement** (значение отбрасывается, побочный эффект в
   ветках) — можно лоуэрить как `MirCondJump` на два блока, каждый из
   которых после выполнения своего содержимого делает `MirJump` на общий
   "continue"-блок (стандартный if-without-value CFG), вместо текущей
   привязки к `MirReturn`. Отдельная функция `mir_lower_if_statement`
   (по аналогии с `mir_lower_while_statement`/`mir_lower_for_statement`,
   уже в этом файле).
2. **`if` как rvalue** (значение используется дальше) — сложнее: нужен
   способ "материализовать" результат обеих веток в общий local
   (аллоцировать local до branch, в каждой ветке сделать `MirAssign` в
   этот local вместо `MirReturn`, затем `MirJump` на continue-блок, читать
   local после). Похоже на классический SSA phi-node, но без полного SSA
   — просто shared mutable local. Больший объём работы, чем (1).

Приоритет: (1) закрывает больше реальных случаев (mutation внутри if как
statement — самый частый паттерн) дешевле; (2) можно отложить отдельным
STEP, если окажется существенно дороже.

**Важно:** делать этот fix **после** `TRACK_VM_BLOCK_ID_COLLISION` — обе
новые ветки лоуэринга (if-statement, if-as-rvalue) будут переиспользовать
`mir_lower_allocate_block_index`/`mir_lower_finish_current_block`, тот же
паттерн, где сегодняшний баг живёт; исправлять point fix дважды в двух
разных местах не нужно.

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | Gap 1: `MirRvalueUnary` (или аналог) в `mir_types.mlc` + lowering кейс в `lower_fn.mlc` (оба места) + eval в `mir_eval.mlc` для `!`/`-`/`~`. Regression: repro выше + существующий корпус. | pending |
| 2 | Gap 2.1: `mir_lower_if_statement` (if как statement, значение отбрасывается) — новая функция по аналогии с `mir_lower_while_statement`. Regression: repro выше + существующий корпус. | pending |
| 3 | Gap 2.2: `if` как rvalue (shared local + continue-block) — в `mir_lower_rvalue_from_expression`. Более дорогой шаг, может быть отдельным STEP при необходимости декомпозиции. | pending |
| 4 | Verify-gate + close: self-host (`mlcc`→`mlcc2`→`diff`), `regression_gate.sh`, полный VM-корпус. | pending |

## Out of scope

- Унарный `move` (`ExprUn("move", ...)`, `compiler/frontend/parser/exprs.mlc:599-602`)
  — уже отдельная семантика (ownership), не арифметический/логический
  унарный оператор, не в этом треке.
