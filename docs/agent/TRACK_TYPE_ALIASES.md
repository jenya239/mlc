# Track: Type aliases (Phase 2.5 #3)

Parent: [../PLAN.md](../PLAN.md) Phase 2.5; previous: [TRACK_DESTRUCTURING_APPLY.md](TRACK_DESTRUCTURING_APPLY.md)

## Status: **open** (step 5 pending)

**STEP=1 note (2026-06-05 Meta recovery):** `DeclTypeAlias` parser + semantic stub; sum/ADT disambiguation; `test_parser` (+2). Gate **963**/0; self-host `diff_exit=0`. Commit `99b11438`. Guard `Driver:1:TYPE_ALIASES` loop cleared.

**Planner (2026-05-19):** RENAME_ABBREV closed; TYPE_ALIASES active. Step 1 parser-only; checker/codegen in steps 2–3. Depends on stable names in `cpp_ast.mlc` (step 5).

**Goal:** Support `type Alias = ExistingType` (not ADT, not sum). Apply repeated `[Shared<CppExpression>]`, `[Shared<CppStatement>]` etc. in `cpp_ast.mlc` and parser.

```mlc
// before:
type CppFunctionDef = CppFunctionDef(string, [Shared<CppType>], [Shared<CppParam>], [Shared<CppStatement>])

// after:
type CppExpressions = [Shared<CppExpression>]
type CppStatements  = [Shared<CppStatement>]
type CppParameters  = [Shared<CppParameter>]
type CppFunctionDef = CppFunctionDef(string, CppExpressions, CppParameters, CppStatements)
```

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
| 1 | Parser: `DeclTypeAlias` — `type Name = Type` (not ADT) | done (`99b11438`) |
<!-- sub-steps: 1) add `DeclTypeAlias(name, type_params, Shared<TypeExpr>, Span)` to `frontend/ast.mlc` + `decl_name`/`decl_inner` arms; 2) in `parse_type_decl` (`decls.mlc`): after `=`, if token is type-expr start (not `{`/variant ident), `parse_type` → `DeclTypeAlias`; 3) minimal `semantic_ir`/`transform` stub or skip decl in checker until step 2; 4) `test_parser.mlc` positive + negative (`type Foo = Bar | Baz` stays sum) -->
| 2 | Checker: alias resolves as underlying type; diagnostics on cycle | done (`dae2eacd`) |
| 3 | Codegen: alias emits no C++ decl — underlying type only | done (`9cc3eae8`) |
| 4 | Tests in `test_checker.mlc` + `test_parser.mlc` | done (`<hash>`) |
| 5 | Apply in `cpp/cpp_ast.mlc` — `CppExpressions`, `CppStatements`, `CppParameters`, `CppFields` | pending |
| 6 | Apply in `frontend/parser/predicates.mlc` — `ParseResult<T>` (after generics stable) | pending |
| 7 | Audit + close track | pending |
