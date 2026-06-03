# Track: Native fn proto codegen (Phase 2 §3 cont.)

Parent: [../PLAN.md](../PLAN.md) §Phase 2; previous: [TRACK_TRAIT_STRUCT_BRIDGE.md](TRACK_TRAIT_STRUCT_BRIDGE.md) (**closed**, step 5 audit)

## Status: **closed** (step 5 audit, uncommitted)

**Goal:** eliminate `cpp_decls_from_string_parts(collect_all_decl_parts(...).fn_protos)` — native `CppFnProto` for phase-2 fn protos in production bundle instead of string fragments.

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
| 1 | `decl_segment_cpp` phase 2 — `SDeclFn` → `gen_proto_cpp` | done |
| 2 | `decl_segment_cpp` phase 2 — `SDeclExported` inner fn paths | done |
| 3 | Wire `collect_decl_parts_cpp` phase 2 native | done |
| 4 | Wire `collect_all_decl_parts_cpp` — native `fn_protos` | done |
| 5 | Audit survivors; close track | done |

## Step 5 audit (2026-05-31)

**Production path:** `module.mlc` → `collect_all_decl_parts_cpp` → `fn_protos` = native `collect_decl_parts_cpp(..., 2)` + string-wrapped `extend_helper_protos_for_exported_types`. No full-string `fn_protos` bundle.

**Module:** `decl_segment_cpp` phase 2 for `SDeclFn` / `SDeclExported`; tests in `test_decl_gen.mlc`.

**String path:** `decl.mlc::gen_proto` / `collect_all_decl_parts` parity only.

## Survivors (expected)

| Site | Use |
|------|-----|
| `extend_helper_protos_for_exported_types` | string append to native `fn_protos` (exported extend helpers) |
| `gen_decl_cpp` / `gen_proto_cpp` | `SDeclExtend` and non-fn fallthrough (string) |
| `cpp_decl_from_string_output` | guard/includes/namespace in `module.mlc`; extern stubs |
| `gen_decl_extend` (string) | extend method definitions |
| `gen_stmts_str` / `stmt_cpp` | string parallel path |
| `decl.mlc` | string `collect_all_decl_parts` parallel bundle |

## Deferred (out of track)

- Native `SDeclExtend` body (`gen_decl_extend` → CppDecl).
- Native extend helper protos in `fn_protos`.
- `gen_stmts_str` / expr visitor string-path elimination.
- Phase 4 `MLCC_BOOTSTRAP=1`.
- Parser `ref mut` — [TRACK_PLAN.md](TRACK_PLAN.md) step 15.
