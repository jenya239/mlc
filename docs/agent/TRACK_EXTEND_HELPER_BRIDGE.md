# Track: Native extend helper protos (Phase 2 §3 cont.)

Parent: [../PLAN.md](../PLAN.md) §Phase 2; previous: [TRACK_FN_PROTO_BRIDGE.md](TRACK_FN_PROTO_BRIDGE.md) (**closed**, step 5 audit)

## Status: **closed** (step 5 audit, uncommitted)

**Goal:** eliminate `cpp_decls_from_string_parts(extend_helper_protos_for_exported_types(...))` in `collect_all_decl_parts_cpp.fn_protos` — native `CppFnProto` for exported-type extend helper protos instead of string fragments.

## Verify gate (every step)

```
compiler/tests/build_tests.sh
compiler/build.sh
compiler/out/mlcc -o .tmp_selfhost/p1 compiler/main.mlc
compiler/build_bin.sh .tmp_selfhost/p1 .tmp_selfhost/mlcc2
.tmp_selfhost/mlcc2 -o .tmp_selfhost/p2 compiler/main.mlc
diff -rq .tmp_selfhost/p1 .tmp_selfhost/p2   # empty
```

---

| Step | Item | Status |
|------|------|--------|
| 1 | Export `exported_type_names_from_decls`; `extend_helper_protos_for_exported_types_cpp` via `gen_proto_cpp` | done |
| 2 | Wire `collect_all_decl_parts_cpp.fn_protos` — native append, drop string wrap | done |
| 3 | Parity tests in `test_decl_gen.mlc` (helper + full bundle `fn_protos`) | done |
| 4 | Audit call sites — no remaining production string helper path | done |
| 5 | Audit survivors; close track | done |

## Step 5 audit (2026-05-31)

**Track goal met:** production `fn_protos` uses native `extend_helper_protos_for_exported_types_cpp`; no string wrap in `decl_cpp.mlc`.

**Production path:** `module.mlc:150` → `collect_all_decl_parts_cpp` → `fn_protos` native phase 2 + native extend helpers.

**Survivors (intentional):**

| Site | Use |
|------|-----|
| `extend_helper_protos_for_exported_types` (string) | `decl.mlc` `collect_all_decl_parts` parity |
| `gen_decl_cpp` / `gen_proto_cpp` | `SDeclExtend` body fallthrough (string) — next: EXTEND_BODY_BRIDGE |
| `gen_decl_extend` (string) | extend method definitions |
| `cpp_decl_from_string_output` | guard/includes/namespace in `module.mlc`; extern stubs |
| `gen_stmts_str` / `stmt_cpp` | string parallel path |

## Step 4 audit (2026-05-31)

**Production path:** `module.mlc` → `collect_all_decl_parts_cpp` → `fn_protos` append via `extend_helper_protos_for_exported_types_cpp`. No `cpp_decls_from_string_parts(extend_helper_protos_for_exported_types(...))` in `decl_cpp.mlc`.

| Symbol / site | Callers in `compiler/**/*.mlc` | Notes |
|---------------|-------------------------------|-------|
| `extend_helper_protos_for_exported_types_cpp` | `collect_all_decl_parts_cpp` | native exported-type extend helper protos |
| `extend_helper_protos_for_exported_types` (string) | `decl.mlc` `collect_all_decl_parts` | parallel string bundle / parity only |
| `extend_helper_protos_for_exported_types` (string) | `test_decl_gen.mlc` | parity assertions vs native |

## Survivors (expected after close)

| Site | Use |
|------|-----|
| `gen_decl_cpp` / `gen_proto_cpp` | `SDeclExtend` body fallthrough (string) |
| `gen_decl_extend` (string) | extend method definitions |
| `cpp_decl_from_string_output` | guard/includes/namespace in `module.mlc`; extern stubs |
| `gen_stmts_str` / `stmt_cpp` | string parallel path |
| `decl.mlc` | string `extend_helper_protos_for_exported_types` parity bundle |

## Deferred (out of track)

- Native `SDeclExtend` body (`gen_decl_extend` → CppDecl).
- `gen_stmts_str` / expr visitor string-path elimination.
- Phase 4 `MLCC_BOOTSTRAP=1`.
- Parser `ref mut` — [TRACK_PLAN.md](TRACK_PLAN.md) step 15.
