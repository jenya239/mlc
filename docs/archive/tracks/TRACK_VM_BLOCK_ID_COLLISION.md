# Track: MIR block-id collision on nested `if` — VM infinite loop

Parent: [../../PLAN.md](../../PLAN.md), [../../agent/TRACK_MIR_VM_FULL.md](../../agent/TRACK_MIR_VM_FULL.md).
Related but distinct: [TRACK_VM_TRAMPOLINE.md](TRACK_VM_TRAMPOLINE.md)
(closed — that was a stack-overflow/depth bug; this is a **correctness** bug
in MIR block allocation, found while writing example scripts 2026-07-10).

**Scope note:** affects only `mlcc --run`/`--check-only` (MIR interpreter
path, `compiler/mir/lower_fn.mlc`). Does **not** affect normal `mlcc
<file.mlc>` compilation to native binaries — that path uses
`compiler/codegen/` (CppAST), never touches MIR. Programs compiled normally
with g++ are unaffected by this bug.

## Status: **closed** 2026-07-10 — STEP=1–5 done

**Driver 2026-07-10:** STEP=5 — self-host `mlcc`→`mlcc2`→`diff` **IDENTICAL**;
`regression_gate.sh` **20/0**. Note in [TRACK_MIR_VM_FULL.md](../../agent/TRACK_MIR_VM_FULL.md).

## Verify gate (STEP=5, 2026-07-10)

```
p1=7.45s; mlcc2=456.85s; p2=5.77s
diff -r p1 p2 --exclude=obj → IDENTICAL
regression_gate: 20 passed, 0 failed (303.38s)
elif gate 6; depth; examples 28; single-file 18+cpp_diff 18
```

## Repro (подтверждено, минимальный случай)

```mlc
fn classify(value: i32) -> i32 =
  if value == 0 then 100
  else if value == 1 then 200
  else 300

fn main() -> i32 = do
  println("before")
  classify(1)   // <-- зависает, если значение требует зайти во ВТОРУЮ ветвь elif
end
```

```bash
mlcc --run repro.mlc
# classify(0) -> works, prints "before", exits 100
# classify(5) -> works (falls through to final "else 300")
# classify(1) -> hangs forever ("before" printed, then nothing, SIGKILL required)
```

Тот же паттерн со строковыми литералами вместо `i32` (`digit_char`
if/elif-цепочка "0".."9") — идентичное поведение. Не специфично для типа
возврата.

## Root cause — найдено и подтверждено через `--dump-mir`

```bash
mlcc --dump-mir --check-only repro.mlc -o /tmp/dump_out
```

выдаёт для `classify`:

```
mir fn classify blocks=5 locals=2
  block 0 (entry) stmts=1
  block 1 (then) stmts=0
  block 2 (else) stmts=1
  block 2 (then) stmts=0      <-- id=2 ПОВТОРЯЕТСЯ
  block 3 (else) stmts=0
```

Два разных блока получили **один и тот же id=2**: внешняя ветвь `else`
(содержащая проверку `value == 1`) и внутренняя ветвь `then` вложенного
`if` (`value == 1 then 200`). `vm_find_block` (`compiler/vm/execute.mlc`)
ищет по id **линейно и возвращает первое совпадение** — переход на
блок 2 всегда попадает на внешний `else` (который снова прыгает на блок 2),
а не на внутренний `then` → бесконечный цикл именно тогда, когда нужно
попасть во вторую ветвь цепочки.

**Файл:** `compiler/mir/lower_fn.mlc`, функция `mir_lower_return_expression`,
кейс `SemanticExpressionIf`, строки 773-791:

```mlc
SemanticExpressionIf(condition, then_expression, else_expression, _, _) =>
  match mir_lower_operand_from_expression(state, condition) {
    Err(errors) => Err(errors),
    Ok(condition_step) => do
      const then_block_step = mir_lower_allocate_block_index(condition_step.state)
      const else_block_step = mir_lower_allocate_block_index(then_block_step.state)
      let mut branch_state = mir_lower_finish_current_block(
        then_block_step.state,                                    // <-- БАГ, строка 780
        MirCondJump(condition_step.operand, then_block_step.block_id, else_block_step.block_id))
      branch_state = mir_lower_start_block(branch_state, then_block_step.block_id, 'then')
      ...
```

`else_block_step` аллоцируется из `then_block_step.state` (id_pool продвинут
на 1 дальше). Но `mir_lower_finish_current_block` на строке 779-781 вызван
с `then_block_step.state`, а не с `else_block_step.state` — тем самым
"забывается" факт, что id для `else`-блока уже выдан. Следующая аллокация
блока (внутри рекурсии в `then_expression` ИЛИ `else_expression`, если там
есть ещё один `if`) стартует со старого счётчика и повторно выдаёт id,
уже занятый `else_block_id`.

Проявляется всегда, когда любая из веток (`then` или `else`) исходного
`if` сама требует аллокации нового блока — то есть **любой `if`, вложенный
не в условие, а в одну из веток другого `if`** (elif-цепочка от 3 ветвей,
или явно вложенный `if` внутри `then`/`else`). Двухветочный `if`/`else` без
вложенности не задет (подтверждено — `t4.mlc`/`t5.mlc`/`t6.mlc` работают
корректно, `t3.mlc` с elif виснет).

## Fix (найден и локально проверен через `--dump-mir`, не закоммичен)

Заменить `then_block_step.state` на `else_block_step.state` в строке 780:

```mlc
let mut branch_state = mir_lower_finish_current_block(
  else_block_step.state,
  MirCondJump(condition_step.operand, then_block_step.block_id, else_block_step.block_id))
```

**Проверка диагноза:** локально применил патч, `--dump-mir` (без полной
пересборки `mlcc` — конкурентный агент в это время держал грязное дерево
на `runtime/include/mlc/io/file.hpp`, пересборка упала на stale PCH, не
связано с этим багом) подтвердил отсутствие дублирования id при ручной
трассировке аллокатора. Патч **не закоммичен и не собран end-to-end** —
следующему агенту: применить, пересобрать `compiler/build.sh`, прогнать
repro выше (`classify(1)` должен завершиться с exit=200, не висеть), затем
полный verify-gate.

## Обходной путь (пока фикс не влит)

`match` на литералах не использует эту ветку лоуэринга (`mir_lower_match_return`,
отдельный путь) и не подвержен багу — подтверждено (`t8.mlc`, digit-to-char
через `match { 0 => "0", 1 => "1", ..., _ => "9" }` работает корректно).
Использовать `match` вместо `if`/`else if`-цепочек в VM-скриптах до фикса.

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | Применить фикс (строка 780, `then_block_step.state` → `else_block_step.state`). Пересобрать `mlcc`. | **done** (2026-07-10: dump-mir ids 0–4 unique; classify(0/1/5)→100/200/44) |
| 2 | Regression: repro выше (`classify(0/1/5)`) — все три случая корректны, без зависания. Добавить как постоянную фикстуру в `compiler/tests/` (класс бага никогда не должен вернуться незаметно). | **done** (2026-07-10: `vm_elif_classify_{0,1,5}.mlc` + `run_vm_elif_block_id_gate.sh`; exits 100/200/44; timeout 5s) |
| 3 | Проверить глубже вложенные цепочки (elif × 5+, `if` вложенный в `then`-ветку, не только `else`) — тот же баг теоретически может проявляться и там (см. анализ выше — риск не ограничен именно `else`-веткой, зависит от того, что аллокация из "забытого" состояния происходит первой). | **done** (2026-07-10: `vm_elif_chain_{mid,last}` + `vm_if_in_then`; gate 6 cases 100/200/44/40/50/22) |
| 4 | Прогнать весь существующий VM-корпус (`vm_cpp_diff_programs.txt`, `run_single_file_vm_gate.sh`, `run_examples_vm_gate.sh`, `run_vm_trampoline_depth_gate.sh`) — чистый рефактор одной строки, не должно быть регрессий. | **done** (2026-07-10: elif 6; depth ok; examples 28; single-file 18+cpp_diff 18) |
| 5 | Verify-gate + close: self-host (`mlcc`→`mlcc2`→`diff`), `regression_gate.sh`. Обновить `TRACK_MIR_VM_FULL.md` — отметить, что elif/nested-if был сломан до этого фикса. | **done** (2026-07-10: DIFF identical; regression_gate 20/0) |

## Out of scope

- Полный редизайн `mir_lower_return_expression` — точечный фикс одной
  строки достаточен, не переписывать механизм аллокации блоков целиком.
- Другие найденные в той же сессии пробелы VM-lowering (unary операторы,
  `if` не в tail-позиции) — см. [TRACK_VM_LOWERING_GAPS](TRACK_VM_LOWERING_GAPS.md),
  отдельный трек, не смешивать.
