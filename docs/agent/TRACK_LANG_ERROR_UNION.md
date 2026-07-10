# Track: `T!E` error-union sugar над `Result<T,E>`

Parent: [../PLAN.md](../PLAN.md) §10 ("Zig — Error unions", "применимо").
Предшественник: [../archive/tracks/TRACK_LANG_RESULT_COMBINATORS.md](../archive/tracks/TRACK_LANG_RESULT_COMBINATORS.md)
(closed 2026-07-09 — `map`/`and_then`/`map_err`, `?` с `From`-конвертацией).

## Status: **open, низкий приоритет, чистый сахар** — не блокирует и не
блокируется ничем; можно взять в любой момент без зависимостей.

## Идея

Zig: `fn divide(a: i32, b: i32) i32!DivError`. В MLC сегодня то же самое
пишется явно: `fn divide(a: i32, b: i32) -> Result<i32, DivError>`. `T!E` —
чистая нотация сверху существующего `Result<T,E>`, без нового runtime-
представления.

```mlc
fn divide(a: i32, b: i32) -> i32!DivError = if b == 0 then Err(DivError.Zero) else Ok(a / b)
```

раскрывается в парсере в `Result<i32, DivError>` до попадания в checker —
**чисто синтаксический сахар**, ноль изменений в SemanticIR/codegen.

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | Design: точная грамматика — `T!E` только в позиции возвращаемого типа функции (`-> T!E`), не как тип поля/переменной (избежать конфликта с потенциальным будущим использованием `!` как оператора). Проверить отсутствие конфликта с существующим `?`-оператором и с унарным `!` (логическое "не", если есть в языке — проверить `MLC.md`). | pending |
| 2 | Парсер (Ruby): `T!E` в позиции `-> ...` → десугаринг в `Result<T, E>` AST-узел на этапе парсинга (не отдельный AST-тип). | pending |
| 3 | Парсер (self-hosted, `compiler/frontend/parser/`): то же самое, после верификации на Ruby. | pending |
| 4 | Тесты: unit-тест десугаринга, e2e на функции с `T!E`, включая совместимость с `?`-оператором из `RESULT_COMBINATORS`. | pending |
| 5 | Документация: `MLC.md` — добавить `T!E` как алиас-нотацию, явно отметить, что это не новый тип, семантика идентична `Result<T,E>`. Verify-gate + close. | pending |

## Токенизация — проверено, не блокер

`compiler/frontend/lexer.mlc:430-431` уже различает `!=` (двухсимвольный
`Op("!=")`) и одиночный `!` (`Op("!")` через `scan_op_single`) на уровне
лексера — конфликта токенов нет. Открытый вопрос STEP=1 — только
грамматика парсера типов (`T ! E` в позиции `-> ...`), не лексер.

## Out of scope

- Произвольные error-union без явного `E` (`T!` в стиле Zig "inferred error
  set") — не переносится осмысленно на nominal-типизацию MLC (`Result<T,E>`
  требует конкретный `E`), не рассматривать.
