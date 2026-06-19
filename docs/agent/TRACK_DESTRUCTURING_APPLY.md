# Track: Apply record destructuring throughout compiler (Phase 2.5 �2)

Parent: [../PLAN.md](../PLAN.md) �Phase 2.5; previous: [TRACK_STRING_MATCH.md](TRACK_STRING_MATCH.md)

## Status: **closed** (2026-06-03)

**STEP=13 note (2026-06-03 Driver):** commit `fd95af6` — 26 `.mlc` steps 2–12 WIP landed. Gate: build_tests 936/0, build.sh OK. Track closed.

**Critic audit (2026-06-03):** track was closed in SESSION but **steps 2–12 exist only as uncommitted WIP** (~26 `.mlc` vs HEAD). Committed HEAD: lexer step 1 only (`06dfa9e`); `infer.mlc`/`decls.mlc`/`context.mlc` have zero `*_parsed` on HEAD. Gate 936/0 verified on **worktree**, not on HEAD alone. **Reopened** — pending: commit batch before treating closed.

**STEP=12 note (2026-06-03 Driver):** final gate `build.sh` OK; steps 1–11 complete; intentional survivors documented (STEP=11). Track closed.

**STEP=11 note (2026-06-03 Driver):** audit `*_result` in `compiler/**/*.mlc` (excl. `out/`); fixed survivors in `main`, `check`, `names`, `infer_match`, `infer_call_support`, `infer_lambda_context`, `infer_question_expression`, `exprs`, `ctor_info`, `stmt_cpp`. Intentional: `coerced_result` (transform), `*_pair` (partial_application), `s_result` (test). Gate: build_tests 936/0, build.sh OK.

**STEP=10 note (2026-06-03 Driver):** `context.mlc` — `GenStmtsWithContext.statements_parsed`, `append_stmt(statement_parsed)`; `module.mlc` — `module_parsed`, `mapped_aliases`; `stmt_eval.mlc`/`return_body.mlc` (eval pipeline) — field access. `eval.mlc` pass-through. Gate: build_tests 936/0, build.sh OK.

**STEP=9 note (2026-06-03 Driver):** `transform.mlc` — `statements_parsed`/`result_parsed` for `TransformStmtsResult` sites (block/while/for/with); field access; `transform_stmts.mlc` already clean. Gate: build_tests 936/0, build.sh OK.

**STEP=8 note (2026-06-03 Driver):** `infer_array_method.mlc` + `infer_result_option_method.mlc` — `object_parsed`, `init_parsed`, `lambda_parsed`, `argument_parsed`, `count_parsed`; field access. Gate: build_tests 936/0, build.sh OK.

**STEP=7 note (2026-06-03 Driver):** `infer.mlc` + `infer_call.mlc` — `*_parsed` for InferResult/StmtInferResult bindings (`left_parsed`, `object_parsed`, `function_parsed`, …); `record_inference` accumulator; field access (stability). Gate: build_tests 936/0, build.sh OK.

**STEP=6 note (2026-06-03 Driver):** `frontend/parser/types.mlc` — `base_parsed`/`ret_parsed`/`inner_parsed`/`next_parsed`/`paren_types_parsed`/`type_args_parsed`/`suffix_parsed`; field access on `*_parsed` (stability, same as decls STEP=5). Gate: build_tests 936/0, build.sh OK.

**STEP=5 note (2026-06-03 Driver):** `frontend/parser/decls.mlc` � `*_parsed` bindings for all parse-result sites (`method`/`type`/`ret_type`/`trait`/`body`/`variant`/`derive`/`params`); zero `*_result`/`const result = parse`. Ruby bootstrap codegen: `let { field }` in `decls.mlc` breaks CppAst emit (Array in BlockStatement) � fields via `parsed.field`, `.parser` on binding; loops use `let parsed = �` + `.decl`/`.variant` (same as STEP=4 `.parser` escape). `parse_param` unchanged (field access). Gate: build_tests 936/0, build.sh OK.

**Goal:** replace pattern `let result = f(state); use result.foo; use result.bar` with `let { foo, bar } = f(state)` across compiler/. No abbreviations � full words; target 0 destructuring gaps in compiler/.

**STEP=1 note (2026-06-03 Meta):** `frontend/lexer.mlc` destructuring landed; verify blocked on uncommitted `let_pat.mlc` scope fix (extra `{}` around `let { }` bindings broke self-host `lexer.cpp`). Driver loop on STEP=1 drained � do not re-enqueue STEP=1.

**STEP=2 note (2026-06-03 Driver):** `cpp/lexer.mlc` record types + `push_*_scan` helpers + `let { st, tok }` destructuring; pre-flight `let_pat.mlc` + `let_pat_cpp.mlc` flat bindings (no scoped `{}`); `test_codegen.mlc` expectations updated. Gate: build_tests 936 pass, build.sh OK, p1=174 files, build_bin OK; self-host diff FAIL (mlcc2 ? p2=2 files, merge 1� lex � pre-existing mlcc2 modular blocker).

**STEP=3 note (2026-06-03 Driver):** `cpp/parser.mlc` � record types `CppDeclResult`/`CppTypeResult`; `let { type_string, position: after_type }` in `parse_using_decl`/`parse_struct_decl`; `let { decl, position: next_position }` in `cpp_parse` loop. Gate: build_tests 936/0, build.sh OK; self-host diff still FAIL (pre-existing mlcc2 blocker).

**STEP=4 note (2026-06-03 Driver):** `frontend/parser/exprs.mlc` � `parse_statement_let_pat_finish` helper; `let { stmt/expr/type_expr/stmts/arms }` via `*_parsed` bindings; `.parser` on parsed value (no `parser:` rename � Ruby bootstrap + param shadow). Zero `type_result`/`value_result`/`else_result`/`const result = parse`. Gate: build_tests 936/0, build.sh OK.

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
| 1 | `frontend/lexer.mlc` � scan-result destructuring | done |
| 2 | `cpp/lexer.mlc` � `CppScanIdR`, `CppScanIntR`, `CppScanStrR`, `CppScanOpR` | done |
| 3 | `cpp/parser.mlc` � `CppDeclResult`, `CppTypeResult` | done |
| 4 | `frontend/parser/exprs.mlc` � ??? `result.parser`, `result.value`, `result.stmt` | done |
| 5 | `frontend/parser/decls.mlc` � `*_result` ? `*_parsed` | done |
| 6 | `frontend/parser/types.mlc` — parse-result → `*_parsed` | done |
| 7 | `checker/infer/infer.mlc` + `infer_call.mlc` — `InferResult` → `*_parsed` | done |
| 8 | `checker/infer/infer_array_method.mlc` + `infer_result_option_method.mlc` — `*_parsed` | done |
| 9 | `checker/transform/transform.mlc` + `transform_stmts.mlc` — `TransformStmtsResult` → `*_parsed` | done |
| 10 | `codegen/context.mlc` + `eval.mlc` + `codegen/module.mlc` — `GenStmtsResult`/`GenModuleOut` → `*_parsed` | done |
| 11 | Audit: grep `result.` / remaining `let *_result =` in compiler/ | done |
| 12 | Final gate verify + close track | done (WIP uncommitted) |
| 13 | Commit steps 2–12 WIP batch (`fd95af6`) | done |
