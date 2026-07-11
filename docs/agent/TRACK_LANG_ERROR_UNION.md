# Track: `T!E` error-union sugar над `Result<T,E>`

Parent: [../PLAN.md](../PLAN.md) §17 ("Zig — Error unions", применимо).
Предшественник: [../archive/tracks/TRACK_LANG_RESULT_COMBINATORS.md](../archive/tracks/TRACK_LANG_RESULT_COMBINATORS.md)
(closed 2026-07-09 — `map`/`and_then`/`map_err`, `?` с `From`-конвертацией).

## Status: **active** — STEP=5 next (MLC.md + verify-gate + close)

**Driver 2026-07-11 STEP=4:** E2E `with_question.mlc` (T!E + `?`) compile/link/run
stdout `6` / `division by zero`; smoke extended; Ruby tests +`?`/to_cpp (6/0).

**Driver 2026-07-11 STEP=3:** Self-hosted `types.mlc` —
`apply_error_union_sugar` after `parse_base_type`, before `->`; smoke
`run_error_union_sugar_smoke.sh` ok; tests in `test_parser.mlc`.

**Driver 2026-07-11 STEP=2:** Ruby `type_parser.rb` — `T!E` →
`GenericType(Result, [T,E])` via `apply_error_union_sugar` (after generics,
before `[]`; E via `allow_error_union: false`). Tests
`test/mlc/error_union_sugar_test.rb` 4/0.

**Driver 2026-07-11 STEP=1:** Decision **locked** (table below). Lexer `!=`/`!`
OK; unary `!` = expr-only (`parse_unary` → `ExprUn`); `?` = expr postfix.
Type postfix `!` after base, before function-type `->`.

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

## Decision (STEP=1) — **locked 2026-07-11**

| Вопрос | Locked |
|--------|--------|
| Новый AST-узел | **нет** — сразу `TyGeneric("Result", [T, E])` / Ruby `GenericType` `Result` |
| Где в грамматике | postfix после `parse_base_type`, **до** правила `T -> U` (`TyFn`) |
| Позиции | любой type-context (ret/field/param/let) — тот же сахар; docs фокус на `-> T!E` |
| Правая часть `E` | один `parse_base_type` (без второго `!`); `T!` без `E` → parse error |
| Унарный `!` | только exprs (`exprs.mlc` `parse_unary`); type `!` не пересекается |
| `?` | только exprs (`ExprQuestion`); без конфликта |
| Лексер | `!=` vs `!` уже разделены (`lexer.mlc:430-431`) |
| Inferred `T!` | **out of scope** (нужен явный `E`) |
| Checker/codegen | **без изменений** |

Precedence: `-> (i32) -> i32!E` = `TyFn([i32], Result<i32,E>)` (bang binds
tighter than arrow). `i32!E -> bool` = `TyFn(Result<i32,E>, bool)`.

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | Design: грамматика / конфликты `?` / унарный `!`. | **done** |
| 2 | Парсер (Ruby): `T!E` → десугаринг в `Result<T, E>` на этапе парсинга. | **done** |
| 3 | Парсер (self-hosted, `compiler/frontend/parser/`): то же самое, после верификации на Ruby. | **done** |
| 4 | Тесты: unit-тест десугаринга, e2e на функции с `T!E`, включая совместимость с `?`. | **done** |
| 5 | Документация: `MLC.md` — `T!E` алиас; verify-gate + close. | pending |

<!-- STEP=2 sub-steps: 1) `type_parser.rb` after base, before ARROW: if `!` then parse E + GenericType Result; 2) unit test parse ret/field; 3) no new AST node -->
<!-- STEP=3 sub-steps: 1) `types.mlc` mirror after `parse_base_type`; 2) smoke `mlcc --check-only`; 3) parity fixtures -->
<!-- STEP=4 sub-steps: 1) desugar unit; 2) e2e `T!E` + `?`; 3) `T!` without E → parse error -->
<!-- STEP=5 sub-steps: 1) MLC.md; 2) REG 20/0; 3) self-host DIFF 0; archive → Critic -->

## Токенизация — проверено, не блокер

`compiler/frontend/lexer.mlc:430-431` уже различает `!=` (двухсимвольный
`Op("!=")`) и одиночный `!` (`Op("!")` через `scan_op_single`) на уровне
лексера — конфликта токенов нет.

## Out of scope

- Произвольные error-union без явного `E` (`T!` в стиле Zig "inferred error
  set") — не переносится осмысленно на nominal-типизацию MLC (`Result<T,E>`
  требует конкретный `E`), не рассматривать.
- Mass-annotate remaining stdlib `W-EXTERN-ATTR` (FFI_SAFETY residual) —
  отдельный hygiene, не этот трек.
- `#line` / DEBUG_SOURCE_MAP — остаётся queued низкий приоритет.
