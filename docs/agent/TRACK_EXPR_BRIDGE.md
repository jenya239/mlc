# Track: Expr fragment bridge elimination (Phase 2 §3 cont.)

Parent: [../PLAN.md](../PLAN.md) §Phase 2; previous: [TRACK_PARSER_PARITY.md](TRACK_PARSER_PARITY.md) (**closed**, `d5442d6`), [TRACK_CPPGEN.md](TRACK_CPPGEN.md) (**closed**, `005c65b`)

## Status: **active** (step 4 pending)

**Goal:** remove remaining `expr_fragment_codegen` string bridges in `expr_visitor_cpp`; emit native `CppExpr` for call/method/if/block/array/lambda/record_update arms.

**Constraints:**
- One concern per sub-step; no `compiler/` + `lib/mlc/` in one commit.
- Self-host gate when `compiler/**` touched: `build_bin.sh` + diff empty.
- One expr arm batch per step (mirror TRACK_CPPGEN steps 1–3).

## Verify gate (every step)

```
bundle exec rake test_compiler_mlc   # 756 pass (baseline post PARSER_PARITY)
compiler/build.sh
compiler/out/mlcc -o .tmp_selfhost/p1 compiler/main.mlc
compiler/build_bin.sh .tmp_selfhost/p1 .tmp_selfhost/mlcc2
.tmp_selfhost/mlcc2 -o .tmp_selfhost/p2 compiler/main.mlc
diff -rq .tmp_selfhost/p1 .tmp_selfhost/p2   # empty
```

---

| Step | Item | Status |
|------|------|--------|
| 1 | `expr_visitor_cpp` — call + method as native CppExpr (no fragment bridge) | done (`d9505b9`) |
| 2 | `expr_visitor_cpp` — if + block as native CppExpr | done (`0ba605c`) |
| 3 | `expr_visitor_cpp` — array + lambda as native CppExpr | done (`e66e547`) |
| 4 | `expr_visitor_cpp` — record_update as native CppExpr | pending |
| 5 | Shrink/delete `expr_fragment_codegen.mlc`; audit; close track | pending |

## Survivors (post step 3)

`expr_visitor_cpp.mlc` still routes 2 arms through `expr_fragment_codegen.*_via_visitor`: record_update (+ match guards).

## Next step (Driver)

**STEP=4** — record_update native CppExpr in `expr_visitor_cpp.mlc`.

## Step 3 detail

- `expr_visitor_cpp.mlc`: `gen_array_via_cpp_visitor` (CppInitList / CppAggregateInit), `gen_lambda_via_cpp_visitor` (CppMutableLambda).
- `cpp_printer.mlc`: `print_mutable_lambda_expression`.
- mlcc codegen workaround: `lambda_capture_list` uses push+do/end (not `else [Shared.new(...)]`).

## Step 2 detail

- `expr_visitor_cpp.mlc`: `gen_if_via_cpp_visitor` (CppTernary / typed lambda-IIFE), `gen_block_via_cpp_visitor` (CppInvokedBlock).
- `cpp_ast.mlc` / `cpp_printer.mlc`: `CppInvokedBlock` for block IIFE without `-> auto`.

## Step 1 detail (done `d9505b9`)

- `mut_actual_argument.mlc`: CppExpr materialization + mut-prelude lambda wrap.
- `method_gen.mlc`: `gen_method_expr_cpp` native dispatch.
- `expr_visitor_cpp.mlc`: `gen_call_via_cpp_visitor` / `gen_method_via_cpp_visitor` without fragment bridge.

## Deferred (not in this track)

- Parser `ref mut` — [TRACK_PLAN.md](TRACK_PLAN.md) step 15, separate branch.
- `lib/mlc/` Ruby backend parity — not in scope.
- Record default expression parity (E4) — separate track if needed.
- Phase 4 `MLCC_BOOTSTRAP=1` — separate track after Phase 2 bridges.
