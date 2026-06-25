# Track: MLC MIR (lowering center)

Parent: [../PLAN.md](../PLAN.md) ù2; [../ARCHITECTURE.md](../ARCHITECTURE.md)

## Status: **open** STEP=2 pending

**North star:** SemanticIR ? **MLC MIR** ? CppIR ? C++ emit ? fast native build.

**Legacy:** `ir/core.mlc` (CoreIR sketch) ? migrate into `mir/`; do not maintain two IRs.

**Depends on:** CLEAN_ARCHITECTURE closed; BUILD_SPEED STEP 3+ parallel (emit layout).

**Baseline:** `build_tests` **1297/0**; `ir/core.mlc` + `verify_core` exist (no pipeline lowering).

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

Optional when MIR wired: `mlcc --dump-mir --check-only ù`

---

| Step | Item | Status |
|------|------|--------|
| 1 | `mir/mir_types.mlc`, `mir_dump`, `verify_mir`, tests | done |
| 2 | `CompilerDb` in-memory: FileStore, parse/check caches | pending |
| 3 | Lower fn body: literals, locals, if, while, return, calls | pending |
| 4 | `--dump-mir`; pipeline hook (optional behind flag) | pending |
| 5 | C++ backend path: MirFunction ? CppAST (simple fns only) | pending |
| 6 | MIR passes: simplify_cfg, const_fold (minimal) | pending |
| 7 | Lower match, `?`, closures (staged) | pending |
| 8 | Bootstrap report: MIR/CppIR diff + timing | pending |
| 9 | Deprecate/remove `ir/core.mlc` after parity | pending |
| 10 | Stable IDs (LocalId/BlockId) in hot lowering path | pending |

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
| step | <1ù10> |
| done | <one line> |
| verify | build_tests N/0; arch_lint; optional --dump-mir |
```
