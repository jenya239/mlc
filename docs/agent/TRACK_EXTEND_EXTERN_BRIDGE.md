# Track: Native extend extern stubs (Phase 2 §3 cont.)

Parent: [../PLAN.md](../PLAN.md) §Phase 2; previous: [TRACK_EXTEND_TRAIT_SUFFIX_BRIDGE.md](TRACK_EXTEND_TRAIT_SUFFIX_BRIDGE.md) (**closed**, step 5 audit)

## Status: **closed** (step 5 audit, uncommitted)

**Goal:** eliminate `cpp_decl_from_string_output(gen_extend_extern_method(...))` in `gen_decl_extend_cpp` — native `CppDecl` for extern extend method stubs instead of string fragments.

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
| 1 | `gen_extend_extern_method_cpp` — native extern stub decl | done |
| 2 | Wire `gen_decl_extend_cpp` — extern SDeclFn → native | done |
| 3 | Parity tests in `test_decl_gen.mlc` | done |
| 4 | Audit call sites — no string extern stub in production | done |
| 5 | Audit survivors; close track | done |

## Step 5 audit (2026-06-02)

| Check | Result |
|-------|--------|
| Track goal | no `cpp_decl_from_string_output(gen_extend_extern_method(...))` in `decl_cpp.mlc` |
| Production | `gen_decl_extend_cpp:637-641` → `append_cpp_declarations(..., gen_extend_extern_method_cpp(...))` |
| parity tests | extern cpp vs string; mlc::to_string; non-i32 empty (+3) |
| survivors | non-SDeclFn fallback; string `gen_extend_extern_method`/`gen_decl_extend` in `decl.mlc` |
| verify | 869 pass; DIFF_EXIT=0 |

## Survivors (confirmed)

| Site | Use |
|------|-----|
| `gen_extend_extern_method` (string) | `decl.mlc` parity bundle via `gen_decl_extend` |
| `gen_decl(method, extend_context)` | non-SDeclFn fallback in `gen_decl_extend_cpp` |
| `gen_decl_extend` (string) | `decl.mlc` parity bundle |
| `gen_decl_extend_trait_suffix` (string) | `decl.mlc` parity bundle |
| `cpp_decl_from_string_output` | guards/includes; fragment helpers in suffix/extern helpers |
| `decl.mlc` | string `collect_all_decl_parts` parallel bundle |

## Deferred (out of track)

- Remove non-SDeclFn `gen_decl` fallback in `gen_decl_extend_cpp`.
- Native CppFnDef for inline trait wrapper fragments.
- `gen_stmts_str` / expr visitor string-path elimination.
- Phase 4 `MLCC_BOOTSTRAP=1`.
- Parser `ref mut` — [TRACK_PLAN.md](TRACK_PLAN.md) step 15.
