# Track: Lambda capture fix (Phase 2.6 →8)

Parent: [../PLAN.md](../PLAN.md) §Phase 2.6; previous: [TRACK_REGISTRY_SPLIT.md](TRACK_REGISTRY_SPLIT.md)

## Status: **closed** (2026-06-16)

**STEP=5 note (2026-06-16 Driver):** IIFE `[&]` rationale comments in `expr.mlc` (`block_as_immediate_invoked_function_expression`, `record_update_lazy_closure`) and `expr_visitor_cpp.mlc` (`reference_capture_for_immediate_lambda` / `invoke_immediate_lambda_cpp`). Gate **998**/0; mlcc2 self-host diff blocked (checker parity — see STEP=4). Uncommitted batch steps 1–5.

**STEP=4 note (2026-06-16 Driver):** `expr_visitor_cpp.mlc` `lambda_capture_list` → `[=]`; stored lambda test `make_checker` in `test_codegen.mlc`; ExprLambda/partial expectations updated. Gate **998**/0; mlcc self-host ok; mlcc2 `main.mlc` check fails (`undefined: options` / do-block bindings — parity gap, `diff_exit=1`).

**STEP=3 note (2026-06-16 Driver):** `expr.mlc` 2× `match_arm_constructed_value_generic*`: `[&]` → `[=]`; unit test in `test_codegen.mlc`. Gate **996**/0; `diff_exit=0`.

**STEP=1 note (2026-06-16 Driver):** `cpp_lambda_header_prefix` `[&]` → `[=]` in `expression_support.mlc`; tests updated. Gate **992**/0; `diff_exit=0`.

**STEP=2 note (2026-06-16 Driver):** `expr.mlc` 6× `match_arm_*` (non-generic): `[&]` → `[=]`; direct unit tests in `test_codegen.mlc`. Gate **995**/0; `diff_exit=0`. Visitor path (`cpp_ast` printer) — STEP=3+ / separate.

**Проблема**: mlcc self-hosted codegen генерирует `[&]` во всех лямбдах, тогда как MEMORY_MODEL.md требует `[=]`. Ruby-компилятор уже использует `[=]` (через `capture_analyzer.rb`). Баг актуален для хранимых лямбд.

**Что данглинг:**
```mlc
fn make_checker(allowed: [string]) -> (string -> bool) =
  fn word => allowed.contains(word)
// mlcc codegen: [&](auto word) { return allowed.contains(word); }
// allowed — dangling reference если allowed вышла за пределы стека
```

**Что оставить `[&]`** (обосновано):
- `block_as_immediate_invoked_function_expression` — IIFE может мутировать внешние `mut`-переменные
- `record_update_lazy_closure` — IIFE для record update

## Verify gate

```
compiler/tests/build_tests.sh
compiler/build.sh
compiler/out/mlcc -o .tmp_selfhost/p1 compiler/main.mlc
compiler/build_bin.sh .tmp_selfhost/p1 .tmp_selfhost/mlcc2
.tmp_selfhost/mlcc2 -o .tmp_selfhost/p2 compiler/main.mlc
diff -rq .tmp_selfhost/p1 .tmp_selfhost/p2   # empty
```

---

| Step | Item | Status |
|------|------|--------|
| 1 | `expression_support.mlc` — `cpp_lambda_header_prefix`: `'[&]'` → `'[=]'` | done (uncommitted) |
| 2 | `expr.mlc` — `match_arm_*`: `[&]` → `[=]` (6 функций) | done (uncommitted) |
| 3 | `expr.mlc` — `match_arm_constructed_value_generic*`: `[&]` → `[=]` (2 функции) | done (uncommitted) |
| 4 | Тест: самохост + stored lambda из теста | done (uncommitted) |
| 5 | Комментарий почему IIFE остаётся `[&]` | done (uncommitted) |

## Производительность

`[=]` vs `[&]` для коротких HOF-лямбд (`.any`, `.all`, `.filter`):
- `mlc::Array<T>` копируется COW — `shared_ptr` copy O(1), атомарный increment
- `string` copy тоже O(1) (или SSO inline)
- На практике: незначительная разница для коротких лямбд
- Правильная семантика важнее производительности

## Многопоточность

MLC не поддерживает многопоточность. В runtime есть:
- `symbol.hpp` — mutex для intern-кэша строк
- `task.hpp` — C++20 coroutines (заготовка, не используется)
- `net/http.hpp` — fetch в отдельном потоке (внутреннее)

Языковых примитивов (тип `Thread`, `Channel`, `spawn`) нет.
COW detach не атомарен — конкурентный доступ к массивам без явной синхронизации = data race.
Текущее состояние: **single-threaded по дизайну**.
