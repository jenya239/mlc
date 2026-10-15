# Синтаксис лямбды на этом срезе

Frontend уже строит узел `ExprLambda([string], Shared<Expr>, Span)` в `compiler/frontend/ast.mlc`. Имена параметров сохраняются. Аннотации типов читаются и отбрасываются: в узел попадают только имена.

Принятая форма среза — скобки, типы параметров и `=>`, как в `misc/examples/vm_lambda.mlc`:

```mlc
const add = (x: i32, y: i32) => x + y
```

Однопараметрический вызов сразу:

```mlc
let f = (x: i32) => x + 1
f(5)
```

Это `parse_primary_parenthesized` в `compiler/frontend/parser/exprs.mlc`: `looks_like_typed_lambda_params` требует идентификатор и `:`, `parse_typed_lambda_params` снимает `IDENT : Type` через запятую и останавливается после `)`, тело — `parse_expr` после `=>` (`is_fat_arrow`).

Тот же узел дают ещё две формы, их этот срез не берёт за образец:

- `(x, y) => x` — `looks_like_lambda_params` / `parse_lambda_params`, без типов
- `x => x + 1` — `parse_primary_identifier`, один идентификатор и `=>`
- `() => body` — пустой список параметров

`|x: i32| -> i32 do x + 1 end` парсер выражения в `ExprLambda` не собирает. `|` в выражениях — это `parse_pipe` (`pipe_desugar`), не заголовок лямбды. `->` — не `FatArrow`.

Значение лямбды в переменной — `ValueHeapRef` (`script_vm/value.mlc`): ссылка на объект кучи, как у записи и строки. Тип объекта — `heap_type_closure` = 4 (`script_vm/heap_types.mlc`). Это не `ValueInt32` и не `ValueBool`.
