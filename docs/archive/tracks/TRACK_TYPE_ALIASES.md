# Track: Type aliases (Phase 2.5 #3)

Parent: [../PLAN.md](../PLAN.md) Phase 2.5; previous: [TRACK_DESTRUCTURING_APPLY.md](TRACK_DESTRUCTURING_APPLY.md)

## Status: **closed** (step 7 audit — 2026-06-06 Driver)

**STEP=7 audit (2026-06-06):** Steps 1–5 verified on HEAD (`5bf2bd55`). STEP=6 WIP (ParseResult\<T\>, `field_type_from_object` substitution) **reverted** — self-host `build_bin` failed (`apply_type_parameter_substitution` match codegen, `Value` leak). STEP=6 **deferred** until record generics (`TRACK_GENERICS_RECORD` — Planner). Gate: **978**/0; `build.sh` OK; self-host `diff_exit=0`.

**STEP=6 skip rationale:** `type ParseResult<Value> = { value: Value, parser: Parser }` requires generic record field substitution in checker/codegen; not stable on current mlcc. See [RESEARCH.md](RESEARCH.md) §Generics.

**Delivered (steps 1–5):** `DeclTypeAlias` parser; checker resolve + E081 cycle; codegen underlying-type only (`type_alias_annotations`); tests; `CppExpressions`/`CppStatements`/`CppParameters`/`CppFields` in `cpp_ast.mlc`.

**Deferred:** `frontend/parser/predicates.mlc` — `ParseResult<T>` aliases (blocked on generics).

## Verify gate (reference)

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
| 1 | Parser: `DeclTypeAlias` — `type Name = Type` (not ADT) | done (`99b11438`) |
| 2 | Checker: alias resolves as underlying type; diagnostics on cycle | done (`dae2eacd`) |
| 3 | Codegen: alias emits no C++ decl — underlying type only | done (`9cc3eae8`) |
| 4 | Tests in `test_checker.mlc` + `test_parser.mlc` | done (`6ed2911b`) |
| 5 | Apply in `cpp/cpp_ast.mlc` — `CppExpressions`, `CppStatements`, `CppParameters`, `CppFields` | done (`5bf2bd55`) |
| 6 | Apply in `frontend/parser/predicates.mlc` — `ParseResult<T>` | skip — deferred (`TRACK_GENERICS_RECORD`) |
| 7 | Audit + close track | done (2026-06-06) |
