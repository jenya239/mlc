# Ruby ⇄ mlcc parity audit (2026-07-04)

Сырой отчёт `claude-opus-4-8` через `mlc-support/review_mlc2_parity.rb`
(бандл `lib/mlc/{source,representations,backends,registries}` +
`compiler/{frontend,checker,codegen}`, 873K input-токенов, $4.56).
Не read-only проверено на реальном компиляторе — только результат чтения
кода моделью. Статус верификации указан построчно ниже.

**Известный дефект**: в H4 и H12 часть текста повреждена (склейка кириллицы
на границе TCP-чанка при стриминге ответа — баг буферизации в
`review_mlc2_parity.rb`/`review_with_claude.rb`, исправлен постфактум,
на уже полученный ответ не влияет).

## Статус находок

| ID | Тема | Статус |
|----|------|--------|
| H1 (тут) | побитовые `&\|^<<>>` отсутствуют | **подтверждено на бинаре** → перенесено в `docs/mlc2.md` H1 |
| H2 (тут) | унарные `~`/`+` отсутствуют | **подтверждено на бинаре** → `docs/mlc2.md` H2 |
| H3 (тут) | диагностика вызова не-функции при `TUnknown` | не проверено, гипотеза |
| H4 (тут) | тернарник vs if-lambda, generic-инстанс полей | не проверено, гипотеза (текст повреждён) |
| H5 (тут) | суффиксы числовых литералов неполные | **подтверждено на бинаре** → `docs/mlc2.md` H3 |
| H6 (тут) | `\0` в строках: `length()` vs `bytesize` | не проверено, узкий гипотетический кейс |
| H7 (тут) | постфиксный `if`/`unless` отсутствует | **подтверждено на бинаре** → `docs/mlc2.md` H4 |
| H8 (тут) | if-lambda вместо тернарника, ctor vs var | не проверено, cosmetic-класс |
| H9 (тут) | пустой `[]` вне coercion-путей | не проверено, узкий гипотетический кейс |
| H10 (тут) | приоритет пайпа относительно сравнений | ложный сигнал — модель сама себя опровергла, паритет соблюдён |
| H11 (тут) | generic-аргументы для не-Ok/Err конструкторов | не проверено, гипотеза |
| H12 (тут) | trait-as-param: пустые trait maps, невалидный C++ | **подтверждено на бинаре** → `docs/mlc2.md` H12 |

Верификация — компиляция минимального MLC-сниппета параллельно через
`compiler/out/mlcc` и Ruby bootstrap (`ModularCompiler`), сравнение
поведения. Методика и точные ссылки на строки — см. `docs/mlc2.md` H1-H4.

---

## Сырой отчёт модели

## H1 — приоритет побитовых операторов `&`, `|`, `^`, `<<`, `>>`

### Эталон (Ruby)

Полная лестница приоритетов в `lib/mlc/source/parser/expression_parser.rb`:
`parse_logical_or` → `parse_logical_and` → `parse_bitwise_or` (`|`) →
`parse_bitwise_xor` (`^`) → `parse_bitwise_and` (`&`) → `parse_equality`
(`==`, `!=`) → `parse_pipe` → `parse_comparison` (`<`, `>`, `<=`, `>=`) →
`parse_shift` (`<<`, `>>`) → `parse_addition` → `parse_multiplication`. То есть
`&`/`|`/`^`/`<<`/`>>` — отдельные уровни с корректным взаимным приоритетом.

### Self-hosted (mlcc)

В `compiler/frontend/parser/exprs.mlc` лестница обрывается: `parse_or` (`||`)
→ `parse_and` (`&&`) → `parse_equality` → `parse_pipe` →
`parse_comparison_relational` → `parse_add` → `parse_mul` → `parse_unary`. Ни
`parse_bitwise_or/xor/and`, ни `parse_shift` вообще нет. Лексер выдаёт `<<`,
`>>` как `Op("<<")`/`Op(">>")`, а `&`, `|`, `^` — как `Bar`/`Op("&")`/`Op("^")`,
но парсер выражений их не потребляет ни на одном уровне (`&&`/`||` матчатся
через `op_val()`, одиночные `&`/`|`/`^`/`<<`/`>>` — нет).

### Расхождение

**missing-feature** (частично **compile-error**): любое выражение с побитовыми
операторами или сдвигами (`a & b`, `x | y`, `n << 2`, `h ^ k`) в self-hosted не
парсится — `&`/`^`/`<<`/`>>` остаются непотреблёнными токенами, `|` трактуется
как разделитель ветки/типа. Rakefile-код чекера сам избегает побитовых операций
(derive Hash в mlcc эмитит C++-строку напрямую), поэтому bootstrap проходит, но
любой пользовательский MLC-код с `&`/`|`/`^`/`<<`/`>>` компилируется Ruby и
отвергается self-hosted. Требует проверки на реальном компиляторе.

## H2 — оператор `~` (побитовое НЕ) в унарной позиции

### Эталон (Ruby)

`lib/mlc/source/parser/lexer.rb` (`tokenize_operator`) выдаёт `~` как
`OPERATOR("~")`, а `UnaryRule` (`lib/mlc/backends/cpp/rules/expressions/unary_rule.rb`)
и парсер (`parse_unary` в `expression_parser.rb` матчит `!`, `-`, `+`, `~`)
поддерживают унарный `~`.

### Self-hosted (mlcc)

`compiler/frontend/lexer.mlc` (`scan_op_single`) отдаёт `~` как `Op("~")`, но
`parse_unary` в `compiler/frontend/parser/exprs.mlc` матчит только `!` и `-`:
`if kind.is_op() && (kind.op_val() == "!" || kind.op_val() == "-")`. Токен `~`
проваливается в `parse_postfix`/`parse_primary`, где нет ветки для `Op(_)` →
`parse_primary_unit_fallback` (запись ошибки парсинга).

### Расхождение

**compile-error / missing-feature**: `~x` в Ruby компилируется, в mlcc даёт
ошибку парсинга. Также в mlcc отсутствует унарный `+` (Ruby принимает `+x`;
`parse_unary` матчит только `!`/`-`). Требует проверки на реальном компиляторе.

## H3 — сообщение об ошибке при вызове не-функции

### Эталон (Ruby)

`lib/mlc/backends/cpp/rules/expressions/call_rule.rb` / инференс: при вызове
значения, тип которого не функция и не unknown, Ruby формирует ошибку через
семантический слой; конкретный текст в mlcc — `'called value is not a
function (got <type>)'` c кодом E002 (`infer_expr_call_non_constructor_not_callable`).

### Self-hosted (mlcc)

В `compiler/checker/infer/infer_call_support.mlc`
(`infer_expr_call_non_constructor_not_callable`) реализовано:
`'called value is not a function (got ' + type_description(...) + ')'`, код E002.
Но при `type_is_unknown(function_parsed.inferred_type)` → возвращается `base`
без ошибки, а тип результата берётся из `return_type_from_inferred_function_type`,
который для не-функции даёт `TUnknown`.

### Расхождение

**требует проверки на реальном компиляторе**: если callee имеет тип `TUnknown`
(например при неразрешённом имени или ошибке вывода выше), self-hosted молча
пропускает проверку "не функция" и продолжает с `TUnknown`, тогда как Ruby при
неизвестном callee тоже часто подавляет вторичные ошибки — совпадение вероятно,
но текст и точный триггер (например вызов поля-строки `obj.name()` без метода)
могут расходиться. Класс — potential compile-error/missing-diagnostic.

## H4 — тернарник vs if-lambda при разных конструкторах в ветках `if`

### Эталон (Ruby)

`lib/mlc/backends/cpp/rules/expressions/if_rule.rb`: если тип результата —
sum/generic вариант (`variant_result_type?`), обе ветки оборачиваются в
`wrap_in_type(then/else, sum_type_name)`, то есть `SumType(then) : SumType(else)`
в тернарнике — приведение к общему типу вручную. Для не-вариантных типов —
обычный тернарник; пустые массивы `{}` в ветке заменяются на `else_type{}`.

### Self-hosted (mlcc)

`compiler/expr_visitor_cpp.mlc` (`gen_if_via_cpp_visitor`, `if_ternary_is_safe`,
`if_branch_constructor_name`): mlcc **не** оборачивает ветки в общий sum-тип, а
вместо этого детектирует небезопасный тернарник (разные имена конструкторов или
разные inferred C++-типы веток) и в этом случае падает на if/else-лямбду с
явным return-типом. Комментарий в коде прямо описывает `LChar{x}` vs `LStr{x}`.

### Расхождение

**silent-wrong-codegen (риск)** [текст повреждён стриминг-багом, восстановлен
по смыслу]: Ruby приводит каждую ветку к sum-типу и сохраняет тернарник; mlcc
либо тернарник (если ветки одинакового конструктора/типа), либо if-lambda.
Краевой случай: обе ветки — **один** конструктор, но с полями разного
generic-инстанса (например `if c then Ok(a) else Ok(b)` где `a: Foo<i32>`,
`b: Foo<i64>`) — mlcc считает `if_ternary_is_safe = true` (одинаковый ctor
name), но фактические C++-типы `Ok<Foo<int>>` и `Ok<Foo<int64_t>>` несовместимы
в тернарнике. Требует проверки на реальном компиляторе.

## H5 — суффиксы числовых литералов: набор и `f32`

### Эталон (Ruby)

`lib/mlc/source/parser/lexer.rb` (`NUMERIC_SUFFIXES`): `i8 i16 i32 i64 u8 u16
u32 u64 usize f32 f64`. Литералы вроде `1u16`, `5i8`, `2.0f32` лексируются как
`SUFFIXED_INT_LITERAL`/`SUFFIXED_FLOAT_LITERAL` и дают `TypedNumLit`.

### Self-hosted (mlcc)

`compiler/frontend/lexer.mlc` (`try_scan_suffix`): `numeric_type_suffixes =
['i64', 'u8', 'usize', 'f64', 'f32']`. То есть отсутствуют `i8`, `i16`, `i32`,
`u16`, `u32`, `u64`. Литерал `1i32` в mlcc: суффикс `i32` не распознан → откат,
`1` лексируется как `LInt(1)`, затем `i32` — отдельный `Ident("i32")`. Для `f32`
в `scan_int` суффикс обрабатывается только на float-ветке (после `.`); целочис­
ленный `f32` игнорируется молча (в `token_kind` матч на `i64`/`u8`/`usize`, `f32`
не покрыт → `LInt`).

### Расхождение

**compile-error / silent-wrong-codegen**: `1i32`, `10u32`, `3i8` и т.п. в Ruby —
типизированные литералы; в mlcc `i32`/`u32`/... становятся отдельным
идентификатором сразу после числа, что либо ломает парсинг (`5i8` → `5` затем
`i8` как имя), либо молча меняет семантику. Кроме того `SUFFIXED_INT_LITERAL`
для `f32`/`f64` в целой позиции (`1f64`) в mlcc не порождает float-литерал.
Требует проверки на реальном компиляторе.

## H6 — экранирование `\0` (null-байт) в строковых литералах

### Эталон (Ruby)

`lib/mlc/backends/cpp/services/utils/string_helpers.rb`
(`cpp_string_literal`): если строка содержит `\0`, эмитится
length-aware 2-арг форма `mlc::String("...", bytesize)`, чтобы null-байт не
обрезал C-строку. `escape_cpp_string` кодирует `\0` как `\\0`.

### Self-hosted (mlcc)

`compiler/codegen/expr/literals.mlc` (`gen_string_literal`):
`'mlc::String("' + escape_str(string_value) + '", ' + string_value.length()
.to_string() + ')'` — **всегда** двухаргументная форма с length. `escape_str`
(`compiler/codegen/cpp_naming.mlc`) кодирует `\0` как `\\0`. То есть mlcc
безусловно передаёт длину, а Ruby — только при наличии `\0`.

### Расхождение

**cosmetic → потенциально silent-wrong**: обычно совпадает по результату, но
mlcc использует **`length()`** (число codepoint/символов), а Ruby при null —
**`bytesize`**. Для многобайтовых UTF-8 строк с `\0` (гипотетически) длина в
символах ≠ длина в байтах → неверный размер `mlc::String`. Для ASCII совпадает.
Требует проверки на реальном компиляторе (крайне узкий кейс).

## H7 — `unless` с постфиксной формой и `else if`-цепочки

### Эталон (Ruby)

`lib/mlc/source/parser/expression_parser.rb`: поддержаны постфиксные
`expr if cond` и `expr unless cond` (`parse_postfix_conditional`, только если
if/unless на той же строке, что и выражение), а также `unless` как префиксная
форма (инверсия условия). `else if` разбирается как вложенный if.

### Self-hosted (mlcc)

`compiler/frontend/parser/exprs.mlc`: `parse_if_expr` поддерживает префиксный
`if`/`unless` (`is_negated` инвертирует условие через `ExprUn("!", ...)`) и
`else if`/`else unless` цепочки. Но **постфиксной** формы `expr if cond` /
`expr unless cond` нет — `if`/`unless` разбираются только в `parse_primary` как
начало выражения. Постфиксный if в mlcc не распознаётся.

### Расхождение

**missing-feature**: `foo() if condition` (statement-modifier) и
`x unless flag` в Ruby компилируются, в mlcc — `if`/`unless` после выражения не
потребляется парсером выражений (окажется отдельным примари-выражением или
ошибкой). Требует проверки на реальном компиляторе.

## H8 — точность `if_branch_type_matches_result` при вызовах-конструкторах

### Эталон (Ruby)

Ruby в `if_rule.rb` полагается на семантический тип узла if и оборачивает ветки
в sum-тип независимо от формы ветки (вызов, литерал записи и т.п.).

### Self-hosted (mlcc)

`compiler/expr_visitor_cpp.mlc`: `if_branch_type_matches_result` сравнивает
`sem_type_to_cpp(sexpr_type(branch))` с `result_cpp`. Но конструкторные вызовы
(`Ok(x)`, `LChar(x)`) в чекере инферятся к **owner**-типу (см. комментарий в
коде и `infer_call_support.mlc`), поэтому `if_branch_constructor_name`
добавлена как отдельная эвристика: разные имена ctor → небезопасно. Однако если
одна ветка — конструктор (`Ok(x)`, ctor name = "Ok"), а другая — **не вызов**, а
идентификатор того же Result-типа (`result_var`), то `then_ctor="Ok"`,
`else_ctor=""` → `then_ctor != else_ctor` (`"Ok" != ""`) → тернарник помечен
небезопасным → if-lambda.

### Расхождение

**cosmetic / потенциальный перегиб**: mlcc в случае «конструктор в одной ветке,
переменная того же типа в другой» без нужды падает на if-lambda вместо
тернарника, тогда как Ruby генерирует тернарник (или обёртку). Это не неверный
код, но лишний IIFE — отличие в форме сгенерированного C++. Может влиять на
constexpr-совместимость (IIFE не constexpr). Требует проверки на реальном
компиляторе; серьёзность — на грани cosmetic/silent-behavior-change.

## H9 — обработка пустого массива `[]` в возврате / бинарных операциях

### Эталон (Ruby)

Ruby в нескольких местах (`if_rule.rb`, block/lower_expression) резолвит пустой
`{}` через тип другой ветки или ожидаемый тип; `ArrayLiteralRule` для пустого
массива эмитит `{}` и полагается на вывод типа из контекста C++.

### Self-hosted (mlcc)

`compiler/expr_visitor_cpp.mlc` (`gen_array_via_cpp_visitor`,
`array_from_tarray_inner_type`): для пустого массива с элементом типа `auto`
эмитит `{}` (через `CppInitList`), иначе `mlc::Array<T>{}`. Coercion пустого
массива в ветках `if` реализован (`gen_if_else_for_array_else`,
`empty_array_inner_type_for_if_else`). Но пустой массив как **операнд бинарной
операции** или как **аргумент вызова** без явной аннотации coercion не проходит
через этот путь — берётся inferred тип из `SemanticExpressionArray`, который для
`[]` даёт `TArray(TUnknown)` → `mlc::Array<auto>` невалиден как явный тип,
эмитится `{}`.

### Расхождение

**требует проверки на реальном компиляторе**: базовые случаи (`let x: [i32] =
[]`, пустой массив в ветке if) покрыты обеими сторонами через coercion. Краевой
случай — пустой `[]` в позиции, где C++ CTAD/`{}` неоднозначен (например
аргумент перегруженной функции или элемент внешнего массива смешанного типа) —
может дать разный код. Гипотеза, узкий кейс.

## H10 — приоритет `parse_comparison_relational` относительно пайпа

### Эталон (Ruby)

Лестница Ruby: `parse_equality` (== !=) → **`parse_pipe`** → **`parse_comparison`**
(< > <= >=) → `parse_shift` → `parse_addition`. То есть `|>` **выше** сравнений
`< > <= >=`, но **ниже** равенства `== !=` — как задокументировано в E5.

### Self-hosted (mlcc)

`compiler/frontend/parser/exprs.mlc`: `parse_equality` → **`parse_pipe`** →
**`parse_comparison_relational`** (< > <= >=) → `parse_add`. Порядок совпадает:
`parse_pipe` вызывает `parse_comparison_relational` как операнд, а
`parse_equality` вызывает `parse_pipe`. Совпадает с E5.

### Расхождение

**нет расхождения** (перепроверка E5): относительный порядок equality/pipe/
comparison идентичен. Однако в mlcc `parse_and`/`parse_or` (`&&`/`||`) стоят
**над** `parse_equality`, а в Ruby `parse_logical_or`/`parse_logical_and` — над
`parse_bitwise_or`, который над equality. Поскольку в mlcc bitwise-уровней нет
(см. H1), сама лестница логики/равенства/пайпа совпадает. Отмечено для полноты —
здесь **паритет соблюдён**, реальное расхождение только в отсутствии
bitwise/shift (H1).

## H11 — generic sum: конструктор, match, i32→i64 (было: Ok/Err CTAD)

**Статус (2026-07-06):** закрыто в mlcc; regression `generic_sum_ctor`, self-host p1→p2.

### Эталон (Ruby)

`lib/mlc/backends/cpp/rules/expressions/call_rule.rb`
(`maybe_add_variant_template_args`, `resolve_concrete_generic_type`): для вызова
вариантного конструктора Ruby вычисляет конкретный generic-тип результата
(в т.ч. подстановкой из `expected_return_type`) и добавляет явные template-
аргументы `Ctor<Args>{...}`, включая частичную инстанциацию только по полям
варианта.

### Self-hosted (mlcc)

- `expr_visitor_cpp.mlc`: `generic_sum_variant_ctor_type_argument` — Ok/Err из
  enclosing `Result`, произвольный generic sum из enclosing return **или** из
  типа вызова после coerce аргумента (`Opt<i64>` у `take(Some(42))`).
- `match_gen.mlc`: `Some<T>&` / unit-варианты в match.
- Checker: инстанцирование `TGeneric(Opt, [TI64])`, widening i32→i64.

### Расхождение

**нет** для покрытого контура (`Opt<T>`, `Some`/`None`, match, `Some(42)`→`Opt<i64>`).
Остаётся узкая numeric tower (не все пары целых).

## H12 — impl trait как тип параметра (trait-as-param) с несколькими use

### Эталон (Ruby)

Ruby десугарит trait-имя в позиции типа параметра через
`lib/mlc/representations/semantic/gen/services/trait_param_expand_ast.rb`
(`expand_function_declaration`), генерируя синтетический type-param
`__trait_param_N` с bound. Для каждого вхождения trait-имени — свежий параметр.

### Self-hosted (mlcc)

`compiler/checker/transform/trait_param_expand.mlc`
(`expand_type_expression_for_trait_param`) реализует то же: `__trait_param_${counter}`
с bound-строкой, рекурсивно в `TyArray`/`TyShared`/`TyGeneric`/`TyFn`. Использует
`trait_declaration_names` минус `nominal_type_declaration_names`.

### Расхождение

**закрыто** (`2d1d933b`, 2026-07-06). Реальная причина — не «двойная экспансия», а
`build_trait_nominal_maps`: мутация `Map` внутри closure терялась (захват по
значению), карты trait/type имён оставались пустыми, `expand_type_expression_for_trait_param`
не переписывал `Display` → `__trait_param_N`. Исправление: fold с явным
`Trait_nominal_maps_fold_state`, как в `trait_name_conflict_fold_step`.

Проверено: `fn f(x: Display) -> unit` → `template<typename __trait_param_0> requires Display<__trait_param_0> void f(__trait_param_0 x)`; линковка OK. Unit: `compiler/tests/test_trait_param_expand.mlc`.

Гипотеза про двойную экспансию (`build_registry` + `transform_load_items`) —
ложная для покрытого кейса: повторный проход идемпотентен (`TyNamed("__trait_param_0")`
не совпадает с trait-именем).

Regression gate (Ruby vs mlcc stdout) для trait-as-param **нет** — Ruby bootstrap
на этом сниппете не даёт рабочий C++ (пустой header).
