# Track: Result/Option combinators + `?` auto-conversion via `From`

Parent: [../PLAN.md](../PLAN.md), [../MLC.md](../MLC.md) §A2. Source:
[../LANGUAGE_AUDIT_2026_07.md](../LANGUAGE_AUDIT_2026_07.md) #3.

## Status: **open** (STEP=3a done 2026-07-09 — From skips E085; codegen convert pending)

Комбинаторы уже специфицированы в `docs/MLC.md` §A2. Аудит добавляет
**новую** часть — авто-конверсия ошибки в `?` через `From`.

### Steps

| Step | Status | Notes |
|------|--------|-------|
| 1 | **done** | Inventory: A2 combinators **shipped**; gap is `?` + `From` / E-mismatch |
| 2 | **done** | Hard error E085 when `?` Err ≠ enclosing fn Result Err; `CheckContext.expected_return_type` |
| 3a | **done** | Parser keeps `From<T>` on extend; registry stores full+bare; E085 skipped when `E2: From<E1>` |
| 3b | pending | Codegen: on `?` Err mismatch with From, call `E2_from(...)` before return |
| 4 | pending | verify-gate + close |

**Critic notes (2026-07-09):** extend-method check contexts leave `expected_return_type` as `TUnknown` — `?` inside `extend` methods does not get E085 yet.

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

Smoke (`mlcc --check-only`, 2026-07-09): TRACK pipeline repro with `.and_then`/`.map`/`.map_err` → exit 0.  
Unit coverage in `test_checker.mlc`: map ok; negatives for `and_then`/filter; not a full method matrix.

TRACK file list claiming “add Result/Option branches” is **stale** — already present.

### `?` operator — **partial**

| Piece | Status |
|-------|--------|
| Parse `ExprQuestion` | yes (`frontend/parser/exprs.mlc`) |
| Infer unwrap Ok | yes (`infer_question_expression.mlc`) — any `TGeneric`, E068 if not |
| Check E vs enclosing fn return `Result<_,E>` | **done** (E085; skip if From) |
| Trait `From` / `extend T : From<U>` | **done** (parser `From<T>` key; checker lookup) |
| Codegen early-return Err | partial — still returns Err **as-is**; From convert = STEP=3b |

**Repro gap (confirmed):**

```mlc
type Result<T, E> = Ok(T) | Err(E)
type ParseError = { message: string }
type AppError = { message: string }
fn parse(s: string) -> Result<i32, ParseError> = Err(ParseError { message: "bad" })
fn run(s: string) -> Result<i32, AppError> = do
  let value = parse(s)?   // should fail or From-convert; today check-only exit 0
  Ok(value)
end
```

`mlcc --check-only` → exit 0. Emit returns `ParseError` from `Result<i32,AppError>` body — invalid C++ if linked.

## Scope (remaining)

1. ~~Result/Option A2 methods~~ — done (pre-existing).
2. **`?` + E mismatch:** diagnose when inner `E` ≠ function `E` and no `From`.
3. **`From` conversion:** `extend E2 : From<E1> { fn from(...) }` + codegen call before return.

Prefer staged commits: (2a) hard error on mismatch → (2b) `From` auto-convert.

## Verify gate

```bash
scripts/dev_gate_fast.sh   # each turn
# before close:
compiler/build.sh
# self-host p1/mlcc2/p2 + regression_gate + build_tests.sh
```
