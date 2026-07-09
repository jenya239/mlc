# Track: Result/Option combinators + `?` auto-conversion via `From`

Parent: [../PLAN.md](../PLAN.md), [../MLC.md](../MLC.md) §A2. Source:
[../LANGUAGE_AUDIT_2026_07.md](../LANGUAGE_AUDIT_2026_07.md) #3.

## Status: **closed** (2026-07-09) — STEP=4 verify-gate green

Комбинаторы A2 + `?` E085 + `From` skip + codegen `E2_from(...)` shipped.

### Steps

| Step | Status | Notes |
|------|--------|-------|
| 1 | **done** | Inventory: A2 combinators **shipped**; gap was `?` + `From` / E-mismatch |
| 2 | **done** | Hard error E085 when `?` Err ≠ enclosing fn Result Err; `CheckContext.expected_return_type` |
| 3a | **done** | Parser keeps `From<T>` on extend; registry stores full+bare; E085 skipped when `E2: From<E1>` |
| 3b | **done** | Codegen: `question_from_converter_name` → `CppQuestionTry` / try_unwrap / return_body call `E2_from(...)` |
| 4 | **done** | verify-gate: build.sh, self-host DIFF_EXIT:0, regression 20/0, build_tests 1454/0; parser Shared\<T\> fix |

**Critic notes (2026-07-09):** extend-method check contexts leave `expected_return_type` as `TUnknown` — `?` inside `extend` methods does not get E085 yet (known gap, out of scope).

## Inventory (STEP=1, `compiler/` only)

### Combinators — **shipped**

| Layer | Location |
|-------|----------|
| Names / arity / result type | `checker/check/method_types/result_option_method_types.mlc` |
| Infer + diagnostics E055–E064 | `checker/infer/infer_result_option_method.mlc` |
| Transform HOF | `checker/transform/transform.mlc` (`transform_result_option_hof_method_call`) |
| Codegen | `codegen/expr/method_gen.mlc` (`gen_result_option_combinator_*`, `ok` → `ok_into_optional`) |
| Runtime | `runtime/include/mlc/core/result_combinators.hpp`, `optional_combinators.hpp` |

**Result:** `map`, `map_err`, `and_then`, `or_else`, `unwrap_or`, `unwrap_or_else`, `ok`  
**Option:** `map`, `and_then`, `or_else`, `unwrap_or`, `filter`, `ok_or`

### `?` operator — **done**

| Piece | Status |
|-------|--------|
| Parse `ExprQuestion` | yes (`frontend/parser/exprs.mlc`) |
| Infer unwrap Ok | yes (`infer_question_expression.mlc`) |
| Check E vs enclosing fn return `Result<_,E>` | **done** (E085; skip if From) |
| Trait `From` / `extend T : From<U>` | **done** (parser `From<T>` key; checker lookup) |
| Codegen early-return Err | **done** — mismatch → `ExpectedErr_from(*get_if<1>)` |

**STEP=4 self-host fix:** `parse_extend_decl` must consume type-name generics (`Shared<T>`) before trait generics (`From<U>`); otherwise `extend Shared<T>` polluted `trait_name`. Also `empty_diagnostic_list()` — no `return do` early-return (broken mlcc emit).

## Verify gate (STEP=4, 2026-07-09)

```
dev_gate_fast 1454/0
self-host p1/mlcc2/p2 DIFF_EXIT:0
regression_gate 20/0
build_tests.sh TESTS_EXIT:0 (1454 passed)
```
