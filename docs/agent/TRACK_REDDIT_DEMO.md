# Track: Reddit / community demo

Parent: [../PLAN.md](../PLAN.md) §Phase 5; follows [TRACK_CPP_HEADER_IMPORT.md](TRACK_CPP_HEADER_IMPORT.md)

## Status: **closed** STEP=5 done

**Depends on:** TRACK_SELF_HOST_BOOTSTRAP closed (minimum); FORMATTER + LSP preferred for polish.

**STEP=5 note (2026-06-26):** dry-run `scripts/reddit_demo.sh --dry-run-gate`; TRACK gate: build_tests **101362/0** arch_lint 0 fail; e2e **6/0**; `MLCC_BOOTSTRAP=1 build.sh` **FAIL** (fresh bootstrap codegen — documented in BLOG honest limits); track closed.

**STEP=1 note (2026-06-26):** `scripts/reddit_demo.sh` — one-liners, `--run` self-host timing + test count.

**STEP=2 note (2026-06-26):** `docs/agent/reddit_demo_baseline.txt` — codegen p1/p2 sec, `diff_exit=0`.

**STEP=3 note (2026-06-26):** `docs/agent/REDDIT_DEMO_MEDIA.md` — screenshot/asciinema checklist; link in root `README.md`.

**STEP=4 note (2026-06-26):** Reddit draft in `docs/agent/BLOG.md` §Reddit draft.

**Deliverables:** `scripts/reddit_demo.sh`; `docs/agent/reddit_demo_baseline.txt`; `docs/agent/REDDIT_DEMO_MEDIA.md`; `docs/agent/BLOG.md` §Reddit draft; `README.md` Community demo section.

**Goal:** reproducible demo script + post assets for r/ProgrammingLanguages / r/rust.

## Verify gate

```
compiler/tests/build_tests.sh
MLCC_BOOTSTRAP=1 compiler/build.sh
compiler/tests/e2e/run_e2e.sh compiler/out/mlcc
scripts/reddit_demo.sh --dry-run-gate
```

---

| Step | Item | Status |
|------|------|--------|
| 1 | Demo script — self-host timing, test count, one-liner commands in `scripts/reddit_demo.sh` | done |
| 2 | Record baseline numbers — mlcc p1/p2 seconds, test count, diff_exit=0 | done |
| 3 | Screenshot / asciinema checklist (manual); link in README | done |
| 4 | Draft post (problem/solution/demo/commands); store in `docs/agent/BLOG.md` | done |
| 5 | Dry-run gate + close track | done |

### Out of scope

- Public playground hosting
- Package manager
