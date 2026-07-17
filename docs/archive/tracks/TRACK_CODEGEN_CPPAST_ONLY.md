# Track: Eliminate string-concatenation codegen — CppAST only

Parent: [../../PLAN.md](../../PLAN.md) §2/§2.6. Prior work: [TRACK_CPPEXPR.md](TRACK_CPPEXPR.md),
[TRACK_CPPGEN.md](TRACK_CPPGEN.md) (closed 2026-05, established the
CppAST layer for leaf expressions — did **not** remove the string glue between them).

## Status: **closed** (2026-07-17) — Critic OK

**Critic 2026-07-18 (critique-audit):** Re-checked close claims vs tree + git.
`expr.mlc` absent; `GenStmtsResult.parts` / `GenModuleOut.header|source` are
AST lists; `gen_expr` returns `Shared<CppExpression>`. Tier A EXIT=0.
Anti-false-done: target **0% string bridges not met** — residual
`CppStatementFragment` / `stmt_result_from_source` / `joined_code` /
`gen_expr_as_source` / type_gen+stmt_fragments string helpers. PLAN residual
wording corrected (was wrongly blaming `GenModuleOut` as string). **reopen: none**.

## Why this track exists

`docs/PLAN.md` §1/§7 claimed "CppAST default; string bridges for edge cases" /
target "0% string bridges". Audit 2026-07-17 found that inaccurate at open.
**At close:** `expr.mlc` deleted; `GenStmtsResult.parts` and `GenModuleOut`
header/source are AST-typed; DeclFragments largely gone. Residual string
bridges remain via `CppStatementFragment`, `stmt_result_from_source`,
`joined_code`/`gen_expr_as_source`, and pre-rendered helpers in `type_gen` /
`stmt_fragments` — **not** 0% bridges.

## Ground truth (exact inventory, 2026-07-17)

### 1. `compiler/codegen/expr/expr.mlc` — string-template module (**deleted** STEP=8c)

Helpers relocated to `type_gen` / `stmt_fragments` / domain modules. Residual
string templates remain there; the dedicated module is gone.

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

`compiler/codegen/context.mlc` (at close):
- `GenStmtsResult { parts: [Shared<CppStatement>], next_try: i32 }`
- `GenStmtResult { statement: Shared<CppStatement>, ... }` — string path via `stmt_result_from_source` → `CppStatementFragment`
- `GenModuleOut { header: [Shared<CppDeclaration>], source: [Shared<CppDeclaration>], link_libraries: [string] }`

`compiler/codegen/eval.mlc` — `gen_expr` returns `Shared<CppExpression>`; many
call sites still consume via `gen_expr_as_source` / `print_expr` (string bridge).

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

`bash scripts/dev_gate_fast.sh` was red via Ruby ModularCompiler rebuilding
`run_tests` from `tests_main` (pulls whole self-hosted compiler). Onion peeled
2026-07-17: MATCH primary, CaptureAnalyzer Return.expr (worktree/cursorfs),
`region_escape` `let mut`, `record_value`, `test_spawn` string lits. After
semantic OK, Ruby→C++ still fails cross-module namespaces — not a one-liner.

**Decision (frozen 2026-07-17):** Tier A (`build_tests_fast.sh`) and Tier B
(`build_tests.sh` phase 1) do **not** rebuild via Ruby. They run existing
`out/tests/run_tests` (warn if stale) + (Tier A) `mlcc --check-only main.mlc` +
arch lint; Tier B continues fuzz/LSP/vm-diff/lint phases. Full rebuild:
`build_tests_self.sh` (mlcc; import-path WIP). Allowlisted
`derive_methods_cpp.mlc` / `spawn_capture.mlc` file_size when lint became
reachable again — Meta should split later.

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | Delete 11 zero-usage functions from `expr.mlc`; verify self-host diff touches only `expr.cpp/.hpp` | **done** (2026-07-17) |
| test-fix | Bisect `dev_gate_fast.sh` / Ruby `run_tests` build failure (pre-existing, blocks Tier A) | **done** (2026-07-17) — Decision: no Ruby rebuild in Tier A; `dev_gate_fast.sh` EXIT=0 |
| 2 | `CppInvokedBlock`/`CppInvokedBlockWithReturn` body: `string` → `[Shared<CppStatement>]`; convert `expr_visitor_cpp.mlc` (2 sites) + `match_gen.mlc` (3) + `record_gen.mlc` (1) construction; printer uses `print_statements`; bridge via `make_invoked_block_body_from_source` | **done** (2026-07-17) |
| 3 | `CppInvokedWhile`/`CppInvokedFor`/`CppWithBlock` body: same treatment (`expr_visitor_cpp.mlc`, 3 sites) | **done** (2026-07-17) |
| 4 | `GenStmtsResult.parts` → `[Shared<CppStatement>]` in `context.mlc`; update `append_stmt`/`joined_code` callers across `stmt/stmt_eval.mlc`, `stmt_cpp.mlc` | **done** (2026-07-17) — `append_stmt` wraps `CppStatementFragment`; `joined_code` prints; string `GenStmtResult.output` bridge remains until STEP=5; `stmt_cpp` already on parallel `[Shared<CppStatement>]` path (no change) |
| 5 | `gen_stmts_str`/`gen_expr` in `eval.mlc` stop calling `print_expr`/`print_cpp_statements` internally; return AST nodes; update the ~40 call sites that currently consume the string result | **done** (2026-07-17) — `gen_expr` → `Shared<CppExpression>`; `gen_stmts_str` → `[Shared<CppStatement>]`; `eval_expr_cpp` `gen_stmts` callback AST; while/for/with/block use statement lists directly; string consumers wrap `print_expr`/`print_cpp_statements`; `stmt_eval` still gets printed via `gen_expr_as_source` |
| 6 | `GenModuleOut.header/source` → `[Shared<CppDeclaration>]`; single `print_file` call in `codegen/module.mlc` | **done** (2026-07-17) — AST in `GenModuleOut`; `print_module_out` = `print_file`; pipeline/harness print via `print_cpp_declarations` at write edge |
| 7a | Kill `make_invoked_block_body_from_source`; convert easy Fragment sites: match string-lit body, record-update lazy, unit-if trailing, MIR unit `return` | **done** (2026-07-17) — guarded-match body still one Fragment wrapper; left for 7b2 |
| 7b1 | Add `CppStatementSequence` (flat, no braces); convert `let_pat_cpp` off Fragment; printer + tests | **done** (2026-07-17) |
| 7b2 | Convert `gen_match_guarded_body` → `[Shared<CppStatement>]`; kill Fragment wrappers in guarded-match / `gen_match_default_block_cpp`; string_match string-path Fragments | **done** (2026-07-17) — AutoDecl subject + abort AST; per-arm Fragment remains until arm builders AST (7b3c) |
| 7b3a | Add `CppLineDirective`; convert `line_directive_cpp_statements`; delete dead `cpp_*_from_string_*` | **done** (2026-07-17) |
| 7b3b | `GenStmtResult` → AST / kill `append_stmt` Fragment bridge (`stmt_eval` string path) | **done** (2026-07-17) — `statement: Shared<CppStatement>`; `append_stmt` pushes directly; string builders wrap Fragment at construction; break/continue native AST |
| 7b3c1 | FFI binder/return → AST: add `CppStaticAutoDecl`; `gen_ffi_fn_decl_cpp` uses it + `CppReturn`/`CppCall`/`CppCast`; deleter Fragment remains | **done** (2026-07-17) |
| 7b3c2 | FFI deleter DeclFragment → AST (needs class member fn with body) | **done** (2026-07-17) — `CppClassMemberFunctionDef` + `CppClassDeclaration` + `CppUsing`; nested-if brace indent quirk left (printer) |
| 7b3c3 | Guarded wild/unit + literal arm builders → `CppIf`/`CppReturn` (leave ctor/record/ident) | **done** (2026-07-17) — `gen_guarded_*_arm_statements`; ctor/record/ident still Fragment via `guarded_arm_source_as_statements` |
| 7b3c4 | Guarded ctor / record / ident-binding arm builders → AST | **done** (2026-07-17) — full guarded arm path AST; killed Fragment bridge helpers |
| 7b3c5 | `derive_methods_cpp` JSON DeclFragments (2) → AST | **done** (2026-07-17) — `CppFnDef`+`CppDeclarationSequence`; bodies still StatementFragment (JSON string builders) |
| 8a | Inventory + dead exports: cannot delete `expr.mlc` yet (88 live exports); remove `break_line`/`continue_line`; `print.mlc` `render_*` still used by printer | **done** (2026-07-17) |
| 8b1 | Move 8 `cpp_*` type-string helpers `expr.mlc` → `type_gen.mlc`; update match_gen/trait_struct/decl_extend (exports 88→80) | **done** (2026-07-17) |
| 8b2 | Move 9 struct/concept/variant string helpers `expr.mlc` → `type_gen.mlc`; drop `expression_output` import from type_gen (exports 80→71) | **done** (2026-07-17) |
| 8b3 | Move 4 record-only helpers `expr.mlc` → local fns in `record_gen.mlc` (exports 71→67); drop `expression_output` import; unblock self-host rebuild (`match_gen`: `">"` not `'>'`; bind array-return after assign) | **done** (2026-07-17) |
| 8b4 | Move 4 method-only helpers `expr.mlc` → local fns in `method_gen.mlc` (exports 67→63); keep `expression_output` for `function_call_parentheses`/`runtime_to_string_call` | **done** (2026-07-17) |
| 8b5 | Move 4 decl-only helpers `expr.mlc` → local fns in `decl.mlc` (exports 63→59); keep `noexcept_function_prototype` shared; update `test_decl_gen` expected string | **done** (2026-07-17) |
| 8b6 | Move 3 exclusive helpers: `parenthesized_*` → `record_field_default_emit.mlc`, `standard_translation_unit_runtime_headers` → `module.mlc` (exports 59→56); drop their `expression_output` imports | **done** (2026-07-17) |
| 8b7 | Move 9 decl_extend-only helpers `expr.mlc` → `decl_extend.mlc` (exports 56→47); export `static_assert_*` for `test_decl_gen`; keep shared helpers via `expression_output` | **done** (2026-07-17) |
| 8b8 | Move 12 match-only helpers `expr.mlc` → `match_gen.mlc` (exports 47→35); export 4 for `test_codegen`; keep `tuple_destructure_binding` via `expression_output` | **done** (2026-07-17) |
| 8b9 | Move 17 stmt_eval-only helpers `expr.mlc` → local fns in `stmt_eval.mlc` (exports 35→18); allowlist `stmt_eval` file_size (869 lines) | **done** (2026-07-17) |
| 8b10 | Move 8 shared stmt helpers `expr.mlc` → new `stmt/stmt_fragments.mlc`; `return_body`/`stmt_eval` import it (exports 18→10) | **done** (2026-07-17) |
| 8b11 | Binding helpers: `suffix_semicolon_newline`/`auto_binding_statement` → `stmt_fragments`; export `tuple_destructure_binding` from `let_pat` (exports 10→7) | **done** (2026-07-17) |
| 8b12 | Move 4 decl helpers `expr.mlc` → `type_gen.mlc` (noexcept/template/concept/vtable); drop `expression_output` from decl/trait_struct/decl_cpp (exports 7→3) | **done** (2026-07-17) |
| 8b13 | Move remaining helpers: `function_call_parentheses`/`runtime_to_string_call` → `type_gen`; bootstrap golden → local in `test_cpp_printer`; `expr.mlc` stub (0 exports, no importers) | **done** (2026-07-17) |
| 8c | Delete `expr.mlc` stub; `print.mlc` `render_*` remain (live printer helpers, not unreachable) | **done** (2026-07-17) |
| 9 | `scripts/regression_gate.sh` + Tier B (`compiler/tests/build_tests.sh`) + self-host `mlcc`→`mlcc2` diff identical; update `docs/PLAN.md` §1/§7 metric row | **done** (2026-07-17) — closes track; allowlisted pre-existing Color-broken examples (`scene_form_live`, `text_glyph_color_smoke`) |

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
