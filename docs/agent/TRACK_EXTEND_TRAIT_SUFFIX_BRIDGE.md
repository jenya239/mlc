# Track: Native extend trait suffix (Phase 2 §3 cont.)

Parent: [../PLAN.md](../PLAN.md) §Phase 2; previous: [TRACK_EXTEND_METHOD_BRIDGE.md](TRACK_EXTEND_METHOD_BRIDGE.md) (**closed**, step 5 audit)

## Status: **closed** (step 5 audit, uncommitted)

**Goal:** eliminate `cpp_decls_from_string_parts([gen_decl_extend_trait_suffix(...)])` in `gen_decl_extend_cpp` — native `CppDecl` for trait wrappers, concept adapters, and static_assert instead of string suffix.

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
| 1 | `gen_decl_extend_trait_suffix_cpp` — native wrappers + adapters + static_assert | done |
| 2 | Wire `gen_decl_extend_cpp` — native trait suffix append | done |
| 3 | Parity tests in `test_decl_gen.mlc` | done |
| 4 | Audit call sites — no string trait suffix in production | done |
| 5 | Audit survivors; close track | done |

## Step 5 audit (2026-06-02)

| Check | Result |
|-------|--------|
| Track goal | no `cpp_decls_from_string_parts([gen_decl_extend_trait_suffix(...)])` in `decl_cpp.mlc` |
| Production | `gen_decl_extend_cpp:632-635` → `gen_decl_extend_trait_suffix_cpp` |
| parity tests | suffix vs string; static_assert; wrapper forward (+3) |
| survivors | extern stub; non-SDeclFn fallback; string `gen_decl_extend`/`gen_decl_extend_trait_suffix` in `decl.mlc` |
| verify | 866 pass; DIFF_EXIT=0 (prior turn) |

## Survivors (confirmed)

| Site | Use |
|------|-----|
| `gen_extend_extern_method` (string) | extern extend stubs in `gen_decl_extend_cpp` |
| `gen_decl(method, extend_context)` | non-SDeclFn fallback in `gen_decl_extend_cpp` |
| `gen_decl_extend_trait_suffix` (string) | `decl.mlc` parity bundle via `gen_decl_extend` |
| `gen_decl_extend` (string) | `decl.mlc` parity bundle |
| `cpp_decl_from_string_output` | guards/includes; extern stubs; per-decl fragments in suffix helpers |
| `decl.mlc` | string `collect_all_decl_parts` parallel bundle |

## Deferred (out of track)

- Native `gen_extend_extern_method_cpp`.
- Remove non-SDeclFn `gen_decl` fallback.
- Native CppFnDef for inline trait wrappers (currently CppDeclFragment from expression_output).
- `gen_stmts_str` / expr visitor string-path elimination.
- Phase 4 `MLCC_BOOTSTRAP=1`.
- Parser `ref mut` — [TRACK_PLAN.md](TRACK_PLAN.md) step 15.
