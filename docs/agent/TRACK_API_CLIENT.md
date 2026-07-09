# Track: API-клиенты (derive Json + OpenAPI codegen)

Parent: [../PLAN.md](../PLAN.md), [../API_CLIENT.md](../API_CLIENT.md)
(полная спецификация — читать перед началом, там же факт-таблица и открытые
конвенции §3).

Приоритет: **средний** — часть `STDLIB_BACKEND.md`, но не зависит от
`FFI_LAYER` (JSON/HTTP уже есть, `derive` — существующий языковой механизм).
Может стартовать независимо от очереди FFI/concurrency.

## Status: **open** — STEP=3 done; STEP=4 next

**Driver 2026-07-09:** STEP=3 — sum `derive { Json }` tagged (`"Active"` / `{tag,value}` / `{tag,fields}`);
STEP=2 records done earlier.

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | Исправить рассинхронизацию MLC-типа JSON и C++ runtime (`API_CLIENT.md` §2): `JsonNumber(f32)`→`f64`, `JsonObject(str, JsonValue)`→`JsonObject(Map<str, JsonValue>)` в `lib/mlc/common/stdlib/data/json.mlc`. Ruby-бутстрап первым, self-hosted не отстаёт (там же используется `extern`-декларация, без отдельного codegen). Регрессия: `test/mlc/` на JSON, если есть. | **done** |
| 2 | `JsonError` тип (`MissingField`/`TypeMismatch`/…) + `derive { Json }` для record-типов в Ruby-бутстрапе: `lib/mlc/registries/` (список derivable traits) + `lib/mlc/backends/cpp/codegen.rb` (генерация `to_json`/`from_json`). Тест: round-trip на типе с `i64`/`string`/`Option<T>`/`Array<T>` полями. | **done** |
| 3 | `derive { Json }` для sum-типов — решить tagged-representation конвенцию (`API_CLIENT.md` §3), реализовать в Ruby. Тест: round-trip на каждом варианте. | **done** |
| 4 | Self-hosted: `derive { Json }` в `compiler/checker/check/derive_validation.mlc` (добавить `"Json"`) + `compiler/codegen/decl.mlc`. Self-host verify gate. | pending |
| 5 | OpenAPI codegen — Ruby-скрипт (не часть `mlcc`), вход `openapi.yaml`, выход `.mlc` (types + client fns). MVP: `object`/`array`/примитивы + простой `oneOf`. Тест на публичной Petstore-спеке или аналоге. | pending |
| 6 | Verify-gate + close: self-host (`mlcc`→`mlcc2`→`diff`), `regression_gate.sh`, критерий приёмки `API_CLIENT.md` §8. | pending |

## Out of scope (см. `API_CLIENT.md` §6)

- Настоящая async-интеграция `fetch()` — отдельная задача.
- gRPC/protobuf, GraphQL-клиент — отдельные будущие треки.
- Полное покрытие OpenAPI 3.1 — минимальное практичное подмножество.

## Per-turn template

```
| step | <1-6> |
| done | <one line> |
| verify | <числа/факт> |
| next | ROLE=Driver STEP=<n+1> TRACK_API_CLIENT |
```
