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

- Self-hosted исходники: `compiler/`. Детальный roadmap: `docs/ROADMAP.md`. Политика Ruby: `docs/SELF_HOSTED_PLAN.md`.
- `lib/mlc/` не удалять — эталон семантики и codegen, резерв при регрессиях.

### Два компилятора

| | Bootstrap (Ruby) | Self-hosted (MLC) |
|-|-----------------|-------------------|
| Путь | `lib/mlc/` | `compiler/` |
| Сборка | — (интерпретатор) | `compiler/build.sh` → `compiler/out/mlcc` |
| Пайплайн | `MLC → SemanticIR → C++ AST DSL → C++ source` | `MLC → AST → checker → codegen → C++ source` |
| Тесты | `rake test_mlc` (1106 runs) | `rake test_compiler_mlc` (192 теста) |
| Статус | Эталон, полный стек | Компилирует весь `compiler/`, E2E работает |

### Ключевые компоненты

| Слой | Путь | Назначение |
|------|------|------------|
| Parser (Ruby) | `lib/mlc/source/parser/` | Лексер и парсер MLC |
| AST (Ruby) | `lib/mlc/source/ast/nodes.rb` | AST-узлы |
| SemanticIR | `lib/mlc/representations/semantic/gen/` | Семантический анализ, типы |
| C++ Backend (Ruby) | `lib/mlc/backends/cpp/` | Lowering SemanticIR → C++ AST |
| C++ AST DSL | `lib/mlc/backends/cpp/cpp_ast/` | Представление C++ кода |
| Runtime | `runtime/include/mlc/` | C++20 runtime |
| Registries | `lib/mlc/registries/` | TypeRegistry, FunctionRegistry |
| Self-hosted lexer/parser | `compiler/lexer.mlc`, `compiler/parser/` | — |
| Self-hosted checker | `compiler/checker/` | names.mlc, infer.mlc, registry.mlc |
| Self-hosted codegen | `compiler/codegen/` (`module.mlc`, `eval.mlc`, `decl.mlc`, …) | → C++20 |
| E2E тесты mlcc | `compiler/tests/e2e/` | 4 программы через mlcc |

### Паттерны проекта (Ruby-компилятор)

- **Rule Engine**: `rules/` — по одному файлу на каждый тип AST-узла (`call_rule.rb`, `if_rule.rb`, …)
- **Visitor Pattern**: `expression_visitor.rb`, `statement_visitor.rb`
- **Registry Pattern**: `TypeRegistry`, `FunctionRegistry`, `TraitRegistry`, `ImplRegistry`
- **Pipeline**: `PassManager` с последовательными проходами

### Целевые паттерны self-hosted (roadmap Фаза 2)

- Codegen уже каталог `compiler/codegen/`; дальнейшее — вынос из крупных файлов (в т.ч. `eval.mlc`) по одному коммиту.
- Каждый модуль — `export fn` для публичного API.
- Checker: по мере разбиения избегать циклических импортов с `infer.mlc`; при «Unknown identifier» во вложенных областях — `import * as alias` и `alias.foo`.

### Синтаксис MLC

```
fn name(args) -> RetType = expr                     # однострочная функция
fn name(args) -> RetType = do ... end               # многострочная функция (НЕ { ... })
const x = expr                                      # неизменяемая привязка (stmt)
let x = expr                                        # неизменяемая привязка (let-форма)
let mut x = expr                                    # изменяемая; нужна для x = … и .push/.set на биндинге
let const x = expr                                  # compile-time constexpr
type Name = { field: Type }                         # record type
type Name = Variant1(Type) | Variant2               # sum type
type Result<T, E> = Ok(T) | Err(E)                  # generic sum type
extend Type { fn method(self: Type) -> T = ... }    # instance method
extend Type : Trait { fn method(self: Type) = ... } # trait impl
Type.static_method()                                # static call
obj.instance_method()                               # instance call → Type_method(obj)
expr?                                               # ? оператор — ранний возврат Err из Result<T,E>
```

### Команды

```bash
# Ruby-компилятор: тесты
bundle exec ruby -Ilib:test test/mlc/some_test.rb   # один тест
bundle exec rake test_mlc                            # test/mlc/** (быстро, CI)
bundle exec rake test_unit                           # весь test/ кроме integration

# Self-hosted: тесты
bundle exec rake test_compiler_mlc                  # compiler/tests/ → 192 теста
bundle exec rake test_self_hosted_stack             # test_mlc + test_compiler_mlc
compiler/tests/e2e/run_e2e.sh [mlcc_binary]         # E2E: 4 программы
compiler/tests/build_tests_self.sh [mlcc_binary]    # unit-тесты через self-hosted бинарь

# Сборка
compiler/build.sh                                   # Ruby → mlcc
MLCC_BOOTSTRAP=1 compiler/build.sh                 # Ruby → mlcc → mlcc_bootstrap (g++)
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
