# Track: Native zero-param main() codegen (Phase 2 §3 cont.)

Parent: [../PLAN.md](../PLAN.md) §Phase 2; previous: [TRACK_EXTEND_BRIDGE.md](TRACK_EXTEND_BRIDGE.md) (**closed**, `52f5c2f`)

## Status: **open**

**Goal:** eliminate `function_proto_needs_string_bridge` and string `gen_fn_proto`/`gen_fn_decl` for zero-param `main()` — native `CppFnProto`/`CppFnDef` with argc/argv parameters and `set_args` preamble.

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
| 1 | `main_program_parameter_items_cpp` — argc/argv param list for CppFnProto/CppFnDef | done |
| 2 | `gen_fn_proto_cpp` / `gen_fn_decl_cpp` — native main proto (drop string bridge branch) | done |
| 3 | `main_set_args_preamble_cpp` — native `mlc::io::set_args` prelude stmts | done (`25103b9`) |
| 4 | Remove `function_proto_needs_string_bridge`; wire always-native fn proto/decl | done |
| 5 | Audit survivors; close track | pending |

## Context

TEMPLATE_BRIDGE + EXTEND_BRIDGE closed template and extend forward native paths. Sole fn string bridge survivor:

- `function_proto_needs_string_bridge` → `name == 'main' && params.length() == 0`
- String helpers in `decl.mlc` / `expr.mlc`: `main_program_parameter_list`, `user_main_arguments_copy_into_runtime_statement`

## Survivors (expected after close)

| Site | Use |
|------|-----|
| `cpp_decl_from_string_output` | non-fn decls, trait struct phase 4, SDeclExtend body, `module.mlc` |
| `gen_stmts_str` / `stmt_cpp` | string parallel path |
| `decl.mlc` | non-fn string codegen |

## Deferred (out of track)

- Non-fn decl native (`gen_decl_cpp` / `collect_decl_parts_cpp` phases 0–2).
- `gen_stmts_str` / expr visitor string-path elimination.
- Phase 4 `MLCC_BOOTSTRAP=1`.
- Parser `ref mut` — [TRACK_PLAN.md](TRACK_PLAN.md) step 15.
