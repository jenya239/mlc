# Track: Reddit / community demo

Parent: [../PLAN.md](../PLAN.md) §Phase 5; follows [TRACK_CPP_HEADER_IMPORT.md](TRACK_CPP_HEADER_IMPORT.md)

## Status: **open** (STEP=1 pending)

**Depends on:** TRACK_SELF_HOST_BOOTSTRAP closed (minimum); FORMATTER + LSP preferred for polish.

**Goal:** reproducible demo script + post assets for r/ProgrammingLanguages / r/rust.

## Verify gate

```
compiler/tests/build_tests.sh
MLCC_BOOTSTRAP=1 compiler/build.sh
compiler/tests/e2e/run_e2e.sh compiler/out/mlcc
```

---

| Step | Item | Status |
|------|------|--------|
| 1 | Demo script — self-host timing, test count, one-liner commands in `scripts/reddit_demo.sh` | pending |
| 2 | Record baseline numbers — mlcc p1/p2 seconds, test count, diff_exit=0 | pending |
| 3 | Screenshot / asciinema checklist (manual); link in README | pending |
| 4 | Draft post (problem/solution/demo/commands); store in `docs/agent/BLOG.md` | pending |
| 5 | Dry-run gate + close track | pending |

### Out of scope

- Public playground hosting
- Package manager
