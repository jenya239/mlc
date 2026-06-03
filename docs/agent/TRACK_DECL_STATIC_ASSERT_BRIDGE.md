# Track: Native CppStaticAssert (Phase 2 §3 cont.)

Parent: [../PLAN.md](../PLAN.md) §Phase 2; previous: [TRACK_DECL_CPP_FRAGMENT_BRIDGE.md](TRACK_DECL_CPP_FRAGMENT_BRIDGE.md) (**closed**, step 5 audit)

## Status: **closed** (5/5, step 5 audit)

**Goal:** replace `make_fragment_cpp_declaration(static_assert_…)` in `extend_trait_static_assert_decl_cpp` with native `CppStaticAssert` node.

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
| 1 | Reachability audit — static_assert fragment site; string helpers; tests | done |
| 2 | `CppStaticAssert` in `cpp_ast.mlc` + `cpp_printer.mlc` + printer tests | done |
| 3 | Native `extend_trait_static_assert_decl_cpp` (`:650-667`) | done |
| 4 | Parity tests — trait suffix static_assert output | done |
| 5 | Survivors audit; close track | done |

## Step 5 audit (2026-06-02)

| Check | Result |
|-------|--------|
| Track goal | static_assert site native; no `make_fragment` for static_assert in `decl_cpp.mlc` |
| Fragment grep | **0** static_assert fragments; `CppStaticAssert` at `:662`, `:665` |
| parity tests | Display + ExprVisitor assert_eq; full trait suffix assert_eq — pass |
| verify | 890 pass; DIFF_EXIT=0 |

## Survivors (confirmed)

| Site | Use |
|------|-----|
| `:417-466` | empty `cpp_decl_from_string_output('')` in gen_decl_cpp/gen_proto_cpp |
| `:189` | `cpp_using_from_struct_using_line` parse fallback |
| `:952-956` | `cpp_decl_from_native_declarations` empty + 2+ print path |
| `derive_methods_cpp` | hash specialization `make_fragment_cpp_declaration` — out of track |
| `decl.mlc` / `decl_extend.mlc` | string parity bundle via `expr.mlc` helpers |

## Deferred (out of track)

- Empty `cpp_decl_from_string_output('')` arms.
- `cpp_decl_from_native_declarations` 2+ print path.
- Phase 4 `MLCC_BOOTSTRAP=1`.
- Parser `ref mut` — [TRACK_PLAN.md](TRACK_PLAN.md) step 15.

## Step 1 audit (2026-06-02)

| Check | Result |
|-------|--------|
| Fragment sites | **2 arms** — ExprVisitor + generic trait |
| Caller | `extend_trait_static_assert_decl_cpp` ← `gen_decl_extend_trait_suffix_cpp:698` |
| verify | 887 pass; DIFF_EXIT=0 |

## Step 2–4 implementation (2026-06-02)

| Change | Result |
|--------|--------|
| `CppStaticAssert` AST + printer | `test_cpp_printer.mlc` golden tests |
| `extend_trait_static_assert_decl_cpp` | native both arms |
| Parity tests | Display + ExprVisitor (`test_decl_gen.mlc:292-305`) |

Result: 890 pass; DIFF_EXIT=0.
