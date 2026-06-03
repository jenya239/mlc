# Track: Native collect_decl_parts_cpp else fallback (Phase 2 §3 cont.)

Parent: [../PLAN.md](../PLAN.md) §Phase 2; previous: [TRACK_EXTEND_HELPER_PROTO_GUARD_BRIDGE.md](TRACK_EXTEND_HELPER_PROTO_GUARD_BRIDGE.md) (**closed**, step 5 audit)

## Status: **closed** (step 5 audit, uncommitted)

**Goal:** eliminate `cpp_decls_from_string_parts(collect_decl_parts(...))` in the `collect_decl_parts_cpp` else branch — last string `collect_decl_parts` call in `decl_cpp.mlc` (was `:873`).

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
| 1 | Reachability audit — production phases 0–4 only; else branch semantics | done |
| 2 | Native else — replace string fallback with `empty_cpp_declarations()` | done |
| 3 | Parity tests — phases 0–4 + bundle unchanged vs string | done |
| 4 | Audit call sites — drop unused `collect_decl_parts` / `collect_all_decl_parts` imports | done |
| 5 | Audit survivors; close track | done |

<!-- step 1 sub-steps:
  1) grep all `collect_decl_parts_cpp(` call sites; list phase literals
  2) confirm else branch unreachable in production (phases outside 0–4)
  3) document string-path semantics for unknown phase (decl.mlc parity only)
-->

<!-- step 2 sub-steps:
  1) replace `:872-873` else arm with `empty_cpp_declarations()`
  2) optional: consolidate phase dispatch to single exhaustive match
-->

<!-- step 3 sub-steps:
  1) existing `test_decl_gen.mlc` phases 0/1/2/4 + bundle tests must pass
  2) add test: unknown phase (e.g. 99) returns empty native output
-->

<!-- step 4 sub-steps:
  1) verify no `collect_decl_parts(` in `decl_cpp.mlc`
  2) remove dead imports from line 10
-->

## Context (Planner 2026-06-02)

| Site | Phase | Caller |
|------|-------|--------|
| `collect_all_decl_parts_cpp:860` | 0 | type_fwds |
| `collect_all_decl_parts_cpp:861` | 1 | type_defs |
| `collect_all_decl_parts_cpp:855` | 2 | fn_protos |
| `collect_fn_defs_cpp:929` | 4 | per-decl trait struct append |
| implicit via `collect_all_decl_parts_cpp:863` | 3 | fn_defs (`collect_fn_defs_cpp`) |

Else branch (`phase` ∉ {0,1,2,3,4}): string bridge via `collect_decl_parts` → `cpp_decls_from_string_parts`. No production caller uses unknown phase.

## Step 1 audit (2026-06-02)

| Check | Result |
|-------|--------|
| Production entry | `module.mlc:150` → `collect_all_decl_parts_cpp` |
| Direct `collect_decl_parts_cpp` callers | `decl_cpp.mlc:855,860,861,929`; tests `test_decl_gen.mlc:570-586` |
| Phase literals in production | **0, 1, 2, 4** only (no dynamic phase) |
| Phase 3 path | `collect_all_decl_parts_cpp:863` → `collect_fn_defs_cpp` directly; **no** `collect_decl_parts_cpp(..., 3)` in production |
| Phase 3 branch (`:870-871`) | reachable only via explicit `collect_decl_parts_cpp(..., 3)` call — tests/production use `collect_fn_defs_cpp` instead |
| Else branch (`:872-873`) | `phase` ∉ {0,1,2,3,4} → string `collect_decl_parts` → `cpp_decls_from_string_parts`; **unreachable** in production |
| String semantics (unknown phase) | `decl.mlc:323-330` — `decl_segment` per decl, filtered; parity bundle only |
| Dead import | `collect_all_decl_parts` on `decl_cpp.mlc:10` — unused in file |
| verify | 886 pass; DIFF_EXIT=0 (`TMPDIR=.tmp_verify`) |

**Step 2 recommendation:** replace `:872-873` with `empty_cpp_declarations()`; phase 3 branch unchanged.

## Step 2 implementation (2026-06-02)

| Change | Result |
|--------|--------|
| `collect_decl_parts_cpp:872-873` | `empty_cpp_declarations()` instead of string bridge |
| `collect_decl_parts` call | **removed** from body; import still on `:10` (step 4) |

Result: 886 pass; DIFF_EXIT=0.

## Step 3 parity tests (2026-06-02)

| Test | Pattern |
|------|---------|
| existing `:569-587` | phases 0/1/2/4 vs string |
| existing `:593-628` | bundle + fn_defs parity |
| new `:589-590` | unknown phase 99 → length 0 |

Result: 887 pass; DIFF_EXIT=0.

## Step 4 call-site audit (2026-06-02)

| Check | Result |
|-------|--------|
| `collect_decl_parts(` in body | **none** |
| `collect_all_decl_parts(` in body | **none** |
| Import `:10` | removed `collect_decl_parts`, `collect_all_decl_parts`; kept `compute_fn_body_context`, `exported_type_names_from_decls` |
| String `collect_decl_parts` | `decl.mlc` parity bundle only |

Result: 887 pass; DIFF_EXIT=0.

## Step 5 audit (2026-06-02)

| Check | Result |
|-------|--------|
| Track goal | no string `collect_decl_parts` in `decl_cpp.mlc`; native else empty |
| Production | `collect_decl_parts_cpp:867-873` native-only dispatch |
| parity tests | phases 0–4 + bundle + unknown phase (`:569-628`, `:589-590`) |
| verify | 887 pass; DIFF_EXIT=0 |

## Survivors (confirmed)

| Site | Use |
|------|-----|
| `collect_decl_parts` / `collect_all_decl_parts` | `decl.mlc` parity bundle only |
| `gen_decl_cpp` print round-trips | `:396-405` Type/Extend |
| `gen_proto_cpp` print round-trip | `:439` Extend per-method |
| `cpp_decl_from_string_output` | empty fragments, struct_using, extern stubs, inline wrappers |

## Deferred (out of track)

- `gen_decl_cpp` / `gen_proto_cpp` print round-trips.
- `cpp_decl_from_string_output` fragments (extern stubs, struct_using).
- `collect_decl_parts` / `collect_all_decl_parts` — `decl.mlc` parity bundle only after import removal.
- Phase 4 `MLCC_BOOTSTRAP=1`.
- Parser `ref mut` — [TRACK_PLAN.md](TRACK_PLAN.md) step 15.
