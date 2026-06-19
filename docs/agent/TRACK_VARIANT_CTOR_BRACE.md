# Track: variant ctor brace init (codegen stability)

Parent: [../PLAN.md](../PLAN.md) §Phase 1 stabilization; follows [TRACK_COW_P2_AUDIT.md](TRACK_COW_P2_AUDIT.md) (**closed**)

## Status: **closed** (STEP=5, 2026-05-19)

**STEP=5 note (2026-05-19 Driver):** Track closed. Gate summary: build_tests **998/1** (E046 only, out of scope); parity **2/2**; **diff_exit=0**; mlcc **1.81s** / mlcc2 **1.56s**. Change: `expr_visitor_cpp.mlc` `gen_constructor_call_cpp_expression` → always `CppAggregateInit`. Uncommitted.

**After STEP=4:** self-host **diff_exit=0**; `main_check=0`; mlcc **1.81s** / mlcc2 **1.56s** (g++ link **206s** one-time).

**Scope:** Cpp AST call path `SemanticExpressionCall` → `is_constructor_call` → `CppAggregateInit` (`Name{…}`) для unary/multi-arg variant ctors; не `CppCall` (`Name(…)`).

### Out of scope

- `registry.mlc` COW `.push` (10) — отдельный perf/COW track
- parser `ref mut` — TRACK_PLAN step 15
- Multi-arg record variants; generic ctor overload resolution beyond test cases

## Verify gate

```
compiler/tests/build_tests.sh          # target: ctor tests green
bash compiler/tests/run_mlcc2_checker_parity.sh
compiler/build.sh
compiler/out/mlcc -o .tmp_selfhost/p1 compiler/main.mlc
compiler/build_bin.sh .tmp_selfhost/p1 .tmp_selfhost/mlcc2
.tmp_selfhost/mlcc2 --check-only compiler/main.mlc
.tmp_selfhost/mlcc2 -o .tmp_selfhost/p2 compiler/main.mlc
diff -rq .tmp_selfhost/p1 .tmp_selfhost/p2   # empty
```

---

| Step | Item | Status |
|------|------|--------|
| 1 | Locate call codegen path; confirm `test_codegen` expectations | **done** |
| 2 | Emit brace init for unary variant ctors (`Ok`, `Err`) | **done** |
| 3 | `build_tests` green (ctor cases); extend if needed | **done** |
| 4 | Full self-host verify gate | **done** |
| 5 | Close track | **done** |

### Step 1 sub-steps

1. Read `test_codegen.mlc` lines ~204–212; trace `gen_expr` / `SemanticExpressionCall` lowering
2. Grep `SemanticExpressionCall` / ctor / `CppInitList` in `compiler/codegen/`
3. Document target fn(s) in inventory below; gate skip ok

### Step 2 sub-steps

1. Detect variant ctor call (name + arity) in expr codegen
2. Lower to `Name{arg}` not `Name(arg)` for unary tuple variants
3. Run `build_tests` (ctor subset ok)

### Step 3 sub-steps

1. Full `build_tests.sh`; fix regressions
2. Gate parity + build.sh

### Step 4 sub-steps

1. Self-host diff gate per TRACK

### Step 5 sub-steps

1. Mark track closed; update TRACK_PLAN

---

## Inventory (STEP=1)

### Test expectations (`test_codegen.mlc` L204–212)

| Case | Expected | Actual |
|------|----------|--------|
| `Ok(42)` | `Ok{42}` | `Ok(42)` |
| `Err("oops")` | `Err{mlc::String("oops", 4)}` | `Err(mlc::String("oops", 4))` |

### Call chain (active Cpp AST path)

```
gen_expr (codegen/eval.mlc)
  → eval_expr_cpp (expr_visitor_cpp.mlc)
  → dispatch_codegen_pass → visit_call
  → gen_call_via_cpp_visitor
  → is_constructor_call? → gen_constructor_call_cpp_expression
  → print_expr → CppAggregateInit | CppCall
```

### Root cause

| File | Symbol | Issue |
|------|--------|-------|
| `compiler/expr_visitor_cpp.mlc` | `gen_constructor_call_cpp_expression` L84–89 | `argument_expressions.length() == 0` → `CppAggregateInit`; else → `CppCall` (parens) |
| `compiler/codegen/expr/expression_support.mlc` | `is_constructor_call` L38–42 | Uppercase ident heuristic — OK for `Ok`/`Err` |
| `compiler/cpp/cpp_ast.mlc` | `CppAggregateInit` / `aggregate_initializer_expression` L188–189 | Prints `TypeName{args}` — correct target |
| `compiler/codegen/expr/expr.mlc` | `constructor_call_braces` L60–61 | String helper — correct |

### Legacy string path (already correct)

`compiler/out/expr_fragment_codegen.cpp` (generated from removed/split module): `is_constructor_call` → `constructor_call_braces` for **all** arities. Active pipeline uses Cpp AST visitor, not this fragment path.

### Ruby reference

- `lib/mlc/backends/cpp/rules/expressions/call_rule.rb` L148–157 — nullary `CtorName{}`; unary+ via callee as variant ctor identifier + brace lowering in call path
- `lib/mlc/backends/cpp/rules/expressions/var_ref_rule.rb` L34–37 — nullary `identifier{}`; unary ctor as identifier (call site adds args with braces via call_rule)

### STEP=2 change

`compiler/expr_visitor_cpp.mlc` — `gen_constructor_call_cpp_expression`: `SemanticExpressionIdent` → always `CppAggregateInit` (removed zero-arg-only branch + `CppCall` fallback). Perf: one fewer branch, no extra alloc.
