# Track: Native zero-param main() codegen (Phase 2 §3 cont.)

Parent: [../PLAN.md](../PLAN.md) §Phase 2; previous: [TRACK_EXTEND_BRIDGE.md](TRACK_EXTEND_BRIDGE.md) (**closed**, `52f5c2f`)

## Status: **closed** (`f2aa065` step 4 + step 5 audit)

**Goal:** eliminate string `gen_fn_proto`/`gen_fn_decl` bridge for zero-param `main()` — native `CppFnProto`/`CppFnDef` with argc/argv parameters and `set_args` preamble.

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
| 1 | `main_program_parameter_items_cpp` — argc/argv param list for CppFnProto/CppFnDef | done (`0987fff`) |
| 2 | `gen_fn_proto_cpp` / `gen_fn_decl_cpp` — native main proto | done (`7a3e06e`) |
| 3 | `main_set_args_preamble_cpp` — native `mlc::io::set_args` prelude stmts | done (`25103b9`) |
| 4 | Remove `function_decl_needs_string_bridge`; wire always-native fn decl | done (`f2aa065`) |
| 5 | Audit survivors; close track | done |

## Step 5 audit

**Track goal met:** production fn proto/def for zero-param `main()` use native `CppFnProto`/`CppFnDef` — argc/argv via `main_program_parameter_items_cpp`, `set_args` via `main_set_args_preamble_cpp` prepended in `native_fn_decl_cpp`. No `function_decl_needs_string_bridge` / `function_proto_needs_string_bridge` in `compiler/**/*.mlc`.

| Symbol / site | Callers in `compiler/**/*.mlc` | Notes |
|---------------|-------------------------------|-------|
| `gen_fn_proto_cpp` / `gen_fn_decl_cpp` | `gen_decl_cpp`, `gen_proto_cpp`, `collect_fn_defs_cpp` | always `native_fn_*_cpp` (incl. zero-param `main()`) |
| `main_program_parameter_items_cpp` | `function_parameter_*_items` | `['int argc', 'char** argv']` when `name == 'main' && params.length() == 0` |
| `main_set_args_preamble_cpp` | `prepend_main_set_args_preamble` → `native_fn_decl_cpp` | native `CppCall` to `mlc::io::set_args` |
| `gen_fn_proto` / `gen_fn_decl` (string) | `decl.mlc`, `collect_all_decl_parts` | parallel string bundle; main argc/argv + preamble still in string path |
| `main_program_parameter_list` / `user_main_arguments_copy_into_runtime_statement` | `decl.mlc` string `gen_fn_*` | parity helpers; tests compare native param list |
| `cpp_decl_from_string_output` | `gen_decl_cpp`, `gen_proto_cpp` (non-fn), phase-4 trait struct, `module.mlc` | not used for fn proto/def |

Production zero-param `main()`: `gen_fn_decl_cpp` → `native_fn_decl_cpp` → argc/argv params + `prepend_main_set_args_preamble`.

## Survivors (intentional)

| Site | Use |
|------|-----|
| `cpp_decl_from_string_output` | non-fn decls, trait struct phase 4, SDeclExtend body, `module.mlc` |
| `gen_stmts_str` / `stmt_cpp` | string parallel path |
| `decl.mlc` | non-fn string codegen; string `gen_fn_proto`/`gen_fn_decl` main() branch (parallel bundle) |
| `expr.mlc` | `main_program_parameter_list`, `user_main_arguments_copy_into_runtime_statement` (string path + parity tests) |

## Deferred (out of track)

- Non-fn decl native (`gen_decl_cpp` / `collect_decl_parts_cpp` phases 0–2).
- `gen_stmts_str` / expr visitor string-path elimination.
- Phase 4 `MLCC_BOOTSTRAP=1`.
- Parser `ref mut` — [TRACK_PLAN.md](TRACK_PLAN.md) step 15.
