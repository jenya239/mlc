# Track: mlcc optimization (plan 2026-05)

Source: `.cursor/plans/mlcc_optimization_plan_*.plan.md` (do not edit plan file).

## Status: verified, **commit pending**

| Phase | Item | Status |
|-------|------|--------|
| 0 | Profile sub-phases + `record_baseline.sh` | done |
| 1 | method_owners, trait maps, Map paths, reuse registry | done |
| — | test_partial_application, test_merge_imports | done |
| 2 | desugar push, codegen join, no double ctx, single-pass decls, lookup maps | done |
| 4 | compare_baseline.sh, CI optional, test_codegen extended | done |
| 3 | lexer byte_substring fast path | done |
| 3 | parser `ref mut` position | **deferred** (bootstrap limit; separate branch) |

## Last verify (2026-05-20, re-run)

| Check | Result | Duration |
|-------|--------|----------|
| `compiler/build.sh` | ok | ~19 min |
| `compiler/tests/build_tests.sh` | **463 passed**, 0 failed | ~27 min |
| self-host `diff -rq` mlcc1/mlcc2 | **identical** (`SELFHOST_DIFF_EXIT=0`) | ~26 min |

Uncommitted (source, excl. generated `compiler/out/`): **~77 files**, +956 / −294 lines (compiler, benchmarks/profile, profile runtime, tests, CI).

## Next steps (ordered)

1. [x] Re-run full verify — green.
2. [ ] **Commit** — файлы в staging (`git status`); ждёт команды user `коммит`.
3. [ ] `record_baseline.sh` after commit.
4. [ ] Close track; open next from [PLAN.md](../PLAN.md).

## Notes

- `build_trait_nominal_maps`: inline fold (COW Map + nested `ref mut` broke maps).
- Orchestration queue: `cursor_agent_register` + `cursor_enqueue_send({ token, text })`; `AGENT_TOKEN=` в каждом промпте.
