# Track: Eliminate string-concatenation codegen — CppAST only

Parent: [../PLAN.md](../PLAN.md) §2/§2.6. Prior work: [archive/tracks/TRACK_CPPEXPR.md](../archive/tracks/TRACK_CPPEXPR.md),
[archive/tracks/TRACK_CPPGEN.md](../archive/tracks/TRACK_CPPGEN.md) (closed 2026-05, established the
CppAST layer for leaf expressions — did **not** remove the string glue between them).

## Status: **active** (2026-07-17) — STEP=1 done, STEP=test-fix in progress, STEP=2 blocked on green Tier A

## Why this track exists

`docs/PLAN.md` §1/§7 claims "CppAST default; string bridges for edge cases" /
target "0% string bridges". Audit 2026-07-17 found this is **not** accurate:
string concatenation is not an edge case, it is the primary mechanism by
which statement bodies, function bodies, and control-flow-as-expression are
assembled. `GenStmtsResult { parts: [string], next_try: i32 }` and
`GenModuleOut { header: string, source: string }` (`compiler/codegen/context.mlc:62-63,77`)
are `string`-typed by design — every statement generator returns joined text,
not `[Shared<CppStatement>]`.

## Ground truth (exact inventory, 2026-07-17)

### 1. `compiler/codegen/expr/expr.mlc` — string-template module

390 → ~330 lines after STEP=1 dead-code removal. Every export takes/returns
`string` and does `${...}` interpolation — this **is** the string-concat
codegen path, not a helper around it.

Usage count per exported function (`grep -c` across `compiler/**/*.mlc`,
excluding self): 11 functions had **0** call sites (removed STEP=1); ~50 have
exactly 1; a handful (`suffix_semicolon_newline`: 31, `function_call_parentheses`:
14, `if_brace_block`: 8) are on the hot path for nearly every statement/call
in the compiler.

### 2. `CppAST` string-typed escape hatches (`compiler/cpp_ir/cpp_ast.mlc`)

| Variant | Field types that should not be `string` |
|---|---|
| `CppStatementFragment(string)` | whole statement as raw text |
| `CppDeclarationFragment(string)` | whole declaration as raw text |
| `CppInvokedWhile(Shared<CppExpression>, string)` | body — should be `[Shared<CppStatement>]` |
| `CppInvokedFor(string, Shared<CppExpression>, string)` | body — should be `[Shared<CppStatement>]` |
| `CppInvokedBlock(string)` | body — should be `[Shared<CppStatement>]` |
| `CppInvokedBlockWithReturn(string, string)` | return type + body |
| `CppWithBlock(Shared<CppExpression>, string, string)` | body |
| `CppFnProto(string,string,string,[string])` / `CppFnDef(...,[string],...)` | return type + params as strings, not `CppType`/`CppParam` |
| `CppVariant`, `CppConceptRequires`, `CppStaticAssert`, `CppUsing`, `CppAggregateInit` | type lists / names as `[string]`/`string` |

Construction call sites for the `*Fragment`/`Invoked*` variants (production,
excludes `tests/`): `compiler/expr_visitor_cpp.mlc` (7), `compiler/codegen/expr/match_gen.mlc` (3),
`compiler/codegen/expr/record_gen.mlc` (1), `compiler/codegen/stmt_cpp.mlc` (1),
`compiler/codegen/decl_cpp.mlc` (3), `compiler/codegen/stmt/let_pat_cpp.mlc` (2),
`compiler/cpp_ir/mir_to_cpp.mlc` (1), `compiler/codegen/expr/match_gen.mlc` (2),
`compiler/codegen/decl/derive_methods_cpp.mlc` (2), `compiler/cpp_parse/parser/cpp_decls.mlc`/`to_source.mlc`
(C++ header **parser**, round-trips foreign source text — out of scope, a
parsed-then-reprinted `#define`/typedef fragment is not "our" codegen).

### 3. Root cause — `CodegenContext` types are string-typed

`compiler/codegen/context.mlc`:
- `GenStmtsResult { parts: [string], next_try: i32 }` (line 62)
- `GenStmtsWithContext { statements_parsed: GenStmtsResult, codegen_context: CodegenContext }` (line 63)
- `GenStmtResult { output: string, next_try: i32, codegen_context: CodegenContext }` (line 64)
- `GenModuleOut { header: string, source: string, link_libraries: [string] }` (line 77)

`compiler/codegen/eval.mlc` — `gen_expr` **already** builds a real
`Shared<CppExpression>` via `expr_visitor_cpp.eval_expr_cpp`, then immediately
calls `print_expr` and returns `string` (line 21-22). Every caller receives
already-printed text and string-concatenates it further. This is the actual
bridge point: fixing it means `gen_expr`/`gen_stmts_str` must return AST nodes,
not call the printer internally — and every one of their ~40+ call sites
across `decl/*.mlc`, `stmt/*.mlc`, `expr/*.mlc` must accept an AST node instead
of a string.

## Decision (frozen 2026-07-17)

| Item | Choice |
|------|--------|
| Scope | Production codegen only (`compiler/codegen/**`, `compiler/expr_visitor_cpp.mlc`, `compiler/cpp_ir/**`). `compiler/cpp_parse/**` (foreign C++ header parser) stays out of scope — it re-emits parsed foreign text, unrelated to MLC→C++ codegen. |
| Target | `GenStmtsResult.parts: [Shared<CppStatement>]`; `GenModuleOut.header/source: [Shared<CppDeclaration>]`; `gen_expr` returns `Shared<CppExpression>` (printer called once, at `CppFile` emission in `module.mlc`) |
| `CppInvokedWhile/For/Block/BlockWithReturn/WithBlock` | body fields become `[Shared<CppStatement>]`; printer already has `print_statements`, reuse it |
| `CppStatementFragment`/`CppDeclarationFragment` | removed once every construction site is converted; if a genuine escape hatch remains needed (should not, after conversion) it stays but every current caller must be proven necessary, not default |
| `CppFnProto`/`CppFnDef` params/return type | later sub-track once body conversion lands — do not do both in one step (violates one-concern-per-step) |
| Order | Bottom-up: leaf `expr.mlc` dead code (done) → `CppInvokedBlock`/`While`/`For`/`WithBlock` body strings → `gen_stmts_str`/`GenStmtsResult` → `GenModuleOut` → delete `expr.mlc` once zero callers remain |
| Verify per step | `compiler/out/mlcc --check-only compiler/main.mlc`; `compiler/out/mlcc -o .tmp_verify/after compiler/main.mlc` then diff against a `git stash`-based `before` build — only touched modules' `.cpp`/`.hpp` may differ; `bash scripts/regression_gate.sh` before closing the track |
| `dev_gate_fast.sh` / Ruby `run_tests` | **currently red independent of this track** (`Unexpected token: MATCH` parsing `compiler/tests/tests_main.mlc` dependency via Ruby bootstrap — self-hosted `mlcc` syntax the Ruby parser does not support). Pre-existing, reproduced on clean `main`. Not this track's regression; flagged separately below |

## Known pre-existing blocker (found during this audit, not caused by this track)

`bash scripts/dev_gate_fast.sh` fails on a clean tree building `compiler/out/tests/run_tests`
through the Ruby bootstrap. Onion (2026-07-17 test-fix): (1) nested `match` as
primary after `&&` — Ruby `parse_primary`; (2) `CaptureAnalyzer` `Return.expr` (worktree,
cursorfs blocks `git add`); (3) `region_escape.mlc` `let mut`; (4) `VmRecord(record)`
keyword binding → `record_value`; (5) `test_spawn.mlc` `'error['+code+']'` — `']'` is
char, not string — fixed to `"..."`. **Still red:** after semantic OK, g++ on
Ruby-generated `compiler/out/tests/*.cpp` fails with missing cross-module names
(`infer_match::match_exhaustiveness_diagnostic`, `method_arity_after_receiver`, …).
Tier A unusable until that layer is fixed or `build_tests_fast` stops using Ruby for
the full compiler+tests graph; STEP=2 stays blocked.

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | Delete 11 zero-usage functions from `expr.mlc`; verify self-host diff touches only `expr.cpp/.hpp` | **done** (2026-07-17) |
| test-fix | Bisect `dev_gate_fast.sh` / Ruby `run_tests` build failure (pre-existing, blocks Tier A) | **in progress** — MATCH/`let mut`/`record`/string+char (`test_spawn` `']'` char lit) fixed; next: Ruby-codegen C++ namespace errors linking `run_tests` |
| 2 | `CppInvokedBlock`/`CppInvokedBlockWithReturn` body: `string` → `[Shared<CppStatement>]`; convert `expr_visitor_cpp.mlc` (2 sites) + `match_gen.mlc` (3) + `record_gen.mlc` (1) construction; printer already has `print_statements` | pending |
| 3 | `CppInvokedWhile`/`CppInvokedFor`/`CppWithBlock` body: same treatment (`expr_visitor_cpp.mlc`, 3 sites) | pending |
| 4 | `GenStmtsResult.parts` → `[Shared<CppStatement>]` in `context.mlc`; update `append_stmt`/`joined_code` callers across `stmt/stmt_eval.mlc`, `stmt_cpp.mlc` | pending |
| 5 | `gen_stmts_str`/`gen_expr` in `eval.mlc` stop calling `print_expr`/`print_cpp_statements` internally; return AST nodes; update the ~40 call sites that currently consume the string result | pending |
| 6 | `GenModuleOut.header/source` → `[Shared<CppDeclaration>]`; single `print_file` call in `codegen/module.mlc` | pending |
| 7 | Remove `CppStatementFragment`/`CppDeclarationFragment` construction sites once callers pass real AST (11 sites, see inventory) | pending |
| 8 | Delete `compiler/codegen/expr/expr.mlc` once zero callers remain; delete corresponding `render_*`/mirror functions in `cpp_emit/print.mlc` if now unreachable | pending |
| 9 | `scripts/regression_gate.sh` + Tier B (`compiler/tests/build_tests.sh`) + self-host `mlcc`→`mlcc2` diff identical; update `docs/PLAN.md` §1/§7 metric row | pending, closes track |

## Non-goals (this track)

- `CppFnProto`/`CppFnDef` params-as-string / return-type-as-string (separate
  sub-track after body conversion; touches decl codegen, different call graph).
- `compiler/cpp_parse/**` (C++ header parser/reprinter) — foreign text
  round-trip, not MLC codegen.
- Any change to checker/, MIR, or SemanticIR.
- Risk note for whoever picks up STEP=2+: this touches the **core mutual
  recursion** (`gen_expr` ⇄ `gen_stmts_str`) used by every function body in
  the self-hosted compiler. Each step must be verified individually (Tier B,
  not just `--check-only`) before the next — this is not a one-shot rewrite,
  matching how `TRACK_CPPEXPR`/`TRACK_CPPGEN` were originally split.
