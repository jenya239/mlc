# Float во frontend

Снято чтением `compiler/frontend`. Файлы frontend не менялись.

## Литерал

Токен `LFloat(string)` — `compiler/frontend/ast_tokens.mlc`, вариант `TokenKind`.

Узел `ExprFloat(string, Span)` — `compiler/frontend/ast.mlc`, вариант `Expr`. Строка — текст литерала, не разобранное число.

Пример. Текст `1.5` даёт токен `LFloat("1.5")`, затем узел `ExprFloat("1.5", span)`.

Лексер: `scan_int` в `compiler/frontend/lexer.mlc`. Точка и цифра после целой части переключают разбор на float. Суффикс после точки читается `try_scan_suffix` и отбрасывается: в `LFloat` попадает только текст до суффикса. Список суффиксов там же содержит и `f64`, и `f32`.

Целое с суффиксом `f32` или `f64` и без точки остаётся `LInt`, не `LFloat`. Ветка `match suffix` в `scan_int` не называет `f32` и `f64`, срабатывает ветка по умолчанию `LInt`.

Разбор выражения: `parse_primary` в `compiler/frontend/parser/exprs.mlc`. Предикат `TokenKind.is_float` истинен для `LFloat`. Узел строится как `ExprFloat(kind.float_val(), source_span)`. Предикаты `is_float` и `float_val` — `compiler/frontend/parser/predicates.mlc`.

## Бинарные операции

Отдельного узла float-арифметики нет. И целые, и float попадают в один узел `ExprBin(string, Shared<Expr>, Shared<Expr>, Span)` — `compiler/frontend/ast.mlc`. Первый аргумент — текст оператора.

Пример. Текст `1.5 + 2.0` даёт `ExprBin("+", ExprFloat("1.5", span), ExprFloat("2.0", span), span)`.

Операторы, которые `compiler/frontend/parser/exprs.mlc` кладёт в `ExprBin`:

- `parse_add`: `+`, `-`
- `parse_mul`: `*`, `/`, `%`
- `parse_equality`: `==`, `!=`
- `parse_comparison_relational`: `<`, `>`, `<=`, `>=`
- `parse_shift`: `<<`, `>>`
- побитовые: `|`, `^`, `&`
- логические: `&&`, `||`

Унарный минус перед литералом — не `ExprBin`, а `ExprUn("-", ExprFloat(...), span)`. `parse_unary` в том же файле строит `ExprUn` для `!`, `-`, `~`, `+`.

## Тип `f32` / `f64`

Отдельных вариантов `TyF32` и `TyF64` нет. `TypeExpr` в `compiler/frontend/ast.mlc` содержит `TyI32`, `TyString`, `TyBool`, `TyUnit`, `TyNamed(string)`.

`parse_base_type_ident` в `compiler/frontend/parser/types.mlc` выделяет только `i32`, `string`, `bool`, `unit`, `ref`. Имя `f32` и имя `f64` проходят в `parse_type_after_ident_name` и становятся `TyNamed("f32")` и `TyNamed("f64")`.

Оба написания `f32` и `f64` есть в списке суффиксов лексера. Ни одно из них не становится отдельным типом и не меняет вид токена `LFloat`.
