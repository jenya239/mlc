# Track: Native trait vtable struct (Phase 2 §3 cont.)

Parent: [../PLAN.md](../PLAN.md) §Phase 2; previous: [TRACK_DERIVE_BRIDGE.md](TRACK_DERIVE_BRIDGE.md) (**closed**, step 5 audit)

## Status: **closed** (step 5 audit, uncommitted)

**Goal:** eliminate `append_string_segments(..., collect_decl_parts(..., 4))` / string `gen_trait_struct` in production bundle paths — native `CppStruct` vtable for trait methods instead of string fragments.

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
| 1 | `trait_method_field_cpp` — native `std::function` field line per trait method | done |
| 2 | `gen_trait_struct_cpp` — native vtable `CppStruct` (template + fields) | done |
| 3 | Wire `decl_segment_cpp` / `collect_decl_parts_cpp` phase 4 native for `SDeclTrait` | done |
| 4 | Wire `collect_fn_defs_cpp` — drop `append_string_segments` for phase 4 trait struct | done |
| 5 | Audit survivors; close track | done |

## Step 5 audit (2026-05-31)

**Production path:** `collect_fn_defs_cpp` → `collect_decl_parts_cpp(..., 4)` → `decl_segment_cpp` → `gen_trait_struct_cpp`. No `append_string_segments` for phase 4.

**Module:** `compiler/codegen/decl/trait_struct_cpp.mlc` — `trait_method_field_cpp`, `gen_trait_struct_cpp`.

**String path:** `decl_extend.mlc::gen_trait_struct` + `decl.mlc` phase 4 only (parity / legacy bundle).

## Survivors (expected)

| Site | Use |
|------|-----|
| `decl_extend.mlc` / `decl.mlc` | string `gen_trait_struct` parity / legacy |
| `gen_decl_cpp` / `gen_proto_cpp` | `SDeclExtend` and non-fn fallthrough (string) |
| `extend_helper_protos_for_exported_types` | string append in native `fn_protos` (FN_PROTO survivor) |
| `cpp_decl_from_string_output` | guard/includes/namespace in `module.mlc`; extern stubs |
| `gen_decl_extend` (string) | extend method definitions |
| `gen_stmts_str` / `stmt_cpp` | string parallel path |

## Deferred (out of track)

- Native `SDeclExtend` body (`gen_decl_extend` → CppDecl).
- `gen_stmts_str` / expr visitor string-path elimination.
- Phase 4 `MLCC_BOOTSTRAP=1`.
- Parser `ref mut` — [TRACK_PLAN.md](TRACK_PLAN.md) step 15.
