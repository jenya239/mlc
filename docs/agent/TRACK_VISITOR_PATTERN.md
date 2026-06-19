# Track: Visitor pattern for expression passes (Phase 2.6 �1�4)

Parent: [../PLAN.md](../PLAN.md) �Phase 2.6; previous: [TRACK_RENAME_ABBREV.md](TRACK_RENAME_ABBREV.md)

## Status: **closed** (2026-06-15)

**MAX_ITER:** 20  
**GATE:** `compiler/tests/build_tests.sh` + self-host diff (see Verify gate below)

**STEP=1 note (2026-06-06):** `ExprVisitor<VisitorResult>` trait + `dispatch_expr` in `expr_visitor.mlc` (29 `visit_*` arms); `test_expr_visitor.mlc` stub `TagExprVisitor` (+6). Gate **984**/0; self-host `diff_exit=0`.

**STEP=12 note (2026-06-15 Driver):** Audit `compiler/**/*.mlc` (excl. `out/`). Entry points `infer_expr`, `transform_expr`, `gen_expr` — thin wrappers → `dispatch_*_pass` only; no rogue duplicate match bodies. **Intentional survivors:** (1) parser-Expr legacy twins `check_names_expr`, `check_mutation_expr`/`check_fn_body_mutations` in `check.mlc` fn-body pipeline (visitor twins exist; parity in step 11); (2) `dispatch_*_pass` / `dispatch_expr` single-match routers; (3) `partial_application_desugar_expr` pre-pass; (4) `coerce_expr_to_type` semantic coercion; (5) `transform_expr_maybe_lambda_context` 2-arm lambda pre-dispatch; (6) stmt helpers `return_body.mlc`/`stmt_eval.mlc` shape checks calling `gen_expr` callback; (7) `default_expr_to_cpp` field-default printer. Gate **992**/0; `diff_exit=0`.

**STEP=13 note (2026-06-15 Driver):** Final gate **992**/0; `diff_exit=0`. Track closed. **Uncommitted batch (steps 7–12):** `eval.mlc` CodegenContext methods; `infer_result.mlc` helpers; `names.mlc` NameCheckResult helpers; `cpp_ast.mlc` printer + `cpp_printer.mlc` removed; `test_visitor_pass_parity.mlc` (+8); STEP=12 audit note. Commit pending user command.

**Goal:** ????????? ??? ??????? ??????????? (names, mutations, infer, transform, codegen) ? `fn pass_expr(context, expression)` + ??????? `match` ?? `extend Pass : ExprVisitor<Result>`. `ExprVisitor<Result>` ????? ??? ?????????? ? `expr_visitor.mlc`. ?????????? ????? ????? AST ?????? ?????? ?????? ?????????? ??? ?????????? ???????????.

## Verify gate (every step)

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
| 1 | `ExprVisitor<Result>` trait + `dispatch_expr` + `visit_*` for SemanticExpression | done (`23b65206`) |
| 2 | `checker/names.mlc` → `extend NamesPass : ExprVisitor<NameCheckResult>` | done |
| 3 | `checker/check_mutations.mlc` → `extend MutationsPass : ExprVisitor<MutationCheckResult>` | done (`1f99fb8f`) |
| 4 | `checker/infer/infer.mlc` → `InferPass` + `dispatch_infer_pass` | done (`80e5268d`) |
| 5 | `checker/transform/transform.mlc` → `TransformPass` + `dispatch_transform_pass` | done (`edf88d24`) |
| 6 | `expr_visitor_cpp.mlc` → `CodegenPass` + `dispatch_codegen_pass` | done (`b5f5e16d`) |
| 7 | `eval.mlc`: `extend CodegenContext` — `gen_*` as methods; export wrappers | done (uncommitted) |
| 8 | `infer_result.mlc`: `.map_type`, `.merge_errors`, `.and_then` | done (uncommitted) |
| 9 | `names.mlc`: `extend NameCheckResult` — `.merge`, `.with_scope`, `.append_diagnostics` | done (uncommitted) |
| 10 | Display/printer: `print_*` on Cpp AST in `cpp_ast.mlc`; `cpp_printer.mlc` removed | done (uncommitted) |
| 11 | Parity tests: names/mutations legacy vs visitor; codegen gen_expr vs eval_expr_cpp (+8) | done (uncommitted) |
| 12 | Audit: infer_expr/transform_expr/gen_expr survivors documented (STEP=12 note) | done (uncommitted) |
| 13 | Close track | done (uncommitted batch 7–12) |

## Sub-steps (step 4 detail � infer split)

??????? `checker/infer/infer.mlc` (~592 ??????) ???????? `dispatch_infer_expr` ? ~25 ???????.
??????? ?? ?????? (?????? ??? ???????????? `infer_call.mlc`, `infer_match.mlc`):
- `infer_literal.mlc` � `ExprInt`, `ExprStr`, `ExprBool`, `ExprUnit`
- `infer_ident.mlc` � `ExprIdent`
- `infer_record.mlc` � `ExprRecord`, `ExprUpdate`
- `infer_lambda.mlc` � `ExprLambda`, `ExprPartialApp`
- `infer_block.mlc` � `ExprBlock`, `ExprIf`, `ExprWhile`
?????? ???? ????????? ??????????????? `visit_*` ?????? ??????.

## Sub-steps (step 5 detail � transform split)

`transform/transform.mlc` (~828 ?????), `transform_expr` ~290 ?????.
??????? ?? ????? AST ?? ???????? ? infer:
- `transform_call.mlc`
- `transform_record.mlc`
- `transform_match.mlc`
- `transform_lambda.mlc`
- `transform_block.mlc`
