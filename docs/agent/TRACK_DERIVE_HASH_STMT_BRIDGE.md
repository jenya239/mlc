# Track: Native derive Hash body stmts (Phase 2 §3 cont.)

Parent: [../PLAN.md](../PLAN.md) §Phase 2; previous: [TRACK_DERIVE_HASH_FRAGMENT_BRIDGE.md](TRACK_DERIVE_HASH_FRAGMENT_BRIDGE.md) (**closed**, step 5 audit)

## Status: **closed** (step 5 audit, 5/5)

**Goal:** eliminate `make_fragment_cpp_statement` in derive Hash body helpers (`derive_methods_cpp.mlc:362-370`) — native `CppStmt` for seed, combine, and empty-return instead of string fragments. **Achieved.**

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
| 1 | Reachability audit — 3 stmt fragment sites; callers; tests | done |
| 2 | Native stmt nodes / helpers (typed local `size_t h`, combine expr, cast return) + printer if needed | done |
| 3 | Wire `make_hash_*_statement` helpers → native nodes; drop fragments | done |
| 4 | Parity tests — record + sum Hash body; not `CppStmtFragment` | done |
| 5 | Survivors audit; close track | done |

<!-- step 1 sub-steps:
  1) grep `make_fragment_cpp_statement` in derive_methods_cpp (expect 3)
  2) trace via derive_hash_record_body_statements / derive_hash_sum_body_statements
  3) note existing print-contains tests test_decl_gen.mlc :781-788
-->

<!-- step 2 sub-steps:
  1) seed: `size_t h = …` — likely needs typed local decl (not CppConstDecl/CppAutoDecl)
  2) combine: `h ^= std::hash<T>{}(…) + …` as CppExprStmt + CppBinary
  3) empty return: CppReturn + CppCastStatic size_t
-->

<!-- step 3 sub-steps:
  1) replace make_hash_combine_statement
  2) replace make_hash_seed_declaration_statement
  3) replace make_hash_empty_return_statement
-->

## Context (Planner 2026-06-02)

| Site | Pattern |
|------|---------|
| `derive_methods_cpp.mlc:362-364` | combine line → `CppStmtFragment` |
| `derive_methods_cpp.mlc:366-367` | FNV seed `size_t h = …` → fragment |
| `derive_methods_cpp.mlc:369-370` | empty-record return → fragment |
| `derive_methods_cpp.mlc:375-389` | `derive_hash_record_body_statements` consumer |
| `derive_methods_cpp.mlc:391+` | sum variant body (reuses combine/seed) |

Prior work: [TRACK_DERIVE_HASH_FRAGMENT_BRIDGE.md](TRACK_DERIVE_HASH_FRAGMENT_BRIDGE.md) — `CppStdHashSpecialization` decl wrapper native; body stmts still fragments.

**Why this track (over deferred):** bounded 3-site scope in one file; completes Hash derive native path. `module.mlc` TU scaffolding and `type_gen.mlc` string hash remain deferred (wider / parallel path).

## Step 1 audit (2026-06-02)

| Check | Result |
|-------|--------|
| `make_fragment_cpp_statement` call sites | **3** — `:363`, `:367`, `:370` |
| Fragment helpers | `make_hash_combine_statement:362-364`, `make_hash_seed_declaration_statement:366-367`, `make_hash_empty_return_statement:369-370` |
| Native helper (no fragment) | `make_hash_return_h_statement:372-373` — `CppReturn` + ident |
| Record body consumer | `derive_hash_record_body_statements:375-389` — empty → empty-return; else seed + N×combine + return h |
| Sum body consumer | `derive_hash_sum_body_statements:466-477` — seed + variant branches + return h |
| Combine reuse | `derive_hash_discriminant_combine_statement:399`, tuple `:411`, record variant `:428` |
| Production path | `gen_derive_hash_cpp:489` → `derive_hash_record_cpp` / `derive_hash_sum_cpp` → `hash_specialization_declaration` (`CppStdHashSpecialization`) |
| `make_fragment_cpp_statement` elsewhere in file | **0** (only the 3 helpers) |

| Tests | Site | Coverage |
|-------|------|----------|
| String derive Hash | `test_decl_gen.mlc:216-251` | record fields, empty seed, sum variants |
| Native print contains | `:781-788` | struct hash, combine i32, holds_alternative |
| Native decl node | `:790-808` | `CppStdHashSpecialization`, not `CppDeclFragment` |
| Anti-fragment stmt | **0** — no `CppStmtFragment` assertion (step 4) |

Result: **920 pass; DIFF_EXIT=0**.

**Step 2:** native stmt nodes/helpers — typed local `size_t h`, combine expr, cast return; printer if needed.

## Step 2 implementation (2026-06-03)

| Change | Site |
|--------|------|
| `CppVarDecl` + `variable_declaration` printer | `cpp_ast.mlc:20`, `cpp_printer.mlc:255-256,425-426` |
| `make_variable_cpp_statement`, `make_size_t_type` | `emit_helpers.mlc:32-40` |
| `make_hash_seed/combine/empty_return_cpp_statement` | `hash_stmt_helpers.mlc` |
| Golden tests (inline AST; helpers via separate module) | `test_cpp_printer.mlc:144-175` |

Note: hash helpers in separate module — Ruby bootstrap `import { … } from hash_stmt_helpers` fails in test; golden uses inline AST. Helpers compile via mlcc (used step 3).

Result: **923 pass; DIFF_EXIT=0**.

**Step 3:** wire `make_hash_*_statement` in `derive_methods_cpp.mlc`; drop fragments.

## Step 3 implementation (2026-06-03)

| Change | Site |
|--------|------|
| `import * as hash_stmt_helpers` + delegate `make_hash_*_statement` | `derive_methods_cpp.mlc:6,371-379` |
| Native access exprs (field, discriminant, tuple, variant) | `derive_methods_cpp.mlc:362-369` |
| Remove module-level `const` (Ruby parser → 0 decls) | `hash_stmt_helpers.mlc` — inline FNV/combine literals |
| `make_fragment_cpp_statement` in derive hash helpers | **0** |

Result: **923 pass; DIFF_EXIT=0**.

**Step 4:** parity tests — Hash body stmts not `CppStmtFragment`.

## Step 4 implementation (2026-06-03)

| Change | Site |
|--------|------|
| `cpp_stmt_tree_contains_fragment` + hash body walkers | `test_decl_gen.mlc:128-175` |
| record + sum + empty record body anti-fragment | `:853-861` |
| `gen_type_decl_body_cpp` record/sum Hash anti-fragment | `:871-875` |

Result: **928 pass; DIFF_EXIT=0**.

**Step 5:** survivors audit; close track.

## Step 5 audit (2026-06-03)

| Check | Result |
|-------|--------|
| `make_fragment_cpp_statement` in `derive_methods_cpp.mlc` | **0** |
| `make_fragment_cpp_statement` in `hash_stmt_helpers.mlc` | **0** |
| Hash body helpers | `hash_stmt_helpers.make_hash_*_cpp_statement` via `:371-379` |
| Production path | `gen_type_decl_body_cpp` → `gen_derive_methods_cpp` → `gen_derive_hash_cpp` → `CppStdHashSpecialization` + native `[CppStmt]` body |
| Anti-fragment tests | `test_decl_gen.mlc:853-875` (record/sum/empty + body wiring) |
| `make_fragment_cpp_statement` elsewhere in `compiler/` | `stmt_cpp.mlc:88`, `expr_visitor_cpp.mlc:196`, `emit_helpers.mlc:20` export — **deferred** (non-derive) |
| String parallel path | `type_gen.mlc` `derive_hash_record` / `derive_hash_sum` — **deferred** |

Result: **928 pass; DIFF_EXIT=0**. Track **closed** (5/5).

## Deferred (out of track)

- `module.mlc` — `cpp_declarations_from_text_parts` (guards, includes, namespace scaffolding).
- `cpp_decl_from_string_output` export + `cpp_decls_from_string_parts` (no production consumer).
- `type_gen.mlc` — string `derive_hash_record` / `derive_hash_sum` parallel path.
- `decl_cpp.mlc:28` — `cpp_decl_from_string_output` fallback.
- Phase 4 `MLCC_BOOTSTRAP=1`.
- Parser `ref mut` — [TRACK_PLAN.md](TRACK_PLAN.md) step 15.
