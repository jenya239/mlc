# Track: Structured binding fragment elimination (Phase 2 §3 cont.)

Parent: [../PLAN.md](../PLAN.md) §Phase 2; previous: [TRACK_FRAGMENT_BRIDGE.md](TRACK_FRAGMENT_BRIDGE.md) (**closed**, `49b7091`)

## Status: **closed** (`ba1f723` step 4 + step 5 audit)

**Goal:** replace `structured_binding_statement` `CppStmtFragment` in `let_pat_cpp.mlc` with native `CppStmt` nodes for all let-pat tuple/TPair/Ctor destructure fallbacks.

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
| 1 | `CppStructuredBinding` in `cpp_ast` + `cpp_printer` + emit_helpers | done (`a4e6d92`) |
| 2 | PatTuple / TPair non–PatIdent fallback — native structured binding | done (`8d53184`) |
| 3 | Record-as-tuple fallback paths — native structured binding | done (`edd01c2`) |
| 4 | PatCtor complex subpatterns — native structured binding | done (`ba1f723`) |
| 5 | Remove `structured_binding_statement` fragment helper; audit; close track | done |

## Step 5 audit

**Track goal met:** `let_pat_cpp.mlc` has no `CppStmtFragment` / `make_fragment_cpp_statement`. All structured-binding fallbacks use `CppStructuredBinding` via `tuple_structured_binding_statement`.

| Symbol / site | Callers in `compiler/**/*.mlc` | Notes |
|---------------|-------------------------------|-------|
| `CppStructuredBinding` | `let_pat_cpp.mlc` via `emit_helpers` | production native let-pat tuple/ctor destructure |
| `make_fragment_cpp_statement` | `stmt_cpp.mlc`, `expr_visitor_cpp.mlc` | string path adapters only |
| `pat_bind_names` | `let_pat.mlc`, `match_gen.mlc` | string parallel path |
| `function_proto_needs_string_bridge` | `decl_cpp.mlc` | template fn + `main()` string decl path |
| `cpp_decl_from_string_output` | `decl_cpp.mlc`, `module.mlc` | extend forwards, non-fn decls |
| `gen_stmts_str` | `eval.mlc`, tests | string parallel path |

Production fn bodies (non-template): native let-pat via `gen_stmts_cpp` → `let_pat_cpp` (no fragments).

## Survivors (intentional)

| Site | Use |
|------|-----|
| `function_proto_needs_string_bridge` | template fn + `main()` string decl path |
| `cpp_decl_from_string_output` | extend forwards, non-fn decls |
| `gen_stmts_str` / `stmt_cpp` fragment print | string parallel path |
| `expr_visitor_cpp.mlc` | unit-if block arm via string `gen_stmts` |
| `let_pat.mlc` / `match_gen.mlc` | string tuple destructure |
| `decl.mlc` | non-fn string codegen |

## Deferred (out of track)

- Native template fn defs — separate track.
- Extend forward segments native CppDecl.
- `gen_stmts_str` / string expr visitor fragment elimination.
- Phase 4 `MLCC_BOOTSTRAP=1`.
- Parser `ref mut` — [TRACK_PLAN.md](TRACK_PLAN.md) step 15.
- Security: parser panic audit continuation.
