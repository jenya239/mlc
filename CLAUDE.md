# Инструкции для Claude Code

## Стиль Общения

Строгий, технически корректный, без фантазий. Только факты. Кратко.

- Использовать точные номера строк, имена файлов, функций
- Не предполагать — проверять (Read, Grep, Bash, тесты)
- Не использовать расплывчатые формулировки
- Без эмодзи, без энтузиазма, без отчётов без запроса
- Язык: русский или английский

## Проект MLC

MLC — компилятор языка программирования. Цель: self-hosted compiler.

- Self-hosted исходники: `compiler/`. Дорожная карта и политика **Ruby как резерв/bootstrap**: `docs/SELF_HOSTED_PLAN.md`.
- `lib/mlc/` не удалять — эталон и запасной компилятор.

### Архитектура

```
MLC Source → Lexer/Parser → AST → SemanticIR → C++ AST DSL → C++ source
```

- **Компилятор**: Ruby (lib/mlc/)
- **Target**: C++20 (runtime/include/mlc/)
- **Тесты**: Minitest (test/)

### Ключевые компоненты

| Слой | Путь | Назначение |
|------|------|------------|
| Parser | lib/mlc/source/parser/ | Лексер и парсер MLC |
| AST | lib/mlc/source/ast/nodes.rb | AST-узлы |
| SemanticIR | lib/mlc/representations/semantic/gen/ | Семантический анализ, типы |
| C++ Backend | lib/mlc/backends/cpp/ | Lowering SemanticIR → C++ AST |
| C++ AST DSL | lib/mlc/backends/cpp/cpp_ast/ | Представление C++ кода |
| Runtime | runtime/include/mlc/ | C++20 runtime библиотека |
| Registries | lib/mlc/registries/ | TypeRegistry, FunctionRegistry |
| Self-hosted | compiler/ | MLC lexer/parser/checker/codegen → C++ |

### Паттерны проекта

- **Rule Engine**: правила (rules/) для каждого типа AST-узла
- **Visitor Pattern**: expression_visitor.rb, statement_visitor.rb
- **Registry Pattern**: TypeRegistry, FunctionRegistry, TraitRegistry, ImplRegistry
- **Pipeline**: PassManager с последовательными проходами
- **extend блоки**: `extend Type { fn method(self: Type, ...) }` → `Type_method(obj, ...)`

### Синтаксис MLC

```
fn name(args) -> RetType = expr                    # однострочная функция
fn name(args) -> RetType = do ... end              # многострочная функция (НЕ { ... })
let x = expr                                       # неизменяемая привязка
let mut x = expr                                   # изменяемая привязка
type Name = { field: Type }                        # record type
type Name = Variant1(Type) | Variant2              # sum type
extend Type { fn method(self: Type) -> T = ... }   # instance method
extend Type : Trait { fn method(self: Type) = ... } # trait impl
Type.static_method()                                # static call
obj.instance_method()                               # instance call → Type_method(obj)
```

### Команды

```bash
bundle exec ruby -Ilib:test test/mlc/some_test.rb    # один тест
bundle exec ruby -Ilib:test -e "Dir['test/mlc/**/*_test.rb'].each { |f| require_relative f }"  # все unit-тесты
bundle exec rake test_mlc                              # только test/mlc/** (CI)
bundle exec rake test_compiler_mlc                     # compiler/tests (нужен g++)
bundle exec rake test_self_hosted_stack                # test_mlc + test_compiler_mlc
bundle exec rake test_unit                             # весь test/ кроме integration
bundle exec rake test                                  # все тесты (долго, E2E компилирует C++)
```

### Важные соглашения

- Коммиты только по команде, без --trailer
- Типы в SemanticIR: RecordType, ArrayType, MapType, GenericType, FunctionType, UnitType, TypeVariable
- Array → `mlc::Array<T>` (COW), Map → `mlc::HashMap<K,V>` (COW)
- Smart pointers: `Shared<T>` → `std::shared_ptr`, `Owned<T>` → `std::unique_ptr`
- Строки: `string` → `mlc::String`
- Мутирующие методы (.push, .set, .pop, .remove) требуют `let mut`

## Подход к Разработке

- Качество важнее скорости
- Читать код перед редактированием
- Проверять существующие паттерны перед добавлением новых
- Запускать unit-тесты после изменений (`test/mlc/`)
- E2E тесты (`test/integration/`) запускать выборочно (каждый компилирует C++)
- Работать самостоятельно, уточнять только при критической неопределённости
- Каждое утверждение — проверяемый факт
