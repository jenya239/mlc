# Track: `T!E` error-union sugar над `Result<T,E>`

Parent: [../PLAN.md](../PLAN.md) §17 ("Zig — Error unions", применимо).
Предшественник: [../archive/tracks/TRACK_LANG_RESULT_COMBINATORS.md](../archive/tracks/TRACK_LANG_RESULT_COMBINATORS.md)
(closed 2026-07-09 — `map`/`and_then`/`map_err`, `?` с `From`-конвертацией).

## Status: **active** — STEP=1 next (Design: grammar `-> T!E` only)

**Planner 2026-07-11:** Activated after FFI_SAFETY Critic OK. Chose §17 over
§15 `DEBUG_SOURCE_MAP` (research until external MLC project) and design-only
PACKAGE/AUTO_CYCLE/CANVAS. Pure sugar, no deps.

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
| 1 | Design: точная грамматика — `T!E` только в позиции возвращаемого типа функции (`-> T!E`), не как тип поля/переменной (избежать конфликта с потенциальным будущим использованием `!` как оператора). Проверить отсутствие конфликта с существующим `?`-оператором и с унарным `!` (логическое "не"). | pending |
| 2 | Парсер (Ruby): `T!E` в позиции `-> ...` → десугаринг в `Result<T, E>` AST-узел на этапе парсинга (не отдельный AST-тип). | pending |
| 3 | Парсер (self-hosted, `compiler/frontend/parser/`): то же самое, после верификации на Ruby. | pending |
| 4 | Тесты: unit-тест десугаринга, e2e на функции с `T!E`, включая совместимость с `?`-оператором из `RESULT_COMBINATORS`. | pending |
| 5 | Документация: `MLC.md` — добавить `T!E` как алиас-нотацию, явно отметить, что это не новый тип, семантика идентична `Result<T,E>`. Verify-gate + close. | pending |

<!-- STEP=1 sub-steps: 1) cite lexer `!=` vs `!` (already OK); 2) confirm unary `!` exists in exprs (ExprUn) — return-type grammar must not steal it; 3) lock: only after `->` in fn/method ret type, reject in field/let/param type; 4) write Decision table in this TRACK -->
<!-- STEP=2 sub-steps: 1) Ruby type parser after `->`; 2) build GenericType Result; 3) unit test parse; 4) no new AST node -->
<!-- STEP=3 sub-steps: 1) mirror in `compiler/frontend/parser/` type parse; 2) smoke `mlcc --check-only`; 3) parity with Ruby fixtures -->
<!-- STEP=4 sub-steps: 1) desugar unit; 2) e2e `T!E` + `?`; 3) negative: `T!E` in field → parse error -->
<!-- STEP=5 sub-steps: 1) MLC.md; 2) REG 20/0; 3) self-host DIFF 0; archive → Critic -->

## Токенизация — проверено, не блокер

`compiler/frontend/lexer.mlc:430-431` уже различает `!=` (двухсимвольный
`Op("!=")`) и одиночный `!` (`Op("!")` через `scan_op_single`) на уровне
лексера — конфликта токенов нет. Открытый вопрос STEP=1 — только
грамматика парсера типов (`T ! E` в позиции `-> ...`), не лексер.

## Out of scope

- Произвольные error-union без явного `E` (`T!` в стиле Zig "inferred error
  set") — не переносится осмысленно на nominal-типизацию MLC (`Result<T,E>`
  требует конкретный `E`), не рассматривать.
- Mass-annotate remaining stdlib `W-EXTERN-ATTR` (FFI_SAFETY residual) —
  отдельный hygiene, не этот трек.
- `#line` / DEBUG_SOURCE_MAP — остаётся queued низкий приоритет.
