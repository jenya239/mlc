# Track: Compiler architecture (IR layers + passes)

Parent: [../ARCHITECTURE.md](../ARCHITECTURE.md); [../PLAN.md](../PLAN.md) �Phase 2.8

## Status: **closed** (STEP=12 done — 2026-05-19)

**Depends on:** TRACK_LSP closed; TRACK_CPP_PARSER_FULL closed.

**Baseline:** `build_tests` **1290/0**; architecture lint allowlist from STEP=2.

**STEP=12 note (2026-05-19):** `docs/QUERY_ENGINE.md` stub (security invariants, query surface sketch); track **closed**. Next: **Planner plan-refresh** → BUILD_SPEED or REDDIT_DEMO per TRACK_PLAN.

**STEP=11 note (2026-05-19):** `ir/load_item.mlc` (LoadItem out of codegen); `ir/record_field_default_validate.mlc` + `codegen/record_field_default_emit.mlc` (split validate vs emit; `acceptable_for_codegen` → 0 in checker/); `test_record_field_default_validate.mlc` (+2); allowlist shrink. Gate **1290/0** + arch_lint ok.

**STEP=10 note (2026-06-24):** `tests/support/` suite_registry (IR levels), golden_harness, path_from_root; thin `tests_main.mlc`; `test_golden_harness.mlc` (+6); security `golden_relative_path_is_safe`, `test_relative_path_is_safe`. Gate **1288/0** + arch_lint ok; while-push in tests_main removed.

**STEP=9 note (2026-06-24):** split `cpp/` → `cpp_ir/cpp_ast.mlc`, `cpp_parse/`, `cpp_emit/print.mlc`; `verify_cpp_ast.mlc` (security: `cpp_identifier_is_safe`, `cpp_include_path_is_safe`); `test_verify_cpp_ast.mlc` (+6). IR rename `cpp_ir/ast` → `cpp_ir/cpp_ast` (no clash with `frontend/ast`). Gate **1282/0** + arch_lint ok.

**STEP=6 note (2026-06-24):** `--dump-ast`, `--dump-sem`, `--time-passes`; `dump_flags.mlc`, `ast_printer`, `semantic_ir_dump`; pass-level profile labels in PassManager.

**STEP=3 note (2026-06-24 recovery):** verifiers + `--verify-each` wired. Gate **1239/0** + arch_lint ok. mlcc2 C++ on `verify_*.mlc` FAIL (mlcc codegen); Ruby OK.

**STEP=4 note (2026-06-24):** PassManager + preserved-analyses stub; pipeline via `run_pass_manager_descriptors`. Security: `pass_name_is_safe`, duplicate/limit guards. Gate **1251/0** + arch_lint ok. mlcc2 C++ FAIL (verify modules, pre-existing).

**STEP=5 note (2026-06-24):** transform вынесен из `codegen/module.mlc` → `program_to_semantic.mlc` + `tests/codegen_harness.mlc`; allowlist shrink. Security: `module_path_is_safe`, reject `..`. Gate **1256/0** + arch_lint ok.

**STEP=8 note (2026-06-24):** driver split `compiler/driver/` (path_normalize, module_loader, program_merge, compile_driver, cli); thin `main.mlc`. Security: `driver_source_path_is_safe` on entry + load. `test_driver.mlc` (+4). Gate **1276/0** + arch_lint ok. mlcc2 C++ FAIL (pre-STEP=3 codegen).

**STEP=7 note (2026-06-24):** CoreIR sketch `ir/core.mlc`, `verify_core.mlc`, `core_dump.mlc`; security: `core_node_id_is_valid`, name/label guards. No pipeline lowering. Gate **1272/0**.

**Goal:** ??????? IR + verified passes + driver/core split + clean code gates. ?? enterprise CA. ?? ????????????? ? ????.

**Strategy:** strangler. ?????? STEP � ???? ????????. Gate green ?????? ???.

## Verify gate (every step)

```
compiler/tests/build_tests.sh
compiler/build.sh
compiler/out/mlcc -o .tmp_selfhost/p1 compiler/main.mlc
compiler/build_bin.sh .tmp_selfhost/p1 .tmp_selfhost/mlcc2
.tmp_selfhost/mlcc2 --check-only compiler/main.mlc
bash compiler/tests/run_architecture_lint.sh
```

`run_architecture_lint.sh` = IR boundary lint + code quality gates (baseline allowlist).

---

## IR ladder (mlcc mapping)

```text
Tokens       frontend/lexer
AST          frontend/ast, parser/
HIR          checker/names, registry        (partial)
Typed        SemanticIR + infer/transform
CoreIR       (future TRACK_CORE_IR)         STEP=7 = sketch only
CppAST       cpp_ir/cpp_ast, cpp_emit/print, codegen/
Emit         printer, File.write (driver)
```

---

| Step | Item | Status |
|------|------|--------|
| 1 | [ARCHITECTURE.md](../ARCHITECTURE.md) � IR ladder, invariants, anti-patterns | done |
| 2 | `run_architecture_lint.sh` - IR boundary + code quality gates + allowlist | done |
| 3 | `verify_ast.mlc`, `verify_semantic_ir.mlc`; `--verify-each` | done |
| 4 | PassManager � wire `CompilerPass`; preserved-analyses stub | done |
| 5 | Dedup `codegen/module.mlc` transform path | done |
| 6 | Dump flags: `--dump-ast`, `--dump-sem`, `--time-passes` | done |
| 7 | CoreIR sketch (`ir/core.mlc`); no full lowering | done |
| 8 | Driver split: `driver/` vs core | done |
| 9 | Split `cpp/` → `cpp_ir/`, `cpp_parse/`, `cpp_emit/`; `verify_cpp_ast` | done |
| 10 | Test layout by IR level + golden harness | done |
| 11 | Remove checker↔codegen coupling | done |
| 12 | `docs/QUERY_ENGINE.md` stub; close track | done |

---

## Code quality gates

Clean code ??? ?????? perf: ???????? ??????? ? ADT ? cold path, ??????? ?????? ? hot path. Gates enforce ??????????, ?? OOP.

### A. IR boundary lint

| Rule | Check |
|------|-------|
| parser/frontend ? codegen | no import |
| parser/frontend ? cpp emit | no import |
| checker/check ? codegen | no import |
| codegen ? transform rerun | no `transform_load_items` in module.mlc after STEP=5 |

### B. Survivor patterns (grep, warn ? fail when count drops)

| Pattern | Target | Note |
|---------|--------|------|
| `acceptable_for_codegen` | 0 in checker/ after STEP=11 | cross-layer smell |
| `while index < .*\.length\(\).*push` | trend down | prefer `.map`/`.fold` in cold path |
| `else if word ==` keyword chains | trend down | prefer string `match` |
| manual suite push in tests_main | replace with suite registry STEP=10 | |

### C. File size

| Limit | Action |
|-------|--------|
| **800 lines** | warn (allowlist known god files) |
| **1000 lines** | must split before adding logic |
| new file in hot path | profile if >200 lines |

Current allowlist (baseline): `transform.mlc`, `exprs.mlc`, `decl_cpp.mlc`, `infer.mlc`, `registry.mlc`, `test_checker.mlc`, `test_decl_gen.mlc`.

### D. Function size (manual review + future)

| Limit | Scope |
|-------|-------|
| ~80 lines | soft target cold path |
| ~150 lines | hard split candidate |
| hot path | split only if profile shows benefit |

No automated line-count per fn yet � STEP=2 documents; optional `scripts/count_fn_lines.sh` later.

### E. Formatter

`compiler/fmt/` applies to new/edited files. Gate: no unformatted diff in touched `.mlc` (optional hook later).

### F. New code checklist (agent)

1. Pass changed ? verifier or unit test added.
2. No new cross-IR import (lint green).
3. No new file >800 lines.
4. Cold path: destructure / HOF / or-patterns where readable.
5. Hot path: no new abstraction without profile note in SESSION.

### Allowlist file

`compiler/tests/architecture_lint_allowlist.txt` � one rule per line:

```
# format: RULE:path_or_pattern
cross_import:checker/check/check.mlc:record_defaults
file_size:checker/transform/transform.mlc
file_size:frontend/parser/exprs.mlc
...
```

New violations fail CI; shrinking allowlist = progress metric.

---

## Test systematization (STEP=10)

```text
tests/
  unit/syntax/     lexer, parser, verify_ast
  unit/hir/        names, registry
  unit/types/      infer, checker
  unit/cpp/        cpp_ir, parser, verify_cpp_ast
  integration/     pipeline, merge_imports
  e2e/             fuzz/, mlcc2 parity
  fixtures/        .mlc, .h, golden/
  support/         test_runner, golden_diff, path_from_root
```

---

## Parallel / language (Phase 2.7)

| Feature | ????? |
|---------|-------|
| `opaque type NodeId`, `TypeId`, `SymbolId` | stable IDs |
| `for x in xs` adoption in compiler/ | ?????? while-push boilerplate |
| `?` / guard let | thin pass code |

---

## Incremental rules (agent)

1. Move-only commit, then logic � never both.
2. Lint must pass (or allowlist shrinks, never grows without Meta review).
3. New pass ? verifier + test.
4. Profile before abstracting hot path.
5. Fuse passes only when profile says so.

---

## Per-turn template

```
| turn | <ISO date> |
| role | Driver |
| step | <1�12> |
| done | <one line> |
| verify | build_tests N/0; architecture_lint; diff_exit |
| next | ROLE=Driver STEP=<n+1> |
```
