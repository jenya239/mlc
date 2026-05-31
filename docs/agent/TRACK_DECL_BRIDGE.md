# Track: Native non-fn decl codegen (Phase 2 §3 cont.)

Parent: [../PLAN.md](../PLAN.md) §Phase 2; previous: [TRACK_MAIN_BRIDGE.md](TRACK_MAIN_BRIDGE.md) (**closed**, `521f574`)

## Status: **open**

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
| 4 | Wire `collect_decl_parts_cpp` / `collect_all_decl_parts_cpp` phases 0–1 native | pending |
| 5 | Audit survivors; close track | pending |

## Context

Fn proto/def bridges closed (template, extend forwards, zero-param `main`). Remaining production string bridge for decls:

- `gen_decl_cpp` / `gen_proto_cpp` → `cpp_decl_from_string_output(gen_decl|gen_proto(...))` for non-`SDeclFn`
- `collect_decl_parts_cpp` phases 0–1 → `cpp_decls_from_string_parts(collect_decl_parts(...))`
- `collect_all_decl_parts_cpp` → string bundle for `type_fwds` / `type_defs`

String helpers remain in `decl/type_gen.mlc`, `decl_extend.mlc` for parallel path and parity tests.

## Survivors (expected after close)

| Site | Use |
|------|-----|
| `cpp_decl_from_string_output` | phase-4 trait struct, `SDeclExtend` body, `module.mlc` fragments |
| `gen_decl_extend` (string) | extend method definitions |
| `gen_stmts_str` / `stmt_cpp` | string parallel path |
| `decl.mlc` | string `collect_all_decl_parts` parallel bundle |

## Deferred (out of track)

- Native trait vtable struct (phase 4 `gen_trait_struct`).
- Native `SDeclExtend` body (`gen_decl_extend` → CppDecl).
- `gen_stmts_str` / expr visitor string-path elimination.
- Phase 4 `MLCC_BOOTSTRAP=1`.
- Parser `ref mut` — [TRACK_PLAN.md](TRACK_PLAN.md) step 15.
