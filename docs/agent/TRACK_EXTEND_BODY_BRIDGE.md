# Track: Native SDeclExtend body (Phase 2 §3 cont.)

Parent: [../PLAN.md](../PLAN.md) §Phase 2; previous: [TRACK_EXTEND_HELPER_BRIDGE.md](TRACK_EXTEND_HELPER_BRIDGE.md) (**closed**, step 5 audit)

## Status: **closed** (step 5 audit, uncommitted)

**Goal:** eliminate `cpp_decl_from_string_output(gen_decl(...))` for `SDeclExtend` in `gen_decl_cpp` / `decl_fn_def_cpp` — native `CppFnDef` for extend method bodies instead of string `gen_decl_extend`.

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
| 1 | `gen_decl_extend_cpp` — native extend method defs (trait + non-trait paths) | done |
| 2 | Wire `gen_decl_cpp` — `SDeclExtend` → native | done |
| 3 | Wire `decl_fn_def_cpp` / `collect_fn_defs_cpp` if string segments remain | done |
| 4 | Parity tests in `test_decl_gen.mlc` | done |
| 5 | Audit survivors; close track | done |

## Step 5 audit (2026-06-01)

| Check | Result |
|-------|--------|
| `gen_decl_cpp` SDeclExtend | `gen_decl_extend_cpp` via `print_cpp_declarations` — no `gen_decl(SDeclExtend)` |
| `decl_fn_def_cpp` SDeclExtend | direct `gen_decl_extend_cpp` |
| string `gen_decl_extend` production | `decl.mlc` only (`gen_decl` + `collect_all_decl_parts` fn_defs) |
| parity tests | 4 tests: trait/non-trait `gen_decl_extend_cpp`, `gen_decl_cpp` vs `gen_decl` |
| verify | 860 pass; DIFF_EXIT=0 |

**Internal hybrid (accepted, out of track scope):** `gen_decl_extend_cpp` method bodies still string-wrapped `gen_decl` (printer indent parity); trait suffix via `gen_decl_extend_trait_suffix` string.

## Survivors (confirmed)

| Site | Use |
|------|-----|
| `gen_decl_extend` (string) | `decl.mlc` parity bundle |
| `cpp_decl_from_string_output` | guards/includes in `module.mlc`; extern stubs; extend fragment wrap |
| `gen_stmts_str` / `stmt_cpp` | string parallel path |
| `decl.mlc` | string `collect_all_decl_parts` parallel bundle |

## Deferred (out of track)

- Native extend method bodies inside `gen_decl_extend_cpp` (drop inner `gen_decl` wrap).
- `gen_stmts_str` / expr visitor string-path elimination.
- Phase 4 `MLCC_BOOTSTRAP=1`.
- Parser `ref mut` — [TRACK_PLAN.md](TRACK_PLAN.md) step 15.
