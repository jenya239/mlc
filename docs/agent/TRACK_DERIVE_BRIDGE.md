# Track: Native derive method codegen (Phase 2 §3 cont.)

Parent: [../PLAN.md](../PLAN.md) §Phase 2; previous: [TRACK_DECL_BRIDGE.md](TRACK_DECL_BRIDGE.md) (**closed**, `9e2d47e`)

## Status: **open**

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
| 1 | `gen_derive_display_cpp` — native Display derive (record + sum) | pending |
| 2 | `gen_derive_eq_cpp` — native Eq derive | pending |
| 3 | `gen_derive_ord_cpp` — native Ord derive | pending |
| 4 | `gen_derive_hash_cpp` + `gen_derive_methods_cpp`; wire `gen_type_decl_body_cpp` | pending |
| 5 | Audit survivors; close track | pending |

## Context

`gen_type_decl_body_cpp` emits native struct bodies but appends derive via string:

```mlc
result.push(cpp_decl_from_string_output(gen_derive_methods(...)))
```

String helpers remain in `decl/type_gen.mlc` for parity tests until track close.

## Survivors (expected after close)

| Site | Use |
|------|-----|
| `gen_decl_cpp` / `gen_proto_cpp` | `SDeclExtend` and non-fn fallthrough (string) |
| `collect_decl_parts_cpp` | phase 2 fn protos, phase 4 trait struct (string) |
| `collect_fn_defs_cpp` | phase 4 trait struct via `append_string_segments` |
| `cpp_decl_from_string_output` | guard/includes/namespace in `module.mlc`; extern stubs |
| `gen_decl_extend` (string) | extend method definitions |
| `gen_stmts_str` / `stmt_cpp` | string parallel path |
| `decl.mlc` | string `collect_all_decl_parts` parallel bundle |

## Deferred (out of track)

- Native trait vtable struct (phase 4 `gen_trait_struct`).
- Native `SDeclExtend` body (`gen_decl_extend` → CppDecl).
- `gen_stmts_str` / expr visitor string-path elimination.
- Phase 4 `MLCC_BOOTSTRAP=1`.
- Parser `ref mut` — [TRACK_PLAN.md](TRACK_PLAN.md) step 15.
