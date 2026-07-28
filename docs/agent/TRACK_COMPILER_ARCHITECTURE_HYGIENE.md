# Track: Compiler core architecture/perf/testing hygiene — execute the 2026-06-29 review

Parent: [../PLAN.md](../PLAN.md) §104. Authorized 2026-07-28 (user request: "трек
чисто по улучшению производительности, архитектуры и тестирования"). Scope:
`compiler/**` (self-hosted compiler core), distinct from §97/§101 (editor
render) and §102/§103 (new feature epics).

## Status: **open** — §100 closed 2026-07-28, §104-1/2/3 found already
implemented (see correction below, 2026-07-28), **queue head is now §104-12**
(priority override 2026-07-28, user: "это должно быть приоритетом сейчас" —
Wave 1 moved ahead of §101/§102/§103; Wave 2 stays queued after §103, Wave 3
stays gated)

## Correction 2026-07-28 (Driver STEP=0 audit) — §104-1/§104-2/§104-3 already done

The "why this track exists" section below claims "0 of 24 steps ever
actioned", verified by file-existence check at the review's own suggested
paths (`build/file_store.mlc`, `build/intern.mlc`). That check was **wrong**
for Steps 1/2/3: the same functionality exists under different paths/names,
predating this track:

| Step | Review's suggested path/name | Actual path/name | Added | Evidence |
|------|-------------------------------|-------------------|-------|----------|
| 1 `FileId`/`FileStore` | `build/file_store.mlc`, `FileId`/`FileStore` | `compiler/infrastructure/file_store.mlc`, `SourceFileId`/`SourceFileStore` | commit `36a1e372` 2026-06-30 (one day after the review) | identical fields/functions (`paths`/`sources`, `_new`/`_intern`/`_path`/`_source`); dedup-by-path intern; isolated (zero non-test importers, confirmed by grep) |
| 3 `StringInterner` | `build/intern.mlc`, `InternTable` | `compiler/infrastructure/intern.mlc`, `StringInternTable` | commit `ab088d90` 2026-07-01 ("feat: add string interner infrastructure") | identical fields/functions; isolated |
| 2 `Span.file_id` | add `file_id: FileId` field | `Span` already has `start_offset`/`end_offset: i32` + `span_make(file, line, column)` constructor (the concrete "До/После" code the review shows); **no** `file_id` field | same era, in `compiler/frontend/ast.mlc` | grep confirms exactly 2 raw `Span { ... }` literals in the whole tree — both are the canonical constructors (`span_unknown`/`span_make`) themselves, zero stray literals elsewhere; 345 call sites already use `span_make`/`span_unknown` |

Verification run this turn: `compiler/tests/support/suite_registry.mlc`
already imports and runs `file_store_tests`/`intern_tests`
(`compiler/tests/file_store_test.mlc`, `compiler/tests/intern_test.mlc`,
round-trip intern/dedup/resolve assertions); `compiler/out/tests/run_tests`
(binary newer than all four source files) → `1471 passed, 0 failed`. Both
modules remain intentionally unwired outside tests (grep for
`infrastructure/file_store`/`infrastructure/intern` importers → only the two
test files), matching the review's own Step 1 instruction ("Пока не
подключать к driver").

**Disposition:** §104-1 and §104-3 — **done**, no further action, do not
recreate. §104-2 — the code the review actually specifies (offsets +
`span_make` + literal replacement) is **done**; the `file_id: FileId` field
named only in the step's title (not in its own code sample) has **no
consumer inside Wave 1** (only Wave-3 §104-5 span-preservation would use it)
— **deferred, not a Wave-1 gap**, revisit only if/when §104-5 is
re-authorized. Queue head moves to **§104-12** (`transform.mlc` split).

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

- **§104-1** `FileId`/`FileStore` (review Step 1) — **done**, pre-existing (see Correction above)
- **§104-2** `Span.file_id` extension via `span_make` (Step 2) — **done** except deferred `file_id` field (see Correction above)
- **§104-3** `StringInterner` (Step 3) — **done**, pre-existing (see Correction above)
- **§104-12** split `transform/transform.mlc` (1765 lines now, was 1558) (Step 12) — **queue head**
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

## §104-12 `transform/transform.mlc` split (1765 lines)

### Slice 1 — `transform_coerce.mlc` (do first — zero circular dependency)

#### Decision (STEP=0) — **frozen** 2026-07-28

| Item | Choice |
|------|--------|
| Problem | `transform.mlc` is 1765 lines. Review's Step 12 names 3 groups (coerce/call_args/method) to extract, flags **высокий риск** (circular import: extracted code needs `transform_expr`, defined later in the same file). Re-derived against the **current** file (drifted since 2026-06-29 review): the coerce group (`coerce_expr_to_type` + 10 helpers, lines 492-566 + 599-716) does **not** actually call `transform_expr`/`dispatch_transform_pass` at all — it only needs 3 small leaf helpers already in `transform.mlc` (`semantic_type_is_tarray`, `array_element_type_from_semantic_type`, `generic_type_name`, lines 139/161/183, used both inside and outside the coerce group). Moving those 3 helpers together with the group makes the new module a pure leaf (imports only `ast`/`registry`/`semantic_ir`) with **zero** cycle — `transform.mlc` becomes the only side that imports across the new module boundary. The call_args/method groups (review's other two) genuinely do call back into `transform_expr` (confirmed by grep: `transform_one_call_argument_using_optional_expected_type` alone has 25 `transform_expr(` calls) and need the injection pattern (`transform_expr_fn` parameter, matching `infer_expr_fn` in `checker/infer/`) — deferred to later slices below, not this one |
| Strategy (v1) | New `compiler/checker/transform/transform_coerce.mlc`. Move: `semantic_type_is_tarray`, `array_element_type_from_semantic_type`, `generic_type_name` (export, no behavior change), `coerce_unknown_else_array_when_then_known_inner`, `conditional_else_coerce_empty_array_using_then_type`, `array_literal_else_maybe_coerce`, `conditional_else_empty_unknown_array_coerced_to_then_array_element` (export — called from `dispatch_transform_pass`), `coerce_array_semantic_expression_to_type`, `coerce_record_semantic_expression_to_type`, `coerce_block_semantic_expression_to_type`, `coerce_if_semantic_expression_to_type`, `coerce_match_arm_to_type`, `coerce_match_arms_to_type`, `coerce_expr_to_type` (already exported). `transform.mlc` imports all of these back (it still calls `coerce_expr_to_type` from `transform_one_call_argument_using_optional_expected_type` and `conditional_else_empty_unknown_array_coerced_to_then_array_element`/`semantic_type_is_tarray`/`array_element_type_from_semantic_type` from `dispatch_transform_pass`/`merge_conditional_expression_types`/`array_element_type_from_semantic_expression`). **No signature change, no algorithm change** — pure relocation. `TransformContext` and everything after line ~720 (call-args, method-call, dispatch, `transform_expr`) stay untouched in `transform.mlc` this slice |
| Primary gate | Red: `coerce_expr_to_type` still defined in `transform.mlc`, no `transform_coerce.mlc`. Green: `transform_coerce.mlc` exists with the 14 moved items; **bootstrap diff empty** (`mlcc -o /tmp/p1 compiler/main.mlc` before, `/tmp/p2` after, `diff -r` empty — codegen output must not change one byte, since this is pure relocation); `rake test_compiler_mlc` (1471+ passed, 0 failed); self-host mlcc2 diff per `.cursor/rules/mlcc-self-host-verification.mdc` before Critic close |
| Module touch | new `compiler/checker/transform/transform_coerce.mlc`; `compiler/checker/transform/transform.mlc` (shrinks by ~230 lines, gains 2 import lines) |
| REG | no (`compiler/**` only) |
| Out of scope | call_args/method groups (next slices, need `transform_expr_fn` injection); `dispatch_transform_pass`/`TransformContext`/`transform_expr` relocation; algorithm changes; MIR |

#### Steps (§104-12 — slice 1: coerce)

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze | **done** |
| 1 | Red: confirm current boundaries (`transform_coerce.mlc` absent, 14 items located at the lines above) | **done** — `find compiler/checker/transform -iname transform_coerce.mlc` empty; grep confirms all 14 items at the exact documented lines (139/161/183/492/505/521/531/599/608/623/637/655/667/675), `transform.mlc` still 1765 lines |
| 2 | Green: create `transform_coerce.mlc`, wire `transform.mlc` imports, bootstrap diff empty, `rake test_compiler_mlc`, mlcc2 self-host diff | **done** — see verification below |
| 3 | Critic: full re-audit | pending |

**STEP=2 verification (2026-07-28):** `transform.mlc` 1765→1505 lines, new `transform_coerce.mlc` 268 lines (14 items + 2 additional `export` keywords added for the 2 items now called cross-module: `semantic_type_is_tarray`/`array_element_type_from_semantic_type`/`generic_type_name`/`conditional_else_empty_unknown_array_coerced_to_then_array_element` gained `export`, `coerce_expr_to_type` already was). Bootstrap diff (`mlcc -o p1` before-split source, `mlcc -o p2` after-split source, same pre-existing `mlcc` binary both times): **not literally empty** — honest finding, refined from the Decision's overstated "not one byte" wording. Diff restricted to exactly 4 files: `transform.cpp`/`.hpp` (shrink, expected), `transform_coerce.cpp`/`.hpp` (new, expected) — plus 2 **other** modules that call the relocated `coerce_expr_to_type` directly, `transform_decl.cpp`/`transform_stmts.cpp`, each with a 1-token change per call site (`transform::coerce_expr_to_type` → `transform_coerce::coerce_expr_to_type`): C++ namespace-qualification always resolves to the symbol's defining module, so any relocation mechanically renames its external call sites' namespace prefix — logic unchanged, confirmed by full-tree `diff -rq` finding **zero** other differing files among the ~335. `compiler/build.sh` (fresh rebuild, cleared a stale precompiled-header cache first — unrelated pre-existing staleness, not from this change) → new `mlcc` built successfully. `rake test_compiler_mlc` (rebuilds `run_tests` from the split source) → **1471 passed, 0 failed**, matches pre-split baseline exactly, `arch lint failures=0` (transform.mlc stays on the file-size allowlist, now smaller: 1505 vs 1765). Determinism cross-check: new-`mlcc`-translated output of `compiler/main.mlc` byte-identical to old-`mlcc`-translated output of the same (post-split) source. Self-host `mlcc2` identity check per `.cursor/rules/mlcc-self-host-verification.mdc`: `build_bin.sh` (g++) built `mlcc2` from the new `mlcc`'s own translation of `compiler/main.mlc`; `mlcc2` re-translating the same source produced a **byte-identical** result (`diff -r --exclude=obj` empty) — self-hosting determinism holds.

### Next slices (deferred, need `transform_expr_fn` injection — separate Decisions)

- **Slice 2** `transform_call_args.mlc`: `transform_one_call_argument_using_optional_expected_type` + `transform_lambda_call_argument` + `transform_call_arguments_fold_step` + `transform_call_arguments_using_callee_semantic_type` + `expected_call_argument_type_at_index` + `function_return_type_from_callee_type`. Needs `transform_expr_fn`/`transform_exprs_fn` injected (both defined in `transform.mlc`, used ~26 call sites combined).
- **Slice 3** `transform_method.mlc`: `transform_result_option_hof_method_call` + `transform_array_hof_method_call` + `transform_regular_method_call` + `transform_extend_method_as_call` + `transform_region_alloc_method_call`. Needs `transform_exprs_fn` injected (3 call sites). `transform_method_call_after_object` stays in `transform.mlc` (calls `dispatch_transform_pass` directly, tightly coupled to the dispatcher).

## Verification discipline

Every sub-track: `mlcc -o /tmp/p1 compiler/main.mlc` before, apply change,
`mlcc -o /tmp/p2 compiler/main.mlc` after, `diff -r /tmp/p1 /tmp/p2` empty
(bootstrap diff) unless the sub-track explicitly changes output by design
(§104-18/19/20/24 — those gate on `rake test_compiler_mlc` + a working
`clang++` build instead, exactly as the source review specifies per-step).
**Refined 2026-07-28 (§104-12 slice 1 finding):** for a god-file split
specifically, "empty" cannot be literal — the split module's own
`.cpp`/`.hpp` pair is new and the source module's shrinks, and any *other*
module that calls a relocated symbol gets its call site's C++ namespace
prefix mechanically renamed to the symbol's new home (C++ codegen always
qualifies by defining module, regardless of import path). The actual gate
for a split: restrict `diff -rq` to files **other than** the split
modules and confirm **zero** further differences; any change touching a
direct caller of a moved symbol must be exactly 1 renamed namespace
prefix per call site, not a logic change — verify by reading the diff,
don't just eyeball line counts. `scripts/regression_gate.sh` after any
`lib/mlc/` touch. Self-host verification per
`.cursor/rules/mlcc-self-host-verification.mdc` — mlcc2 diff check —
required before Critic close on any Wave 1/2 sub-track.
