# `match` во frontend

Снято чтением `compiler/frontend`. Файлы frontend не менялись.

Выражение `match` — узел `ExprMatch(Shared<Expr>, [Shared<MatchArm>], Span)` в `compiler/frontend/ast.mlc`. Рукава — `MatchArm { pattern, has_guard, when_condition, body }`. Разбор: `parse_primary_match` вызывает `parse_match_expr` в `compiler/frontend/parser/exprs.mlc`. Скобки рукавов: `{ ... }`, `do ... end` или ведущая `|`.

Паттерн рукава читает `parse_or_pattern` → `parse_pattern`. Сумма `Pattern` объявлена в `compiler/frontend/ast.mlc`.

## Литерал `i32`

Узел `PatternInt(i32, Span)`.

`parse_pattern` на токене целого вызывает `parse_pattern_integer`. Пример: текст `1` даёт `PatternInt(1, span)`.

## Литерал `bool`

Узел `PatternBool(bool, Span)`.

`parse_pattern` на `true` и `false` вызывает `parse_pattern_boolean`. Пример: текст `true` даёт `PatternBool(true, span)`.

## Литерал строки

Узел `PatternStringLit(string, Span)`.

`parse_pattern` на строковом токене вызывает `parse_pattern_string`. Пример: текст `"ok"` даёт `PatternStringLit("ok", span)`.

Вариант `PatternStr(string, Span)` есть в сумме `Pattern`, но `parse_pattern` его не строит.

## Wildcard `_`

Узел `PatternWild(Span)`.

`parse_pattern_identifier_branch`: имя `_` даёт `PatternWild`. Токен `else` в `parse_pattern` тоже даёт `PatternWild`, без ошибки разбора. Неожиданный токен даёт `PatternWild` и ошибку `parse: unexpected token in pattern` (`parse_pattern_fallback_wildcard`).

## Биндинг переменной

Узел `PatternIdent(string, Span)`.

Имя, у которого первый символ не в `A`–`Z` и которое не `_`, даёт `PatternIdent`. Предикат конструктора — `is_ctor_name` в `compiler/frontend/parser/predicates.mlc`. Пример: текст `value` даёт `PatternIdent("value", span)`.

## Деструктуризация записи

Узел `PatternRecord(string, [Shared<Pattern>], Span)`.

Имя конструктора и `{ ... }` в `parse_pattern_identifier_branch` вызывает `parse_record_pattern_fields`. Пример: текст `Point { x, y }` даёт `PatternRecord("Point", [PatternIdent("x", span), PatternIdent("y", span)], span)`.

Поля — только `PatternIdent` по имени. Тип `FieldPattern` объявлен в `compiler/frontend/ast.mlc`, `parse_record_pattern_fields` его не строит. Вложенный паттерн поля (`x: 1`) парсер не кладёт в узел: после имени поля он идёт дальше до `}`.

`let { x }` вне `match` тоже строит `PatternRecord`, с пустым именем типа. Это ветка `parse_statement_let_mutable_body`, не `parse_pattern`.

## Конструктор

Узел `PatternCtor(string, [Shared<Pattern>], Span)`.

Имя с первой буквы `A`–`Z`. Без `(` и `{` — пустой список вложенных паттернов. С `(` — вложенные паттерны через `parse_pattern`. Пример: текст `Ok(value)` даёт `PatternCtor("Ok", [PatternIdent("value", span)], span)`.

## Другие формы, которые читает `parse_pattern`

- `PatternOr([Shared<Pattern>], Span)` — `parse_or_pattern`, разделитель `|`. Один альтернативный паттерн остаётся без `PatternOr`.
- `PatternArray([Shared<Pattern>], string, Span)` — `parse_array_pattern`. Второй аргумент — имя после `...`, либо пустая строка.

`parse_pattern` не читает кортеж. `PatternTuple` строит только `let` в `parse_statement_let_mutable_body`. `PatternUnit` в разборе `match` не строится.
