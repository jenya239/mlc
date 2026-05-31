# Track: Native template fn decl codegen (Phase 2 §3 cont.)

Parent: [../PLAN.md](../PLAN.md) §Phase 2; previous: [TRACK_BINDING_BRIDGE.md](TRACK_BINDING_BRIDGE.md) (**closed**, `3904513`)

## Status: **open**

**Goal:** eliminate `function_proto_needs_string_bridge` fallback for template functions in `decl_cpp.mlc` — native `CppFnProto`/`CppFnDef` with `template<>` / `requires` prefix instead of `cpp_decl_from_string_output(gen_fn_decl(...))`.

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
| 1 | `CppFnProto`/`CppFnDef` — add `template_prefix` field + `cpp_printer` | done |
| 2 | `native_fn_proto_cpp` — wire `template_prefix` + `type_bounds` for template fns | done |
| 3 | `native_fn_decl_cpp` — template fn bodies native | pending |
| 4 | Drop template branch from `function_proto_needs_string_bridge`; keep `main()` audit | pending |
| 5 | `main()` native or documented survivor; audit; close track | pending |

## Context

PREFIX_BRIDGE + BINDING_BRIDGE closed all non-template native paths. `function_proto_needs_string_bridge` still true when:

- `type_params.length() > 0` (template functions)
- `name == 'main' && params.length() == 0`

String path uses `template_prefix(type_params) + requires_clause(...)` prepended in `decl.mlc` `gen_fn_proto` / `gen_fn_decl`. Native `CppFnProto`/`CppFnDef` currently omit template prefix.

## Survivors (expected after close)

| Site | Use |
|------|-----|
| `cpp_decl_from_string_output` | extend forwards, non-fn decls |
| `gen_stmts_str` / `stmt_cpp` fragment print | string parallel path |
| `expr_visitor_cpp.mlc` | unit-if block arm via string `gen_stmts` |
| `let_pat.mlc` / `match_gen.mlc` | string tuple destructure |
| `decl.mlc` | non-fn string codegen |

## Deferred (out of track)

- Extend forward segments native CppDecl.
- `gen_stmts_str` / string expr visitor fragment elimination.
- Phase 4 `MLCC_BOOTSTRAP=1`.
- Parser `ref mut` — [TRACK_PLAN.md](TRACK_PLAN.md) step 15.
- Security: parser panic audit continuation.
