# Track: Native empty CppDecl (Phase 2 §3 cont.)

Parent: [../PLAN.md](../PLAN.md) §Phase 2; previous: [TRACK_DECL_MULTI_DECL_BRIDGE.md](TRACK_DECL_MULTI_DECL_BRIDGE.md) (**closed**, step 5 audit)

## Status: **closed** (5/5, step 5 audit)

**Goal:** eliminate `cpp_decl_from_string_output('')` in `decl_cpp.mlc` — native empty `CppDecl` instead of empty `CppDeclFragment`. **Done.**

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
| 1 | Reachability audit — empty fragment sites; callers; tests | done |
| 2 | Native empty decl in `cpp_ast.mlc` + `cpp_printer.mlc` + tests | done |
| 3 | Replace all `cpp_decl_from_string_output('')` in `decl_cpp.mlc` | done |
| 4 | Parity tests — Import/AssocBind/extern/proto empty print | done |
| 5 | Survivors audit; close track | done |

## Step 5 audit (2026-06-02)

| Check | Result |
|-------|--------|
| Track goal | no `cpp_decl_from_string_output('')`; `empty_cpp_declaration()` at 9 sites |
| Empty fragment grep | **0** |
| Native node | `CppDeclEmpty` + `empty_cpp_declaration()` `:30-31` |
| parity tests | Import/AssocBind/Type/Trait + extern + Extend all-extern — pass |
| verify | 904 pass; DIFF_EXIT=0 |

## Survivors (confirmed)

| Site | Use |
|------|-----|
| `:192` | `cpp_using_from_struct_using_line` parse fallback → non-empty fragment |
| `:27-34` | `cpp_decl_from_string_output` export; `cpp_decls_from_string_parts` consumer |
| `derive_methods_cpp` | hash specialization `make_fragment_cpp_declaration` |
| `decl.mlc` / `decl_extend.mlc` | string parity bundle |

## Deferred (out of track)

- `cpp_using_from_struct_using_line` parse fallback (`:192`) — next track candidate.
- `derive_methods_cpp` hash specialization `make_fragment_cpp_declaration`.
- Non-empty `cpp_decl_from_string_output` (`:192`, `parts.map` consumer).
- Phase 4 `MLCC_BOOTSTRAP=1`.
- Parser `ref mut` — [TRACK_PLAN.md](TRACK_PLAN.md) step 15.

<!-- step 1 sub-steps:
  1) grep `cpp_decl_from_string_output('')` in decl_cpp.mlc
  2) map gen_decl_cpp / gen_proto_cpp / cpp_decl_from_native_declarations arms
  3) note existing tests (:752-756, :776-780) expecting print ''
-->

<!-- step 2 sub-steps:
  1) add CppDeclEmpty (or equivalent) variant
  2) print_decl → ''
  3) test_cpp_printer golden
-->

<!-- step 3 sub-steps:
  1) export empty_cpp_declaration() helper if useful
  2) replace 9 sites (:417, :430-431, :439, :442-443, :465-466, :952)
-->

## Context (Planner 2026-06-02)

| Site | Arm | Returns |
|------|-----|---------|
| `:417` | `gen_decl_cpp` extern fn | empty |
| `:430-431` | Import / AssocBind | empty |
| `:439` | `gen_proto_cpp` extern fn | empty |
| `:442-443` | Type / Trait proto | empty |
| `:465-466` | Import / AssocBind proto | empty |
| `:952` | `cpp_decl_from_native_declarations` length 0 | empty |

Existing parity tests in `test_decl_gen.mlc`: Import/AssocBind/Type/Trait empty (`:774-778`, `:780-784`, `:798-802`).

## Step 1 audit (2026-06-02)

| Check | Result |
|-------|--------|
| Empty fragment sites | **9** — all in `decl_cpp.mlc` |
| Non-empty `cpp_decl_from_string_output` | **1** — `:189` struct_using fallback (deferred) |

| Line | Function | Arm | When empty |
|------|----------|-----|------------|
| `:417` | `gen_decl_cpp` | `SDeclFn` + `SExprExtern` | standalone / exported generic extern fn body |
| `:430` | `gen_decl_cpp` | `SDeclImport` | always |
| `:431` | `gen_decl_cpp` | `SDeclAssocBind` | always |
| `:439` | `gen_proto_cpp` | `SDeclFn` + `SExprExtern` | standalone / exported generic extern proto |
| `:442` | `gen_proto_cpp` | `SDeclType` | always |
| `:443` | `gen_proto_cpp` | `SDeclTrait` | always |
| `:465` | `gen_proto_cpp` | `SDeclImport` | always |
| `:466` | `gen_proto_cpp` | `SDeclAssocBind` | always |
| `:952` | `cpp_decl_from_native_declarations` | `length == 0` | Extend proto with 0 non-extern methods |

| Production callers | Site | Path |
|--------------------|------|------|
| `decl_fn_def_cpp:973` | `:417` | exported generic `SDeclFn` extern → `gen_decl_cpp` |
| `decl_fn_def_cpp:976,980` | `:430-431` via recurse | exported inner Import/AssocBind |
| `decl_segment_cpp:883` | `:439` | phase 2 exported generic extern proto |
| `decl_segment_cpp:886,888` | `:442-443`, `:465-466` | phase 2 exported inner Type/Trait/Import/AssocBind |
| `gen_proto_cpp:463` | `:952` | Extend with all-extern or no fn protos |

| Tests (existing) | Lines | Covers |
|------------------|-------|--------|
| `gen_decl_cpp SDeclImport empty` | `:774-775` | `:430` |
| `gen_decl_cpp SDeclAssocBind empty` | `:777-778` | `:431` |
| `gen_proto_cpp SDeclType empty` | `:780-781` | `:442` |
| `gen_proto_cpp SDeclTrait empty` | `:783-784` | `:443` |
| `gen_proto_cpp SDeclImport empty` | `:798-799` | `:465` |
| `gen_proto_cpp SDeclAssocBind empty` | `:801-802` | `:466` |

| Tests (gap — step 4) | Site |
|----------------------|------|
| extern `SDeclFn` decl/proto | `:417`, `:439` |
| Extend all-extern proto collapse | `:952` |

**Step 2 recommendation:** add `CppDeclEmpty`; `print_decl` → `''`; shared `empty_cpp_declaration()` helper; replace 9 sites.

## Step 2 implementation (2026-06-02)

| Change | Result |
|--------|--------|
| `CppDeclEmpty` | `cpp_ast.mlc:74` |
| `print_decl_node` | `CppDeclEmpty => ''` |
| Printer test | `test_cpp_printer.mlc:250-251` |

Result: 896 pass; DIFF_EXIT=0.

## Step 3 implementation (2026-06-02)

| Change | Result |
|--------|--------|
| `empty_cpp_declaration()` | `Shared.new(CppDeclEmpty)` |
| 9 sites | `:420`, `:433-434`, `:442`, `:445-446`, `:468-469`, `:955` |
| grep `cpp_decl_from_string_output('')` | **0** |

Result: 896 pass; DIFF_EXIT=0.

## Step 4 implementation (2026-06-02)

| Change | Result |
|--------|--------|
| `is_cpp_decl_empty` helper | `test_decl_gen.mlc` |
| extern fn decl/proto | native `CppDeclEmpty` + empty print |
| Extend all-extern proto | native `CppDeclEmpty` via `:955` collapse |
| Import/AssocBind | native node assertions added |

Result: 904 pass; DIFF_EXIT=0.

## Deferred (out of track)

- `cpp_using_from_struct_using_line` parse fallback (`:192`) — next track candidate.
- `derive_methods_cpp` hash specialization `make_fragment_cpp_declaration`.
- Non-empty `cpp_decl_from_string_output` (`:192`, `parts.map` consumer).
- Phase 4 `MLCC_BOOTSTRAP=1`.
- Parser `ref mut` — [TRACK_PLAN.md](TRACK_PLAN.md) step 15.
