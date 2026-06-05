# Track: Rename abbreviations + fix string quotes (Phase 2.6 �7-8)

Parent: [../PLAN.md](../PLAN.md) �Phase 2.6; previous: [TRACK_TYPE_ALIASES.md](TRACK_TYPE_ALIASES.md)

## Status: **open** (step 13 done — 2026-06-05)

**STEP=13 note (2026-06-05 Driver):** `checker/infer/` locals/bindings: `inner_expression`, `match_arms`, `callback_argument`, `argument_types`, `required_arity`, etc.; quotes ok. Gate: **961**/0; self-host diff empty. Commit `c487d3fc`.

**STEP=12 note (2026-06-05 Driver):** `names.mlc` `sub_patterns`/`alternatives`/`inner_expression`/`statements`/`variable_name`; `check/check_mutations.mlc` `then_expression`/`match_arms`/`check_mutation_statements_errors`/`block_mutable_locals`; quotes ok. Gate: **961**/0; self-host diff empty. Commit `c3eb310d`.

**STEP=11 note (2026-06-05 Driver):** `FunctionIndex.function_types`; `AdtIndex` `constructor_types`/`constructor_parameters`/`private_constructors`; locals `declaration`/`return_type`/`type_arguments`/`builtin_required_arity`; `semantic_type_structure` tuple/pair/substitute binders; quotes ok (`"` char compares + string literals only). Gate: **961**/0; self-host diff empty. Commit `25b971c0`.

**STEP=10 note (2026-06-05 Driver):** `emit_helpers.mlc`/`hash_stmt_helpers.mlc` already compliant; comment fix; quotes ok. Gate: **961**/0; self-host diff empty. Commit `0a5cbcc6`.

**STEP=9 note (2026-06-05 Driver):** `CppToken.column`; scan `state`/`token`/`error`; `CppDeclarationResult.declaration`; `predicates.mlc` quotes ok. Gate: **961**/0; self-host diff empty. Commit `0428125c`.

**STEP=8 note (2026-06-05 Driver):** `CppField.type_value`, `CppProgram.declarations`; ripple parser/codegen/tests; `cpp_printer.mlc` already clean; quotes ok. Gate: **961**/0; self-host diff empty. Commit `a11b5372`.

**STEP=7 note (2026-06-05 Driver):** `types.mlc` locals `return_type_parsed`, `type_arguments`; string quotes: `"` char compares only — ok. Gate: **961**/0; self-host diff empty. Commit `3f61a746`.

**STEP=6 note (2026-06-05 Driver):** `decls.mlc` `parse_declaration`/`parse_function_declaration`; `return_type_parsed`, `type_parameters_parsed`, `declarations` locals; string quotes ok. Gate: **961**/0; self-host diff empty. Commit `ced551de`.

**STEP=5 note (2026-06-05 Driver):** `exprs.mlc` locals `expression`/`pattern`; `parse_pattern`/`parse_statement`/`parse_statements_until_*`; removed `let { }` destructuring; ripple `decls.mlc` `parse_record_pattern_fields`. String quotes: `"` only char compares — ok. Gate: **961**/0; self-host diff empty. Commit `36c8dfab`.

**STEP=4 note (2026-06-05 Driver):** `predicates.mlc` Parser `position`; result fields `expression`/`pattern`/`statement`/`declaration`/`patterns`; ripple `comma_separated`/`decls`/`exprs`/`test_parser`. No `let { field: local }` destructuring (parse error). Gate: **961**/0; self-host diff empty. Commit `bd26730b`.

**STEP=3 note (2026-06-05 Driver):** `frontend/lexer.mlc` LexState: `source`, `position`, `column`; ripple `cpp/lexer.mlc` (LexState only, `CppToken.col` step 9). Shadowing fix: `input_text`, `byte_index`, `token_column`. String quote audit: `"` only char compares — ok. Gate: **961**/0; self-host diff empty.

**STEP=2 note (2026-06-05 Driver):** `ast.mlc`/`ast_tokens.mlc` fields: `column`, `pattern`, `value`, `type_value`; ripple checker/codegen/parser/tests + `semantic_ir.mlc`. Fix: local `binding_span` (not `pattern_span`). Gate: **961**/0; self-host diff empty. Next: step 3 `lexer.mlc`.

**STEP=1 note (2026-06-05 Driver):** wip-rename stash applied; merged self-host fixes (`temp_name_counter` i32, `parameter_type_cpp` mut `&`). Gate: **961**/0; self-host diff empty.

**Planner (2026-05-19):** STRING_MATCH closing; **TYPE_ALIASES** next. Step 1 partial WIP in worktree (`SemanticExpression`, `TokenKind`, `Pattern` — verify `Pattern*` variants before gate). Do **not** blind re-run STEP=1; audit diff then continue step 2+.

**Goal:** ????????? ??? ?????????? ? ?????? ??????????, ??????????, ????? ? ?????. ????????????? ??????? ????????? ?????????. ?????? ????????? ? ????????? selfhost diff.

**??????? ???????:**
- `'text'` � ??? ?????? ??? ????????????
- `` `text ${x}` `` � ?????? ????????????
- `"text"` � ?? ????????????

**??????? ??????????????:**

| ?????????? | ?????? ??? |
|------------|-----------|
| `ctx` | `context` |
| `expr` (??????????) | `expression` |
| `SExpr` / `CppExpr` | `SemanticExpression` / `CppExpression` |
| `decl` (??????????) | `declaration` |
| `SDecl` / `CppDecl` | `SemanticDeclaration` / `CppDeclaration` |
| `pat` (??????????) | `pattern` |
| `Pat` / `PatKind` | `Pattern` / `PatternKind` |
| `typ` | `type_value` |
| `st` | `state` |
| `tok` (??????????) | `token` |
| `TKind` / `CppTKind` | `TokenKind` / `CppTokenKind` |
| `stmt` (??????????) | `statement` |
| `SStmt` / `CppStmt` | `SemanticStatement` / `CppStatement` |
| `col` | `column` |
| `ident` | `identifier` |
| `src` | `source` |
| `arg` | `argument` |
| `pos` | `position` |
| `val` | `value` |
| `err` | `error` |
| `ret` | `return_value` |
| `acc` | `accumulator` |
| `idx` | `index` |
| `len` | `length` |
| `buf` | `buffer` |
| `msg` | `message` |
| `res` | `result` |

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
| 1 | Core type renames: `SExpr`→`SemanticExpression`, `CppExpr`→`CppExpression`, `SDecl`→`SemanticDeclaration`, `CppDecl`→`CppDeclaration`, `SStmt`→`SemanticStatement`, `CppStmt`→`CppStatement`, `TKind`→`TokenKind`, `CppTKind`→`CppTokenKind`, `Pat`→`Pattern` | done |
| 2 | `frontend/ast.mlc` + `frontend/ast_tokens.mlc` � ?????????? + ??????? | done |
| 3 | `frontend/lexer.mlc` — поля + кавычки | done |
| 4 | `frontend/parser/predicates.mlc` � ?????????? + ??????? | done |
| 5 | `frontend/parser/exprs.mlc` � ?????????? + ??????? | done |
| 6 | `frontend/parser/decls.mlc` � ?????????? + ??????? | done |
| 7 | `frontend/parser/types.mlc` — abbreviations + quotes | done |
| 8 | `cpp/cpp_ast.mlc` + `cpp/cpp_printer.mlc` — abbreviations + quotes | done |
| 9 | `cpp/lexer.mlc` + `cpp/parser.mlc` + `cpp/predicates.mlc` — abbreviations + quotes | done |
| 10 | `cpp/emit_helpers.mlc` + `cpp/hash_stmt_helpers.mlc` — abbreviations + quotes | done |
| 11 | `checker/registry.mlc` + `checker/semantic_type_structure.mlc` � ?????????? | done |
| 12 | `checker/names.mlc` + `checker/check_mutations.mlc` � ?????????? | done |
| 13 | `checker/infer/` (??? ?????) � ?????????? | done |
| 14 | `checker/transform/` (??? ?????) � ?????????? | pending |
| 15 | `checker/check/` (??? ?????) � ?????????? | pending |
| 16 | `codegen/context.mlc` + `codegen/eval.mlc` + `codegen/module.mlc` � ?????????? + ??????? | pending |
| 17 | `codegen/decl/` (??? ?????) � ?????????? + ??????? | pending |
| 18 | `codegen/expr/` (??? ?????) � ?????????? + ??????? | pending |
| 19 | `codegen/stmt/` (??? ?????) � ?????????? + ??????? | pending |
| 20 | `ir/`, `main.mlc`, `pipeline.mlc`, `pass.mlc`, `compile_options.mlc` � ?????????? + ??????? | pending |
| 21 | `tests/` � ?????????? + ??????? | pending |
| 22 | Audit: ??? `"` ????? char-????????? ? ???????; ??? ?????????? ?? ??????? | pending |
| 23 | ??????? ???? | pending |

## Sub-steps (step 1 detail � ?????????????? ?????)

???? ???????????? ? import-???????? � ???????? ? ???????? ???????, ???? ?????.
- ???????: `ir/semantic_ir.mlc` ? `frontend/ast.mlc` ? parser ? checker ? codegen
- ?????? ???: rename + `compiler/build.sh` + ?????
