# Track: Native gen_decl_cpp / gen_proto_cpp print round-trips (Phase 2 §3 cont.)

Parent: [../PLAN.md](../PLAN.md) §Phase 2; previous: [TRACK_COLLECT_DECL_PARTS_CPP_BRIDGE.md](TRACK_COLLECT_DECL_PARTS_CPP_BRIDGE.md) (**closed**, step 5 audit)

## Status: **closed** (step 5 audit, uncommitted)

**Goal:** eliminate `cpp_decl_from_string_output(print_cpp_declarations(...))` in `gen_decl_cpp` and `gen_proto_cpp` — native multi-`CppDecl` collapsed via print→fragment (`decl_cpp.mlc:396-400`, `:403-405`, `:439`).

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
| 1 | Reachability audit — three print round-trip sites; callers; return-type constraints | done |
| 2 | `gen_decl_cpp` Type arm — native multi-decl without print round-trip | done |
| 3 | `gen_decl_cpp` Extend arm — native without print round-trip | done |
| 4 | `gen_proto_cpp` Extend arm — native without print round-trip | done |
| 5 | Parity tests audit; survivors; close track | done |

<!-- step 1 sub-steps:
  1) grep `print_cpp_declarations` inside `gen_decl_cpp` / `gen_proto_cpp`
  2) map production callers of `gen_decl_cpp` / `gen_proto_cpp` for Type/Extend
  3) note `Shared<CppDecl>` return vs native `[Shared<CppDecl>]` at call sites
-->

<!-- step 2 sub-steps:
  1) `SDeclType` `:395-400` — replace print wrapper; record may emit body-only native
  2) existing `test_decl_gen.mlc` Type tests must pass
-->

<!-- step 3 sub-steps:
  1) `SDeclExtend` `:403-405` — native fragment or caller returns array
  2) extend parity tests in `test_decl_gen.mlc`
-->

<!-- step 4 sub-steps:
  1) `gen_proto_cpp` Extend `:420-439` — eliminate print on `proto_declarations`
  2) extend proto parity tests
-->

## Context (Planner 2026-06-02)

| Site | Pattern | Returns |
|------|---------|---------|
| `gen_decl_cpp` `SDeclType` `:395-400` | print(fwd+body native) → fragment | `Shared<CppDecl>` |
| `gen_decl_cpp` `SDeclExtend` `:403-405` | print(`gen_decl_extend_cpp`) → fragment | `Shared<CppDecl>` |
| `gen_proto_cpp` `SDeclExtend` `:420-439` | print(proto array) → fragment | `Shared<CppDecl>` |

`cpp_decl_from_string_output` = `CppDeclFragment(code)` — no parse; print is the bridge.

Production callers of `gen_decl_cpp`: `decl_fn_def_cpp:904-911` (exported inner); tests `test_decl_gen.mlc`.

## Step 1 audit (2026-06-02)

| Check | Result |
|-------|--------|
| Print round-trip sites | **3 only** — `gen_decl_cpp` Type `:395-400`, Extend `:403-405`; `gen_proto_cpp` Extend `:439` |
| Pattern | native `[Shared<CppDecl>]` → `print_cpp_declarations` → `cpp_decl_from_string_output` (= `CppDeclFragment`) |
| Return constraint | both fns return `Shared<CppDecl>`; native helpers return `[Shared<CppDecl>]` |
| Module bundle bypass | `decl_segment_cpp:782-786` Type phase 0/1 native; `decl_fn_def_cpp:897-910` Extend → `gen_decl_extend_cpp` array |
| Extend protos bypass | `extend_method_forward_segments_cpp:462`, `extend_helper_protos:497`, `extend_forward:754` — per-method `gen_proto_cpp`, not Extend decl |
| `gen_decl_cpp` production | exported inner `SDeclFn` only (`:904-907`); Type/Extend **not** via `gen_decl_cpp` in module path |
| `gen_proto_cpp(extend)` production | **tests only** (`test_decl_gen.mlc:755`); module uses per-method native paths |
| Type native shape | `gen_type_decl_fwd_cpp` → `[]` (record) or fwd array (sum); `gen_type_decl_body_cpp` → body array |
| Extend native shape | `gen_decl_extend_cpp:672` → `[Shared<CppDecl>]` (methods + extern stubs) |
| Existing tests | Type record compares body print (`:720-721`); sum/trait/extend compare string `gen_decl` |
| verify | 887 pass; DIFF_EXIT=0 (`TMPDIR=.tmp_verify`) |

**Step 2 recommendation:** Type record — return native body decl directly (single element); sum — explicit fragment helper or body-only where fwd empty; avoid print when `[Shared<CppDecl>].length() == 1`.

## Step 2 implementation (2026-06-02)

| Change | Result |
|--------|--------|
| `cpp_decl_from_native_declarations` | 0→empty; 1→direct; 2+→fragment via print |
| `gen_decl_cpp` Type `:395-399` | uses helper instead of unconditional print round-trip |

Result: 887 pass; DIFF_EXIT=0.

## Step 3 implementation (2026-06-02)

| Change | Result |
|--------|--------|
| `gen_decl_cpp` Extend `:402-404` | `cpp_decl_from_native_declarations(gen_decl_extend_cpp(...))` |

Result: 887 pass; DIFF_EXIT=0.

## Step 4 implementation (2026-06-02)

| Change | Result |
|--------|--------|
| `gen_proto_cpp` Extend `:438` | `cpp_decl_from_native_declarations(proto_declarations)` |
| `print_cpp_declarations` in `gen_decl_cpp`/`gen_proto_cpp` | **none** (helper internal only for 2+ decls) |

Result: 887 pass; DIFF_EXIT=0.

## Step 5 audit (2026-06-02)

| Check | Result |
|-------|--------|
| Track goal | no direct `print_cpp_declarations` in `gen_decl_cpp`/`gen_proto_cpp` arms |
| Helper | `cpp_decl_from_native_declarations:889-895` — print only when length > 1 |
| parity tests | Type `:719-725`, Extend `:301-312`, proto Extend `:754-755` — all pass |
| verify | 887 pass; DIFF_EXIT=0 |

## Survivors (confirmed)

| Site | Use |
|------|-----|
| `cpp_decl_from_native_declarations` 2+ path | print→fragment for multi-decl (sum type, multi-method extend) |
| `cpp_decl_from_string_output` | empty fragments, struct_using `:208`, inline wrappers `:556-655` |
| `gen_decl` / `gen_proto` | `decl.mlc` parity bundle only |

## Deferred (out of track)

- Other `cpp_decl_from_string_output` sites (empty fragments, struct_using, inline wrappers `:557`, `:579`, `:655`).
- `gen_decl` / `gen_proto` string bundle — `decl.mlc` parity only.
- Phase 4 `MLCC_BOOTSTRAP=1`.
- Parser `ref mut` — [TRACK_PLAN.md](TRACK_PLAN.md) step 15.
