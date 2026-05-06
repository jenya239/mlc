# Заметки по языку и компиляторам (mlc2)

Дополнение к `docs/MLC.md`: решения по паритету Ruby ⇄ mlcc и ограничения текущей реализации. Обновлять при изменении чекера или кодогена.

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

## Прочее

- В импортах MLC **не ставить завершающую запятую** в списке символов в фигурных скобках `{ … }` — Ruby-парсер bootstrap даёт ошибку (`Expected IDENTIFIER, got RBRACE`).
