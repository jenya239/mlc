# Track: Compiler core architecture/perf/testing hygiene — execute the 2026-06-29 review

Parent: [../PLAN.md](../PLAN.md) §104. Authorized 2026-07-28 (user request: "трек
чисто по улучшению производительности, архитектуры и тестирования"). Scope:
`compiler/**` (self-hosted compiler core), distinct from §97/§101 (editor
render) and §102/§103 (new feature epics).

## Status: **open** — §104-1 Decision next, **queue head** (priority override
2026-07-28, user: "это должно быть приоритетом сейчас" — Wave 1 moved ahead
of §101/§102/§103; Wave 2 stays queued after §103, Wave 3 stays gated)

## Why this track exists

[`mlc-support/responses/review_20260629_144027.md`](../../../mlc-support/responses/review_20260629_144027.md)
(2026-06-29 architecture review) drafted a full 24-step refactoring plan for
exactly this scope — god-file decomposition, MIR-as-mandatory-layer
migration, C++ build-speed levers (hybrid layout/include planner/fast-build
mode), and a determinism/verifier testing layer — each step with code
sketches, explicit dependencies, and a verification command. **None of the
24 steps were ever opened as a track.** Confirmed 2026-07-28 by file-existence
check: every proposed new module (`build/file_store.mlc`, `build/intern.mlc`,
`mir/mir_builder.mlc`, `checker/registry_type.mlc`, etc.) is still missing.
Worse: the god-files the review flagged have **grown** in the interim —
`transform.mlc` 1558→1765, `decl_cpp.mlc` 1119→1666, `match_gen.mlc`
907→1403, `registry.mlc` 870→1060, `infer.mlc` 786→962 lines. Step 17
(`CppInvokedBlock(string)` → real `CppStatement` list) is the one exception —
it was independently completed by the separately-tracked §44
`TRACK_CODEGEN_CPPAST_ONLY` (closed 2026-07-17); do not reopen it here.

## Non-goals

- Do not touch `misc/editor/**` from this track — that is §97/§101/§102's
  scope, kept separate on purpose (different risk profile, different gate).
- Do not switch the C++ backend to MIR (review's Step 24) until Steps 6-11
  give 100% MIR lowering coverage of `compiler/main.mlc`, verified by
  `--mir-bootstrap-report`. Flagged `**максимальный**` risk in the source
  review — not a Phase-1 goal here.
- Every god-file split step requires **bootstrap diff empty** as a hard gate
  (`mlcc -o /tmp/p1 compiler/main.mlc`, same source through the split
  version, `diff -r` empty) — this is non-negotiable per the review's own
  risk notes on Steps 12-17.

## Sub-tracks — priority order (per review's own "Итоговый приоритет", adapted)

Numbering below matches the review's own step numbers (§104-N = review Step
N) so `review_20260629_144027.md` stays the source of truth for scope; do
not re-derive step content here, read the review file at pickup time.

### Wave 1 — foundation + god-files + build speed (parallelizable, low-to-moderate risk)

- **§104-1** `FileId`/`FileStore` (review Step 1) — isolated addition, zero blast radius
- **§104-2** `Span.file_id` extension via `span_make` (Step 2) — depends on §104-1
- **§104-3** `StringInterner` (Step 3) — isolated, parallel with §104-1
- **§104-12** split `transform/transform.mlc` (1765 lines now, was 1558) (Step 12)
- **§104-13** split `codegen/decl_cpp.mlc` (1666 lines now, was 1119) (Step 13)
- **§104-14** split `codegen/expr/match_gen.mlc` (1403 lines now, was 907) (Step 14)
- **§104-15** split `checker/registry.mlc` (1060 lines now, was 870) — needs re-export language support first, see review Часть 3 §1 (Step 15)
- **§104-16** split `checker/infer/infer.mlc` (962 lines now, was 786) (Step 16)
- **§104-18** `--emit-layout=hybrid` (Step 18) — review's own top pick for build-speed ROI
- **§104-19** include planner / forward-decls (Step 19) — depends on §104-18
- **§104-20** `--cpp-mode=fast-build` (Step 20) — depends on Step 17 (already done via §44)
- **§104-22** `bootstrap-fast.sh`/`bootstrap-full.sh` tooling (Step 22) — depends on §104-18
- **§104-23** determinism checks (`--dump-mir`/`--dump-sem` diff-stable) (Step 23) — depends on §104-22

### Wave 2 — MIR as a real layer (moderate-to-high effort, no immediate payoff, do after Wave 1)

- **§104-6** complete MIR lowering coverage (Step 6)
- **§104-7** `mir/mir_builder.mlc` extraction (Step 7) — depends on §104-6
- **§104-8** MIR verifier extensions (Step 8) — depends on §104-6
- **§104-9** deterministic MIR pretty-printer (Step 9) — depends on §104-6
- **§104-10** `MirPass` trait + `mir/passes/` (Step 10) — depends on §104-7
- **§104-11** optional `--cpp-from-mir` flag, not default (Step 11) — depends on §104-6

### Wave 3 — deferred, high-risk, needs explicit re-authorization when reached

- **§104-5** span preservation through desugar passes (Step 5) — depends on §104-2
- **§104-17** `CppInvokedBlock` full statement-list migration — **already done via §44**, listed only for cross-reference, do not reopen
- **§104-21** `TypeId` interner with runtime `extern fn` singleton (Step 21) — touches `runtime/`, needs its own Decision on whether a runtime change is acceptable here
- **§104-24** switch C++ backend to MIR by default (Step 24) — only after Wave 2 fully green + 100% MIR coverage; flagged maximum risk in source review, do not schedule a date

## Verification discipline

Every sub-track: `mlcc -o /tmp/p1 compiler/main.mlc` before, apply change,
`mlcc -o /tmp/p2 compiler/main.mlc` after, `diff -r /tmp/p1 /tmp/p2` empty
(bootstrap diff) unless the sub-track explicitly changes output by design
(§104-18/19/20/24 — those gate on `rake test_compiler_mlc` + a working
`clang++` build instead, exactly as the source review specifies per-step).
`scripts/regression_gate.sh` after any `lib/mlc/` touch. Self-host
verification per `.cursor/rules/mlcc-self-host-verification.mdc` — mlcc2
diff check — required before Critic close on any Wave 1/2 sub-track.
