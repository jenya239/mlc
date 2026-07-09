# Track: String match patterns (Phase 2.5 �1)

Parent: [../PLAN.md](../PLAN.md) �Phase 2.5; next: [TRACK_DESTRUCTURING_APPLY.md](TRACK_DESTRUCTURING_APPLY.md)

## Status: **closed** (step 12 done — 2026-06-05)

**STEP=12 note (2026-06-05 Driver):** self-host blocker fixed — mlcc codegen omitted `&` on `mut` params (`decl_extend.mlc` `parameter_type_cpp`); lexer `tokens.push_back` was no-op (by-value capture). Also `__tmp_N` let-pat temps + tests. Gate: **941**/0; `diff -rq p1 p2` empty; `p1==p3` (mlcc3 round). Track **closed**. Next: **TYPE_ALIASES**.

**Planner (2026-05-19):** steps 1–11 complete; step 12 = close track + self-host diff. Then **TYPE_ALIASES**.

**STEP=11 note (2026-05-19 Driver):** audit + pipe `match` migrations: `frontend/parser/types.mlc` builtins; `frontend/lexer.mlc` int suffix; `checker/registry.mlc` TyNamed primitives; `codegen/cpp_naming.mlc` map_method/map_builtin; `expression_support.mlc` file/profile; `codegen/decl/type_gen.mlc` type_name_to_cpp + derive traits; `derive_methods_cpp.mlc` trait dispatch. **Deferred (intentional):** operator/path/empty compares (`binary_diagnostics`, `main.mlc` paths); `kind.ident()` parser keyword gates; large method-name chains (`infer_array_method`, `array_method_types`, `method_receiver_diagnostics`); `cpp_keyword` OR-list; `type_cpp == 'auto'`. Gate: **960**/0; build.sh OK.

**STEP=10 note (2026-05-19 Driver):** `method_gen.mlc` pipe string match (static receiver, Result/Option, combinator names); `compile_options.mlc` `is_*_flag` predicates. Gate: **960**/0; build.sh OK.

**STEP=9 note (2026-05-19 Driver):** `semantic_type_structure.mlc` — `binary_operation_result_type`, `builtin_method_*`, `operator_method_for`, `type_description` pipe string match. Gate: **960**/0; build.sh OK.

**STEP=8 note (2026-05-19 Driver):** `cpp/lexer.mlc` pipe match `cpp_keyword_kind`/`cpp_scan_op`; `is_cpp_reserved_keyword` + `reserved_cpp_keyword_kind`. Gate: **960**/0; build.sh OK.

**STEP=7 note (2026-05-19 Driver):** `frontend/lexer.mlc` pipe-style string match (`keyword_kind`/`map_escape`/`scan_op`); `is_reserved_keyword` + `reserved_keyword_kind` (bootstrap: no `Ident(word)` in match arm). Gate: **960**/0; build.sh OK.

**STEP=6 note (2026-06-04 Driver):** tests in `test_parser`, `test_checker`, `test_decl_gen`. Gate: **960**/0; build.sh OK.

**STEP=5 note (2026-06-04 Driver):** string match via nested `CppIf` + `print_statement`; `string_match_arm_else_branch`. Gate: **952**/0; build.sh OK.

**STEP=4 note (2026-06-04 Driver):** `match_gen.mlc` string match → `if`/`else if`/`else` IIFE; `expr_visitor_cpp`; `test_codegen`. Gate: **952**/0; build.sh OK.

**STEP=3 note (2026-06-04 Driver):** `infer_match.mlc` E077–E080; subject `string` or string-literal arms; `test_checker` + catalog 80. Gate: **947**/0; build.sh OK.

**STEP=2 note (2026-06-04 Driver):** `parse_pattern_string` → `PatStringLit`; test_parser match `"x"` arm. Gate: **942**/0; build.sh OK.

**STEP=1 note (2026-06-04 Driver):** `PatStringLit(string, Span)` in `frontend/ast.mlc`; `pat_span`; `test_parser` smoke; `match_gen` arms. Gate: **941**/0; build.sh OK (worktree uncommitted).

**Goal:** ???????? ? ???? `match` ?? ????????? ?????????. ????????? ??? `if/else if word == '...'` ??????? ? compiler/ (~5 ??????, 150+ ?????).

```mlc
// ??:
fn keyword_kind(word: string) -> TokenKind =
  if      word == 'fn'   then KFn
  else if word == 'type' then KType
  ...

// ?????:
fn keyword_kind(word: string) -> TokenKind =
  match word {
  | 'fn'   -> KFn
  | 'type' -> KType
  | _      -> KIdentifier(word)
  }
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
| 1 | Parser: `PatStringLit` in `frontend/ast.mlc` | done |
| 2 | Parser: string literals in match arms (`parser/exprs.mlc`) | done |
| 3 | Checker: E077–E080 (`infer_match.mlc`) | done |
| 4 | Codegen string: if/else if IIFE (`match_gen.mlc`) | done |
| 5 | Codegen CppAST: nested CppIf else-if chain (`match_gen.mlc`) | done |
| 6 | Tests: parser + checker + decl_gen string match | done |
| 7 | ????????? ? `frontend/lexer.mlc` � `keyword_kind`, `map_escape`, `scan_op` | done |
| 8 | ????????? ? `cpp/lexer.mlc` � `cpp_keyword_kind`, `cpp_scan_op` | done |
| 9 | ????????? ? `checker/semantic_type_structure.mlc` � dispatch ?? ????? ?????? | done |
| 10 | ????????? ? `codegen/expr/method_gen.mlc` + `compile_options.mlc` | done |
| 11 | Audit: ??? `if.*==.*'` ? compiler/ ????? ????????? ????????? | done |
| 12 | Close track + self-host diff | done |

## Sub-steps (step 1 detail)

- `frontend/ast.mlc`: ???????? `PatStringLit(string)` ? `type Pattern`
- `frontend/ast.mlc`: ???????? `pat_span` ??? ?????? ????????
- `compiler/tests/test_parser.mlc`: ???? ?? `match s { | 'x' -> 1 | _ -> 0 }`
