# Track: Native extend forward segments (Phase 2 §3 cont.)

Parent: [../PLAN.md](../PLAN.md) §Phase 2; previous: [TRACK_TEMPLATE_BRIDGE.md](TRACK_TEMPLATE_BRIDGE.md) (**closed**, `6db2014`)

## Status: **open**

**Goal:** eliminate `cpp_decl_from_string_output` for extend forward segments in `collect_fn_defs_cpp` — native `CppFnProto`/`CppFnDef` (or structured `CppDecl`) instead of string fragments from `extend_method_forward_segments` / `extend_cpp_forward_segment`.

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
| 1 | `extend_method_forward_segments_cpp` — method forward protos via `gen_proto_cpp` | done |
| 2 | `gen_extend_wrapper_protos_cpp` — trait wrapper forward protos native | done |
| 3 | `extend_cpp_forward_segment_cpp` — SDeclExtend forward bundle native | done |
| 4 | Wire `collect_fn_defs_cpp` — drop string append for extend forwards | done |
| 5 | Audit survivors (`cpp_decl_from_string_output`, main); close track | pending |

## Context

TEMPLATE_BRIDGE closed template + main survivor audit. `collect_fn_defs_cpp` still pushes string segments:

- `append_string_segments(output, extend_method_forward_segments(...))` — per-extend method protos
- `extend_cpp_forward_segment` → `cpp_decl_from_string_output` — trait wrapper protos + method protos for `SDeclExtend`

String helpers live in `decl.mlc` / `decl_extend.mlc` (`gen_extend_wrapper_protos`, `gen_proto`). Native fn proto path exists (`gen_proto_cpp` / `gen_fn_proto_cpp`).

## Survivors (expected after close)

| Site | Use |
|------|-----|
| `function_proto_needs_string_bridge` | zero-param `main()` |
| `cpp_decl_from_string_output` | non-fn decls (type/trait), main(), `collect_decl_parts_cpp` phases 0–2 |
| `gen_stmts_str` / `stmt_cpp` | string parallel path |
| `decl.mlc` | non-fn string codegen |

## Deferred (out of track)

- Native zero-param `main()` (argc/argv + set_args).
- Non-fn decl native (`gen_decl_cpp` / `gen_proto_cpp` for SDeclType/SDeclTrait).
- `gen_stmts_str` / expr visitor string-path elimination.
- Phase 4 `MLCC_BOOTSTRAP=1`.
- Parser `ref mut` — [TRACK_PLAN.md](TRACK_PLAN.md) step 15.
