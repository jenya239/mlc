# Track: Build speed 3 — ccache CI cache, post-closure-escape `-ftime-trace` re-check

Parent: [../PLAN.md](../PLAN.md) §2.9. Продолжение
[TRACK_BUILD_SPEED2](TRACK_BUILD_SPEED2.md) (closed).
[TRACK_LANG_CLOSURE_ESCAPE](TRACK_LANG_CLOSURE_ESCAPE.md) closed 2026-07-09.

## Status: **closed** 2026-07-09 (STEP=1–4; decision **c**)

**Outcome:** CI ccache persisted (`0e976592`). ftime-trace on `exprs.cpp`
([BUILD_SPEED3_FTIME_TRACE_2026_07_09.md](BUILD_SPEED3_FTIME_TRACE_2026_07_09.md)):
`std::function` ~89ms (CLOSURE_ESCAPE win); dominant cost is AST/`TokenKind`
`std::variant`, not Shared/Array/`std::function`. Chose **(c)** — no
`extern template` follow-up, no ninja revival in this track. Residual variant
cost = separate future track only if re-measured.

**Out of scope (explicit):** C++20 modules — not researched (tooling immature
for multi-file clang/gcc projects). [TRACK_MIR_VM_FULL](../../agent/TRACK_MIR_VM_FULL.md)
is about interpretation without g++ (20–80× slower *execution*), not build
wall-clock — do not conflate with build-speed work.

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | CI ccache: workspace `.ccache`, key `os/ref/sha`, `CCACHE_SLOPPINESS`, stats on mlcc workflows | **done** (`0e976592`) |
| 2 | `-ftime-trace` largest TU (`exprs.cpp`) + Shared/Array/`std::function` table | **done** (`5bd39424`) |
| 3 | Decision a/b/c from table → **(c)** | **done** (`5c5a381e`) |
| 4 | PLAN §2.9 + archive this track; resume CONCURRENCY_V2 STEP=2 | **done** |

## Verify gate (historical)

```bash
# STEP=1 local: MLCC_OBJ_CLEAN=1 ×2 → ccache 153/153 direct hits ~2.3s
# STEP=2: clang++ -ftime-trace on compiler/out/exprs.cpp → see ftime-trace doc
```
