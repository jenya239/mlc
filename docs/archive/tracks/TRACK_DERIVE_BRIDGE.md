# Track: Native derive method codegen (Phase 2 §3 cont.)

Parent: [../PLAN.md](../PLAN.md) §Phase 2; previous: [TRACK_DECL_BRIDGE.md](TRACK_DECL_BRIDGE.md) (**closed**, `9e2d47e`)

## Status: **closed** (step 5 audit, uncommitted)

**Goal:** eliminate `cpp_decl_from_string_output(gen_derive_methods(...))` in `gen_type_decl_body_cpp` — native `CppFnDef` for Display/Eq/Ord/Hash derive instead of string fragments.

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
| 1 | `gen_derive_display_cpp` — native Display derive (record + sum) | done |
| 2 | `gen_derive_eq_cpp` — native Eq derive | done |
| 3 | `gen_derive_ord_cpp` — native Ord derive | done |
| 4 | `gen_derive_hash_cpp` + `gen_derive_methods_cpp`; wire `gen_type_decl_body_cpp` | done |
| 5 | Audit survivors; close track | done |

## Step 5 audit (2026-05-31)

**Production path:** `gen_type_decl_body_cpp` → `gen_derive_methods_cpp` → native `CppFnDef` / `CppDeclFragment` (`std::hash<T>`) for Display/Eq/Ord/Hash. No `cpp_decl_from_string_output(gen_derive_methods(...))` in `decl_cpp.mlc`.

**Module:** `compiler/codegen/decl/derive_methods_cpp.mlc` — record + sum for all four traits; tests in `test_decl_gen.mlc`.

**String path:** `type_gen.mlc::gen_derive_methods` + `decl.mlc` only (parity / legacy bundle).

## Survivors (expected)

| Site | Use |
|------|-----|
| `type_gen.mlc` / `decl.mlc` | string `gen_derive_methods` parity / legacy |
| `gen_decl_cpp` / `gen_proto_cpp` | `SDeclExtend` and non-fn fallthrough (string) |
| `collect_decl_parts_cpp` | phase 2 fn protos (string) |
| `cpp_decl_from_string_output` | guard/includes/namespace in `module.mlc`; extern stubs |
| `gen_decl_extend` (string) | extend method definitions |
| `gen_stmts_str` / `stmt_cpp` | string parallel path |

## Deferred (out of track)

- Native trait vtable struct (phase 4 `gen_trait_struct`) — TRAIT_STRUCT_BRIDGE closed.
- Native `SDeclExtend` body (`gen_decl_extend` → CppDecl).
- `gen_stmts_str` / expr visitor string-path elimination.
- Phase 4 `MLCC_BOOTSTRAP=1`.
- Parser `ref mut` — [TRACK_PLAN.md](TRACK_PLAN.md) step 15.
