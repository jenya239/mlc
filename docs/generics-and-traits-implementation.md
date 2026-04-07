# Generics и Traits: план реализации

## Текущее состояние

### Ruby-компилятор (`lib/`)

**Уже реализовано полностью:**

| Компонент | Файл | Статус |
|-----------|------|--------|
| AST: `TypeDecl` с `type_params: [TypeParam]` | `lib/mlc/source/ast/nodes.rb:105` | готово |
| AST: `FuncDecl` с `type_params`, `where_clause` | `nodes.rb:118` | готово |
| AST: `TypeParam(name, constraint)` | `nodes.rb:136` | готово |
| AST: `WhereClause`, `WhereBound` | `nodes.rb:146` | готово |
| AST: `GenericType(base_type, type_params)` | `nodes.rb:870` | готово |
| AST: `TraitDecl(name, type_params, methods, associated_types)` | `nodes.rb:882` | готово |
| Лексер: ключевое слово `trait` | `lexer.rb:35` | готово |
| Парсер: `parse_trait_decl` — синтаксис `trait Name<T> { fn ... }` | `declaration_parser.rb:398` | готово |
| Парсер: `parse_extend_decl` с `extend Type : Trait<T> { ... }` | `declaration_parser.rb:514` | готово |
| Парсер: `parse_type_params` — синтаксис `<T, U: Constraint>` | `declaration_parser.rb:16` | готово |
| TraitRegistry: регистрация трейтов и реализаций | `services/traits/trait_registry.rb` | готово |
| SemanticIR: обработка `TraitDecl`, `ExtendDecl`, `validate_trait_implementation` | `reducers/program_reducer.rb:85` | готово |
| SemanticIR: обработка `type_params` в функциях | `reducers/function_reducer.rb:28` | готово |
| Codegen: `build_template_lines` — `template<typename T>` + `requires` | `header_generator.rb:391` | готово |
| Codegen: `generate_trait_structs` — vtable-структуры | `backends/cpp/codegen.rb:228` | готово |
| Codegen: `generate_trait_adapters` — адаптерные функции | `backends/cpp/codegen.rb:246` | готово |

**Важно:** Ruby-компилятор генерирует трейты как **vtable-структуры** (не C++20 concepts):

```cpp
// Ruby backend генерирует:
struct Display {
  std::function<mlc::String()> to_string;
};
inline Display i32_as_Display(int self) noexcept {
  return Display{ .to_string = [self]() noexcept { return i32_to_string(self); } };
}
```

### mlcc (`compiler/`)

**Не реализовано ничего из дженериков/трейтов:**

```
# compiler/ast.mlc — текущее состояние:
| DeclFn(string, [Shared<Param>], Shared<TypeExpr>, Shared<Expr>)   # нет type_params
| DeclType(string, [Shared<TypeVariant>])                            # нет type_params
| DeclExtend(string, [Shared<Decl>])                                 # нет trait_name
# нет DeclTrait
```

---

## Стратегия для mlcc

Ruby backend использует vtable-подход (динамическая диспетчеризация через `std::function`). Для mlcc выбираем **C++20 concepts** (статическая диспетчеризация):

| Аспект | Ruby backend | mlcc |
|--------|-------------|------|
| Трейт → C++ | `struct Display { std::function<...>... }` | `concept Display = requires(...) { ... }` |
| Дженерик функция | `template<typename T>` (без `requires`) | `template<typename T> requires Trait<T>` |
| Диспетчеризация | runtime vtable | compile-time monomorphization |
| Синтаксис трейта | `trait Name { fn ... }` | `type Name { fn ... }` (без нового ключевого слова) |

---

## Фаза 1: Generic Types и Functions

### 1.1 AST (`compiler/ast.mlc`)

Изменить два варианта:

```
# было:
| DeclFn(string, [Shared<Param>], Shared<TypeExpr>, Shared<Expr>)
| DeclType(string, [Shared<TypeVariant>])
| DeclExtend(string, [Shared<Decl>])

# стало:
| DeclFn(string, [string], [Shared<Param>], Shared<TypeExpr>, Shared<Expr>)
| DeclType(string, [string], [Shared<TypeVariant>])
| DeclExtend(string, string, [Shared<Decl>])
#                   ^^^^^^ trait_name (пустая строка = нет трейта)
```

Второй `[string]` в `DeclFn`/`DeclType` — список имён type-параметров, например `["T", "U"]`.

**Затронутые места матчинга:**
- `compiler/ast.mlc:74-86` — `strip_export`, `decl_name` (~4 строки)
- `compiler/checker/names.mlc` — все match на `DeclFn`, `DeclType`, `DeclExtend` (~8 мест)
- `compiler/checker/infer.mlc` — аналогично (~6 мест)
- `compiler/checker/registry.mlc` — аналогично (~4 места)
- `compiler/codegen.mlc` — все места кодогенерации (~6 мест)

Во всех существующих местах просто добавить `_` на новые поля.

### 1.2 Парсер (`compiler/parser/decls.mlc`)

После имени функции/типа: если следующий токен `<`, читать список идентификаторов до `>`.

```
# parse_fn_decl:
let name = expect_ident(parser)
let type_params = parse_type_params_opt(parser)  # новая функция
# ...
DeclFn(name, type_params, params, ret_type, body)

# parse_type_decl:
let name = expect_ident(parser)
let type_params = parse_type_params_opt(parser)
# ...
DeclType(name, type_params, variants)
```

```
fn parse_type_params_opt(parser: Parser) -> ParseResult =
  if parser.current_is(LAngle) then
    # consume <, читать ident,ident,..., consume >
    ...
  else
    ParseResult { value: [], parser: parser }
```

Тип `LAngle` уже есть в лексере (оператор `<`). Нужна проверка: `<` после имени, не в выражении.

### 1.3 Codegen (`compiler/codegen.mlc`)

В `gen_fn_decl` и `gen_type_decl`:

```
# gen_fn_decl:
fn gen_fn_decl(name: string, type_params: [string], params: ...) -> string =
  let template_prefix =
    if type_params.length() > 0 then
      let params_str = join_strings(type_params, ", ")
      f"template<typename {params_str}>\n"  # TODO: join_map
    else
      ""
  ...
  f"{template_prefix}auto {name}({params_str}) -> {ret_type_str}"
```

`TyNamed("T")` где `T` — type-параметр → codegen уже выведет `T` как есть, дополнительной логики не нужно.

---

## Фаза 2: Trait Definitions

### 2.1 AST (`compiler/ast.mlc`)

Добавить новый вариант:

```
| DeclTrait(string, [string], [Shared<Decl>])
# имя_трейта, type_params, методы (только сигнатуры)
```

### 2.2 Парсер (`compiler/parser/decls.mlc`)

**Проблема:** `type Name { fn ... }` — неотличимо от record-типа `type Name = { field: T }`.

**Решение:** `type Name { fn ... }` (без `=`) → трейт. Парсер уже различает: если после имени стоит `{` без `=` — это тело трейта.

```
# parse_decl:
# "type" IDENT "{" → DeclTrait
# "type" IDENT "=" → DeclType (существующее)
```

Методы в теле трейта — только сигнатуры (`fn name(self: Self, ...) -> T`), тело опционально.

### 2.3 Codegen (`compiler/codegen.mlc`)

`DeclTrait` → C++20 concept:

```cpp
// type Display { fn to_string(self: Self) -> string }
// →
concept Display = requires(const auto& self) {
  { to_string(self) } -> std::convertible_to<mlc::String>;
};
```

Для каждого метода в трейте:
- `fn method(self: Self, arg: T) -> R` → строка `{ method_name(self, arg) } -> std::convertible_to<R>;`

Добавить в `CodegenContext`:
```
# поле trait_methods: Map<string, string>
# ключ: имя метода, значение: имя трейта
# используется при вызове для диспетчеризации
```

---

## Фаза 3: Trait Implementations (extend Type : Trait)

### 3.1 AST

`DeclExtend(string, string, [Shared<Decl>])` — уже изменён в Фазе 1 (второй `string` = `trait_name`, пустая строка если нет трейта).

### 3.2 Парсер (`compiler/parser/decls.mlc`)

```
# parse_extend_decl:
# "extend" TYPE_NAME ":" TRAIT_NAME "{" ... "}"  →  DeclExtend(type, trait, methods)
# "extend" TYPE_NAME "{" ... "}"                 →  DeclExtend(type, "", methods)
```

Уже частично есть в Ruby-парсере, перенести в mlcc.

### 3.3 Codegen

Для `DeclExtend(type_name, trait_name, methods)` где `trait_name != ""`:

1. Каждый метод кодогенерируется как обычная свободная функция `type_name_method_name(...)`.
2. После всех методов добавить: `static_assert(TraitName<TypeName>, "...");`

```cpp
// extend i32 : Display { fn to_string(self: i32) -> string = ... }
// →
auto i32_to_string(int self) -> mlc::String { ... }
static_assert(Display<int>, "i32 does not implement Display");
```

---

## Фаза 4: Bounded Generics

### 4.1 AST

Расширить `DeclFn`:
```
| DeclFn(string, [string], [[string]], [Shared<Param>], Shared<TypeExpr>, Shared<Expr>)
#                           ^^^^^^^^^
#                           bounds[i] — список трейтов для type_params[i]
```

Либо проще — параллельный список: `type_params: [string]` + `type_bounds: [[string]]`.

### 4.2 Парсер

```
fn parse_type_params_with_bounds(parser: Parser) =
  # <T: Display, U: Eq + Clone>
  # T → bounds = ["Display"]
  # U → bounds = ["Eq", "Clone"]
```

### 4.3 Codegen

```cpp
// fn print<T: Display>(x: T) -> unit
// →
template<typename T>
requires Display<T>
auto print(T x) -> void { ... }

// fn zip<T: Clone, U: Eq>(a: T, b: U) -> T
// →
template<typename T, typename U>
requires Clone<T> && Eq<U>
auto zip(T a, U b) -> T { ... }
```

---

## Порядок реализации и зависимости

```
Фаза 1 (Generic Types/Fns)
  └── Фаза 2 (Trait Definitions)
        └── Фаза 3 (Trait Impls)
              └── Фаза 4 (Bounded Generics)
```

Каждая фаза: AST → Parser → Codegen → Checker (только `_` на новые поля).

## Ключевые отличия от Ruby-компилятора

1. **Синтаксис трейтов:** Ruby использует ключевое слово `trait`, mlcc использует `type Name { fn ... }` (без `=`).
2. **Codegen трейтов:** Ruby → vtable structs + adapters; mlcc → C++20 concepts + static_assert.
3. **Диспетчеризация:** Ruby → runtime (std::function); mlcc → compile-time (template instantiation).
4. **Where clauses:** Ruby поддерживает `where T: Trait1 + Trait2`; mlcc в Фазе 4 поддерживает `<T: Trait>` (inline bounds).

## Что НЕ нужно менять в Ruby-компиляторе

Ruby-компилятор уже полностью поддерживает дженерики и трейты для компиляции обычного MLC-кода. Изменения нужны только в `compiler/*.mlc` — т.е. в коде самого `mlcc`.

Единственное исключение: Ruby-компилятор пока не умеет разбирать синтаксис `type Name { fn ... }` (без `=`) как трейт — он использует ключевое слово `trait`. Чтобы тестировать `mlcc` через Ruby-компилятор, нужно будет либо:
- добавить в Ruby-парсер поддержку альтернативного синтаксиса трейтов
- либо компилировать через уже скомпилированный `mlcc`

## Оценка трудоёмкости

| Фаза | Файлы | Сложность | Оценка |
|------|-------|-----------|--------|
| 1: Generic Types/Fns | `ast.mlc`, `decls.mlc`, `codegen.mlc` + патчи | средняя | 3-4 ч |
| 2: Trait Definitions | `ast.mlc`, `decls.mlc`, `codegen.mlc` | средняя | 3-4 ч |
| 3: Trait Impls | `decls.mlc`, `codegen.mlc` | средняя | 2-3 ч |
| 4: Bounded Generics | `ast.mlc`, `decls.mlc`, `codegen.mlc` | высокая | 3-5 ч |

Итого: ~11-16 часов активной разработки.
