# Track: Structured binding fragment elimination (Phase 2 §3 cont.)

Parent: [../PLAN.md](../PLAN.md) §Phase 2; previous: [TRACK_FRAGMENT_BRIDGE.md](TRACK_FRAGMENT_BRIDGE.md) (**closed**, `49b7091`)

## Status: **open**

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
| 1 | `CppStructuredBinding` in `cpp_ast` + `cpp_printer` + emit_helpers | pending |
| 2 | PatTuple / TPair non–PatIdent fallback — native structured binding | pending |
| 3 | Record-as-tuple fallback paths — native structured binding | pending |
| 4 | PatCtor complex subpatterns — native structured binding | pending |
| 5 | Remove `structured_binding_statement` fragment helper; audit; close track | pending |

## Context

FRAGMENT_BRIDGE step 5 left one production-native fragment site: `structured_binding_statement` in `let_pat_cpp.mlc` (4 call sites). Simple PatIdent tuple arms already use `std::get<N>` via `simple_tuple_ident_binding_statements` (step 1 of FRAGMENT_BRIDGE).

Call sites:

- `PatTuple` / `TPair` when `!tuple_subpatterns_are_simple_identifiers`
- Record member destructure when field count mismatch → tuple fallback
- Non-record `TTuple`/`TPair` type fallback
- `PatCtor` when ctor payload subpatterns are not all `PatIdent`

## Survivors (expected after close)

| Site | Use |
|------|-----|
| `function_proto_needs_string_bridge` | template fn + `main()` string decl path |
| `cpp_decl_from_string_output` | extend forwards, non-fn decls |
| `gen_stmts_str` / `stmt_cpp` fragment print | string parallel path |
| `expr_visitor_cpp.mlc` | unit-if block arm via string `gen_stmts` |
| `decl.mlc` | non-fn string codegen |

## Deferred (out of track)

- Native template fn defs — separate track.
- Extend forward segments native CppDecl.
- `gen_stmts_str` / string expr visitor fragment elimination.
- Phase 4 `MLCC_BOOTSTRAP=1`.
- Parser `ref mut` — [TRACK_PLAN.md](TRACK_PLAN.md) step 15.
- Security: parser panic audit continuation.
