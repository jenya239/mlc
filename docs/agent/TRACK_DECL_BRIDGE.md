# Track: Native non-fn decl codegen (Phase 2 §3 cont.)

Parent: [../PLAN.md](../PLAN.md) §Phase 2; previous: [TRACK_MAIN_BRIDGE.md](TRACK_MAIN_BRIDGE.md) (**closed**, `521f574`)

## Status: **closed** (`9e2d47e`)

**Goal:** eliminate `cpp_decl_from_string_output(gen_decl(...))` / `gen_proto(...)` for `SDeclType` and `SDeclTrait` in production bundle paths — native `CppStruct`/`CppVariant`/trait concept decls instead of string fragments.

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
| 1 | `gen_type_decl_fwd_cpp` — native type forward (record/variant) for phase 0 | done (`c435cfa`) |
| 2 | `gen_type_decl_body_cpp` — native type body + derive methods for phase 1 | done (`3c533da`) |
| 3 | `gen_trait_decl_cpp` — native trait concept/requires for phase 0 | done (`8649504`) |
| 4 | Wire `collect_decl_parts_cpp` / `collect_all_decl_parts_cpp` phases 0–1 native | done (`25feca8`) |
| 5 | Audit survivors; close track | done (`9e2d47e`) |

## Step 5 audit (2026-05-31)

**Production path:** `module.mlc` → `collect_all_decl_parts_cpp` → header `type_fwds`/`type_defs`/`fn_protos` native (types/traits) or string-wrapped (fn protos); source `fn_defs` via `collect_fn_defs_cpp`.

**SDeclType / SDeclTrait:** no longer string `gen_decl`/`gen_proto` in module header assembly. Phases 0–1 native (`CppStruct`, `CppVariant`, `CppConceptRequires`).

## Survivors (expected)

| Site | Use |
|------|-----|
| `gen_type_decl_body_cpp` | native derive via `gen_derive_methods_cpp` (DERIVE_BRIDGE closed) |
| `gen_decl_cpp` / `gen_proto_cpp` | `SDeclExtend` and non-fn fallthrough (string) |
| `collect_decl_parts_cpp` | phase 2 fn protos, phase 4 trait struct (string) |
| `collect_fn_defs_cpp` | phase 4 trait struct via `append_string_segments` |
| `cpp_decl_from_string_output` | guard/includes/namespace in `module.mlc`; empty extern stubs |
| `gen_decl_extend` (string) | extend method definitions |
| `gen_stmts_str` / `stmt_cpp` | string parallel path |
| `decl.mlc` | string `collect_all_decl_parts` for parity / legacy |

## Deferred (out of track)

- Native trait vtable struct (phase 4 `gen_trait_struct`).
- Native `SDeclExtend` body (`gen_decl_extend` → CppDecl).
- `gen_stmts_str` / expr visitor string-path elimination.
- Phase 4 `MLCC_BOOTSTRAP=1`.
- Parser `ref mut` — [TRACK_PLAN.md](TRACK_PLAN.md) step 15.
