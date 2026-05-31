# Track: Native extend forward segments (Phase 2 §3 cont.)

Parent: [../PLAN.md](../PLAN.md) §Phase 2; previous: [TRACK_TEMPLATE_BRIDGE.md](TRACK_TEMPLATE_BRIDGE.md) (**closed**, `6db2014`)

## Status: **closed** (`7ab42a8` step 4 + step 5 audit)

**Goal:** eliminate `cpp_decl_from_string_output` for extend forward segments in `collect_fn_defs_cpp` — native `CppFnProto`/`CppFnDef` instead of string fragments from `extend_method_forward_segments` / `extend_cpp_forward_segment`.

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
| 1 | `extend_method_forward_segments_cpp` — method forward protos via `gen_proto_cpp` | done (`315acee`) |
| 2 | `gen_extend_wrapper_protos_cpp` — trait wrapper forward protos native | done (`376f0fc`) |
| 3 | `extend_cpp_forward_segment_cpp` — SDeclExtend forward bundle native | done (`fe78e3a`) |
| 4 | Wire `collect_fn_defs_cpp` — drop string append for extend forwards | done (`7ab42a8`) |
| 5 | Audit survivors (`cpp_decl_from_string_output`, main); close track | done |

## Step 5 audit

**Track goal met:** `collect_fn_defs_cpp` extend forward paths are native — no `cpp_decl_from_string_output` for `extend_method_forward_segments` or `extend_cpp_forward_segment`.

| Symbol / site | Callers in `compiler/**/*.mlc` | Notes |
|---------------|-------------------------------|-------|
| `extend_method_forward_segments_cpp` | `collect_fn_defs_cpp` | per-extend method forward protos via `gen_proto_cpp` (outer context) |
| `gen_extend_wrapper_protos_cpp` | `extend_cpp_forward_segment_cpp` | trait wrapper `CppFnProto` list |
| `extend_cpp_forward_segment_cpp` | `collect_fn_defs_cpp` | wrapper protos + method protos with `extend_context` |
| `extend_method_forward_segments` / `extend_cpp_forward_segment` (string) | `decl.mlc` `collect_all_decl_parts` | parallel string bundle path |
| `cpp_decl_from_string_output` | `gen_decl_cpp`, `gen_proto_cpp` (non-fn), main(), phase-4 trait struct, `module.mlc` | intentional survivors |
| `append_string_segments` | `collect_fn_defs_cpp` phase 4 only | `gen_trait_struct` string segments |

Production extend forwards: `collect_fn_defs_cpp` → `extend_method_forward_segments_cpp` + `extend_cpp_forward_segment_cpp` → native `CppFnProto`.

## Survivors (intentional)

| Site | Use |
|------|-----|
| `function_proto_needs_string_bridge` | zero-param `main()` |
| `cpp_decl_from_string_output` | non-fn decls, main(), trait struct phase 4, `module.mlc` |
| `gen_stmts_str` / `stmt_cpp` | string parallel path |
| `decl.mlc` | non-fn string codegen; string `collect_all_decl_parts` |

## Deferred (out of track)

- Native zero-param `main()` (argc/argv + set_args).
- Non-fn decl native (`gen_decl_cpp` / `gen_proto_cpp` for SDeclType/SDeclTrait).
- `gen_stmts_str` / expr visitor string-path elimination.
- Phase 4 `MLCC_BOOTSTRAP=1`.
- Parser `ref mut` — [TRACK_PLAN.md](TRACK_PLAN.md) step 15.
