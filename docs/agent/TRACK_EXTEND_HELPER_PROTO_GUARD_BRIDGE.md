# Track: Native extend helper proto guard (Phase 2 §3 cont.)

Parent: [../PLAN.md](../PLAN.md) §Phase 2; previous: [TRACK_PROTO_CPP_CATCHALL_BRIDGE.md](TRACK_PROTO_CPP_CATCHALL_BRIDGE.md) (**closed**, step 5 audit)

## Status: **closed** (step 5 audit, uncommitted)

**Goal:** eliminate `const prototype = gen_proto(methods[method_index], context)` length guard in `extend_helper_protos_for_exported_types_cpp` — last production string `gen_proto` call in `decl_cpp.mlc` (was `:492`).

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
| 1 | Reachability audit — confirm `:492` sole production `gen_proto` in `decl_cpp.mlc` | done |
| 2 | Native guard — `SDeclFn` + non-extern check before `gen_proto_cpp` push | done |
| 3 | Parity tests — helper protos unchanged vs string bundle | done |
| 4 | Audit call sites — drop `gen_proto` import from `decl_cpp.mlc` if unused | done |
| 5 | Audit survivors; close track | done |

## Step 5 audit (2026-06-02)

| Check | Result |
|-------|--------|
| Track goal | no string `gen_proto` in `decl_cpp.mlc`; native guard in helper |
| Production | `extend_helper_protos_for_exported_types_cpp:488-503` native-only |
| parity tests | helper + extern skip + bundle (`:611-623`) |
| verify | 886 pass; DIFF_EXIT=0 |

## Survivors (confirmed)

| Site | Use |
|------|-----|
| `gen_proto` / `extend_helper_protos_for_exported_types` | `decl.mlc` parity bundle |
| `collect_decl_parts_cpp` else `:873` | unknown-phase string fallback |
| `gen_decl_cpp` / `gen_proto_cpp` print wrappers | deferred separate tracks |

## Step 1 audit (2026-06-02)

| Check | Result |
|-------|--------|
| Direct `gen_proto` in `decl_cpp.mlc` | **only** `:492` (import `:10`) |
| Indirect string | `collect_decl_parts_cpp` else `:864` → `collect_decl_parts` (unknown phase); not `gen_proto` direct |
| Target function | `extend_helper_protos_for_exported_types_cpp:479-504` |
| Production caller | `collect_all_decl_parts_cpp:849` → `fn_protos` append |
| Guard semantics | `gen_proto` length > 0 ≡ skip extern / empty (string returns `''` for extern) |
| Native mirror | `extend_method_forward_segments_cpp:457-465` — `SDeclFn` + non-extern before push |
| String parity | `decl.mlc:251-252` same guard; tests `test_decl_gen.mlc:611-616` |
| Step 2 recommendation | replace `:491-493` with match on `SDeclFn` body non-extern; drop `gen_proto` import if unused |

## Step 2 implementation (2026-06-02)

| Change | Result |
|--------|--------|
| `extend_helper_protos_for_exported_types_cpp` | `SDeclFn` + non-extern match before `gen_proto_cpp` push |
| `gen_proto` import | removed from `decl_cpp.mlc` |

Result: 885 pass; DIFF_EXIT=0.

## Step 3 parity tests (2026-06-02)

| Test | Pattern |
|------|---------|
| existing `:611-616` | helper matches string; skips non-exported |
| existing `:621-623` | `collect_all_decl_parts_cpp.fn_protos` exported extend |
| new `:617-620` | skips extern method (native guard) |

Result: 886 pass; DIFF_EXIT=0.

## Step 4 call-site audit (2026-06-02)

| Check | Result |
|-------|--------|
| `gen_proto` import | **absent** from `decl_cpp.mlc:10` (removed step 2) |
| Direct `gen_proto(` calls | **none** in `decl_cpp.mlc` |
| `gen_proto_cpp` only | helper `:497`, forward segments `:462`, extend forward `:754`, `decl_segment_cpp` |
| Indirect string | `collect_decl_parts` via `collect_decl_parts_cpp` else `:873` only |
| String `gen_proto` | `decl.mlc` parity bundle only |

## Deferred (out of track)

- `collect_decl_parts_cpp` unknown-phase string fallback elimination.
- `gen_decl_cpp`/`gen_proto_cpp` print round-trip removal.
- Native CppFnDef for inline trait wrapper / extern fragments.
- Phase 4 `MLCC_BOOTSTRAP=1`.
- Parser `ref mut` — [TRACK_PLAN.md](TRACK_PLAN.md) step 15.
