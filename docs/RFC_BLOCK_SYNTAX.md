# RFC: Block Syntax Unification

## Проблема

Текущий синтаксис использует `{ }` для двух целей:
1. **Record literals**: `{ x: 1, y: 2 }`
2. **Block expressions**: `{ let x = 1; x + 1 }`

Это создаёт неоднозначность при парсинге:
```mlc
fn foo() -> i32 = { let x = 1; x }  // Parse error: Expected IDENTIFIER, got LET
```

## Текущее состояние

| Конструкция | Текущий синтаксис | Статус |
|-------------|-------------------|--------|
| Explicit block | `do ... end` | ✅ Работает |
| Unsafe block | `unsafe { ... }` | ⚠️ Конфликт с records |
| Match | `match x { ... }` | ⚠️ Конфликт с records |
| If/else | `if cond then ... else ...` | ⚠️ Нет явного end |
| While | `while cond do ... end` | ✅ Работает |
| For | `for x in arr do ... end` | ✅ Работает |

## Предлагаемые изменения

### 1. Unified block syntax

**Правило**: Все блочные конструкции закрываются `end`:

```mlc
// Explicit block
fn foo() -> i32 = do
  let x = 10
  x + x
end

// Unsafe block (без do!)
fn bar() -> i32 = unsafe
  let ptr = get_ptr()
  *ptr
end

// If/else (без do!)
fn abs(x: i32) -> i32 =
  if x < 0 then
    -x
  else
    x
  end

// Match (без do!)
fn check(opt: Option<i32>) -> i32 =
  match opt
    Some(v) => v,
    None => 0
  end

// While/for (сохраняем do)
fn count() -> i32 = do
  let mut i = 0
  while i < 10 do
    i = i + 1
  end
  i
end
```

### 2. `{ }` только для record literals

```mlc
// Record literal - OK
let point = { x: 1, y: 2 }

// Empty record
let unit = {}

// Nested records
let person = { name: "Alice", address: { city: "NYC", zip: 10001 } }
```

## Грамматика

```
block_expr    := 'do' statements expr? 'end'
unsafe_expr   := 'unsafe' statements expr? 'end'
if_expr       := 'if' condition 'then' branch ('else' branch)? 'end'
match_expr    := 'match' scrutinee match_arms 'end'
while_expr    := 'while' condition 'do' statements 'end'
for_expr      := 'for' pattern 'in' expr 'do' statements 'end'

record_lit    := '{' (field_init (',' field_init)*)? '}'
field_init    := IDENT ':' expr
```

## Затронутые компоненты

### Лексер
- `DO` - уже есть
- `END` - уже есть
- `THEN` - нужно проверить
- `LBRACE`/`RBRACE` - остаются для records

### Парсер (expression_parser.rb)
1. `parse_unsafe_block` - убрать `{ }`, добавить `end`
2. `parse_match_expression` - убрать `{ }`, добавить `end`
3. `parse_if_expression` - добавить `end`
4. `parse_block_expression` - убрать (заменить на `parse_do_expression`)
5. `parse_primary` - убрать `LBRACE` → block

### AST Nodes
- Без изменений (структура та же, меняется только синтаксис)

### Semantic/Backends
- Без изменений (работают с AST)

## План реализации

### Phase 1: TDD - Написать тесты
```ruby
def test_unsafe_block_with_end
  source = "fn foo() -> i32 = unsafe let x = 1; x end"
  ast = @parser.new(source).parse
  # ...
end

def test_if_with_end
  source = "fn foo(x: i32) -> i32 = if x > 0 then x else -x end"
  # ...
end
```

### Phase 2: Лексер
- Проверить токены DO, END, THEN

### Phase 3: Парсер
1. Изменить `parse_unsafe_block`
2. Изменить `parse_match_expression`
3. Изменить `parse_if_expression`
4. Удалить/изменить `parse_block_expression`

### Phase 4: Обновить существующие тесты
- Найти все тесты с `{ }` блоками
- Обновить на `do ... end` или специфичный синтаксис

### Phase 5: Документация
- Обновить примеры
- Обновить CLAUDE.md если нужно

## Риски

1. **Много тестов** - ~1500 тестов, многие могут использовать `{ }` для блоков
2. **Обратная совместимость** - существующий код перестанет работать
3. **Lambda syntax** - `(x) => { ... }` нужно решить

## Lambda syntax

Текущий: `(x) => { body }`

Варианты:
1. `(x) => do body end` - консистентно
2. `(x) => body` - только single expression
3. `(x) -> body end` - другая стрелка для multi-line

**Рекомендация**: Вариант 1 - `(x) => do ... end`

```mlc
let add = (x, y) => x + y                    // single expr
let complex = (x) => do let y = x * 2; y end // multi-line
```

## Решения

1. **`then` обязателен в однострочном if**, опционален в многострочном
2. **Lambda**: single-expr без do/end, multi-line с do/end
3. **Match**: pipe syntax `| pattern => body`
4. **`unless`** как инверсия if + postfix форма

## Финальный синтаксис

```mlc
// If однострочный - then ОБЯЗАТЕЛЕН
if x > 0 then x else -x end

// If многострочный - then опционален
if x > 0
  x
else
  -x
end

// Unless - инверсия if
unless x == 0 then 1 / x else 0 end

unless x == 0
  1 / x
else
  0
end

// Postfix if/unless (как в Ruby)
x * 2 if x > 0
0 unless valid

// Lambda
let f = (x) => x * 2            // single expression
let g = (x) => do               // multi-line
  let y = x * 2
  y + 1
end

// Match с pipe
match opt
| Some(v) => v
| None => 0
end

// Unsafe
unsafe
  let ptr = get_ptr()
  *ptr
end
```
