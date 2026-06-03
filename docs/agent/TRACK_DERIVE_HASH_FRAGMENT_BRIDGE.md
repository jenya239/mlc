# Track: Native std::hash specialization (Phase 2 §3 cont.)

Parent: [../PLAN.md](../PLAN.md) §Phase 2; previous: [TRACK_DECL_STRUCT_USING_BRIDGE.md](TRACK_DECL_STRUCT_USING_BRIDGE.md) (**closed**, step 5 audit)

## Status: **closed** (step 5 audit, 5/5)

**Goal:** eliminate `make_fragment_cpp_declaration` in `hash_specialization_declaration` — native `CppDecl` for `std::hash<T>` specialization instead of string fragment. **Achieved.**

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
| 1 | Reachability audit — `hash_specialization_declaration`; callers; tests | done |
| 2 | Native hash spec node in `cpp_ast.mlc` + `cpp_printer.mlc` + golden test | done |
| 3 | `hash_specialization_declaration` → native node; drop fragment | done |
| 4 | Parity tests — record + sum Hash derive; not `CppDeclFragment` | done |
| 5 | Survivors audit; close track | done |

<!-- step 1 sub-steps:
  1) grep `hash_specialization_declaration` and `make_fragment_cpp_declaration` in derive_methods_cpp
  2) trace body via `derive_hash_record_cpp` / `derive_hash_sum_cpp` → `gen_derive_hash_cpp`
  3) note existing tests test_decl_gen.mlc :755-774
-->

<!-- step 2 sub-steps:
  1) add CppStdHashSpecialization { type_name, body: [CppStmt] } or equivalent
  2) printer: namespace std { template<> struct hash<T> { operator() ... } }
  3) test_cpp_printer golden
-->

<!-- step 3 sub-steps:
  1) replace hash_specialization_declaration string concat + print_hash_operator_body
  2) keep body statements native (already CppStmt)
-->

## Context (Planner 2026-06-02)

| Site | Pattern |
|------|---------|
| `derive_methods_cpp.mlc:491-496` | string `namespace std { template<> struct hash<T> ...` → `CppDeclFragment` |
| `derive_methods_cpp.mlc:498-508` | `derive_hash_record_cpp` / `derive_hash_sum_cpp` callers |
| `derive_methods_cpp.mlc:504-508` | `gen_derive_hash_cpp` export |
| `type_gen.mlc` | string `derive_hash_record` / `derive_hash_sum` (parallel path) |

Prior work: [TRACK_DERIVE_BRIDGE.md](TRACK_DERIVE_BRIDGE.md) — Display/Eq/Ord native `CppFnDef`; Hash body stmts native but wrapper still fragment.

## Step 1 audit (2026-06-02)

| Check | Result |
|-------|--------|
| Fragment site (decl) | **1** — `:491-496` `hash_specialization_declaration` |
| Fragment sites (stmt, body) | **3** — `:364-371` combine/seed/empty-return (`make_fragment_cpp_statement`) |
| `make_fragment_cpp_declaration` in file | **1** — `:492` |
| `hash_specialization_declaration` callers | **2** — `derive_hash_record_cpp:499`, `derive_hash_sum_cpp:502` |
| Production path | `gen_type_decl_body_cpp:248` → `gen_derive_methods_cpp` → `gen_derive_trait_cpp` → `gen_derive_hash_cpp` |
| String round-trip in wrapper | `print_hash_operator_body:481-488` re-prints `[CppStmt]` before fragment |
| String parallel path | `type_gen.mlc` `derive_hash_record` / `derive_hash_sum` |

| Tests | Site | Coverage |
|-------|------|----------|
| String derive Hash | `test_decl_gen.mlc:216-251` | record, empty, sum unit, sum tuple |
| Native `gen_derive_hash_cpp` | `:755-762` | record + sum print contains |
| Native wiring | `:772-774` | `gen_type_decl_body_cpp` + Hash |
| Native node type | **0** — no anti-fragment assertion (step 4) |

Result: **913 pass; DIFF_EXIT=0**.

## Step 2 implementation (2026-06-02)

| Change | Site |
|--------|------|
| `CppStdHashSpecialization(type_name, body)` | `cpp_ast.mlc:73` |
| `std_hash_specialization_definition` | `cpp_printer.mlc:350-368, :482-483` |
| Golden test | `test_cpp_printer.mlc:253-262` |

Result: **914 pass; DIFF_EXIT=0**.

## Step 3 implementation (2026-06-02)

| Change | Site |
|--------|------|
| `hash_specialization_declaration` → `Shared.new(CppStdHashSpecialization(...))` | `derive_methods_cpp.mlc:480-481` |
| Removed `print_hash_operator_body` + `print_statement` import | same file |
| `make_fragment_cpp_declaration` in file | **0** |

Result: **914 pass; DIFF_EXIT=0**.

**Step 4:** parity tests — record + sum Hash derive; assert not `CppDeclFragment`.

## Step 4 implementation (2026-06-02)

| Change | Site |
|--------|------|
| `cpp_decl_is_std_hash_specialization`, `cpp_decl_is_fragment`, list helper | `test_decl_gen.mlc:115-139` |
| record + sum `gen_derive_hash_cpp` native node asserts | `:791-800` |
| `gen_type_decl_body_cpp` record + sum Hash wiring asserts | `:803-808` |

Result: **920 pass; DIFF_EXIT=0**.

**Step 5:** survivors audit; close track.

## Step 5 audit (2026-06-02)

| Check | Result |
|-------|--------|
| `make_fragment_cpp_declaration` in `derive_methods_cpp.mlc` | **0** |
| `hash_specialization_declaration` | `CppStdHashSpecialization` at `:480-481` |
| Stmt fragment survivors (body) | **3** — `:363-370` `make_fragment_cpp_statement` (combine/seed/empty-return); **deferred** |
| `make_fragment_cpp_declaration` elsewhere in `compiler/` | `decl_cpp.mlc:28` (`cpp_decl_from_string_output`), `emit_helpers.mlc` export — **deferred** |
| String parallel path | `type_gen.mlc` `derive_hash_record` / `derive_hash_sum` — **deferred** |

Result: **920 pass; DIFF_EXIT=0**. Track **closed** (5/5).

## Deferred (out of track)

- `module.mlc` — `cpp_declarations_from_text_parts` (guards, includes, namespace scaffolding).
- `cpp_decl_from_string_output` export + `cpp_decls_from_string_parts` (no production consumer).
- `type_gen.mlc` — inline `struct_using_lines` in string struct body.
- Hash body stmt fragments (`make_fragment_cpp_statement` in derive hash body) — separate track candidate.
- Phase 4 `MLCC_BOOTSTRAP=1`.
- Parser `ref mut` — [TRACK_PLAN.md](TRACK_PLAN.md) step 15.
