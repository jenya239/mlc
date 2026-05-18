# Заметки по языку и компиляторам (mlc2)

Дополнение к `docs/MLC.md`: решения по паритету Ruby ⇄ mlcc и ограничения текущей реализации. Обновлять при изменении чекера, кодогена, замыканий или `build_registry`.

---

## E4 — дефолты полей записи

### Эталон (Ruby)

- В объявлении типа: `поле: T = выражение`.
- Литерал **`TypeName {}`** — только если у всех полей без явного значения есть дефолт в типе; иначе ошибка «missing field».
- Частичный литерал — недостающие поля подставляются из дефолтов объявления.
- Дженерики: дефолты полей при параметрах типа запрещены (сообщение про generic parameters).
- **Выражение дефолта в типе** обрабатывается пайплайном Semantic Gen через visitor и может быть произвольным (после проверки типов и наличия окружения).
- В C++: default member initializers у членов struct; литералы записей могут использовать designated initializers при нужной форме literal/rule.

### Self-hosted (mlcc)

**Что совпадает по замыслу:** парсинг `поле: T = expr`; регистрация дефолтов в `TypeRegistry`; слияние явных полей литерала с дефолтами перед проверкой/кодогеном; запрет дефолтов при generic-параметрах типа; диагностика отсутствующих полей с учётом дефолтов.

**Где паритет намеренно уже:**

Выражение дефолта в объявлении записи должно допускать **статический вид**, который можно записать как инициализатор члена C++ в struct (контур codegen без общего lowering выражений через Semantic IR для членов типа).

Разрешённые формы AST задаются в `compiler/record_field_default_initializer.mlc`:

- литералы (`ExprInt`, строки, bool, unit, float/int узкой модели и т.д.);
- **`ExprIdent`** — через `context_resolve` и `map_builtin_identifier_reference` (без эвристики «конструктор с `{}`» из codegen идентификаторов);
- **`ExprBin`** только для операций `+`, `-`, `*`, `/`, `%`, если допустимы оба поддерева;
- **`ExprUn`** только для `+` и `-`, если допустимо внутреннее подвыражение.

Чекер (`compiler/checker/check.mlc`, `record_field_default_value_diagnostics`):

1. Если форма **не** подходит под `record_field_default_expression_acceptable_for_codegen` — ошибка вида «cannot be lowered to a C++ member initializer …».
2. Иначе — **`infer_expr`** в пустом `type_env` + только `registry` (как раньше для дефолтов): тип дефолта сверяется с аннотацией поля.

Кодоген struct (`compiler/codegen/type_gen.mlc`, `variant_record_struct_inline_member_declarations`):

- для полей с `has_default_expression` и допустимой формой добавляется **` = `** и строка из **`record_field_default_expression_cpp_initializer`**.

**Идентификаторы в дефолте:** разрешены синтаксически; если имя не типизируется в пустом окружении (нет привязки в `type_env`), чекер выдаст обычную ошибку вывода (`infer_expr`), а не сообщение про member initializer.

**Вызовы и прочее:** например `zero()` — форма `ExprCall`, не входит в допустимые → сообщение про lowering. Полный паритет с Ruby (произвольное выражение → корректный C++ у члена или отложенное заполнение литералов только) — отдельная задача: понадобится общий путь lowering AST/SExpr для дефолтов или политика «только литералы в struct, всё остальное только через развёртку в литерале» с жёсткой спецификацией.

### Файлы для правок

| Назначение | Путь |
|------------|------|
| Допустимость формы и текст C++ фрагмента | `compiler/record_field_default_initializer.mlc` |
| Диагностики при регистрации типа | `compiler/checker/check.mlc` |
| Члены struct с ` = …` | `compiler/codegen/type_gen.mlc` |
| Слияние литерала с дефолтами | `compiler/checker/record_lit_merge.mlc`, `compiler/checker/infer.mlc` |
| Регистрация дефолтов | `compiler/checker/registry.mlc` |

### Тесты

- Ruby: `test/mlc/record_field_defaults_test.rb` (в т.ч. арифметика в дефолте и codegen struct).
- mlcc: `compiler/tests/test_checker.mlc`, `compiler/tests/test_parser.mlc` (и общий прогон `rake test_compiler_mlc`).

---

## E5 — оператор пайпа `|>`

### Эталон (Ruby)

- Лексер: токен `|>` (`lib/mlc/source/parser/lexer.rb`).
- Парсер: цепочка приоритетов `logical_or` → `logical_and` → … → **`parse_equality`** (`==` / `!=`) → **`parse_pipe`** → **`parse_comparison`** (`<` `>` `<=` `>=`) → ниже (`expression_parser.rb`).
- Семантика: `ExpressionVisitor#desugar_pipe` сводит бинарный узел `|>` к обычному `Call` с первым аргументом слева.

### Self-hosted (mlcc)

- Парсинг и десахар в одном проходе: `compiler/parser/exprs.mlc` (`pipe_desugar`, операнды `|>` через тот же относительный порядок, что у Ruby: сравнения ниже пайпа, равенство выше пайпа, логика `&&` / `||` выше равенства).
- Отдельного узла пайпа в AST после парса нет — только `ExprCall`.

### Файлы для правок

| Назначение | Путь |
|------------|------|
| Преcedence + десахар | `compiler/parser/exprs.mlc` |
| Ruby-паритет | `lib/mlc/source/parser/expression_parser.rb` |

### Тесты

- Ruby: `test/mlc/pipe_operator_test.rb` (в т.ч. приоритет относительно `||`, `&&`, `==`).
- mlcc: `compiler/tests/test_parser.mlc`, `compiler/tests/test_pipe_and_record_update.mlc`, `compiler/tests/test_checker.mlc`.

---

## E6 — частичное применение `_`

### Эталон (Ruby)

- Лексер: `_` в выражении — идентификатор `"_"` (`VarRef`).
- Десахар до вывода типов / lowering: вызовы `Call` (и при необходимости `SafeCall`) с прямым аргументом `_` заменяются на `Lambda` с телом — тот же вызов, где каждая дыра подставлена свежим параметром (`lib/mlc/representations/semantic/gen/services/features/partial_application_desugar.rb`, вызывается из `ExpressionVisitor#visit`).
- Семантика совпадает с правилами § E6 в `docs/MLC.md`.

### Self-hosted (mlcc)

- Десахар после парса, до `check_names` / `infer_expr` и повторно на входе `transform_expr`: `compiler/checker/partial_application_desugar.mlc`, функция `partial_application_desugar_expr`.
- AST: дыры остаются как `ExprIdent("_", …)` до трансформации; результат — `ExprLambda` с телом `ExprCall` / `ExprMethod`.

### Файлы для правок

| Назначение | Путь |
|------------|------|
| Ruby: обход AST и построение лямбды | `lib/mlc/representations/semantic/gen/services/features/partial_application_desugar.rb` |
| Ruby: точка входа | `lib/mlc/representations/semantic/gen/visitors/expression_visitor.rb` |
| mlcc: десахар | `compiler/checker/partial_application_desugar.mlc` |
| mlcc: checker / IR | `compiler/checker/check.mlc`, `compiler/checker/transform.mlc` |

### Тесты

- Ruby: `test/mlc/partial_application_test.rb`, при необходимости `test/mlc/representations/semantic/gen/engine_test.rb`.
- mlcc: `compiler/tests/test_checker.mlc`, `compiler/tests/test_parser.mlc`.

---

## F1 — строковые литералы (`'` / `"` / `` ` ``)

### Эталон (Ruby)

- Лексер: одиночные кавычки `tokenize_raw_string`, двойные `tokenize_string`, шаблоны `` tokenize_backtick_template `` (`lib/mlc/source/parser/lexer.rb`).
- **`'...'`**: без интерполяции; escape `\\`, `\'`, `\n`, `\t`, `\r`, `\0`; неизвестный escape даёт `\` и следующий символ (как в Ruby-подмножестве).
- **`"..."`**: escape через общий механизм; возможна интерполяция **`${`** (строка или сегменты интерполяции).
- **`` `...${}...` ``**: escape `` \` ``, `\${`, `\n`/`\t`/`\r`/`\`; интерполяция только через **`${`**.

### Self-hosted (mlcc)

- Лексер: `compiler/lexer.mlc` — `scan_single_string`, `scan_string`, `scan_template`; первичный разбор литералов в `compiler/parser/exprs.mlc` (`parse_primary`).
- **`'...'`**: без интерполяции; escape выровнен с Ruby: `\\`, `\'`, `\n`, `\t`, `\r`, `\0`; иначе `\` + символ; один декодированный символ → `LChar`, иначе `LStr`.
- **`"..."`**: только плоская строка (`LStr`), без `$`/интерполяции на этапе лексера.
- Интерполяция → только **шаблоны** `` `...${}...` `` (`LTemplate`).

Импорт: `compiler/parser/decls.mlc` (`parse_import_decl`) принимает строковый путь и для `'…'`, и для `"…"` (не один символ).

### Файлы для правок

| Назначение | Путь |
|------------|------|
| Ruby: одиночные / двойные / шаблоны | `lib/mlc/source/parser/lexer.rb` |
| mlcc: одиночные / двойные / шаблоны | `compiler/lexer.mlc` |

### Тесты

- Ruby: `test/mlc/string_operations_test.rb`, при необходимости смежные тесты лексера.
- mlcc: `compiler/tests/test_lexer.mlc`; общий прогон `rake test_compiler_mlc`.

---

## F2 — `match` с оболочкой `do` … `end`

### Эталон (Ruby)

- Парсинг: [lib/mlc/source/parser/expression_parser.rb](lib/mlc/source/parser/expression_parser.rb) — `parse_match_expression`, опциональный `consume(:DO)` перед `parse_pipe_style_match_arms`; тело ветки `parse_match_arm_body_node` (в т.ч. `do`).

### Self-hosted (mlcc)

- [compiler/parser/exprs.mlc](compiler/parser/exprs.mlc): ветка `match scrutinee do … end` — `parse_arms_do_delimited`; сохраняются `{ … }` и `\| … end`.

### Файлы для правок

| Назначение | Путь |
|------------|------|
| Ruby: матч | `lib/mlc/source/parser/expression_parser.rb` |
| mlcc: матч | `compiler/parser/exprs.mlc` |

### Тесты

- Ruby: `test/mlc/pattern_matching_test.rb`.
- mlcc: `compiler/tests/test_parser.mlc`; `rake test_compiler_mlc`.

---

## G1 — замыкания, COW-коллекции и мутации

### Кодоген лямбд с захватом

- `mlc::Array` и `Map` в runtime — COW (`shared_ptr` + `detach()` при мутации).
- Если в C++ лямбда захватывает внешние биндинги **по значению** (`[=]`), копия контейнера может получить отдельное внутреннее состояние; мутации внутри лямбды (например `.push`) не видны снаружи — ломается семантика `fold`/редьюсеров, совпадающая с Ruby.
- Текущее правило кодогена: при непустом списке захватываемых имён префикс **`[&]`** (см. `compiler/codegen/expr_eval.mlc`, `compiler/codegen/expression_support.mlc`). Пустые лямбды — **`[]`**.

### Checker: мутация внешнего `let mut` из тела лямбды

- Внешние изменяемые локалы не должны мутироваться из тела замыкания (включая вложенные вызовы): проверка в `compiler/checker/check_mutations.mlc` даёт диагностику; корректный паттерн — явный цикл (`while`) или передача состояния иначе, без присваивания внешнему `let mut` из лямбды.

### Регистрация типов без квадратичного копирования

- `build_registry` наполняет один `TypeRegistry` через **`register_decl_into(registry: ref mut TypeRegistry, …)`**, а не `fold` с возвратом новой копии на каждую декларацию — иначе при COW у карт внутри реестра получалось **O(n²)** на больших модулях.

### Тесты

- mlcc: `compiler/tests/test_codegen.mlc` (ожидаемый префикс захвата лямбд), `compiler/tests/test_checker.mlc` (мутации и смежные кейсы).

---

## Прочее

- В импортах MLC **не ставить завершающую запятую** в списке символов в фигурных скобках `{ … }` — Ruby-парсер bootstrap даёт ошибку (`Expected IDENTIFIER, got RBRACE`).
