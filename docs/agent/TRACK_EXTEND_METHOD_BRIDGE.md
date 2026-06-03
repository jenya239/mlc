# Track: Native extend method bodies (Phase 2 §3 cont.)

Parent: [../PLAN.md](../PLAN.md) §Phase 2; previous: [TRACK_EXTEND_BODY_BRIDGE.md](TRACK_EXTEND_BODY_BRIDGE.md) (**closed**, step 5 audit)

## Status: **closed** (step 5 audit, uncommitted)

**Goal:** eliminate `cpp_decl_from_string_output(gen_decl(method, extend_context))` inside `gen_decl_extend_cpp` — native `gen_fn_decl_cpp` for extend method bodies (non-extern).

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
| 1 | `gen_decl_extend_cpp` — non-extern methods via `gen_fn_decl_cpp` + `extend_context` | done |
| 2 | Printer/indent parity — `collect_fn_defs_cpp` extend bundle vs string | done |
| 3 | Parity tests in `test_decl_gen.mlc` | done |
| 4 | Audit call sites — no inner `gen_decl` for extend methods | done |
| 5 | Audit survivors; close track | done |

## Step 5 audit (2026-06-01)

| Check | Result |
|-------|--------|
| Track goal | SDeclFn non-extern → `native_fn_decl_cpp(..., extend_context, 0)`; no string `gen_decl` on method path |
| `CppFnDef.body_statement_depth` | 0 extend methods; 1 default fn defs |
| parity tests | +3: native body vs `gen_fn_decl`, compact depth, trait method |
| survivors | extern stub; non-SDeclFn fallback `:500`; trait suffix string; `decl.mlc` bundle |
| verify | 863 pass; DIFF_EXIT=0 |

## Survivors (confirmed)

| Site | Use |
|------|-----|
| `gen_extend_extern_method` (string) | extern extend stubs in `gen_decl_extend_cpp` |
| `gen_decl(method, extend_context)` | non-SDeclFn fallback only (`decl_cpp.mlc:500`) |
| `gen_decl_extend_trait_suffix` (string) | trait wrappers/adapters/static_assert append |
| `gen_decl_extend` (string) | `decl.mlc` parity bundle |
| `cpp_decl_from_string_output` | guards/includes; extern stubs; trait suffix fragments |
| `decl.mlc` | string `collect_all_decl_parts` parallel bundle |

## Deferred (out of track)

- Native `gen_extend_extern_method_cpp` + native trait suffix (`gen_decl_extend_trait_suffix_cpp`).
- Remove non-SDeclFn `gen_decl` fallback in `gen_decl_extend_cpp`.
- `gen_stmts_str` / expr visitor string-path elimination.
- Phase 4 `MLCC_BOOTSTRAP=1`.
- Parser `ref mut` — [TRACK_PLAN.md](TRACK_PLAN.md) step 15.
