# Track: Native gen_proto_cpp catch-all (Phase 2 §3 cont.)

Parent: [../PLAN.md](../PLAN.md) §Phase 2; previous: [TRACK_DECL_CPP_CATCHALL_BRIDGE.md](TRACK_DECL_CPP_CATCHALL_BRIDGE.md) (**closed**, step 5 audit)

## Status: **closed** (step 5 audit, uncommitted)

**Goal:** eliminate `_ => cpp_decl_from_string_output(gen_proto(declaration, context))` in `gen_proto_cpp` — last string fallback for non-fn proto dispatch (was `decl_cpp.mlc:418`).

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
| 1 | Reachability audit — which `SDecl` variants hit catch-all in production | done |
| 2 | Native per-decl dispatch in `gen_proto_cpp` (type/trait/import/assoc/extend/exported) | done |
| 3 | Parity tests in `test_decl_gen.mlc` | done |
| 4 | Audit call sites — no string `gen_proto` in `gen_proto_cpp` | done |
| 5 | Audit survivors; close track | done |

## Step 5 audit (2026-06-02)

| Check | Result |
|-------|--------|
| Track goal | no string `gen_proto` in `gen_proto_cpp`; explicit per-variant dispatch |
| Production | `decl_cpp.mlc:411-444` native arms only |
| parity tests | Type/Trait/Extend/Exported/Import/AssocBind (+7 in step 3) |
| survivors | string `gen_proto` in `decl.mlc`; helper guard `:492`; `gen_proto_cpp` Extend print round-trip |
| verify | 885 pass; DIFF_EXIT=0 |

## Survivors (confirmed)

| Site | Use |
|------|-----|
| `gen_proto` (string) | `decl.mlc` parity bundle (`collect_decl_parts` phase 2) |
| `extend_helper_protos_for_exported_types_cpp:492` | string `gen_proto` length guard before native push |
| `gen_proto_cpp` `SDeclExtend` `:420-440` | print round-trip on method protos (deferred multi-`CppDecl`) |
| `collect_decl_parts_cpp` non-native phases | string bridge |
| `gen_decl_cpp` print wrappers | deferred separate tracks |

## Step 1 audit (2026-06-02)

| Check | Result |
|-------|--------|
| `gen_proto_cpp` arms | `SDeclFn` native `:413-417`; `_` catch-all `:418` |
| Catch-all variants | `SDeclType`, `SDeclTrait`, `SDeclImport`, `SDeclAssocBind`, `SDeclExtend`, `SDeclExported` |
| String `gen_proto` per variant | Type/Trait/Import/AssocBind → `''`; Extend → method `gen_proto` join (no wrapper protos); Exported → recurse |
| Production callers (Fn arm) | `extend_method_forward_segments_cpp:437`, `extend_helper_protos:468`, `extend_forward_segment_for_extend:720`, `decl_segment_cpp:768` — all pass `SDeclFn` methods or top-level fn |
| Production catch-all edge | `decl_segment_cpp:794` — phase 2 `SDeclExported` inner `_` (e.g. exported `SDeclExtend`, Import/AssocBind) |
| Module bundle | phase 2 top-level Type/Trait/Extend → `decl_segment_cpp` empty; extend protos via `extend_cpp_forward_segment_cpp` (native per-method `gen_proto_cpp`) |
| Tests | `test_decl_gen.mlc`: `gen_proto_cpp DeclFn`; string `gen_proto SDeclType` empty — no direct catch-all variant tests |
| Step 2 recommendation | explicit arms: Type/Trait/Import/AssocBind → empty; Extend → map `gen_proto_cpp` on `SDeclFn` methods; Exported → recurse `gen_proto_cpp(sdecl_inner)` |

## Step 2 implementation (2026-06-02)

| Arm | Implementation |
|-----|----------------|
| `SDeclFn` | native `gen_fn_proto_cpp` / empty extern (`:413-417`) |
| `SDeclType` / `SDeclTrait` / `SDeclImport` / `SDeclAssocBind` | empty fragment |
| `SDeclExtend` | per-method `gen_proto_cpp` in `extend_context`; print round-trip (`:420-440`) |
| `SDeclExported` | recurse `gen_proto_cpp(sdecl_inner(...))` |
| catch-all | removed; no `gen_proto` in `gen_proto_cpp` |

Result: 878 pass; DIFF_EXIT=0.

## Step 3 parity tests (2026-06-02)

| Test | Pattern |
|------|---------|
| `SDeclType` / `SDeclTrait` | empty |
| `SDeclExtend` | == `gen_proto` |
| `SDeclExported` | unwrap == unwrapped `SDeclFn`; signature contains string tokens |
| `SDeclImport` / `SDeclAssocBind` | empty |

Result: 885 pass; DIFF_EXIT=0.

## Step 4 call-site audit (2026-06-02)

| Check | Result |
|-------|--------|
| `gen_proto_cpp` body | explicit arms `:411-444`; **no catch-all**; **no `gen_proto` call** |
| `gen_proto` import in `decl_cpp.mlc` | retained for `extend_helper_protos_for_exported_types_cpp:492` length guard only |
| Production callers | `extend_method_forward_segments_cpp:462`, `extend_helper_protos:493`, `extend_forward_segment_for_extend:745`, `decl_segment_cpp:793/814/817/819` |
| Fn arm callers | per-method or top-level `SDeclFn` → native `gen_fn_proto_cpp` |
| Native Extend/Exported | `gen_proto_cpp` Extend arm `:420-440`; Exported recurse `:443`; edge `decl_segment_cpp:819` |

String `gen_proto` remains in `decl.mlc` parity bundle and helper length guard (`:492`).

## Deferred (out of track)

- `gen_decl_cpp` `SDeclExtend`/`SDeclType` print round-trip.
- `collect_decl_parts_cpp` phase string bridge elimination.
- Native CppFnDef for inline trait wrapper / extern fragments.
- `gen_stmts_str` / expr visitor string-path elimination.
- Phase 4 `MLCC_BOOTSTRAP=1`.
- Parser `ref mut` — [TRACK_PLAN.md](TRACK_PLAN.md) step 15.
