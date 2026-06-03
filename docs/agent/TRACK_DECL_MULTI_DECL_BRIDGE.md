# Track: Native multi-decl collapse (Phase 2 §3 cont.)

Parent: [../PLAN.md](../PLAN.md) §Phase 2; previous: [TRACK_DECL_STATIC_ASSERT_BRIDGE.md](TRACK_DECL_STATIC_ASSERT_BRIDGE.md) (**closed**, step 5 audit)

## Status: **closed** (5/5, step 5 audit)

**Goal:** eliminate `cpp_decl_from_string_output(print_cpp_declarations(...))` in `cpp_decl_from_native_declarations` (`decl_cpp.mlc:956`) — native multi-`CppDecl` without print round-trip. **Done.**

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
| 1 | Reachability audit — 2+ print path; callers; when length > 1 | done |
| 2 | Native multi-decl node in `cpp_ast.mlc` + `cpp_printer.mlc` + tests | done |
| 3 | `cpp_decl_from_native_declarations` 2+ arm — native node | done |
| 4 | Parity tests — Type/Extend multi-decl print | done |
| 5 | Survivors audit; close track | done |

## Step 5 audit (2026-06-02)

| Check | Result |
|-------|--------|
| Track goal | 2+ path native; no `print_cpp_declarations` → fragment in helper |
| Print round-trip grep | **0** — `cpp_decl_from_string_output(print…)` absent |
| Native node | `CppDeclSequence` at `:956`; printer `print_decl_sequence` |
| parity tests | Type sum (7 children) + Extend Display (4 children) — pass |
| verify | 895 pass; DIFF_EXIT=0 |

## Survivors (confirmed)

| Site | Use |
|------|-----|
| `:952` | empty `cpp_decl_from_string_output('')` in `cpp_decl_from_native_declarations` |
| `:417-466` | empty `cpp_decl_from_string_output('')` in gen_decl_cpp/gen_proto_cpp |
| `:189` | `cpp_using_from_struct_using_line` parse fallback |
| `derive_methods_cpp` | hash specialization `make_fragment_cpp_declaration` — out of track |
| `decl.mlc` / `decl_extend.mlc` | string parity bundle via `expr.mlc` helpers |
| `print_cpp_declarations` | export; tests + direct array print only (not collapse helper) |

## Deferred (out of track)

- Empty `cpp_decl_from_string_output('')` arms (`:417-466`, `:952`).
- `cpp_using_from_struct_using_line` parse fallback (`:189`).
- `derive_methods_cpp` hash specialization fragment.
- Phase 4 `MLCC_BOOTSTRAP=1`.
- Parser `ref mut` — [TRACK_PLAN.md](TRACK_PLAN.md) step 15.

<!-- step 1 sub-steps:
  1) grep `cpp_decl_from_native_declarations` and trace length>1 cases
  2) map gen_decl_cpp Type/Extend, gen_proto_cpp Extend callers
  3) note empty arm (:952) — deferred or same track step 3
-->
<!-- step 2 sub-steps:
  1) add composite CppDecl variant (e.g. CppDeclSequence) holding [Shared<CppDecl>]
  2) print: join child decls same as print_cpp_declarations
  3) test_cpp_printer golden case
-->

<!-- step 3 sub-steps:
  1) replace :956 print→fragment with native sequence node
  2) optional: empty arm (:952) — empty sequence or keep fragment
-->

## Context (Planner 2026-06-02)

| Site | Pattern | Returns |
|------|---------|---------|
| `:950-957` | 0→empty fragment; 1→direct; 2+→print→fragment | `Shared<CppDecl>` |
| `:421-424` | Type fwd+body via helper | `gen_decl_cpp` |
| `:428-429` | Extend methods via helper | `gen_decl_cpp` |
| `:463` | Extend protos via helper | `gen_proto_cpp` |

Empty `cpp_decl_from_string_output('')` in gen_decl_cpp/gen_proto_cpp catch-alls — deferred (separate track).

## Step 1 audit (2026-06-02)

| Check | Result |
|-------|--------|
| Print round-trip site | **1** — `:956` only (`print_cpp_declarations` → fragment) |
| Helper arms | 0→empty fragment `:952`; 1→direct `:954`; 2+→print `:956` |
| Callers | **3** — `gen_decl_cpp` Type `:421`, Extend `:428`; `gen_proto_cpp` Extend `:463` |

| Caller | Input shape | length > 1 when |
|--------|-------------|-----------------|
| Type `:421-424` | fwd + body + derive | sum type (fwd variants + alias); derive adds methods |
| Extend `:428-429` | `gen_decl_extend_cpp` | trait extend (method body + suffix wrappers) |
| proto Extend `:463` | per-method protos | extend with 2+ non-extern methods |

| Production bypass | `decl_segment_cpp` / `collect_*` use native arrays; `gen_decl_cpp` Type/Extend **tests + exported-inner only** |
| Record type | fwd `[]`, body 1 struct → **length 1** (test `:729-731` compares body print) |
| Sum type | fwd N variants + body alias → **2+** (test `:733-735`) |
| Trait extend Display | method + suffix → **2+** (test `:311-312`) |

| Tests | `:729-735` Type; `:311-312` Extend decl; `:764-766` Extend proto |
| verify | 890 pass; DIFF_EXIT=0 |

**Step 2 recommendation:** add `CppDeclSequence([Shared<CppDecl>])`; printer joins like `print_cpp_declarations` (concat, no `\n\n`); replace `:956`.

## Step 2 implementation (2026-06-02)

| Change | Result |
|--------|--------|
| `CppDeclSequence([Shared<CppDecl>])` | `cpp_ast.mlc:73` |
| `print_decl_sequence` | join child decls with `''` (matches `print_cpp_declarations`) |
| Printer test | `test_cpp_printer.mlc:244-248` |

Result: 891 pass; DIFF_EXIT=0.

## Step 3 implementation (2026-06-02)

| Change | Result |
|--------|--------|
| `:956` 2+ arm | `Shared.new(CppDeclSequence(declarations))` — no `print_cpp_declarations` |
| import | `CppDeclSequence` from `cpp_ast.mlc` |
| empty arm `:952` | unchanged — `cpp_decl_from_string_output('')` (deferred) |

Result: 891 pass; DIFF_EXIT=0.

## Step 4 implementation (2026-06-02)

| Change | Result |
|--------|--------|
| `test_decl_gen.mlc` | `cpp_decl_sequence_children` helper |
| Type sum | child count 7; print parity vs `print_cpp_declarations(children)` |
| Extend Display | child count 4; print parity vs children |

Result: 895 pass; DIFF_EXIT=0.

## Deferred (out of track)

- Empty `cpp_decl_from_string_output('')` arms (`:417-466`, `:952`).
- `cpp_using_from_struct_using_line` parse fallback (`:189`).
- `derive_methods_cpp` hash specialization fragment.
- Phase 4 `MLCC_BOOTSTRAP=1`.
- Parser `ref mut` — [TRACK_PLAN.md](TRACK_PLAN.md) step 15.
