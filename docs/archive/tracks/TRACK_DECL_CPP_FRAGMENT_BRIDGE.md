# Track: Native CppDecl string fragments (Phase 2 §3 cont.)

Parent: [../PLAN.md](../PLAN.md) §Phase 2; previous: [TRACK_DECL_PRINT_ROUNDTRIP_BRIDGE.md](TRACK_DECL_PRINT_ROUNDTRIP_BRIDGE.md) (**closed**, step 5 audit)

## Status: **closed** (5/5, step 5 audit)

**Goal:** replace non-empty `cpp_decl_from_string_output(...)` in `decl_cpp.mlc` with native `CppDecl` nodes — struct_using, extend inline wrappers, extern `i32.to_string` stub.

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
| 1 | Reachability audit — non-empty fragment sites; string helpers; tests | done |
| 2 | Native `CppFnDef` for extend trait wrapper + concept adapter | done |
| 3 | Native `CppFnDef` for extern `i32.to_string` | done |
| 4 | Native `CppUsing` for struct_using lines | done |
| 5 | Parity tests audit; survivors; close track | done |

## Step 5 audit (2026-06-02)

| Check | Result |
|-------|--------|
| Track goal | all 4 non-empty `cpp_decl_from_string_output` sites replaced |
| Print parity fix | `cpp_printer`: `body_statement_depth == -1` → compact `noexcept { return …; }` |
| parity tests | extend wrapper/suffix, extern to_string, assoc extend — all pass |
| verify | 887 pass; DIFF_EXIT=0 (`TMPDIR=.tmp_verify`) |

## Survivors (confirmed)

| Site | Use |
|------|-----|
| `:189` | `cpp_using_from_struct_using_line` fallback for unparseable lines |
| `:417-466` | empty `cpp_decl_from_string_output('')` in gen_decl_cpp/gen_proto_cpp |
| `:662-668` | `extend_trait_static_assert_decl_cpp` — `make_fragment_cpp_declaration` |
| `:957-961` | `cpp_decl_from_native_declarations` empty + 2+ print path |
| `decl.mlc` / `decl_extend.mlc` | string parity bundle via `expr.mlc` helpers |

## Deferred (out of track)

- Empty `cpp_decl_from_string_output('')` arms (extern/type/trait/import/assoc).
- `cpp_decl_from_native_declarations` 2+ print path.
- `extend_trait_static_assert_decl_cpp` fragment.
- Phase 4 `MLCC_BOOTSTRAP=1`.
- Parser `ref mut` — [TRACK_PLAN.md](TRACK_PLAN.md) step 15.

## Step 1 audit (2026-06-02)

| Check | Result |
|-------|--------|
| Non-empty `cpp_decl_from_string_output` | **4 sites** — struct_using, trait wrapper, concept adapter, extern stub |
| Empty fragments | gen_decl_cpp/gen_proto_cpp arms — deferred |
| Multi-decl print | `cpp_decl_from_native_declarations` — out of track |

## Step 2–4 implementation (2026-06-02)

| Change | Result |
|--------|--------|
| `native_inline_noexcept_forwarding_fn_cpp` | trait wrapper + concept adapter |
| `native_noexcept_one_line_returning_fn_cpp` | i32 extern `to_string` |
| `cpp_using_from_struct_using_line` + `struct_using_declarations_cpp` | native `CppUsing` per line |

Result: 887 pass; DIFF_EXIT=0.
