# Track: Native gen_decl_cpp catch-all (Phase 2 §3 cont.)

Parent: [../PLAN.md](../PLAN.md) §Phase 2; previous: [TRACK_EXTEND_FALLBACK_BRIDGE.md](TRACK_EXTEND_FALLBACK_BRIDGE.md) (**closed**, step 5 audit)

## Status: **closed** (step 5 audit, uncommitted)

**Goal:** eliminate `_ => cpp_decl_from_string_output(gen_decl(declaration, context))` in `gen_decl_cpp` — last string fallback for non-fn, non-extend decl dispatch (was `decl_cpp.mlc:398`).

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
| 2 | Native per-decl dispatch in `gen_decl_cpp` (type/trait/import/assoc/exported) | done |
| 3 | Parity tests in `test_decl_gen.mlc` | done |
| 4 | Audit call sites — no string `gen_decl` in `gen_decl_cpp` | done |
| 5 | Audit survivors; close track | done |

## Step 5 audit (2026-06-02)

| Check | Result |
|-------|--------|
| Track goal | no string `gen_decl` in `gen_decl_cpp`; explicit per-variant dispatch |
| Production | `decl_cpp.mlc:388-409` native arms only |
| parity tests | Type/Trait/Exported/Import/AssocBind (+7 in step 3) |
| verify | 878 pass; DIFF_EXIT=0 |

## Survivors (confirmed)

| Site | Use |
|------|-----|
| `gen_decl_cpp` `SDeclExtend` `:403-405` | `print_cpp_declarations(gen_decl_extend_cpp(...))` wrapper (deferred) |
| `gen_decl_cpp` `SDeclType` `:395-400` | print round-trip on native fwd+body (deferred multi-`CppDecl`) |
| `gen_proto_cpp` catch-all `:418` | string `gen_proto` (next track) |
| `collect_decl_parts_cpp` phase ≠ 0/1/2/3/4 `:839` | string `collect_decl_parts` bridge |
| `gen_decl` / `gen_decl_extend` | `decl.mlc` parity bundle |
| `cpp_decl_from_string_output` | empty fragments; extend extern stubs; struct_using |

## Step 1 audit (2026-06-02)

| Check | Result |
|-------|--------|
| `gen_decl_cpp` arms (pre-step-2) | `SDeclFn` native; `SDeclExtend` print-wrapper; `_` catch-all |
| Catch-all variants | `SDeclType`, `SDeclTrait`, `SDeclImport`, `SDeclAssocBind`, `SDeclExported` |
| Production callers | `decl_fn_def_cpp:867` exported inner `_`; module bundle bypasses via `decl_segment_cpp` |

## Step 3 parity tests (2026-06-02)

| Test | Pattern |
|------|---------|
| `SDeclType` sum | `print_cpp_declaration(gen_decl_cpp)` == `gen_decl` |
| `SDeclType` record | native body print (print round-trip formatting) |
| `SDeclTrait` | == `gen_decl` |
| `SDeclExported` | unwrap == unwrapped `SDeclFn`; signature contains string tokens |
| `SDeclImport` / `SDeclAssocBind` | empty |

Result: 878 pass; DIFF_EXIT=0.

## Step 4 call-site audit (2026-06-02)

| Check | Result |
|-------|--------|
| `gen_decl_cpp` imports | `decl.mlc`: `gen_proto`, `collect_*` only — **no `gen_decl`** |
| `gen_decl_cpp` body | explicit arms; **no catch-all**; **no `gen_decl` call** |
| Type arm | native `gen_type_decl_fwd_cpp` + `gen_type_decl_body_cpp` via print round-trip |
| Trait arm | native `gen_trait_decl_cpp` |
| Extend arm | `print_cpp_declarations(gen_decl_extend_cpp(...))` — native extend, not string `gen_decl` |
| Import/AssocBind | empty fragment |
| Exported | recurse `gen_decl_cpp(sdecl_inner(...))` |

**Production callers of `gen_decl_cpp`:**

| Site | When |
|------|------|
| `decl_segment_cpp:790` | phase 2, `SDeclExported` inner generic `SDeclFn` non-extern → body via `gen_decl_cpp` |
| `decl_fn_def_cpp:870` | exported generic extern fn |
| `decl_fn_def_cpp:873` | exported non-generic fn |
| `decl_fn_def_cpp:877` | exported inner `_` (Import/AssocBind edge) |

String `gen_decl` remains only in `decl.mlc` parity bundle (`collect_decl_parts` phase 3, `decl_fn_def` string path).

## Deferred (out of track)

- `gen_decl_cpp` `SDeclExtend` — native multi-`CppDecl` without print round-trip.
- `gen_proto_cpp` catch-all string path.
- Native CppFnDef for inline trait wrapper / extern fragments.
- `gen_stmts_str` / expr visitor string-path elimination.
- Phase 4 `MLCC_BOOTSTRAP=1`.
- Parser `ref mut` — [TRACK_PLAN.md](TRACK_PLAN.md) step 15.
