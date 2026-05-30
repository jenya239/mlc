# Track: CppStmtFragment elimination (Phase 2 §3 cont.)

Parent: [../PLAN.md](../PLAN.md) §Phase 2; previous: [TRACK_PREFIX_BRIDGE.md](TRACK_PREFIX_BRIDGE.md) (**closed**, `174b82a`)

## Status: **closed** (`b90690a` step 4 + step 5 audit)

**Goal:** replace remaining `CppStmtFragment` string snippets in native stmt/expr codegen with structured `CppStmt` nodes (`let_pat_cpp`, `mut_actual_argument`, `return_body` `?` prelude).

## Verify gate (every step)

```
bundle exec rake test_compiler_mlc
compiler/build.sh
compiler/out/mlcc -o .tmp_selfhost/p1 compiler/main.mlc
compiler/build_bin.sh .tmp_selfhost/p1 .tmp_selfhost/mlcc2
.tmp_selfhost/mlcc2 -o .tmp_selfhost/p2 compiler/main.mlc
diff -rq .tmp_selfhost/p1 .tmp_selfhost/p2   # empty
```

---

| Step | Item | Status |
|------|------|--------|
| 1 | `let_pat_cpp` — PatTuple structured binding without `CppStmtFragment` | done (`24204e8`) |
| 2 | `let_pat_cpp` — PatRecord / PatArray / PatCtor destructure fragments | done (`7edfb94`) |
| 3 | `mut_actual_argument` — prelude `CppAutoDecl` native (no fragment lines) | done (`2ac5cd7`) |
| 4 | `return_body` — `?` unwrap prelude as native `CppIf`/`CppReturn` (no fragments) | done (`b90690a`) |
| 5 | Audit survivors; close track | done |

## Step 5 audit

**Track goal met:** production native stmt codegen for PatTuple (PatIdent arms), PatRecord, PatArray rest, PatCtor holds, mut-ref prelude, and return-body `?` unwrap no longer emit `CppStmtFragment`.

| Symbol / site | Callers in `compiler/**/*.mlc` | Notes |
|---------------|-------------------------------|-------|
| `structured_binding_statement` | `let_pat_cpp.mlc` | fallback `auto [a,b,...] = __lt` for non–PatIdent tuple arms, TPair, PatCtor destructure |
| `cpp_stmt_from_string_output` | `stmt_cpp.mlc`, `return_body.mlc` (string path) | string→CppStmt adapter; `print_cpp_statement_line` passthrough for fragments |
| `unit_if_branch_block` | `expr_visitor_cpp.mlc` | wraps `gen_stmts` string output in fragment; **string path only** (native `unit_if_branch_block_cpp` in `stmt_cpp.mlc` is fully native) |
| `function_proto_needs_string_bridge` | `decl_cpp.mlc` | template fns + `main()` string decl path |
| `cpp_decl_from_string_output` | `decl_cpp.mlc`, `module.mlc` | extend forwards, non-fn decls, template fn fallback |
| `gen_stmts_str` | `eval.mlc`, tests | string parallel path; not production native fn bodies |
| `CppStmtFragment` (infra) | `cpp_ast.mlc`, `emit_helpers.mlc`, `cpp_printer.mlc`, `test_cpp_printer.mlc` | AST node + printer + tests |

Production fn bodies (non-template): `collect_fn_defs_cpp` → `gen_fn_decl_cpp` → `native_fn_decl_cpp` → `gen_return_body_cpp` → `gen_stmts_cpp` / native expr visitor (no fragment stmts except `let_pat` structured-binding fallback).

## Survivors (intentional)

| Site | Use |
|------|-----|
| `let_pat_cpp.mlc` | `structured_binding_statement` for complex tuple/TPair/Ctor binds |
| `function_proto_needs_string_bridge` | template fn + `main()` string decl path |
| `cpp_decl_from_string_output` | extend forwards, non-fn decls |
| `gen_stmts_str` / `stmt_cpp` fragment print | string parallel path |
| `expr_visitor_cpp.mlc` | unit-if block arm via string `gen_stmts` (string path only) |
| `decl.mlc` | non-fn string codegen |

## Deferred (out of track)

- `structured_binding_statement` native CppAutoDecl/std::get for all tuple shapes — separate track.
- Native template fn defs — separate track after fragments stable.
- Extend forward segments native CppDecl.
- `gen_stmts_str` / string expr visitor fragment elimination — printer/string-path track.
- Phase 4 `MLCC_BOOTSTRAP=1` — after codegen bridges stable.
- Parser `ref mut` — [TRACK_PLAN.md](TRACK_PLAN.md) step 15.
- Security: parser panic audit continuation — after stability track or parallel Planner pick.
