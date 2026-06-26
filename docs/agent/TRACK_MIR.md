# Track: MLC MIR (lowering center)

Parent: [../PLAN.md](../PLAN.md) ?2; [../ARCHITECTURE.md](../ARCHITECTURE.md)

## Status: **closed** STEP=10 done

**Closed (2026-06-26 Planner):** all pipeline steps 1–10 done; MIR lowering + passes + stable IDs shipped. Further MIR work → new track if needed.

**Baseline:** `build_tests` **1344/0**; `mir_ids.mlc` stable ID pool in lowering.

**STEP=10 note (2026-06-25):** `mir/mir_ids.mlc`; `lower_fn` `MirIdPool`; gate **1344/0**.

---

## Pipeline target

```text
AST ? SemanticIR ? MirProgram ? CppAST ? emit ? build graph ? mlcc
```

**Rules:**

- frontend/checker: no C++ imports
- mir/: no parser, no cpp emit
- cpp_ir/cpp_emit: no parser
- driver: thin orchestration only

---

## Verify gate (every step)

```bash
compiler/tests/build_tests.sh
bash compiler/tests/run_architecture_lint.sh
compiler/out/mlcc --check-only compiler/main.mlc
```

Optional when MIR wired: `mlcc --dump-mir --check-only ?`

---

| Step | Item | Status |
|------|------|--------|
| 1 | `mir/mir_types.mlc`, `mir_dump`, `verify_mir`, tests | done |
| 2 | `CompilerDb` in-memory: FileStore, parse/check caches | done |
| 3 | Lower fn body: literals, locals, if, while, return, calls | done |
| 4 | `--dump-mir`; pipeline hook (optional behind flag) | done |
| 5 | C++ backend path: MirFunction ? CppAST (simple fns only) | done |
| 6 | MIR passes: simplify_cfg, const_fold (minimal) | done |
| 7 | Lower match, `?`, closures (staged) | done |
| 8 | Bootstrap report: MIR/CppIR diff + timing | done |
| 9 | Deprecate/remove `ir/core.mlc` after parity | done |
| 10 | Stable IDs (LocalId/BlockId) in hot lowering path | done |

---

## CompilerDb (STEP 2 sketch)

```text
parse_module(db, id) ? AstModule
typecheck_module(db, id) ? SemanticModule
mir_function(db, id) ? MirFunction
cpp_module(db, id) ? CppFile
```

In-memory only first; disk cache later.

---

## C++ backend modes (parallel: TRACK_BUILD_SPEED / emit track)

| Mode | Use |
|------|-----|
| `readable` | variant/visit, debug |
| `fast-build` | self-host: minimal templates, heavy in .cpp |
| `emit-layout=hybrid` | frontend/sema/mir/cpp/driver TU split |

---

## Per-turn template

```
| step | <1?10> |
| done | <one line> |
| verify | build_tests N/0; arch_lint; optional --dump-mir |
```
