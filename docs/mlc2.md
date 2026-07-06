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

## H1 — побитовые операторы `&`, `|`, `^`, `<<`, `>>`

**Статус:** исправлено mlcc (`30897587`): лестница `parse_bitwise_or/xor/and`, `parse_shift`; лексер `<<`/`>>`; checker — `i32` для побитовых ops.

### Эталон (Ruby)

- Полная лестница приоритетов в `lib/mlc/source/parser/expression_parser.rb`: `parse_logical_or` → `parse_logical_and` → `parse_bitwise_or` (`|`) → `parse_bitwise_xor` (`^`) → `parse_bitwise_and` (`&`) → `parse_equality` → `parse_pipe` → `parse_comparison` → `parse_shift` (`<<`, `>>`) → `parse_addition`.
- `5 & 3`, `5 | 2`, `5 ^ 1`, `1 << 3`, `16 >> 2` компилируются.

### Self-hosted (mlcc)

- `compiler/frontend/parser/exprs.mlc`: лестница обрывается на `parse_or` (`||`) → `parse_and` (`&&`) → `parse_equality` → `parse_pipe` → `parse_comparison_relational` → `parse_add` → `parse_mul` → `parse_unary`. Уровней `parse_bitwise_or/xor/and` и `parse_shift` нет.
- Лексер выдаёт `&`, `|`, `^`, `<<`, `>>` как отдельные операторные токены, но парсер выражений их не потребляет ни на одном уровне.
- **Проверено на бинаре** (2026-07-04): `5 & 3` и т.п. дают каскад ошибок E008/E005 (misparse), не понятную диагностику отсутствия фичи.

### Файлы для правок

| Назначение | Путь |
|------------|------|
| Лестница приоритетов | `compiler/frontend/parser/exprs.mlc` |
| Лексер (токены уже есть) | `compiler/frontend/lexer.mlc` |
| Codegen бинарных операторов | `compiler/codegen/expr_eval.mlc` (проверить обработку `&`/`\|`/`^`/`<<`/`>>`) |

---

## H2 — унарные `~` и `+`

**Статус:** исправлено mlcc (`898b4915`): `parse_unary` принимает `~` и `+`.

### Эталон (Ruby)

- `lib/mlc/source/parser/lexer.rb` выдаёт `~` как `OPERATOR("~")`; `parse_unary` в `expression_parser.rb` матчит `!`, `-`, `+`, `~`.

### Self-hosted (mlcc)

- `compiler/frontend/parser/exprs.mlc:507-516` (`parse_unary`): матчит только `!` и `-` (`kind.op_val() == "!" || kind.op_val() == "-"`). Токены `~x` и `+x` проваливаются в `parse_postfix`/`parse_primary` и дают ошибку разбора.
- **Проверено на бинаре** (2026-07-04): `~5` и `+3` не компилируются mlcc, Ruby — компилирует оба.

### Файлы для правок

| Назначение | Путь |
|------------|------|
| `parse_unary` | `compiler/frontend/parser/exprs.mlc:507` |
| Codegen унарных операторов | `compiler/codegen/expr_eval.mlc` |

---

## H3 — суффиксы числовых литералов

**Статус:** исправлено mlcc (`65da5390`): полный набор суффиксов Ruby; узкие signed/unsigned → `LInt`; fix `"i32"` в `types.mlc`.

### Эталон (Ruby)

- `lib/mlc/source/parser/lexer.rb` (`NUMERIC_SUFFIXES`): `i8 i16 i32 i64 u8 u16 u32 u64 usize f32 f64` — полный набор.

### Self-hosted (mlcc)

- `compiler/frontend/lexer.mlc:144` (`try_scan_suffix`): `numeric_type_suffixes = ['i64', 'u8', 'usize', 'f64', 'f32']` — отсутствуют `i8`, `i16`, `i32`, `u16`, `u32`, `u64`.
- **Проверено на бинаре** (2026-07-04): `5i32` в mlcc даёт `error[E001]: undefined: i32` — суффикс не распознан, откатывается к `LInt(5)` + отдельный идентификатор `i32`. Ruby компилирует `5i32` как типизированный литерал.

### Файлы для правок

| Назначение | Путь |
|------------|------|
| Список суффиксов | `compiler/frontend/lexer.mlc:144` (`try_scan_suffix`) |
| Матч по суффиксу → `LI8`/`LI16`/... | `compiler/frontend/lexer.mlc` (ветка `token_kind` после `try_scan_suffix`, см. `compiler/frontend/lexer.mlc:187`) |

---

## H4 — постфиксный `if`/`unless` (statement modifier)

**Статус:** исправлено mlcc (`ae2cb1fb`): `parse_postfix_conditional` после postfix-цепочки, same-line check.

### Эталон (Ruby)

- `lib/mlc/source/parser/expression_parser.rb` (`parse_postfix_conditional`): `expr if cond` и `expr unless cond` на той же строке — валидный синтаксис, инверсия условия для `unless`.

### Self-hosted (mlcc)

- `compiler/frontend/parser/exprs.mlc` (`parse_if_expr`, начиная со строки 854): `if`/`unless` разбираются только в **префиксной** позиции (внутри `parse_primary`), постфиксной обёртки после произвольного выражения нет.
- **Проверено на бинаре** (2026-07-04): `println("hi") if true` внутри `do…end`-блока не просто не типизируется — ломает разбор последующих деклараций (`fn main` перестаёт распознаваться, каскад "undefined: main"). Ruby даёт чистую (и ожидаемую в этом тесте) семантическую ошибку типов, не затрагивая остальной файл.

### Файлы для правок

| Назначение | Путь |
|------------|------|
| Постфиксная форма `if`/`unless` | `compiler/frontend/parser/exprs.mlc` (рядом с `parse_if_expr`, точка встраивания — там же, где в Ruby `parse_postfix_conditional` оборачивает результат `parse_expression`) |

---

## H11 — generic sum: конструктор, match, i32→i64

**Статус:** закрыто mlcc (`f5644038`, `e5cfe230`, `bd438e7e`, `de106b45`); regression `specs/regression/programs/generic_sum_ctor.mlc`.

### Эталон (Ruby)

- Вызов вариантного конструктора generic sum-типа: явные template-аргументы `Ctor<Args>{…}` из ожидаемого типа (return, аргумент вызова, подстановка полей варианта).
- Числовой литерал `42` (i32) в позиции `i64` — допустим при присвоении/аргументе (Ruby codegen эмитит `Some<int64_t>{x}`).

### Self-hosted (mlcc)

- **Checker:** `infer_call.mlc` — `instantiated_algebraic_constructor_type` после `build_call_type_substitution`; `types_assignment_compatible` / `types_compatible_for_call_argument` — i32→i64 для `Opt<i64>` и `take(Some(42))`.
- **Codegen ctor:** `expr_visitor_cpp.mlc` — `generic_sum_variant_ctor_type_argument` из `enclosing_function_return_type` или из типа вызова после coerce; `transform.mlc` — `SemanticExpressionCall` принимает ожидаемый `TGeneric` у аргумента.
- **Codegen match:** `match_gen.mlc` — `Some<int64_t>&` / `None` без лишнего шаблона; template args из типа scrutinee.
- **Registry index:** `type_index.mlc` — `build_generic_variants_from_decls` (накопление массива).

### Ограничения

- Расширение целых: i32→i64 (и char→i32/i64), не полная numeric tower.
- `variant_used_type_parameter_names` для `Some(T)` может быть пустым — fallback по типу scrutinee / enclosing return / coerce аргумента.

### Файлы

| Назначение | Путь |
|------------|------|
| Ctor + template suffix | `compiler/expr_visitor_cpp.mlc` |
| Match arms | `compiler/codegen/expr/match_gen.mlc` |
| Checker unify / return | `compiler/checker/semantic_type_structure.mlc`, `call_argument_unify.mlc`, `infer_call.mlc` |
| Coerce call arg | `compiler/checker/transform/transform.mlc` |

---

## H12 — trait-as-param (`fn f(x: Display)`)

**Статус:** закрыто mlcc (`2d1d933b`, `61e3835b`); unit `test_trait_param_expand.mlc`; e2e `trait_as_param.mlc`, `trait_e2e.mlc`.

### Эталон (Ruby)

- `trait_param_expand_ast.rb`: trait-имя в типе параметра → синтетический `__trait_param_N` + bound; codegen: `template` + `requires Trait<__trait_param_N>`.

### Self-hosted (mlcc)

- **Корень бага:** `build_trait_nominal_maps` — пустые `trait_declaration_names` из‑за side effect в closure; expand не срабатывал.
- **Исправление:** `trait_nominal_maps_fold_step` + `program.decls.fold` в `trait_param_expand.mlc`.
- **Codegen:** `decl_cpp.mlc` — trait suffix (`Display_to_string`, static_assert) в header; forward decl перед concept; `f(42)` / `id(42)` линкуются.

### Ограничения

- Два параметра `Display` → `__trait_param_0`, `__trait_param_1` (покрыто `test_codegen.mlc` D3).
- Паритет с Ruby bootstrap на compile+link не в regression gate.

### Файлы

| Назначение | Путь |
|------------|------|
| Expand trait-as-param | `compiler/checker/transform/trait_param_expand.mlc` |
| Transform pipeline | `compiler/checker/transform/transform_decl.mlc`, `registry.mlc` |
| C++ proto/def | `compiler/codegen/decl_cpp.mlc` |

---

## Parity audit (2026-07)

Сводка `docs/PARITY_AUDIT_2026_07.md`: H1–H5, H7, H11 — исправлено в mlcc;
H3, H4, H8, H9, H10, H12 — проверено на бинаре; H6 — ASCII OK, UTF-8+null
deferred. Regression gate: 16 программ stdout. e2e mlcc: 9 программ
(compile+link+run). Ruby: **trait-декларации не кодогенятся** (пустой cpp,
pre-existing); trait regression в gate **нет**. Следующий трек: Ruby trait
codegen, не mlcc.

---

## Прочее

- В импортах MLC **не ставить завершающую запятую** в списке символов в фигурных скобках `{ … }` — Ruby-парсер bootstrap даёт ошибку (`Expected IDENTIFIER, got RBRACE`).
