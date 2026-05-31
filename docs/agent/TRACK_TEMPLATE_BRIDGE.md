# Track: Native template fn decl codegen (Phase 2 §3 cont.)

Parent: [../PLAN.md](../PLAN.md) §Phase 2; previous: [TRACK_BINDING_BRIDGE.md](TRACK_BINDING_BRIDGE.md) (**closed**, `3904513`)

## Status: **closed** (`c668bb8` step 4 + step 5 audit)

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
| 1 | `CppFnProto`/`CppFnDef` — add `template_prefix` field + `cpp_printer` | done (`2bcfe2c`) |
| 2 | `native_fn_proto_cpp` — wire `template_prefix` + `type_bounds` for template fns | done (`7da561c`) |
| 3 | `native_fn_decl_cpp` — template fn bodies native | done (`fd04932`) |
| 4 | Drop template branch from `function_proto_needs_string_bridge`; keep `main()` audit | done (`c668bb8`) |
| 5 | `main()` native or documented survivor; audit; close track | done |

## Step 5 audit

**Track goal met:** template fn proto/def use native `CppFnProto`/`CppFnDef` via `function_declaration_template_prefix`. Template branch removed from `function_proto_needs_string_bridge`.

| Symbol / site | Callers in `compiler/**/*.mlc` | Notes |
|---------------|-------------------------------|-------|
| `function_proto_needs_string_bridge` | `decl_cpp.mlc` (`gen_fn_proto_cpp`, `gen_fn_decl_cpp`) | **main() only** — zero-param entry needs argc/argv + `set_args` preamble |
| `native_fn_proto_cpp` / `native_fn_decl_cpp` | `gen_fn_*_cpp` | all non-main fns including templates |
| `cpp_decl_from_string_output` | `decl_cpp.mlc`, `module.mlc`, `collect_fn_defs_cpp` | extend forwards; non-fn decls; main() survivor |
| `gen_fn_proto` / `gen_fn_decl` (string) | `decl.mlc` via bridge for main(); parallel path for non-fn decls | main uses `main_program_parameter_list` + `user_main_arguments_copy_into_runtime_statement` |

Production fn proto/def (non-main): `gen_fn_proto_cpp` / `gen_fn_decl_cpp` → `native_fn_*_cpp` with `template_prefix` when generic.

## Survivors (intentional)

| Site | Use |
|------|-----|
| `function_proto_needs_string_bridge` | zero-param `main()` proto/body string path |
| `cpp_decl_from_string_output` | extend forwards, non-fn decls, main() |
| `gen_stmts_str` / `stmt_cpp` fragment print | string parallel path |
| `expr_visitor_cpp.mlc` | unit-if block arm via string `gen_stmts` |
| `let_pat.mlc` / `match_gen.mlc` | string tuple destructure |
| `decl.mlc` | non-fn string codegen; main() argc/argv helpers |

## Deferred (out of track)

- Native zero-param `main()` (argc/argv proto + `set_args` preamble as `CppStmt` nodes).
- Extend forward segments native CppDecl.
- `gen_stmts_str` / string expr visitor fragment elimination.
- Phase 4 `MLCC_BOOTSTRAP=1`.
- Parser `ref mut` — [TRACK_PLAN.md](TRACK_PLAN.md) step 15.
- Security: parser panic audit continuation.
