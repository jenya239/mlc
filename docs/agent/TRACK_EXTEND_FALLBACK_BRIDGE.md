# Track: Remove extend non-SDeclFn string fallback (Phase 2 §3 cont.)

Parent: [../PLAN.md](../PLAN.md) §Phase 2; previous: [TRACK_EXTEND_EXTERN_BRIDGE.md](TRACK_EXTEND_EXTERN_BRIDGE.md) (**closed**, step 5 audit)

## Status: **closed** (step 5 audit, uncommitted)

**Goal:** eliminate `cpp_decl_from_string_output(gen_decl(method, extend_context))` in `gen_decl_extend_cpp` — last string hook in the extend-method loop (`decl_cpp.mlc:652-655`).

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
| 1 | Reachability audit — extend `methods` types after `transform_decl` | done |
| 2 | Replace `_` branch — no-op or native dispatch; drop `gen_decl` in loop | done |
| 3 | Parity tests — `gen_decl_extend_cpp` bundle unchanged | done |
| 4 | Audit call sites — no string fallback in production extend loop | done |
| 5 | Audit survivors; close track | done |

## Step 5 audit (2026-06-02)

| Check | Result |
|-------|--------|
| Track goal | no string `gen_decl` fallback in `gen_decl_extend_cpp` method loop |
| Production | `decl_cpp.mlc:634-652` native-only; non-fn no-op |
| parity tests | extend bundle + assoc entries (+2) |
| survivors | string `gen_extend_method_parts`/`gen_decl_extend` in `decl.mlc`; `gen_decl_cpp:398` catch-all |
| verify | 871 pass; DIFF_EXIT=0 |

## Survivors (confirmed)

| Site | Use |
|------|-----|
| `gen_extend_method_parts` `_` branch (string) | `decl.mlc` parity via `gen_decl_extend` |
| `gen_decl_extend` (string) | `decl.mlc` parity bundle |
| `gen_decl` in `gen_decl_cpp` catch-all | non-extend decl dispatch (`decl_cpp.mlc:398`) |
| `cpp_decl_from_string_output` | guards/includes; fragment helpers |
| `decl.mlc` | string `collect_all_decl_parts` parallel bundle |
| `module.mlc` assoc `using` lines | associated-type bindings (separate path) |

## Deferred (out of track)

- Native CppFnDef for inline trait wrapper / extern fragments (still `CppDeclFragment`).
- `gen_decl_cpp` catch-all `gen_decl` string path (`decl_cpp.mlc:398`).
- `gen_stmts_str` / expr visitor string-path elimination.
- Phase 4 `MLCC_BOOTSTRAP=1`.
- Parser `ref mut` — [TRACK_PLAN.md](TRACK_PLAN.md) step 15.
