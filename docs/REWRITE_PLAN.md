# План переписывания mlcc на идиоматичном MLC

Май 2026.

Цель: заменить ~354 `while`-цикла с ручными индексами, ~250+ `let mut` биндингов и сотни конкатенаций строк на HOF, `|>`, string interpolation, деструктуризацию и `match`. Каждый шаг — отдельный коммит, проходящий `rake test_compiler_mlc` (192 теста).

---

## Принципы

1. **Листья раньше корней.** Модуль переписывается только после всех его зависимостей.
2. **Один модуль — один коммит.** Никогда не переписывать два модуля в одном изменении.
3. **Тесты после каждого коммита.** `rake test_compiler_mlc` должен быть зелёным.
4. **Не менять публичный API.** Имена экспортируемых функций и типов не меняются.
5. **Не укорачивать ради укорачивания.** Если `while` читается лучше — оставить.

---

## Метрики до переписывания

| Файл | LOC | `while` | `let mut` |
|------|-----|---------|-----------|
| `parser/exprs.mlc` | 1056 | 27 | 47 |
| `checker/transform.mlc` | 650 | 18 | 29 |
| `codegen/type_gen.mlc` | 585 | 31 | 42 |
| `checker/partial_application_desugar.mlc` | 506 | — | — |
| `checker/registry.mlc` | 451 | 17 | 18 |
| `checker/check.mlc` | 379 | — | — |
| `codegen/match_gen.mlc` | 371 | 8 | 9 |
| `checker/infer.mlc` | 388 | 7 | 15 |
| `lexer.mlc` | 418 | 14 | 22 |
| `checker/semantic_type_structure.mlc` | 285 | 6 | 12 |
| **Итого по проекту** | **~10 000** | **~354** | **~250** |

---

## Паттерны замены

### P1. `while` + `push` → `.map()`

```mlc
// ДО
let mut result: [string] = []; let mut i = 0
while i < items.length() do
  result.push(transform(items[i])); i = i + 1
end

// ПОСЛЕ
let result = items.map(item => transform(item))
```

### P2. `while` + conditional `push` → `.filter_map()` / `.filter().map()`

```mlc
// ДО
let mut result: [T] = []; let mut i = 0
while i < items.length() do
  if condition(items[i]) then result.push(items[i]) end; i = i + 1
end

// ПОСЛЕ
let result = items.filter(item => condition(item))
```

### P3. Накопление → `.fold()`

```mlc
// ДО
let mut result = initial; let mut i = 0
while i < items.length() do
  result = result.absorb(process(items[i])); i = i + 1
end

// ПОСЛЕ
let result = items.fold(initial, (accumulated, item) => accumulated.absorb(process(item)))
```

### P4. `.map().join()` вместо ручного join

```mlc
// ДО
let mut parts: [string] = []; let mut i = 0
while i < types.length() do
  parts.push(type_to_cpp(context, types[i])); i = i + 1
end
let joined = parts.join(', ')

// ПОСЛЕ
let joined = types.map(t => type_to_cpp(context, t)).join(', ')
```

### P5. Поиск → `.find()` / `.any()` / `.all()`

```mlc
// ДО
let mut i = 0; let mut found = false
while i < items.length() && !found do
  if items[i].name == target then found = true end; i = i + 1
end

// ПОСЛЕ
let found = items.any(item => item.name == target)
```

### P6. String interpolation вместо конкатенации

```mlc
// ДО
"(" + left + " " + op + " " + right + ")"

// ПОСЛЕ
`(${left} ${op} ${right})`
```

### P7. Деструктуризация параметров (E3)

```mlc
// ДО
fn process_arm(arm: Shared<SMatchArm>) -> string = do
  const pattern = arm.pattern
  const body = arm.body

// ПОСЛЕ
fn process_arm(SMatchArm { pattern, body, guard }: Shared<SMatchArm>) -> string =
```

### P8. Pipeline `|>` для многоступенчатых трансформаций

```mlc
// ДО
let filtered = items.filter(item => condition(item))
let mapped = filtered.map(item => transform(item))
let result = mapped.join(', ')

// ПОСЛЕ
let result = items
  |> xs => xs.filter(item => condition(item))
  |> xs => xs.map(item => transform(item))
  |> xs => xs.join(', ')
```

### P9. `match` вместо if/else-цепочки по строке

```mlc
// ДО
if name == 'i32' then 'int'
else if name == 'string' then 'mlc::String'
else if name == 'bool' then 'bool'
...

// ПОСЛЕ (если конечный набор — Map)
const PRIMITIVES: Map<string, string> = Map.from_pairs([
  ('i32', 'int'), ('string', 'mlc::String'), ('bool', 'bool'),
  ('unit', 'void'), ('i64', 'int64_t'), ('f64', 'double')
])
PRIMITIVES.get(name).unwrap_or_else(() => fallback(name))
```

### P10. `let CtorPat { field } = x else ...` вместо `match x { Pat => ..., _ => default }`

```mlc
// ДО
match items[i] {
  VarTuple(name, types, _) => result.push(process(name, types)),
  _ => ()
}

// ПОСЛЕ (в .flat_map)
items.flat_map(variant =>
  match variant {
    VarTuple(name, types, _) => [process(name, types)],
    _ => []
  })
```

---

## Последовательность переписывания

### Шаг 1. `codegen/expr.mlc` — строковые шаблоны

**Что менять:** ~120 функций-сборщиков C++ фрагментов. Только P6 (интерполяция).

```mlc
// ДО
export fn parenthesized_binary(left_code: string, operation: string, right_code: string) -> string =
  "(" + left_code + " " + operation + " " + right_code + ")"

export fn record_initializer(type_resolved: string, values_comma_separated: string) -> string =
  type_resolved + "\{" + values_comma_separated + "\}"

export fn try_unwrap_result_block(result_variable: string, inner_code: string, success_line: string) -> string =
  'auto ' + result_variable + ' = ' + inner_code + ';\n' +
  'if (std::get_if<1>(&' + result_variable + ')) return *std::get_if<1>(&' + result_variable + ');\n' +
  success_line

// ПОСЛЕ
export fn parenthesized_binary(left_code: string, operation: string, right_code: string) -> string =
  `(${left_code} ${operation} ${right_code})`

export fn record_initializer(type_resolved: string, values_comma_separated: string) -> string =
  `${type_resolved}{${values_comma_separated}}`

export fn try_unwrap_result_block(result_variable: string, inner_code: string, success_line: string) -> string =
  `auto ${result_variable} = ${inner_code};\nif (std::get_if<1>(&${result_variable})) return *std::get_if<1>(&${result_variable});\n${success_line}`
```

**Ожидание:** 345 → ~200 LOC. 0 `while`, 0 `let mut`.

---

### Шаг 2. `codegen/cpp_naming.mlc` — имена идентификаторов

**Что менять:** функции `cpp_safe`, `lower_first`, `template_prefix` — P6.

```mlc
// ДО
export fn template_prefix(name: string, params: [string]) -> string = do
  if params.length() == 0 then name
  else
    let mut result = name + '<'
    let mut i = 0
    while i < params.length() do
      if i > 0 then result = result + ', ' end
      result = result + params[i]; i = i + 1
    end
    result + '>'
  end
end

// ПОСЛЕ
export fn template_prefix(name: string, params: [string]) -> string =
  if params.length() == 0 then name
  else `${name}<${params.join(', ')}>`
  end
```

**Ожидание:** 102 → ~60 LOC.

---

### Шаг 3. `codegen/ctor_info.mlc` — P1, P4, P10

```mlc
// ДО
fn ctor_type_info_for(ctor_name: string, types: [Shared<TypeExpr>]) -> Shared<CtorTypeInfo> = do
  let shared_positions: [i32] = []; let shared_array_positions: [i32] = []; let mut index = 0
  while index < types.length() do
    if is_shared_type(types[index]) then shared_positions.push(index) end
    if is_shared_array_type(types[index]) then shared_array_positions.push(index) end
    index = index + 1
  end
  Shared.new(CtorTypeInfo { name: ctor_name, shared_pos: shared_positions, shared_arr_pos: shared_array_positions })
end

export fn build_ctor_type_infos_from_decls(decls: [Shared<Decl>]) -> [Shared<CtorTypeInfo>] = do
  let result: [Shared<CtorTypeInfo>] = []; let mut i = 0
  while i < decls.length() do
    match decls[i] {
      DeclType(_, _, variants, _) => do
        let infos = add_ctor_infos_from_variants(variants)
        let mut ii = 0; while ii < infos.length() do result.push(infos[ii]); ii = ii + 1 end
      end,
      _ => do end
    }
    i = i + 1
  end
  result
end

export fn lookup_ctor_type_info(infos: [Shared<CtorTypeInfo>], cti_name: string) -> Shared<CtorTypeInfo> = do
  let mut result = Shared.new(CtorTypeInfo { name: '', shared_pos: [], shared_arr_pos: [] })
  let mut i = 0
  while i < infos.length() do
    if infos[i].name == cti_name then result = infos[i] end; i = i + 1
  end
  result
end

// ПОСЛЕ
fn ctor_type_info_for(ctor_name: string, types: [Shared<TypeExpr>]) -> Shared<CtorTypeInfo> =
  Shared.new(CtorTypeInfo {
    name: ctor_name,
    shared_pos: types.indices().filter(index => is_shared_type(types[index])),
    shared_arr_pos: types.indices().filter(index => is_shared_array_type(types[index]))
  })

fn variants_to_ctor_infos(variants: [Shared<TypeVariant>]) -> [Shared<CtorTypeInfo>] =
  variants.flat_map(variant =>
    match variant {
      VarTuple(name, types, _) => [ctor_type_info_for(name, types)],
      VarRecord(name, field_defs, _) => [ctor_type_info_for(name, field_defs.map(fd => fd.typ))],
      VarUnit(_, _) => []
    })

export fn build_ctor_type_infos_from_decls(decls: [Shared<Decl>]) -> [Shared<CtorTypeInfo>] =
  decls.flat_map(decl =>
    match decl_inner(decl) {
      DeclType(_, _, variants, _) => variants_to_ctor_infos(variants),
      _ => []
    })

export fn lookup_ctor_type_info(infos: [Shared<CtorTypeInfo>], target_name: string) -> Shared<CtorTypeInfo> =
  infos.find(info => info.name == target_name)
       .unwrap_or(Shared.new(CtorTypeInfo { name: '', shared_pos: [], shared_arr_pos: [] }))
```

**Ожидание:** 66 → ~30 LOC.

---

### Шаг 4. `codegen/type_gen.mlc` — P1, P4, P6, P9

Самый объёмный codegen-модуль: 31 `while`, 42 `let mut`.

```mlc
// ДО — TTuple arm
TTuple(ts) => do
  let mut tparts: [string] = []; let mut ti = 0
  while ti < ts.length() do
    tparts.push(sem_type_to_cpp(context, ts[ti])); ti = ti + 1
  end
  'std::tuple<' + tparts.join(', ') + ">"
end,

// ДО — TFn arm
TFn(param_types, return_type) => do
  let mut parts: [string] = []; let mut i = 0
  while i < param_types.length() do
    parts.push(sem_type_to_cpp(context, param_types[i])); i = i + 1
  end
  expression_output.cpp_std_function_type(sem_type_to_cpp(context, return_type), parts.join(', '))
end,

// ПОСЛЕ
TTuple(types) =>
  `std::tuple<${types.map(t => sem_type_to_cpp(context, t)).join(', ')}>`,

TFn(param_types, return_type) =>
  expression_output.cpp_std_function_type(
    sem_type_to_cpp(context, return_type),
    param_types.map(t => sem_type_to_cpp(context, t)).join(', ')),
```

```mlc
// ДО — type_name_to_cpp: 10-ветковая if/else цепочка
export fn type_name_to_cpp(context: CodegenContext, type_name: string) -> string =
  if type_name == 'i32' then 'int'
  else if type_name == 'string' then 'mlc::String'
  else if type_name == 'bool' then 'bool'
  ...

// ПОСЛЕ
const PRIMITIVE_TYPE_MAP: Map<string, string> = Map.from_pairs([
  ('i32', 'int'), ('string', 'mlc::String'), ('bool', 'bool'), ('unit', 'void'),
  ('i64', 'int64_t'), ('f64', 'double'), ('u8', 'uint8_t'), ('usize', 'size_t'), ('char', 'char32_t')
])

export fn type_name_to_cpp(context: CodegenContext, type_name: string) -> string =
  if type_name == 'Self' || type_name == 'self' then
    if context.self_type.length() > 0 then type_name_to_cpp(context, context.self_type) else 'void' end
  else
    PRIMITIVE_TYPE_MAP.get(type_name).unwrap_or_else(() => context_resolve(context, type_name))
  end
```

**Ожидание:** 585 → ~300 LOC.

---

### Шаг 5. `codegen/match_gen.mlc` — P1, P4, P7

```mlc
// ДО
fn gen_or_pattern_arms(arms: [Shared<SMatchArm>]) -> [Shared<SMatchArm>] = do
  let result: [Shared<SMatchArm>] = []; let mut i = 0
  while i < arms.length() do
    match arms[i] {
      SMatchArmOr(alts, body, guard) => do
        let mut j = 0
        while j < alts.length() do
          result.push(Shared.new(SMatchArm(alts[j], body, guard))); j = j + 1
        end
      end,
      _ => result.push(arms[i])
    }
    i = i + 1
  end
  result
end

// ПОСЛЕ
fn gen_or_pattern_arms(arms: [Shared<SMatchArm>]) -> [Shared<SMatchArm>] =
  arms.flat_map(arm =>
    match arm {
      SMatchArmOr(alts, body, guard) => alts.map(alt => Shared.new(SMatchArm(alt, body, guard))),
      _ => [arm]
    })
```

**Ожидание:** 371 → ~230 LOC.

---

### Шаг 6. `codegen/stmt_eval.mlc` — P1, P6

Уже частично чистый (2 `while`). Добавить интерполяцию, убрать оставшиеся циклы.

**Ожидание:** 290 → ~220 LOC.

---

### Шаг 7. `codegen/record_gen.mlc`, `codegen/method_gen.mlc`, `codegen/decl.mlc` — P1, P4, P6

Параллельно, т.к. независимы между собой.

```mlc
// ДО — record_gen: поля записи
let mut field_initializers: [string] = []; let mut i = 0
while i < fields.length() do
  field_initializers.push(gen_field_init(context, fields[i])); i = i + 1
end
record_initializer(type_name, field_initializers.join(', '))

// ПОСЛЕ
fields.map(field => gen_field_init(context, field)).join(', ')
|> field_list => record_initializer(type_name, field_list)
```

---

### Шаг 8. `checker/semantic_type_structure.mlc` — P5, P3

```mlc
// ДО — types_structurally_equal для TGeneric
TGeneric(left_name, left_args) => match right {
  TGeneric(right_name, right_args) =>
    if left_name != right_name || left_args.length() != right_args.length() then false
    else
      let mut gi = 0; let mut gok = true
      while gi < left_args.length() && gok do
        if !types_structurally_equal(left_args[gi], right_args[gi]) then gok = false end
        gi = gi + 1
      end
      gok
    end,
  _ => false
},

// ПОСЛЕ
TGeneric(left_name, left_args) => match right {
  TGeneric(right_name, right_args) =>
    left_name == right_name &&
    left_args.length() == right_args.length() &&
    left_args.indices().all(i => types_structurally_equal(left_args[i], right_args[i])),
  _ => false
},
```

**Ожидание:** 285 → ~190 LOC.

---

### Шаг 9. `checker/registry.mlc` — P5, P1

```mlc
// ДО
fn type_implements_trait(self, type_name: string, trait_name: string) -> bool =
  if trait_impls.has(type_name) then
    let mut index = 0; let mut found = false
    const impls = trait_impls.get(type_name)
    while index < impls.length() && !found do
      if impls[index] == trait_name then found = true end
      index = index + 1
    end
    found
  else false end

// ПОСЛЕ
fn type_implements_trait(self, type_name: string, trait_name: string) -> bool =
  trait_impls.has(type_name) && trait_impls.get(type_name).any(impl => impl == trait_name)
```

**Ожидание:** 451 → ~320 LOC.

---

### Шаг 10. `checker/pattern_env.mlc`, `checker/let_pattern_infer.mlc` — P1, P7

Небольшие модули, но с вложенными циклами по паттернам. P7 (деструктуризация параметров) сделает код значительно чище.

---

### Шаг 11. `checker/infer_call.mlc`, `checker/infer_array_method.mlc`, `checker/infer_result_option_method.mlc` — P1, P4

---

### Шаг 12. `checker/transform.mlc` — P1–P8, P10

Самый сложный checker-модуль: 650 строк, 18 `while`, 29 `let mut`.

```mlc
// ДО — transform_has_named_args
fn transform_has_named_args(args: [Shared<Expr>]) -> bool = do
  let mut i = 0; let mut found = false
  while i < args.length() do
    match args[i] {
      ExprNamedArg(_, _, _) => do found = true; () end,
      _ => ()
    }
    i = i + 1
  end
  found
end

// ДО — tunk_args
fn tunk_args(count: i32) -> [Shared<Type>] = do
  const args: [Shared<Type>] = []; let mut i = 0
  while i < count do args.push(Shared.new(TUnknown)); i = i + 1 end
  args
end

// ПОСЛЕ
fn transform_has_named_args(args: [Shared<Expr>]) -> bool =
  args.any(arg => match arg { ExprNamedArg(_, _, _) => true, _ => false })

fn tunk_args(count: i32) -> [Shared<Type>] =
  Array.range(0, count).map(_ => Shared.new(TUnknown))
```

**Ожидание:** 650 → ~350 LOC.

---

### Шаг 13. `checker/infer.mlc` — P3, P5, P7

```mlc
// ДО — накопление ошибок вручную
fn infer_arguments_errors(initial: InferResult, expressions: [Shared<Expr>], context: CheckContext) -> InferResult = do
  let mut result = initial
  let mut index = 0
  while index < expressions.length() do
    result = result.absorb(infer_expr(expressions[index], context))
    index = index + 1
  end
  result
end

// ПОСЛЕ
fn infer_arguments_errors(initial: InferResult, expressions: [Shared<Expr>], context: CheckContext) -> InferResult =
  expressions.fold(initial, (accumulated, expression) => accumulated.absorb(infer_expr(expression, context)))
```

**Ожидание:** 388 → ~260 LOC.

---

### Шаг 14. `checker/check.mlc`, `checker/check_mutations.mlc` — P1, P5

---

### Шаг 15. `checker/names.mlc` — P1, P5, P6

```mlc
// ДО
let mut i = 0
while i < decls.length() do
  const name = decl_name(decls[i])
  if name.length() > 0 then names.set(name, decls[i]) end
  i = i + 1
end

// ПОСЛЕ
decls.filter(decl => decl_name(decl).length() > 0)
     .for_each(decl => names.set(decl_name(decl), decl))
```

---

### Шаг 16. `checker/trait_param_expand.mlc` — P1, P4

---

### Шаг 17. `checker/partial_application_desugar.mlc` — P1, P7

506 строк, логика частичного применения — много проходов по спискам.

---

### Шаг 18. `lexer.mlc` — P5, P6

```mlc
// ДО — ручной scan токена
let mut end_pos = start + 1
while end_pos < source.length() && is_digit(source[end_pos]) do
  end_pos = end_pos + 1
end
let lexeme = source.slice(start, end_pos)

// ПОСЛЕ — если появится .take_while() или scan
// Если нет — оставить while: здесь семантика "scan" важна
```

Лексер — единственное место, где часть `while` может остаться: посимвольный scan не выражается чисто через HOF без `scan`/`take_while`. Оставить только те циклы, которые нельзя заменить без потери ясности.

**Ожидание:** 418 → ~330 LOC (только строковый scan).

---

### Шаг 19. `parser/exprs.mlc`, `parser/decls.mlc` — P1, P8

Парсер — комбинаторный стиль уже частично есть. `while` остаются в loop-разборе списков через запятую.

```mlc
// ДО — parse_comma_list pattern (повторяется ~15 раз)
const items: [Shared<T>] = []
items.push(first)
let mut parser_state = first_parser
while parser_state.kind().is_comma() do
  const next = parse_item(parser_state.advance())
  items.push(next.item)
  parser_state = next.parser
end

// ПОСЛЕ — вынести в функцию parse_comma_list
fn parse_comma_list<T>(
  first: T,
  first_parser: Parser,
  parse_one: (Parser) -> { item: T, parser: Parser }
) -> { items: [T], parser: Parser } = do
  const items = [first]
  let mut state = first_parser
  while state.kind().is_comma() do
    const next = parse_one(state.advance())
    items.push(next.item)
    state = next.parser
  end
  { items, parser: state }
end
```

Парсер специфичен: рекурсивный спуск с явным состоянием `Parser`. Механически заменить `while` на HOF сложно — здесь главное выделить `parse_comma_list` и аналоги как переиспользуемые функции.

**Ожидание:** 1056 → ~750 LOC.

---

### Шаг 20. `main.mlc` — P6, финальная уборка

Точка входа. Цепочка вызовов `pipeline` может стать читабельнее с `|>`.

---

## Последовательность коммитов

```
rewrite: codegen/expr.mlc — string interpolation
rewrite: codegen/cpp_naming.mlc — string interpolation
rewrite: codegen/ctor_info.mlc — HOF, flat_map, find
rewrite: codegen/type_gen.mlc — HOF, map/join, interpolation, primitive map
rewrite: codegen/match_gen.mlc — flat_map, HOF
rewrite: codegen/stmt_eval.mlc — interpolation
rewrite: codegen/record_gen.mlc — HOF, pipeline
rewrite: codegen/method_gen.mlc — HOF, interpolation
rewrite: codegen/decl.mlc — HOF, interpolation
rewrite: checker/semantic_type_structure.mlc — all/any
rewrite: checker/registry.mlc — any, HOF
rewrite: checker/pattern_env.mlc — HOF, destructuring
rewrite: checker/let_pattern_infer.mlc — HOF
rewrite: checker/infer_call.mlc — HOF
rewrite: checker/infer_array_method.mlc — HOF
rewrite: checker/infer_result_option_method.mlc — HOF
rewrite: checker/transform.mlc — HOF, fold, range, flat_map
rewrite: checker/infer.mlc — fold, any, destructuring
rewrite: checker/check.mlc — HOF
rewrite: checker/check_mutations.mlc — HOF
rewrite: checker/names.mlc — HOF, interpolation
rewrite: checker/trait_param_expand.mlc — HOF
rewrite: checker/partial_application_desugar.mlc — HOF, destructuring
rewrite: lexer.mlc — selective HOF, interpolation
rewrite: parser/types.mlc — parse_comma_list refactor
rewrite: parser/preds.mlc — HOF
rewrite: parser/exprs.mlc — parse_comma_list, HOF
rewrite: parser/decls.mlc — parse_comma_list, HOF
rewrite: main.mlc — pipeline cleanup
```

---

## Ожидаемые итоги

| Метрика | До | После |
|---------|-----|-------|
| `while`-циклы | ~354 | ~20 (только posix-scan в lexer + parser combinators) |
| `let mut` биндинги | ~250+ | ~30 |
| Суммарный LOC | ~10 000 | ~6 500 |
| Строковые конкатенации `+` в codegen | ~400 | ~10 |
| HOF-вызовы (map/filter/fold/flat_map) | ~50 | ~500+ |

---

## Структурные изменения: уход от процедурного стиля

Помимо замены циклов на HOF, есть системные структурные проблемы. Они решаются независимо от шагов 1–20 и затрагивают архитектуру модулей.

---

### С1. `CodegenContext`: свободные функции → методы через `extend`

**Проблема.** 14 свободных функций принимают `CodegenContext` первым аргументом. Это процедурный C-стиль: `context_resolve(context, name)`, `context_add_shared(context, name)`, `make_body_context(base, sp, sap, aet, smp, st, vp, md)`.

**Решение.** Перенести все в `extend CodegenContext`. Параллельно — заменить `make_body_context` на record spread.

```mlc
// ДО — make_body_context: 8 параметров, полное перечисление всех 16 полей
export fn context_add_shared(context: CodegenContext, name: string) -> CodegenContext = do
  let new_shared_params = context.shared_params; new_shared_params.push(name)
  make_body_context(context, new_shared_params, context.shared_array_params,
    context.array_elem_types, context.shared_map_params, context.self_type,
    context.value_params, context.match_deref_params)
end

export fn context_with_struct_using_lines(base: CodegenContext, struct_using_lines: Map<string, [string]>) -> CodegenContext =
  CodegenContext {
    field_orders: base.field_orders,
    namespace_prefix: base.namespace_prefix,
    qualified: base.qualified,
    // ... 13 полей вручную ...
    struct_using_lines: struct_using_lines
  }

// ПОСЛЕ — extend + record spread
extend CodegenContext {
  fn resolve(self, name: string) -> string =
    if self.qualified.has(name) then `${self.qualified.get(name)}${cpp_safe(name)}`
    else if self.namespace_prefix.length() > 0 then `${self.namespace_prefix}${cpp_safe(name)}`
    else cpp_safe(name)
    end

  fn add_shared(self, name: string) -> CodegenContext =
    { ...self, shared_params: self.shared_params + [name] }

  fn add_shared_array(self, name: string) -> CodegenContext =
    { ...self, shared_array_params: self.shared_array_params + [name] }

  fn add_value(self, name: string) -> CodegenContext =
    { ...self, value_params: self.value_params + [name] }

  fn add_match_deref(self, name: string) -> CodegenContext =
    { ...self, match_deref_params: self.match_deref_params + [name] }

  fn with_self_type(self, type_name: string) -> CodegenContext =
    { ...self, self_type: type_name, shared_params: [], shared_array_params: [],
               array_elem_types: Map.new(), shared_map_params: [], value_params: [], match_deref_params: [] }

  fn with_struct_using_lines(self, lines: Map<string, [string]>) -> CodegenContext =
    { ...self, struct_using_lines: lines }

  fn with_namespace_alias_prefixes(self, prefixes: Map<string, string>) -> CodegenContext =
    { ...self, namespace_alias_prefixes: prefixes }

  fn update_from_statement(self, stmt: Shared<SStmt>) -> CodegenContext =
    match stmt {
      SStmtLetConst(name, _, _, _) => self.add_value(name),
      SStmtLet(name, _, _, value_type, _) =>
        match value_type {
          TShared(_)    => self.add_shared(name),
          TArray(inner) => match inner { TShared(_) => self.add_shared_array(name), _ => self.add_value(name) },
          _             => self.add_value(name)
        },
      SStmtLetPat(pat, _, _, _, _, _, _) =>
        pat.bind_names().fold(self, (ctx, name) => ctx.add_value(name)),
      _ => self
    }
}
```

Вместо `context_resolve(context, name)` — `context.resolve(name)`. Вместо `update_context_from_statement(stmt, context)` — `context.update_from_statement(stmt)`.

---

### С2. `CheckContext`: методы вместо field-доступа

**Проблема.** `CheckContext` — голая запись. Везде: `check_context_new(inner_env, inference_context.registry)`, `check_context_new(lambda_environment, inference_context.registry)`. Создание нового контекста с расширенным `type_env` — ручная операция в 15+ местах.

```mlc
// ДО
const loop_context = check_context_new(inner_environment, inference_context.registry)
inner_environment.set(variable_name, element_type)

// ПОСЛЕ
extend CheckContext {
  fn bind(self, name: string, type_value: Shared<Type>) -> CheckContext =
    { ...self, type_env: self.type_env.with(name, type_value) }

  fn bind_all(self, bindings: Map<string, Shared<Type>>) -> CheckContext =
    { ...self, type_env: self.type_env.merge(bindings) }

  fn lookup(self, name: string) -> Shared<Type> =
    if self.type_env.has(name) then self.type_env.get(name)
    else Shared.new(TUnknown)
    end

  fn with_extend_type(self, type_name: string) -> CheckContext =
    { ...self, current_extend_type: type_name }
}

// Использование
const loop_context = inference_context.bind(variable_name, element_type)
```

---

### С3. `Pat` — метод `bind_names` через `extend`

**Проблема.** `collect_pat_bind_names_for_context` — свободная рекурсивная функция в `context.mlc`, дублируется с `pattern_bindings` в `names.mlc` и `pat_bind_names` в `match_analysis.mlc`. Три разных функции делают одно.

**Решение.** Один метод на типе `Pat`, вынесенный в `ast.mlc`.

```mlc
// ДО — три копии рекурсивного обхода Pat в разных модулях

// ПОСЛЕ — один extend в ast.mlc
extend Pat {
  fn bind_names(self) -> [string] =
    match self {
      PatIdent(name, _)          => [name],
      PatCtor(_, subs, _)        => subs.flat_map(sub => sub.bind_names()),
      PatRecord(_, fields, _)    => fields.flat_map(field => field.bind_names()),
      PatTuple(subs, _)          => subs.flat_map(sub => sub.bind_names()),
      PatArray(subs, rest, _)    => subs.flat_map(sub => sub.bind_names())
                                       + (if rest != '' && rest != '_' then [rest] else []),
      PatOr(left, right, _)      => left.bind_names(),
      _                          => []
    }
}
```

Убирает `collect_pat_bind_names_for_context`, `pattern_bindings`, и частично `pat_bind_names`. Все три заменяются на `pat.bind_names()`.

---

### С4. `InferResult` — продолжить расширение монадического API

`InferResult` уже имеет `absorb`, `with_type`, `absorb_stmt`. Достаточно добавить:

```mlc
extend InferResult {
  // ... уже есть absorb, with_type, absorb_stmt

  fn absorb_all(self, others: [InferResult]) -> InferResult =
    others.fold(self, (accumulated, other) => accumulated.absorb(other))

  fn absorb_expressions(self, expressions: [Shared<Expr>], context: CheckContext, infer: (Shared<Expr>, CheckContext) -> InferResult) -> InferResult =
    expressions.fold(self, (accumulated, expression) => accumulated.absorb(infer(expression, context)))
}
```

Тогда `infer_arguments_errors` исчезает как отдельная функция:

```mlc
// ДО
fn infer_arguments_errors(initial: InferResult, expressions: [Shared<Expr>], context: CheckContext) -> InferResult = do
  let mut result = initial; let mut index = 0
  while index < expressions.length() do
    result = result.absorb(infer_expr(expressions[index], context)); index = index + 1
  end
  result
end

// ПОСЛЕ — просто метод
infer_ok(TUnknown).absorb_expressions(expressions, context, infer_expr)
```

---

### С5. Диспетчеризация типов через `Map` вместо if/else-цепочки

Для `type_name_to_cpp`, `sem_type_to_cpp` и аналогов — выделить primitive-маппинг в константу. Это не `match`, потому что диспетч по строке, а не по sum-type.

```mlc
// В модуле codegen/type_gen.mlc
const PRIMITIVE_CPP_TYPES: Map<string, string> = Map.from_pairs([
  ('i32', 'int'), ('i64', 'int64_t'), ('u8', 'uint8_t'), ('usize', 'size_t'),
  ('f64', 'double'), ('char', 'char32_t'),
  ('string', 'mlc::String'), ('bool', 'bool'), ('unit', 'void')
])

// Тогда оба type_name_to_cpp и sem_type_to_cpp используют одну таблицу
fn primitive_or(name: string, fallback: () -> string) -> string =
  PRIMITIVE_CPP_TYPES.get(name).unwrap_or_else(fallback)
```

---

### С6. `GenStmtsResult` — fluent chain

```mlc
// ДО — ручное разворачивание в stmt_eval.mlc
const stmt_result = gen_stmt_with_try(stmt, context, try_counter)
let accumulated_code = code + stmt_result.output
let next_counter = stmt_result.next_try

// ПОСЛЕ
extend GenStmtsResult {
  fn append_stmt(self, stmt_result: GenStmtResult) -> GenStmtsResult =
    GenStmtsResult { code: self.code + stmt_result.output, next_try: stmt_result.next_try }
}

// Тогда цикл по statements → fold
statements.fold(
  GenStmtsResult { code: '', next_try: initial_try },
  (accumulated, stmt) => accumulated.append_stmt(gen_stmt_with_try(stmt, context, accumulated.next_try))
)
```

---

### С7. `TypeRegistry` — уже хороший пример, стандарт для остальных

`TypeRegistry` в `registry.mlc` использует `extend TypeRegistry { fn has_fields(...), fn type_implements_trait(...), ... }`. Это образцовый паттерн. Все остальные record-типы с нетривиальной логикой должны следовать тому же.

---

### Итог: до и после (структурно)

| Паттерн | ДО | ПОСЛЕ |
|---------|-----|-------|
| Свободные функции на контексте | `context_resolve(ctx, name)` | `ctx.resolve(name)` |
| Record reconstruction | 16-поля вручную | `{ ...base, field: value }` |
| Рекурсивный обход Pat | 3 копии в 3 модулях | `pat.bind_names()` |
| Накопление InferResult | `while` + `absorb` | `.absorb_expressions(...)` |
| Строковый диспетч | 10-ветковый if/else | `PRIMITIVE_MAP.get(name).unwrap_or_else(...)` |
| Шаг по statements | `while` + manual `code +=` | `.fold(initial, (acc, stmt) => acc.append_stmt(...))` |

---

### Порядок структурных изменений

Структурные изменения вносятся **до** соответствующих шагов переписывания:

1. `extend Pat { fn bind_names }` в `ast.mlc` → убирает дублирование (перед шагом 10)
2. `extend CodegenContext` в `context.mlc` → перед шагами 3–9
3. `extend CheckContext` в `check_context.mlc` → перед шагами 10–17
4. `extend InferResult { fn absorb_expressions, fn absorb_all }` → перед шагом 13
5. `extend GenStmtsResult { fn append_stmt }` → перед шагом 6
6. `PRIMITIVE_CPP_TYPES` константа → часть шага 4

---

## Что не переписывается

- `compiler/out/` — генерируемые файлы, не трогать.
- `compiler/tests/` — тесты переписываются только если сам тестируемый модуль меняет API (не меняет).
- `compiler/cpp/` — C++ лексер/парсер, используется отдельно, низкий приоритет.
- `compiler/ast.mlc`, `compiler/semantic_ir.mlc` — чистые type definitions, почти без логики.
