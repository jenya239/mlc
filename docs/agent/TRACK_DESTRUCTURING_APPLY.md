# Track: Apply record destructuring throughout compiler (Phase 2.5 §2)

Parent: [../PLAN.md](../PLAN.md) §Phase 2.5; previous: [TRACK_STRING_MATCH.md](TRACK_STRING_MATCH.md)

## Status: **open** (step 1 pending)

**Goal:** ???????? ??????? `let result = f(state); use result.foo; use result.bar` ?? `let { foo, bar } = f(state)` ?? ???? ?????? compiler/. ???? ??? ???????????? — ????? ????, ? compiler/ 0 ?????????????.

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
| 1 | `frontend/lexer.mlc` — scan-result ???????????????? | pending |
| 2 | `cpp/lexer.mlc` — `CppScanIdR`, `CppScanIntR`, `CppScanStrR`, `CppScanOpR` | pending |
| 3 | `cpp/parser.mlc` — `CppDeclResult`, `CppTypeResult` | pending |
| 4 | `frontend/parser/exprs.mlc` — ??? `result.parser`, `result.value`, `result.stmt` | pending |
| 5 | `frontend/parser/decls.mlc` — ??? result-???? | pending |
| 6 | `frontend/parser/types.mlc` — ??? result-???? | pending |
| 7 | `checker/infer/infer.mlc` + `infer_call.mlc` — `InferResult` ???? | pending |
| 8 | `checker/infer/infer_array_method.mlc` + `infer_result_option_method.mlc` | pending |
| 9 | `checker/transform/transform.mlc` + `transform_stmts.mlc` | pending |
| 10 | `codegen/context.mlc` + `codegen/eval.mlc` + `codegen/module.mlc` | pending |
| 11 | Audit: ??? `result\.` ????????? ??? ???? ?????????????? `let result = ` | pending |
| 12 | ??????? ???? | pending |
