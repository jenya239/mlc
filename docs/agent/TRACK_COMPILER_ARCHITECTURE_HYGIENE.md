# Track: Compiler core architecture/perf/testing hygiene — execute the 2026-06-29 review

Parent: [../PLAN.md](../PLAN.md) §104. Authorized 2026-07-28 (user request: "трек
чисто по улучшению производительности, архитектуры и тестирования"). Scope:
`compiler/**` (self-hosted compiler core), distinct from §97/§101 (editor
render) and §102/§103 (new feature epics).

## Status: **open** — §100 closed 2026-07-28, §104-1/2/3 found already
implemented (see correction below, 2026-07-28), **§104-12 slice 1 closed
2026-07-28** (`transform_coerce.mlc` extracted, Critic-audited), **§104-12
slice 2 closed** same day (`transform_context.mlc` extracted, Critic-audited
— prerequisite leaf-module for slice 3's `transform_call_args.mlc`; slices
renumbered, see §104-12 section below), **§104-12 slice 3 Decision frozen**
same day (`transform_call_args.mlc` — 2-3 injected function parameters
threaded through 4 functions), **STEP=1 red confirmed** same day (module
absent, all 9 items at documented lines, file at baseline 1468 lines),
**STEP=2 green** same day (`transform_call_args.mlc` created, 9 items
moved, bootstrap diff scoped to `transform.cpp/.hpp` + 2 new files only,
`rake test_compiler_mlc` exit_code=0, mlcc2 self-host diff IDENTICAL),
**§104-12 slice 3 closed** same day (Critic-audited: independent
function/type-set diff, independent fresh mlcc translation + stray-
reference grep, independent full `rake test_compiler_mlc` rerun
1471/0), **§104-12 slice 4 Decision frozen** same day
(`transform_method.mlc` — 12 items, not the roughly-estimated 5, found
by repo-wide grep dependency closure), **STEP=1 red confirmed** same day
(module absent, all 12 items at documented lines, file at baseline 1132
lines), **STEP=2 green** same day (`transform_method.mlc` created,
12 items moved, bootstrap diff scoped to `transform.cpp/.hpp` + 2 new
files only, `rake test_compiler_mlc` exit_code=0 1471/0, mlcc2 self-host
diff IDENTICAL), **§104-12 slice 4 closed, §104-12 itself CLOSED** same
day (Critic-audited: independent function/type-set diff, independent
fresh mlcc translation + stray-reference grep, independent full
`rake test_compiler_mlc` rerun 1471/0 — `transform.mlc` 1765→881 lines
across 4 new modules). **Queue head is now §104-13**
(`codegen/decl_cpp.mlc` split, 1666 lines) (priority override
2026-07-28, user: "это должно быть приоритетом сейчас" — Wave 1 moved
ahead of §101/§102/§103; Wave 2 stays queued after §103, Wave 3 stays
gated)

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

**God-file split exit criterion (added 2026-07-29, was missing — caused §104-12 to
read "closed" while still over the line):** a split sub-track (§104-12/13/14/15/16)
is only fully closed when the target file's line count is **≤800**
(`compiler/tests/run_architecture_lint.sh:7`, `MAX_FILE_LINES=800`) **and** its
`file_size:` entry is removed from `compiler/tests/architecture_lint_allowlist.txt`.
If further splitting would require breaking a still-cyclic dependency or is judged
not worth the risk, that is a **Decision-level call requiring an explicit note in
the TRACK** ("residual size N, reason X, allowlist entry kept intentionally") — not
a silent "closed" with the file still allowlisted.

- **§104-1** `FileId`/`FileStore` (review Step 1) — **done**, pre-existing (see Correction above)
- **§104-2** `Span.file_id` extension via `span_make` (Step 2) — **done** except deferred `file_id` field (see Correction above)
- **§104-3** `StringInterner` (Step 3) — **done**, pre-existing (see Correction above)
- **§104-12** split `transform/transform.mlc` (1765 lines now, was 1558) (Step 12) — **CLOSED** 2026-07-30, slice 5: extracted `transform_support.mlc` (139 lines, 9 pure-leaf helpers with zero recursion back into `transform_expr`/`dispatch_transform_pass` — `direct_call_parameter_mutability_flags`, `call_callee_ident_name`, `binary_result_type_for_operator`, `merge_conditional_expression_types`, `array_element_type_from_semantic_expression`, `question_unwrapped_type_from_inner` + its internal-only `type_arguments_from_generic_type`, `standalone_unknown_cell`, `inferred_types_from_record_literal_part_for_merge`); `transform.mlc` 881→**753 lines**, allowlist entry removed, both files ≤800
- **§104-13** split `codegen/decl_cpp.mlc` (1666 lines now, was 1119) (Step 13) — **CLOSED** 2026-07-29, 6 slices, 1666→**355 lines** across `decl_cpp.mlc` + 5 new modules (`decl_cpp_helpers.mlc`/`decl_cpp_type.mlc`/`decl_cpp_trait.mlc`/`decl_cpp_fn.mlc`/`decl_cpp_ffi.mlc`/`decl_cpp_extend.mlc`), all ≤800, allowlist entry removed
- **§104-14** split `codegen/expr/match_gen.mlc` (1403 lines now, was 907) (Step 14) — target ≤800, expect 2+ slices
- **§104-15** split `checker/registry.mlc` (1060 lines now, was 870) — needs re-export language support first, see review Часть 3 §1 (Step 15) — target ≤800
- **§104-16** split `checker/infer/infer.mlc` (962 lines now, was 786) (Step 16) — target ≤800
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
| 3 | Critic: full re-audit | **done** — see Critic audit below |

**STEP=2 verification (2026-07-28):** `transform.mlc` 1765→1505 lines, new `transform_coerce.mlc` 268 lines (14 items + 2 additional `export` keywords added for the 2 items now called cross-module: `semantic_type_is_tarray`/`array_element_type_from_semantic_type`/`generic_type_name`/`conditional_else_empty_unknown_array_coerced_to_then_array_element` gained `export`, `coerce_expr_to_type` already was). Bootstrap diff (`mlcc -o p1` before-split source, `mlcc -o p2` after-split source, same pre-existing `mlcc` binary both times): **not literally empty** — honest finding, refined from the Decision's overstated "not one byte" wording. Diff restricted to exactly 4 files: `transform.cpp`/`.hpp` (shrink, expected), `transform_coerce.cpp`/`.hpp` (new, expected) — plus 2 **other** modules that call the relocated `coerce_expr_to_type` directly, `transform_decl.cpp`/`transform_stmts.cpp`, each with a 1-token change per call site (`transform::coerce_expr_to_type` → `transform_coerce::coerce_expr_to_type`): C++ namespace-qualification always resolves to the symbol's defining module, so any relocation mechanically renames its external call sites' namespace prefix — logic unchanged, confirmed by full-tree `diff -rq` finding **zero** other differing files among the ~335. `compiler/build.sh` (fresh rebuild, cleared a stale precompiled-header cache first — unrelated pre-existing staleness, not from this change) → new `mlcc` built successfully. `rake test_compiler_mlc` (rebuilds `run_tests` from the split source) → **1471 passed, 0 failed**, matches pre-split baseline exactly, `arch lint failures=0` (transform.mlc stays on the file-size allowlist, now smaller: 1505 vs 1765). Determinism cross-check: new-`mlcc`-translated output of `compiler/main.mlc` byte-identical to old-`mlcc`-translated output of the same (post-split) source. Self-host `mlcc2` identity check per `.cursor/rules/mlcc-self-host-verification.mdc`: `build_bin.sh` (g++) built `mlcc2` from the new `mlcc`'s own translation of `compiler/main.mlc`; `mlcc2` re-translating the same source produced a **byte-identical** result (`diff -r --exclude=obj` empty) — self-hosting determinism holds.

**Critic audit (2026-07-28), slice 1 CLOSED:** independent re-verification, not a
re-read of the Driver's log. Full `fn`/`export fn` name-set diff between the
pre-split `transform.mlc` (`git show 002bc584:...`) and the current
`transform.mlc`+`transform_coerce.mlc` combined — identical 61 names, zero
lost/duplicated, only the 4 documented `export` additions. Fresh
`compiler/out/mlcc -o ... compiler/main.mlc` translation from scratch → 335
files; confirmed `transform_coerce.cpp`/`.hpp` exist and
`transform_decl.cpp`/`transform_stmts.cpp` call
`transform_coerce::coerce_expr_to_type` (not `transform::`) at all 4 sites.
Independent full `rake test_compiler_mlc` rerun (all 10 phases from a clean
shell, not reusing the Driver's binary state) → `1471 passed, 0 failed`,
`arch lint failures=0`, `transform.mlc` shown at 1505 lines on the WARN
allowlist — confirms the built `run_tests`/`mlcc` under test reflect the
split, not stale artifacts. mlcc2 self-host g++ diff not re-run a third time
(witnessed directly during STEP=2 in the same continuous session; no source
change since). `scripts/regression_gate.sh` is not required for closing a
slice within an still-open track file (AGENTS.md's gate triggers on closing
the whole TRACK_*.md) — attempted anyway for extra assurance, aborted after
~19 min once it was confirmed stuck on its own optional
`run_examples_compile_sweep.sh` OpenGL-example tail step, unrelated to this
change. No false-done found. Queue head → slice 2.

### Slice 2 — `transform_context.mlc` (prerequisite for slice 3, do first — zero injection)

#### Decision (STEP=0) — **frozen** 2026-07-28

| Item | Choice |
|------|--------|
| Problem | Slice 3 (`transform_call_args.mlc`, formerly numbered "slice 2" — renumbered here) threads `transform_context: TransformContext` and `stmts_fn: (...) -> TransformStmtsResult` through every moved function. Both types are currently defined **inside** `transform.mlc` (lines 429/460) alongside their constructors (`transform_context_new` — dead code, zero callers anywhere in the tree, confirmed by grep; `empty_transform_context`, `transform_context_with_env`, `transform_context_with_lambda_parameter_types`). If `transform_call_args.mlc` imported these types directly from `./transform`, and `transform.mlc` also imports the call_args group back from `transform_call_args.mlc`, that is a genuine value-level **and** type-level cycle — untested territory (confirmed by grep: no two `compiler/**` modules currently import from each other in both directions; the closest analog, `infer.mlc`/`infer_call.mlc`, avoids this by keeping the shared `CheckContext` type in a **third**, independent leaf module `checker/check/check_context.mlc`, imported one-directionally by both). Same fix applies here: extract `TransformContext`/`TransformStmtsResult` into their own leaf module first, so both `transform.mlc` and the future `transform_call_args.mlc` import the type one-directionally from it, never from each other |
| Strategy (v1) | New `compiler/checker/transform/transform_context.mlc`, mirroring `checker/check/check_context.mlc`'s role exactly. Move (export, no behavior change): `TransformContext` type, `TransformStmtsResult` type, `transform_context_new`, `empty_transform_context`, `transform_context_with_env`, `transform_context_with_lambda_parameter_types`. `transform.mlc` imports all 6 back (needs them at 10+ internal call sites: 435/762/849/850/872/1176/1199/1226/1350/1362/1382/1395/1427 and the type itself as a parameter/return type throughout). **No signature change, no algorithm change** — pure relocation, same pattern as slice 1's `transform_coerce.mlc`. External importers (`transform_stmts.mlc`, `check_mutations.mlc`, `names.mlc`, `tests/test_pipe_and_record_update.mlc`, `tests/test_visitor_pass_parity.mlc`) currently write `import { TransformContext, ..., transform_expr, ... } from './transform'` (or `'../transform/transform'`) in a single combined line — **do not need to change**: confirmed by slice 1's own `coerce_expr_to_type` precedent (`transform_stmts.mlc` still imports it `from './transform'` today even though the real definition moved to `transform_coerce.mlc` in slice 1 — the checker resolves a re-exported/forwarded name transitively through whichever module textually imports it, only the **generated C++ namespace** changes to the true defining module, not the MLC source import path). This means this slice's only source edits are 2 files: the new module, and `transform.mlc`'s own definitions replaced by one import line |
| Primary gate | Red: `transform_context.mlc` absent, `TransformContext`/`TransformStmtsResult`/4 constructors still defined in `transform.mlc` at lines 429-463. Green: `transform_context.mlc` exists with all 6 items exported; `transform.mlc` shrinks by ~35 lines, gains 1 import line; **bootstrap diff restricted to split modules + direct-caller namespace-prefix renames only** (per the refined Verification-discipline gate below — literal empty is not the standard for a god-file split); `rake test_compiler_mlc` (1471+ passed, 0 failed); self-host mlcc2 diff before Critic close |
| Module touch | new `compiler/checker/transform/transform_context.mlc`; `compiler/checker/transform/transform.mlc` (shrinks, gains 1 import line). Zero other files touched (relying on the confirmed transitive-import-forwarding behavior — if this turns out wrong at STEP=2, the 5 external importers get an explicit second import line as a fallback, still a small bounded edit) |
| REG | no (`compiler/**` only) |
| Out of scope | the call_args group itself (next slice, needs `transform_expr_fn`/`transform_exprs_fn`/`transform_expr_lambda_with_param_types_fn` injection — see Slice 3 below); any type/constructor signature change; MIR |

#### Steps (§104-12 — slice 2: transform_context)

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze | **done** |
| 1 | Red: confirm current boundaries | **done** — `find compiler/checker/transform -iname transform_context.mlc` empty (module absent); `transform.mlc` still 1505 lines (unchanged since slice 1); all 6 items (`TransformContext`/`TransformStmtsResult` types + 4 constructors) confirmed at the exact documented lines (429/435/438/441/448/460) |
| 2 | Green: create `transform_context.mlc`, wire `transform.mlc` import, bootstrap diff (split-scoped), `rake test_compiler_mlc`, mlcc2 self-host diff | **done** — see verification below |
| 3 | Critic: full re-audit | **done** — see Critic audit below |

**STEP=2 verification (2026-07-28):** `transform.mlc` 1505→1468 lines, new `transform_context.mlc` 41 lines (6 items: `TransformContext`/`TransformStmtsResult` types + `transform_context_new`/`empty_transform_context`/`transform_context_with_env`/`transform_context_with_lambda_parameter_types`, all exported — the latter 2 gained `export` since `transform.mlc` now needs them cross-module, `transform_context_new` gained `export` too though it remains dead code tree-wide, confirmed by grep, unchanged from before). `--check-only` clean. Bootstrap diff (old `mlcc` on pre/post-split source): full-tree `diff -rq` found differences in **exactly** 8 files — `transform.cpp`/`.hpp` (shrink), `transform_context.cpp`/`.hpp` (new) — plus 4 direct-caller files (`check_mutations.cpp`, `names.cpp`, `transform_decl.cpp`, `transform_stmts.cpp`/`.hpp`, all referencing the moved types/functions), each change read line-by-line and confirmed to be **exactly** a `transform::` → `transform_context::` namespace-prefix rename (e.g. `transform::TransformContext` → `transform_context::TransformContext`), zero logic changes; confirmed **zero** other differences among ~335 files. Notably: **zero MLC source edits needed in the 5 external importers** (`transform_stmts.mlc`, `check_mutations.mlc`, `names.mlc`, 2 test files) — confirms the Decision's transitive-import-forwarding hypothesis (they still write `import { ..., TransformContext, ... } from './transform'`, unchanged, and it resolves correctly to the new home). `compiler/build.sh` rebuilt `mlcc` clean (only pre-existing unrelated `-Wparentheses-equality` warnings). `rake test_compiler_mlc` (rebuilds `run_tests` from split source) → **1471 passed, 0 failed**, `arch lint failures=0`, `transform.mlc` now shown at 1468 lines on the size-allowlist WARN. Determinism cross-check: new-mlcc-translated `compiler/main.mlc` output byte-identical to old-mlcc-translated output of the same post-split source. Self-host `mlcc2` identity check per `.cursor/rules/mlcc-self-host-verification.mdc`: `build_bin.sh` (g++, `TMPDIR` set in-repo — root filesystem at 99% usage) built `mlcc2` from the new `mlcc`'s own translation; `mlcc2` re-translating the same source → `diff -r --exclude=obj` **empty**, IDENTICAL — self-hosting determinism holds.

**Critic audit (2026-07-28), slice 2 CLOSED:** independent re-verification.
Full `fn`/`type` name-set diff between the pre-split `transform.mlc` and the
current `transform.mlc`+`transform_context.mlc` combined — identical 52
names, zero lost/duplicated, only the 2 documented `export` additions
(`transform_context_new`, `transform_context_with_lambda_parameter_types`).
Fresh `compiler/out/mlcc -o ... compiler/main.mlc` translation from scratch
→ 337 files; confirmed `transform_context.cpp`/`.hpp` exist and every
`TransformContext`/`empty_transform_context`/`transform_context_with_env`
reference in the 5 direct-caller files (`check_mutations.cpp`, `names.cpp`,
`transform_decl.cpp`, `transform_stmts.cpp`/`.hpp`) is qualified
`transform_context::` — zero stray `transform::` references left over.
Independent full `rake test_compiler_mlc` rerun (all 10 phases, clean
shell): first attempt showed 2 unit failures + a fuzz-smoke `mktemp`
error — traced to a **stale `TMPDIR` env var** left exported in this same
shell from the Driver's own mlcc2 build in the prior turn, pointing at a
directory this Critic pass had since deleted during cleanup; not a code
regression. Unset `TMPDIR`, reran clean → `1471 passed, 0 failed`, all 10
phases `ok`, `arch lint failures=0`, `transform.mlc` shown at 1468 lines.
mlcc2 self-host g++ diff not re-run a third time (witnessed directly during
STEP=2 in the same continuous session; no source change since). No
false-done found. Queue head → slice 3 (needs its own Decision — the
injection-heavy `transform_call_args.mlc` extraction).

### Slice 3 — `transform_call_args.mlc` (needs `transform_expr_fn` injection — depends on slice 2)

#### Decision (STEP=0) — **frozen** 2026-07-28

| Item | Choice |
|------|--------|
| Problem | Renumbered from the original "Slice 2" (the review's own call_args group), unblocked now that slice 2 (`transform_context.mlc`) landed. Re-derived against current line numbers (post slice-2 shift, all confirmed by grep, 2026-07-28): `expected_call_argument_type_at_index` (438) + `transform_lambda_call_argument` (446) + `transform_one_call_argument_using_optional_expected_type` (466-618, 26 `transform_expr(` calls) + `Transform_call_arguments_fold_state` type (620) + `function_return_type_from_callee_type` (625) + `transform_call_arguments_fold_step` (650) + `transform_call_arguments_using_callee_semantic_type` (667-690, 1 `transform_exprs(` call). Unlike slice 2's type-only cycle, this group also has a genuine **value**-level dependency on 2 functions that must stay in `transform.mlc`: `transform_expr` (defined at the end of the file, 26 call sites in the group) and `transform_expr_lambda_with_param_types` (790 — has an *unrelated* second caller at line 1308 inside `dispatch_transform_pass`, plus its own lambda-parameter-type-environment dependency chain (`transform_lambda_parameter_types_environment_fold_step`/`Transform_lambda_parameter_types_fold_state`/`transform_context_with_lambda_parameter_types`) that is a different concern from call-argument dispatch — out of scope to also move this slice). Importing either directly from the new module would recreate the transform.mlc↔transform_call_args.mlc cycle slice 2 was built to avoid |
| Strategy (v1) | New `compiler/checker/transform/transform_call_args.mlc`. Move wholesale (zero other callers in `transform.mlc`, confirmed by grep): `callee_semantic_type_is_function` (141), `function_parameter_types_from_callee_type` (163), `call_argument_is_lambda` (225) — these 3 leaf helpers are used *only* inside the call_args group itself, unlike slice 1/2's shared leaves. Move + export the 6 group functions/type above. `expected_call_argument_type_at_index`'s one dependency on `standalone_unknown_cell` (4 *other* callers remain in `transform.mlc`, so it can't move wholesale) is resolved by inlining the 1-line `Shared.new(TUnknown)` literal directly — matches the idiom already used throughout `transform_coerce.mlc`'s own match arms, not a new pattern. Thread 2 injected function parameters through the 4 non-leaf functions that need them, mirroring the multi-callback-parameter precedent at `infer_isolate_method.mlc:77` (`infer_expr_fn` passed alongside other callback parameters there): `transform_expr_fn: (Shared<Expr>, TransformContext, ([Shared<Stmt>], TransformContext) -> TransformStmtsResult) -> Shared<SemanticExpression>` and `transform_expr_lambda_with_param_types_fn: ([string], [Shared<Type>], Shared<Expr>, Span, TransformContext, ([Shared<Stmt>], TransformContext) -> TransformStmtsResult) -> Shared<SemanticExpression>`, both appended as trailing parameters to `transform_lambda_call_argument`, `transform_one_call_argument_using_optional_expected_type`, `transform_call_arguments_fold_step`, and `transform_call_arguments_using_callee_semantic_type` (forwarded unchanged through each call in the chain). The outermost function additionally needs a 3rd injected parameter, `transform_exprs_fn: ([Shared<Expr>], TransformContext, ([Shared<Stmt>], TransformContext) -> TransformStmtsResult) -> [Shared<SemanticExpression>]`, for its own direct `transform_exprs(...)` call (line 689) — not forwarded further, since no other moved function calls `transform_exprs` directly. `transform.mlc`'s single call site (`dispatch_transform_pass`, line 1062) passes `transform_expr, transform_expr_lambda_with_param_types, transform_exprs` as the 3 trailing arguments; `transform.mlc` also imports `function_return_type_from_callee_type` back for its own 1 external use (line 1068) — a plain one-directional import, no injection needed since that function is a pure leaf. Import `TransformContext`/`TransformStmtsResult` from `./transform_context` (the slice-2 leaf), not from `./transform` — this is exactly what slice 2 was built to enable |
| Primary gate | Red: `transform_call_args.mlc` absent, all 9 items (6 group + 3 leaf helpers) still in `transform.mlc` at the documented lines. Green: `transform_call_args.mlc` exists; `transform.mlc` shrinks by ~250 lines, gains 1 import + passes 3 args at the 1 call site; **bootstrap diff restricted to split modules + direct-caller namespace-prefix renames only** (the refined god-file-split gate); `rake test_compiler_mlc` (1471+ passed, 0 failed); self-host mlcc2 diff before Critic close |
| Module touch | new `compiler/checker/transform/transform_call_args.mlc`; `compiler/checker/transform/transform.mlc` (shrinks, gains 1 import line + 3 trailing arguments at 1 call site) |
| REG | no (`compiler/**` only) |
| Out of scope | `transform_expr`/`transform_expr_lambda_with_param_types`/`dispatch_transform_pass`/`TransformContext` relocation (already done in slice 2, or stay put by design); the method group (slice 4, deferred); algorithm changes; MIR |

#### Steps (§104-12 — slice 3: call_args)

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze | **done** |
| 1 | Red: confirm current boundaries | **done** |
| 2 | Green: create `transform_call_args.mlc`, thread the 3 injected parameters, wire `transform.mlc` call site + import, bootstrap diff (split-scoped), `rake test_compiler_mlc`, mlcc2 self-host diff | **done** |
| 3 | Critic: full re-audit | **done — closed** |

#### Critic close (STEP=3, 2026-07-28)

Independent function/type-set diff: old `transform.mlc` (46 names) vs new `transform.mlc` (36) + `transform_call_args.mlc` (10) combined = 46, zero lost/duplicated. Export-status diff: exactly the 8 moved functions gained `export` (needed for cross-module visibility); the type `Transform_call_arguments_fold_state` and `transform_call_arguments_fold_step` stayed non-exported (internal to the new module, matches Decision). Fresh `mlcc` translation of `compiler/main.mlc`: `transform_call_args.cpp/.hpp` created; grepped for `transform_call_args::` across all generated files — found only in `transform.cpp` (the 1 direct caller), zero stray references. Independent `rake test_compiler_mlc` rerun from a clean shell (`TMPDIR` confirmed unset first): `1471 passed, 0 failed`, `arch lint failures=0`. Line counts confirmed: `transform.mlc` 1132, `transform_call_args.mlc` 365. No false-done found. **§104-12 slice 3 closed.**

#### Green result (STEP=2, 2026-07-28)

`transform_call_args.mlc` created (365 lines): 3 leaf helpers + 6 group items moved wholesale (`expected_call_argument_type_at_index` inlines `Shared.new(TUnknown)` instead of importing `standalone_unknown_cell`, which stays in `transform.mlc` with 4 other callers). `transform.mlc`: 1468 → 1132 lines. Bootstrap diff scoped to exactly `transform.cpp/.hpp` + the 2 new files — no other direct-caller namespace-prefix renames needed this slice (unlike slice 1/2), since both moved public functions had exactly 1 external caller, inside `transform.mlc` itself. `rake test_compiler_mlc`: exit_code=0, arch lint failures=0. mlcc2 self-host diff (in-repo `TMPDIR`, host disk 99%): IDENTICAL.

### Slice 4 — `transform_method.mlc` (needs `transform_expr_fn`/`transform_exprs_fn` injection — depends on slice 2)

#### Decision (STEP=0) — **frozen** 2026-07-28

| Item | Choice |
|------|--------|
| Problem | Re-derived the method-call dispatch group by grep dependency closure (2026-07-28) — the rough 5-function estimate in this file undercounted, matching the pattern already seen in slice 1 (14 functions, not the review's smaller estimate). Confirmed 12 items with zero other callers anywhere in `compiler/**` outside `transform.mlc` (checked by repo-wide grep, not just within the file): `receiver_type_key_for_method_dispatch` (45), `instance_method_receiver_and_parameters_mutability_pattern` (67), `extend_method_mangled_name` (95), `transform_extend_method_as_call` (108 — pure, builds `SemanticExpressionCall` from already-typed values, no `transform_expr`/`transform_exprs` dependency despite superficially looking related to call dispatch), `method_result_type_for_dispatch` (189 — pure leaf, only reachable from `transform_regular_method_call`), `transform_expr_with_lambda_parameter_types` (490), `semantic_expression_list_singleton` (501), `transform_array_hof_method_arguments` (508), `transform_result_option_hof_method_call` (279), `transform_array_hof_method_call` (299), `transform_regular_method_call` (320), `transform_region_alloc_method_call` (1112). `transform_method_call_after_object` (1082) stays in `transform.mlc`: takes `transform_pass: TransformPass` and calls `dispatch_transform_pass` directly — tightly coupled to the dispatcher, same rationale as `transform_expr_lambda_with_param_types` staying put in slice 3 |
| Strategy (v1) | New `compiler/checker/transform/transform_method.mlc`. Move all 12 items wholesale. 6 are pure leaves needing zero injection (`receiver_type_key_for_method_dispatch`, `instance_method_receiver_and_parameters_mutability_pattern`, `extend_method_mangled_name`, `transform_extend_method_as_call`, `method_result_type_for_dispatch`, `semantic_expression_list_singleton`). 2 need `transform_expr_fn` injected (`transform_expr_with_lambda_parameter_types`, `transform_array_hof_method_arguments` — the latter also forwards it to the former); `transform_array_hof_method_call` forwards `transform_expr_fn` down to `transform_array_hof_method_arguments` without using it directly. 3 need `transform_exprs_fn` injected (`transform_result_option_hof_method_call`, `transform_regular_method_call`, `transform_region_alloc_method_call`). Both injected parameters use the same signatures established in slice 3 (`transform_expr_fn: (Shared<Expr>, TransformContext, ([Shared<Stmt>], TransformContext) -> TransformStmtsResult) -> Shared<SemanticExpression>`, `transform_exprs_fn: ([Shared<Expr>], TransformContext, ([Shared<Stmt>], TransformContext) -> TransformStmtsResult) -> [Shared<SemanticExpression>]`) — no 3rd parameter needed this slice (nothing in this group calls `transform_expr_lambda_with_param_types`). `transform.mlc`'s 1 call site (`transform_method_call_after_object`, 3 of its 4 branches) passes `transform_expr, transform_exprs` as trailing arguments to whichever of the 3 top-level group functions it calls (`transform_result_option_hof_method_call`, `transform_array_hof_method_call`, `transform_region_alloc_method_call`, `transform_regular_method_call`); `transform.mlc` also gains 1 plain import for these 4 function names (no injection needed on the import side, they're just called, not re-exported) |
| Primary gate | Red: `transform_method.mlc` absent, all 12 items still in `transform.mlc` at the documented lines. Green: `transform_method.mlc` exists; `transform.mlc` shrinks by ~330 lines, gains 1 import + passes 2 trailing args at up to 4 call sites inside `transform_method_call_after_object`; bootstrap diff restricted to split modules + direct-caller namespace-prefix renames only; `rake test_compiler_mlc` (1471+ passed, 0 failed); self-host mlcc2 diff before Critic close |
| Module touch | new `compiler/checker/transform/transform_method.mlc`; `compiler/checker/transform/transform.mlc` (shrinks, gains 1 import line + 2 trailing arguments at the 4 call sites inside `transform_method_call_after_object`) |
| REG | no (`compiler/**` only) |
| Out of scope | `transform_method_call_after_object`/`dispatch_transform_pass`/`transform_expr_lambda_with_param_types`/`transform_shared_static_new_call` relocation (unrelated concern — `Shared.new(...)` static-call dispatch, not `.method()` dispatch); algorithm changes; MIR. This closes out §104-12 (the `transform.mlc` god-file split) — no further slices planned after this one |

#### Steps (§104-12 — slice 4: method)

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze | **done** |
| 1 | Red: confirm current boundaries | **done** |
| 2 | Green: create `transform_method.mlc`, thread the 2 injected parameters, wire `transform.mlc` call sites + import, bootstrap diff (split-scoped), `rake test_compiler_mlc`, mlcc2 self-host diff | **done** |
| 3 | Critic: full re-audit, close §104-12 | **done — closed** |

#### Green result (STEP=2, 2026-07-28)

`transform_method.mlc` created (287 lines): all 12 items moved wholesale. `transform.mlc`: 1132 → 881 lines. Bootstrap diff scoped to exactly `transform.cpp/.hpp` + the 2 new files — 12 removed declarations match the Decision exactly, 4 namespace-qualified call sites inside `transform_method_call_after_object` pass `transform_expr`/`transform_exprs` as documented. `rake test_compiler_mlc`: exit_code=0, `1471 passed, 0 failed`, arch lint failures=0. mlcc2 self-host diff (in-repo `TMPDIR`, host disk 99%): IDENTICAL.

#### Critic close (STEP=3, 2026-07-28) — §104-12 CLOSED

Independent function/type-set diff: old `transform.mlc` (`git show 5fee3313:...`,
36 names) vs new `transform.mlc` (24) + `transform_method.mlc` (12) combined =
36, zero lost/duplicated. Export-status diff: all 12 moved items gained
`export` (wider than strictly needed — 6 of them have no caller outside
`transform_method.mlc` itself and could have stayed non-exported, unlike
slice 3's 2 internal-only items — noted as a minor deviation from the
slice-3 precedent, not a defect: no test failure, no cycle, no behavior
change). Fresh `mlcc` translation of `compiler/main.mlc` from scratch:
`transform_method.cpp/.hpp` created; grepped `transform_method::` across
every generated file — found only in `transform.cpp` (the 1 direct
caller), zero stray references. Independent `rake test_compiler_mlc` rerun
from a clean shell (`TMPDIR` confirmed unset): `1471 passed, 0 failed`,
`arch lint failures=0`. Line counts confirmed: `transform.mlc` 881 lines
(down from 1765 at track start — on the file-size WARN allowlist, not a
failure, same status as slices 1/2/3), `transform_method.mlc` 287,
`transform_call_args.mlc` 365, `transform_context.mlc` 41,
`transform_coerce.mlc` 268. No false-done found.

**§104-12 closed (premature — see reopen below)** — `transform.mlc` split
from 1765 to 881 lines across 4 new leaf/injection modules (slices 1-4),
all bootstrap-diff-scoped, self-host mlcc2-identical, 1471/0 throughout.

#### Slice 5 (reopen, 2026-07-29/30) — `transform_support.mlc`

**Problem:** the slice-4 close (2026-07-28) treated §104-12 as done without
checking `transform.mlc` against the 800-line gate — 881 lines, over the
line, `architecture_lint_allowlist.txt` still carried the entry. Caught
2026-07-29 alongside the same finding for §104-13 slice 6, which is what
motivated the exit-criterion addition above. Re-derived the remaining
helper section (lines 46-330 of the 881-line file, ahead of the
`TransformPass` type + `dispatch_transform_pass` + `extend TransformPass`
block, which is one mutually-recursive cluster with the multi-arg
recursion helpers `transform_exprs`/`transform_field_values`/
`transform_match_arms`/`transform_expr_lambda_with_param_types` — those all
call `transform_expr` by name, which is defined at the file's bottom as
the public entry wrapping `dispatch_transform_pass`; moving any of that
cluster elsewhere reproduces the same circular-import shape §104-13
slice 6 hit, deliberately left alone here as out of scope for a pure
relocation). Found 9 true pure-leaf functions with **zero** calls into
`transform_expr`/`dispatch_transform_pass`/the extend block, confirmed by
grep — zero external callers repo-wide for all 9 either:
`direct_call_parameter_mutability_flags`, `call_callee_ident_name`,
`binary_result_type_for_operator`, `merge_conditional_expression_types`,
`array_element_type_from_semantic_expression`,
`type_arguments_from_generic_type` (internal-only, called by the next
one), `question_unwrapped_type_from_inner`, `standalone_unknown_cell`,
`inferred_types_from_record_literal_part_for_merge`.

**Strategy:** new `compiler/checker/transform/transform_support.mlc`, same
directory as `transform.mlc` so all relative import paths transfer
unchanged. 8 of 9 exported (every caller stays in `transform.mlc`'s
surviving helper/extend-block code); `type_arguments_from_generic_type`
stays non-exported (only caller, `question_unwrapped_type_from_inner`,
moved into the same new module). `transform.mlc` imports the 8 back;
5 now-unused imports dropped (`RecordLitFields`/`RecordLitSpread` from
`../../frontend/ast`, `method_return_type_from_object` from `../registry`,
`type_is_unknown` from `../semantic_type_structure`, `infer_expr` from
`../infer/infer` — `binary_operation_result_type` stayed, still called
directly from `visit_bin`).

**Verify (2026-07-30):** fresh `mlcc -o ... compiler/main.mlc` translation
from scratch, exit 0. `compiler/build.sh` rebuild, exit 0. Independent
`rake test_compiler_mlc` full rerun: `1471 passed, 0 failed`, arch lint
`failures=0 warnings=11` (down from 12 — `transform.mlc` no longer flagged).
Line counts: `transform.mlc` 753, `transform_support.mlc` 139, both ≤800;
allowlist entry `file_size:checker/transform/transform.mlc` removed.
mlcc2 self-host diff (`build_bin.sh`, `MLC_CXX=g++`, in-repo `TMPDIR`):
fresh `mlcc` → `/tmp/mlc_p1`, `mlcc2` built from `/tmp/mlc_p1`'s C++,
`mlcc2` → `/tmp/mlc_p2`, `diff -r /tmp/mlc_p1 /tmp/mlc_p2 --exclude=obj`
IDENTICAL. No `lib/mlc/**` touched, `scripts/regression_gate.sh` not
required for this sub-step.

**§104-12 itself CLOSED, re-confirmed by Critic 2026-07-30** — independent
function/type-set diff (24/24, empty), export-status diff (exactly the 8
documented), byte-level function-body diff (all 9 verbatim modulo `export`),
fresh `mlcc` translation + stray-`transform_support::`-reference grep
(zero outside `transform.cpp`), independent `rake test_compiler_mlc` rerun
(1471/0, arch lint warnings 12→11), line counts (753/139, both ≤800),
allowlist entry absence confirmed. `transform.mlc` split from 1765 to 753
lines across 5 modules (`transform_coerce.mlc`/`transform_context.mlc`/
`transform_call_args.mlc`/`transform_method.mlc`/`transform_support.mlc`).
Queue head → §104-14 (`codegen/expr/match_gen.mlc` split, 1403 lines) Decision.

## §104-13 `codegen/decl_cpp.mlc` split (1666 lines)

Rough group survey (2026-07-28, by section boundary, subject to revision
per-slice as later slices get their own Decision): leaf declaration-list
helpers (~25 lines, scattered top/bottom of file); type/variant struct
codegen (~35-236, `gen_type_decl_fwd_cpp`/`gen_type_decl_body_cpp` and
helpers); trait decl codegen (~238-334, `gen_trait_decl_cpp`); fn
declaration codegen (~336-565, `gen_fn_proto_cpp`/`gen_fn_decl_cpp`/
`gen_decl_cpp`/`gen_proto_cpp`, large external-test surface); FFI/extern
codegen (~566-784, `gen_ffi_fn_proto_cpp`/`gen_ffi_fn_decl_cpp`/
`collect_ffi_include_lines`); extend/impl codegen (~785-1319, by far the
largest single group, `gen_decl_extend_cpp` and ~30 helpers); decl-segment
orchestration (~1320-1666, `decl_segment_cpp`/`collect_all_decl_parts_cpp`/
`collect_fn_defs_cpp`, calls into every other group — likely stays as the
file's own glue layer, not extracted). Slice order and count for groups
2-6 to be finalized as each slice gets its own Decision, mirroring
§104-12's incremental approach (5 slices ended up replacing an initial
3-group review estimate there).

### Slice 1 — `decl_cpp_helpers.mlc` (do first — zero external callers, zero circular dependency)

#### Decision (STEP=0) — **frozen** 2026-07-28

| Item | Choice |
|------|--------|
| Problem | `decl_cpp.mlc` is 1666 lines. 4 tiny declaration-list helpers — `empty_cpp_declaration` (26), `empty_cpp_declarations` (1536), `append_cpp_declarations` (1541), `cpp_decl_from_native_declarations` (1550) — are pure leaves (only depend on `CppDeclaration`/`CppDeclarationEmpty`/`CppDeclarationSequence` from `cpp_ir/cpp_ast`, each other, and nothing else) but are called pervasively throughout every other group in the file (confirmed by grep: `empty_cpp_declaration()` at 15 call sites, `empty_cpp_declarations()` at 60+, `append_cpp_declarations()` at 15, `cpp_decl_from_native_declarations()` at 4 — spanning type-decl, trait-decl, fn-decl, FFI, extend, and decl-segment groups alike). Repo-wide grep confirms **zero callers outside `decl_cpp.mlc`** for all 4 (the superficially similar `append_cpp_declarations_from_include_text` in `cpp_emit/module_tu_helpers.mlc` is a distinct, unrelated function — false-positive substring match, verified by exact-name grep). Extracting these first, before any of the 5 larger groups, mirrors §104-12 slice 2's `transform_context.mlc` role: a zero-risk prerequisite leaf that every later slice of this track will need to import from, avoiding re-deriving the same 4 helpers' new home 5 times over |
| Strategy (v1) | New `compiler/codegen/decl_cpp_helpers.mlc`. Move all 4 helpers verbatim (export all — needed cross-module by `decl_cpp.mlc` itself, and by whichever slice 2+ module ends up calling them). No signature change, no algorithm change — pure relocation, same as `transform_context.mlc`. `decl_cpp.mlc` imports all 4 back (needs them at ~90+ internal call sites across every group). Zero other files touched — no external importers exist today |
| Primary gate | Red: `decl_cpp_helpers.mlc` absent, all 4 helpers still in `decl_cpp.mlc` at lines 26/1536/1541/1550. Green: `decl_cpp_helpers.mlc` exists with the 4 items exported; `decl_cpp.mlc` shrinks by ~25 lines, gains 1 import line; bootstrap diff restricted to split modules + direct-caller namespace-prefix renames only (the refined god-file-split gate, since `decl_cpp.mlc`'s own ~90 internal call sites get namespace-qualified to the new module, this is expected and is the split's entire point — not a regression); `rake test_compiler_mlc` (1471+ passed, 0 failed); self-host mlcc2 diff before Critic close |
| Module touch | new `compiler/codegen/decl_cpp_helpers.mlc`; `compiler/codegen/decl_cpp.mlc` (shrinks, gains 1 import line) |
| REG | no (`compiler/**` only) |
| Out of scope | the 5 larger groups (type/trait/fn/FFI/extend codegen, decl-segment orchestration) — each needs its own Decision, deferred to later slices; any signature/algorithm change; MIR |

#### Steps (§104-13 — slice 1: decl_cpp_helpers)

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze | **done** |
| 1 | Red: confirm current boundaries (`decl_cpp_helpers.mlc` absent, 4 items at the documented lines, file at baseline 1666 lines) | **done** — `test -f` negative; all 4 helpers confirmed at lines 26/1536/1541/1550, no drift; `decl_cpp.mlc` confirmed at baseline 1666 lines |
| 2 | Green: create `decl_cpp_helpers.mlc`, wire `decl_cpp.mlc` import, bootstrap diff (split-scoped), `rake test_compiler_mlc`, mlcc2 self-host diff | **done** — see verification below |
| 3 | Critic: full re-audit | **done — closed** |

#### Green result (STEP=2, 2026-07-28)

`decl_cpp_helpers.mlc` created (30 lines): all 4 items moved wholesale + exported. `decl_cpp.mlc`: 1666 → 1641 lines (gained 1 import line, dropped the now-unused `CppDeclarationEmpty`/`CppDeclarationSequence` imports since only the new module constructs them directly). Bootstrap diff (`mlcc -o p0` pre-change, `mlcc -o p1` post-change, same pre-existing `mlcc` binary) scoped to exactly 4 files: `decl_cpp.cpp/.hpp` (shrink) + `decl_cpp_helpers.cpp/.hpp` (new) — zero other files touched (confirms the Decision's "zero external importers" claim). Unlike the `transform.mlc` slices, mlcc emits a plain `using namespace decl_cpp_helpers;` at the top of `decl_cpp.cpp` rather than qualifying each of the ~90 call sites — read the full non-`#line` diff anyway: every changed line is either that `using namespace`/`#include` pair, a removed function body, or (at exactly the call sites mlcc chose to qualify explicitly rather than rely on the `using namespace`) a `decl_cpp_helpers::` prefix insertion — zero logic changes. `rake test_compiler_mlc` (`TMPDIR` unset): exit_code=0, `1471 passed, 0 failed`, arch lint failures=0. mlcc2 self-host diff (`build_bin.sh`, in-repo `TMPDIR`, host disk 99%): IDENTICAL. All `.tmp/s104-13-slice1/**` build artifacts cleaned up after.

#### Critic close (STEP=3, 2026-07-28)

Independent function/type-set diff: old `decl_cpp.mlc` (`git show 9a7272df:...`, 119 names) vs new `decl_cpp.mlc` (115) + `decl_cpp_helpers.mlc` (4) combined = 119, zero lost/duplicated. Export-status diff: exactly the 4 moved items gained `export`, matching the Decision exactly. Fresh `mlcc` translation of `compiler/main.mlc` from scratch: `decl_cpp_helpers.cpp/.hpp` created; grepped `decl_cpp_helpers::` across every generated `.cpp`/`.hpp` — found only in `decl_cpp.cpp`/`.hpp` (the 1 direct caller), zero stray references. Independent `rake test_compiler_mlc` rerun from a clean shell (`TMPDIR` confirmed unset first): `1471 passed, 0 failed`, `arch lint failures=0`. Line counts confirmed: `decl_cpp.mlc` 1641, `decl_cpp_helpers.mlc` 30 — no drift. mlcc2 self-host g++ diff not re-run a third time (witnessed directly during Driver STEP=2 in the same continuous session, no source change since). No false-done found. **§104-13 slice 1 closed.** §104-13 itself stays **open** — 5 more groups surveyed (type/trait/fn decl codegen, FFI/extern codegen, extend/impl codegen, decl-segment orchestration), each needs its own Decision. Queue head → §104-13 slice 2 Decision.

### Slice 2 — `decl_cpp_type.mlc` (type/variant struct codegen — zero circular dependency)

#### Decision (STEP=0) — **frozen** 2026-07-28

| Item | Choice |
|------|--------|
| Problem | Re-derived the type/variant-struct group against the current file (post slice-1 shift, all confirmed by grep, 2026-07-28): 13 items at lines 58-234 — `variant_forward_struct_cpp` (58), `variant_alias_arm_cpp` (69), `append_type_forward_declarations` (81), `gen_type_decl_fwd_cpp` (108), `record_field_cpp` (120), `record_fields_cpp` (126), `tuple_fields_cpp` (129), `variant_body_struct_cpp` (142), `cpp_using_from_struct_using_entry` (160), `struct_using_declarations_cpp` (163), `single_variant_body_struct_cpp` (174), `append_type_body_struct_declarations` (192), `gen_type_decl_body_cpp` (219). Repo-wide + in-file grep confirms all dependencies are either already-imported external symbols (`variant_ctor_name`/`variant_used_type_parameters`/`type_phantom_params_for_variants`/`union_string_lists`/`template_prefix`/`type_to_cpp` from `./decl/type_gen`, `cpp_safe` from `./cpp_naming`, `gen_derive_methods_cpp` from `./decl/derive_methods_cpp`) or `append_cpp_declarations` (already relocated to `decl_cpp_helpers.mlc` in slice 1) — **zero dependency on anything else in `decl_cpp.mlc`**, unlike slice 1's leaf helpers this group needs no injection at all. The 11 internal-only helpers each have exactly 1 caller, entirely within the group itself (confirmed by grep, one call site each). Only the 2 exported entry points (`gen_type_decl_fwd_cpp`, `gen_type_decl_body_cpp`) are called from elsewhere in `decl_cpp.mlc`: inside `gen_decl_cpp`'s `SemanticDeclarationType` branch (515-516, fn-decl group) and inside `decl_segment_type_cpp` (1326-1327, decl-segment group) — both are plain downstream calls, no back-reference from this group into either. Also has 1 external test importer (`test_decl_gen.mlc`), unaffected per the transitive-import-forwarding precedent established in §104-12 slice 2 (the MLC source import path `from '../codegen/decl_cpp'` still resolves correctly without editing the test file, since the checker resolves to the true defining module) |
| Strategy (v1) | New `compiler/codegen/decl_cpp_type.mlc`. Move all 13 items wholesale (export the 2 that need cross-module visibility from `decl_cpp.mlc`; keep the 11 internal helpers non-exported, matching §104-12 slice 3's precedent of only exporting what's actually needed cross-module — a stricter approach than slice 1's decl_cpp_helpers, since here the internal/external split is clean and worth preserving). No signature change, no algorithm change — pure relocation. `decl_cpp.mlc` imports `gen_type_decl_fwd_cpp`/`gen_type_decl_body_cpp` back (2 call sites each, no injection needed — pure downstream calls, not the call_args/method-style value-level cycle §104-12 slices 3/4 had to solve) |
| Primary gate | Red: `decl_cpp_type.mlc` absent, all 13 items still in `decl_cpp.mlc` at the documented lines. Green: `decl_cpp_type.mlc` exists; `decl_cpp.mlc` shrinks by ~175 lines, gains 1 import line; bootstrap diff restricted to split modules only (zero direct-caller namespace-prefix renames expected this slice, since both exported functions have exactly 1 external-to-the-group caller and that caller is inside `decl_cpp.mlc` itself, which gets the split-scoped rename automatically); `rake test_compiler_mlc` (1471+ passed, 0 failed); self-host mlcc2 diff before Critic close |
| Module touch | new `compiler/codegen/decl_cpp_type.mlc`; `compiler/codegen/decl_cpp.mlc` (shrinks, gains 1 import line) |
| REG | no (`compiler/**` only) |
| Out of scope | the 4 remaining groups (trait decl codegen, fn decl codegen, FFI/extern codegen, extend/impl codegen — largest, decl-segment orchestration); the misplaced `function_parameter_proto_items`/`function_parameter_def_items`/`main_program_parameter_items_cpp` trio at lines 33-55 (logically part of fn-decl codegen despite their early position, deferred to that slice); any signature/algorithm change; MIR |

#### Steps (§104-13 — slice 2: decl_cpp_type)

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze | **done** |
| 1 | Red: confirm current boundaries (`decl_cpp_type.mlc` absent, 13 items at the documented lines, file at baseline 1641 lines) | **done** — `test -f` negative; all 13 items confirmed at lines 58/69/81/108/120/126/129/142/160/163/174/192/219, no drift; `decl_cpp.mlc` confirmed at baseline 1641 lines |
| 2 | Green: create `decl_cpp_type.mlc`, wire `decl_cpp.mlc` import, bootstrap diff (split-scoped), `rake test_compiler_mlc`, mlcc2 self-host diff | **done** |
| 3 | Critic: full re-audit | **done — closed** |

**Green (2026-07-28):** `decl_cpp_type.mlc` created (190 lines): 13 items moved wholesale + 2 exported (`gen_type_decl_fwd_cpp`, `gen_type_decl_body_cpp`). `decl_cpp.mlc`: 1641 → 1463 lines (gained 1 import line, dropped now-unused `variant_ctor_name`/`union_string_lists`/`type_phantom_params_for_variants`/`gen_derive_methods_cpp` imports and `VarUnit`/`VarTuple`/`VarRecord`/`FieldDef`/`TypeExpr`/`CppStruct`/`CppVariant`/`CppVariantArm`/`CppField`/`StructUsingEntry`). Bootstrap diff (`mlcc -o p0` pre-change, `mlcc -o p1` post-change): scoped to exactly `decl_cpp.cpp/.hpp` (shrink) + `decl_cpp_type.cpp/.hpp` (new) — zero other files touched. Non-`#line` diff of `decl_cpp.cpp`/`.hpp`: every changed line is either the `#include`/`using namespace` swap (`derive_methods_cpp` → `decl_cpp_type`, since the only remaining direct call into `derive_methods_cpp` moved with the group), a removed function body, or a `decl_cpp_type::` prefix insertion at exactly the 4 documented call sites — zero logic changes. `rake test_compiler_mlc` (`TMPDIR` unset): exit_code=0, phase 10/10 arch lint `failures=0`. mlcc2 self-host diff (`build_bin.sh`, `MLC_CXX=g++`, in-repo `TMPDIR`): built `mlcc2` from `p1`, ran on `compiler/main.mlc` → `p2`, `diff -r p1 p2 --exclude=obj` IDENTICAL. `compiler/build.sh` confirmed `compiler/out/mlcc` up to date. All `.tmp/s104-13-slice2/**` build artifacts cleaned up after.

#### Critic close (STEP=3, 2026-07-28)

Independent function/type-set diff: old `decl_cpp.mlc` (`git show b72febeb~1:...`, pre-slice-2 baseline, 115 names) vs new `decl_cpp.mlc` (post-slice-2) + `decl_cpp_type.mlc` combined (115) — `diff` empty, zero lost/duplicated. Export-status diff: identical export sets before and after (`gen_type_decl_fwd_cpp`/`gen_type_decl_body_cpp` were already exported pre-slice and remain the only 2 exported items in `decl_cpp_type.mlc`; the 11 internal helpers stay non-exported), matching the Decision exactly. Fresh `mlcc -o ... compiler/main.mlc` translation from scratch: `decl_cpp_type.cpp/.hpp` created; grepped `decl_cpp_type::` across every generated `.cpp`/`.hpp` — found only in `decl_cpp.cpp` (the 1 direct caller), zero stray references elsewhere. Independent full `rake test_compiler_mlc` rerun from a clean shell (`TMPDIR` confirmed unset first): `1471 passed, 0 failed`, `arch lint failures=0`. Line counts confirmed: `decl_cpp.mlc` 1463, `decl_cpp_type.mlc` 190 — no drift. mlcc2 self-host g++ diff not re-run a third time (witnessed directly during Driver STEP=2 in the same continuous session, no source change since). No false-done found. **§104-13 slice 2 closed.** §104-13 itself stays **open** — 4 more groups surveyed (trait decl codegen, fn decl codegen, FFI/extern codegen, extend/impl codegen — largest, decl-segment orchestration), each needs its own Decision. Queue head → §104-13 slice 3 Decision.

### Slice 3 — `decl_cpp_trait.mlc` (trait decl codegen — needs `is_semantic_declaration_fn` relocated to `decl_cpp_helpers.mlc` first)

#### Decision (STEP=0) — **frozen** 2026-07-28

| Item | Choice |
|------|--------|
| Problem | Re-derived the trait-decl group against the current file (post slice-2 shift, all confirmed by grep, 2026-07-28): 8 items at lines 58-154 — `trait_template_header` (58), `trait_requires_expression_for_method` (65), `trait_requires_expressions_cpp` (79), `trait_concept_dispatch_forward_proto_cpp` (95), `trait_skips_concept_dispatch_forward_protos` (106), `trait_concept_dispatch_forward_protos_body` (109), `trait_concept_dispatch_forward_protos_cpp` (135), `gen_trait_decl_cpp` (143, export). 6 of the 8 have exactly 1 caller, entirely within the group. The 2 exported-or-needs-export entry points are called from elsewhere in `decl_cpp.mlc`: `gen_trait_decl_cpp` from `gen_decl_cpp`'s `SemanticDeclarationTrait` branch (341, fn-decl group) and `decl_segment_trait_phase_cpp` (1164, decl-segment orchestration); `trait_concept_dispatch_forward_protos_cpp` (currently non-exported) only from `decl_segment_trait_phase_cpp` (1163) — both plain downstream calls, no back-reference. Unlike slice 2, one group member (`trait_concept_dispatch_forward_protos_body`, line 118) calls `is_semantic_declaration_fn`, a 4-line pure predicate defined **later** in `decl_cpp.mlc` (386) and also called from 3 *other* groups (370 extend-proto, 682, 813 — confirmed by grep, zero external-to-`decl_cpp.mlc` callers repo-wide). Moving the trait group wholesale while leaving `is_semantic_declaration_fn` in `decl_cpp.mlc` would force either a duplicate copy or a genuine two-way import cycle (`decl_cpp_trait.mlc` imports it from `decl_cpp.mlc`, `decl_cpp.mlc` imports `gen_trait_decl_cpp` back) — the same class of problem §104-12 slice 2 solved by extracting the shared dependency to a third leaf. Here the fix is smaller: `is_semantic_declaration_fn` has zero dependency on anything in `decl_cpp.mlc` beyond the already-imported `SemanticDeclaration`/`SemanticDeclarationFn` types, so it belongs in the existing `decl_cpp_helpers.mlc` leaf (slice 1's role) rather than a new module |
| Strategy (v1) | Step 1: move `is_semantic_declaration_fn` (4 lines) into `decl_cpp_helpers.mlc`, export it; `decl_cpp.mlc` imports it back alongside the 4 existing helpers (still used at its own 3 other call sites: 370, 682, 813). Step 2: new `compiler/codegen/decl_cpp_trait.mlc`, move the 8 trait-decl items wholesale, export only the 2 that need cross-module visibility (`gen_trait_decl_cpp`, `trait_concept_dispatch_forward_protos_cpp`) — the 6 internal helpers stay non-exported, matching slice 2's precedent. All other dependencies (`sem_type_to_cpp`/`concept_requires_expression_method_returns_convertible` from `./decl/type_gen`, `cpp_safe` from `./cpp_naming`, `trait_dispatch_name`/`extract_method_name`/`concept_declval_arguments_for_params` from `./decl/decl_extend`, `param_name` from `../frontend/ast`, `trait_has_associated_types`/`CodegenContext` from `./context`, `CppFnProto`/`CppConceptRequires`/`CppDeclaration` from `../cpp_ir/cpp_ast`, `SemanticDeclaration`/`SemanticDeclarationFn` from `../ir/semantic_ir`) are already-imported external symbols, re-imported directly from their true source modules — no dependency on `decl_cpp.mlc` itself once Step 1 lands. `decl_cpp.mlc` imports the 2 exported functions back (no injection needed — pure downstream calls) |
| Primary gate | Red: `decl_cpp_trait.mlc` absent, `is_semantic_declaration_fn` still at line 386 in `decl_cpp.mlc`, all 8 trait items still at the documented lines. Green: `decl_cpp_helpers.mlc` gains `is_semantic_declaration_fn` (exported); `decl_cpp_trait.mlc` exists with 8 items, 2 exported; `decl_cpp.mlc` shrinks by ~100 lines total, gains 2 import lines; bootstrap diff restricted to split modules + `decl_cpp_helpers.cpp/.hpp` (grows by 1 function) only; `rake test_compiler_mlc` (1471+ passed, 0 failed); self-host mlcc2 diff before Critic close |
| Module touch | new `compiler/codegen/decl_cpp_trait.mlc`; `compiler/codegen/decl_cpp_helpers.mlc` (gains 1 function); `compiler/codegen/decl_cpp.mlc` (shrinks, gains 2 import lines) |
| REG | no (`compiler/**` only) |
| Out of scope | the 3 remaining groups (fn decl codegen, FFI/extern codegen, extend/impl codegen — largest, decl-segment orchestration); the misplaced `function_parameter_proto_items`/`function_parameter_def_items`/`main_program_parameter_items_cpp` trio at lines 33-56 (logically fn-decl codegen, deferred to that slice); any signature/algorithm change; MIR |

#### Steps (§104-13 — slice 3: decl_cpp_trait)

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze | **done** |
| 1 | Red: confirm current boundaries (`decl_cpp_trait.mlc` absent, `is_semantic_declaration_fn` at line 386, 8 trait items at the documented lines, file at baseline 1463 lines) | **done** — `test -f` negative; `is_semantic_declaration_fn` confirmed at line 386; all 8 items confirmed at lines 58/65/79/95/106/109/135/143, no drift; `decl_cpp.mlc` confirmed at baseline 1463 lines |
| 2 | Green: relocate `is_semantic_declaration_fn` to `decl_cpp_helpers.mlc`, create `decl_cpp_trait.mlc`, wire `decl_cpp.mlc` imports, bootstrap diff (split-scoped), `rake test_compiler_mlc`, mlcc2 self-host diff | **done** |
| 3 | Critic: full re-audit | **done — closed** |

**Green (2026-07-28):** `is_semantic_declaration_fn` relocated to `decl_cpp_helpers.mlc` (exported, 37 lines total). `decl_cpp_trait.mlc` created (111 lines): 8 items moved wholesale + 2 exported (`gen_trait_decl_cpp`, `trait_concept_dispatch_forward_protos_cpp`). `decl_cpp.mlc`: 1463 → 1360 lines (gained 1 import line, dropped now-unused `trait_dispatch_name`/`concept_declval_arguments_for_params`/`concept_requires_expression_method_returns_convertible`/`param_name`/`CppConceptRequires` imports). Bootstrap diff (`mlcc -o p0` pre-change, `mlcc -o p1` post-change): scoped to exactly `decl_cpp.cpp/.hpp` (shrink) + `decl_cpp_helpers.cpp/.hpp` (grows by 1 function, additive only) + `decl_cpp_trait.cpp/.hpp` (new) — zero other files touched. Non-`#line` diff: every changed line is either an `#include`/`using namespace` addition, a removed function body, a `decl_cpp_trait::` prefix at the 2 documented external call sites, or a `decl_cpp_helpers::` prefix at the 3 other `is_semantic_declaration_fn` call sites that remain in `decl_cpp.mlc` (extend-proto group, FFI group) — zero logic changes. `rake test_compiler_mlc` (`TMPDIR` unset): exit_code=0, phase 10/10 arch lint `failures=0`. mlcc2 self-host diff (`build_bin.sh`, `MLC_CXX=g++`, in-repo `TMPDIR`): built `mlcc2` from `p1`, ran on `compiler/main.mlc` → `p2`, `diff -r p1 p2 --exclude=obj` IDENTICAL. `compiler/build.sh` confirmed `compiler/out/mlcc` up to date. All `.tmp/s104-13-slice3/**` build artifacts cleaned up after.

#### Critic close (STEP=3, 2026-07-28)

Independent function/type-set diff: old `decl_cpp.mlc` + old `decl_cpp_helpers.mlc` (`git show 37b056a9~1:...`, pre-slice-3 baseline, 106 names combined) vs new `decl_cpp.mlc` + `decl_cpp_helpers.mlc` + `decl_cpp_trait.mlc` combined (106) — `diff` empty, zero lost/duplicated. Export-status diff: exactly the 2 documented items gained `export` — `is_semantic_declaration_fn` (now cross-module from `decl_cpp_helpers.mlc`) and `trait_concept_dispatch_forward_protos_cpp` (now cross-module from `decl_cpp_trait.mlc`); `gen_trait_decl_cpp` was already exported pre-slice and remains so — matching the Decision exactly. Fresh `mlcc -o ... compiler/main.mlc` translation from scratch: `decl_cpp_trait.cpp/.hpp` created; grepped `decl_cpp_trait::` across every generated `.cpp`/`.hpp` — found only in `decl_cpp.cpp` (the 1 direct caller); grepped for `is_semantic_declaration_fn` definitions — exactly 1 (`decl_cpp_helpers.cpp`), zero duplicates; its callers confirmed only in `decl_cpp_trait.cpp` and `decl_cpp.cpp/.hpp`. Independent full `rake test_compiler_mlc` rerun from a clean shell (`TMPDIR` confirmed unset first): `1471 passed, 0 failed`, `arch lint failures=0`. Line counts confirmed: `decl_cpp.mlc` 1360, `decl_cpp_helpers.mlc` 37, `decl_cpp_trait.mlc` 111 — no drift. mlcc2 self-host g++ diff not re-run a third time (witnessed directly during Driver STEP=2 in the same continuous session, no source change since). No false-done found. **§104-13 slice 3 closed.** §104-13 itself stays **open** — 3 more groups surveyed (fn decl codegen, FFI/extern codegen, extend/impl codegen — largest, decl-segment orchestration), each needs its own Decision. Queue head → §104-13 slice 4 Decision.

### Slice 4 — `decl_cpp_fn.mlc` (fn decl codegen — zero circular dependency, more exports than slices 2/3)

#### Decision (STEP=0) — **frozen** 2026-07-28

| Item | Choice |
|------|--------|
| Problem | Re-derived the fn-decl group against the current file (post slice-3 shift, all confirmed by grep, 2026-07-28): 18 items at lines 34-221 — `function_parameter_proto_items` (34), `function_parameter_def_items` (45), `main_program_parameter_items_cpp` (56, export), `main_set_args_vector_argument_cpp` (59), `main_set_args_preamble_cpp` (72, export), `prepend_main_set_args_preamble` (78), `function_declaration_template_prefix` (88, **dead code** — zero callers repo-wide, confirmed by grep; moved as-is per the `transform_context_new` precedent from §104-12 slice 2, dropping dead code is an algorithm change out of scope for a pure-relocation slice), `merged_function_type_parameters_cpp` (91), `function_emits_template_cpp` (94), `context_with_fn_escape_cpp` (97), `native_fn_proto_cpp` (100), `native_fn_proto_cpp_with_escape` (110), `native_fn_decl_cpp` (132), `native_fn_decl_cpp_with_escape` (144), `gen_fn_proto_cpp` (179, export), `gen_fn_decl_cpp` (189, export), `gen_fn_proto_cpp_with_escape` (200, export), `gen_fn_decl_cpp_with_escape` (211, export). Unlike slices 2/3, this group is a shared dependency of **all 3** remaining groups — FFI codegen (444/459/466/486 call into `gen_fn_proto_cpp_with_escape`/`context_with_fn_escape_cpp`/`function_parameter_def_items`/`merged_function_type_parameters_cpp`), extend/impl codegen (763/858 call into `gen_fn_proto_cpp`/`native_fn_decl_cpp`), and decl-segment orchestration (1211/1260/1321/1324 call into `function_emits_template_cpp`/`gen_fn_decl_cpp_with_escape`) — confirmed one-directional by grep: zero calls from any fn-decl-group function into anything defined in the FFI/extend/decl-segment groups. This means more items need `export` than slices 2/3 (11 of 18, vs 2 and 2), but the dependency direction is still clean: fn-decl is a leaf every later group depends on, mirroring `decl_cpp_helpers.mlc`'s own role one level up |
| Strategy (v1) | New `compiler/codegen/decl_cpp_fn.mlc`. Move all 18 items wholesale. Export the 11 that need cross-module visibility: the 6 already-exported (`main_program_parameter_items_cpp`, `main_set_args_preamble_cpp`, `gen_fn_proto_cpp`, `gen_fn_decl_cpp`, `gen_fn_proto_cpp_with_escape`, `gen_fn_decl_cpp_with_escape` — each also has an external test importer, `test_decl_gen.mlc`, unaffected per the transitive-import-forwarding precedent) plus 5 newly needed (`function_parameter_def_items`, `context_with_fn_escape_cpp`, `merged_function_type_parameters_cpp`, `function_emits_template_cpp`, `native_fn_decl_cpp` — zero external-to-`decl_cpp.mlc` callers repo-wide, confirmed by grep, so only cross-module-within-`decl_cpp.mlc` visibility is needed). The remaining 7 stay non-exported (`function_parameter_proto_items`, `main_set_args_vector_argument_cpp`, `prepend_main_set_args_preamble`, `function_declaration_template_prefix`, `native_fn_proto_cpp`, `native_fn_proto_cpp_with_escape`, `native_fn_decl_cpp_with_escape`). All dependencies (`Param` from `../frontend/ast`; `SemanticExpression`/`FnEscapeInfo`/`empty_fn_escape_info` from `../ir/semantic_ir`; `Type` from `../checker/registry`; `CppDeclaration`/`CppFnProto`/`CppFnDef`/`CppExpression`/`CppStatement`/`CppCall`/`CppBinary` from `../cpp_ir/cpp_ast`; `emit_helpers` from `../cpp_emit/emit_helpers`; `CodegenContext` from `./context`; `compute_fn_body_context`/`prototype_context_for_function` from `./decl/decl`; `gen_parameter_proto_items`/`gen_parameter_def_items` from `./decl/decl_extend`; `sem_type_to_cpp`/`template_prefix`/`requires_clause` from `./decl/type_gen`; `gen_return_body_cpp` from `./stmt/return_body`) are already-imported external symbols, re-imported directly from their true source modules — zero dependency on `decl_cpp.mlc` itself. `decl_cpp.mlc` imports the 11 exported items back (no injection needed — pure downstream calls, no back-reference); drops `CppStatement`/`CppBinary`/`prototype_context_for_function`/`gen_return_body_cpp` from its own imports (confirmed zero remaining usage outside the moved group, by grep) |
| Primary gate | Red: `decl_cpp_fn.mlc` absent, all 18 items still at the documented lines. Green: `decl_cpp_fn.mlc` exists with 11 of 18 exported; `decl_cpp.mlc` shrinks by ~190 lines, gains 1 import line, loses 4 now-unused imports; bootstrap diff restricted to split modules + direct-caller namespace-prefix renames in `decl_cpp.cpp/.hpp` only (11 call sites across FFI/extend/decl-segment groups get `decl_cpp_fn::` prefixed — expected, is the split's entire point); `rake test_compiler_mlc` (1471+ passed, 0 failed); self-host mlcc2 diff before Critic close |
| Module touch | new `compiler/codegen/decl_cpp_fn.mlc`; `compiler/codegen/decl_cpp.mlc` (shrinks, gains 1 import line, drops 4 unused imports) |
| REG | no (`compiler/**` only) |
| Out of scope | the 2 remaining groups (FFI/extern codegen, extend/impl codegen — largest, decl-segment orchestration); dropping the dead `function_declaration_template_prefix` (pure relocation only, per non-goals); any signature/algorithm change; MIR |

#### Steps (§104-13 — slice 4: decl_cpp_fn)

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze | **done** |
| 1 | Red: confirm current boundaries (`decl_cpp_fn.mlc` absent, 18 items at the documented lines, file at baseline 1360 lines) | **done** — `test -f` negative; all 18 items confirmed at lines 34/45/56/59/72/78/88/91/94/97/100/110/132/144/179/189/200/211, no drift; `decl_cpp.mlc` confirmed at baseline 1360 lines |
| 2 | Green: create `decl_cpp_fn.mlc`, wire `decl_cpp.mlc` imports, bootstrap diff (split-scoped), `rake test_compiler_mlc`, mlcc2 self-host diff | **done** — `decl_cpp_fn.mlc` created (204 lines, 18 items, 11 exported per Decision); `decl_cpp.mlc` reduced 1360→1171 lines, imports wired via `./decl_cpp_fn`, unused imports dropped (`CppStatement`, `CppBinary`, `prototype_context_for_function`, `gen_return_body_cpp`); controlled bootstrap diff (same mlcc binary, only `decl_cpp.mlc` source toggled) scoped exactly to `decl_cpp.cpp/.hpp` (namespace/line-number only, function bodies byte-identical) + new `decl_cpp_fn.cpp/.hpp`, no other module touched; `rake test_compiler_mlc` 1471 passed 0 failed, arch lint failures=0; mlcc2 built via `build_bin.sh`, `mlcc2 -o out2 compiler/main.mlc` output IDENTICAL to mlcc's |
| 3 | Critic: full re-audit | **done — closed** |

**Green (2026-07-28):** `decl_cpp_fn.mlc` created (204 lines): 18 items moved wholesale, 11 exported (`function_parameter_def_items`, `main_program_parameter_items_cpp`, `main_set_args_preamble_cpp`, `merged_function_type_parameters_cpp`, `function_emits_template_cpp`, `context_with_fn_escape_cpp`, `native_fn_decl_cpp`, `gen_fn_proto_cpp`, `gen_fn_decl_cpp`, `gen_fn_proto_cpp_with_escape`, `gen_fn_decl_cpp_with_escape`); dead code `function_declaration_template_prefix` moved as-is (matches §104-12 slice-2 precedent). `decl_cpp.mlc`: 1360 → 1171 lines (gained 1 import line, dropped now-unused `CppStatement`/`CppBinary`/`prototype_context_for_function`/`gen_return_body_cpp` imports). Controlled bootstrap diff (same `mlcc` binary held fixed, only `decl_cpp.mlc` source toggled between committed and new — `decl_cpp_fn.mlc` present in both runs to isolate the variable): scoped to exactly `decl_cpp.cpp/.hpp` (statement bodies byte-identical, only `#include`/`using namespace`/`#line` annotations differ) + new `decl_cpp_fn.cpp/.hpp` — zero other module touched. `rake test_compiler_mlc` (`TMPDIR` in-repo): 1471 passed, 0 failed, arch lint `failures=0`. mlcc2 self-host diff (`build_bin.sh`, in-repo `TMPDIR`): built `mlcc2` from the generated output, ran on `compiler/main.mlc` → `out2`, `diff -rq out out2 --exclude=obj` IDENTICAL. All `.tmp/s104-13-slice4/**` build artifacts cleaned up after.

#### Critic close (STEP=3, 2026-07-28)

Independent function/type-set diff: old `decl_cpp.mlc` (`git show 6667ea37:...`, pre-slice-4 baseline, 92 names) vs new `decl_cpp.mlc` + `decl_cpp_fn.mlc` combined (92) — `diff` empty, zero lost/duplicated. Export-status diff: exactly the 5 documented items gained `export` (`context_with_fn_escape_cpp`, `function_emits_template_cpp`, `function_parameter_def_items`, `merged_function_type_parameters_cpp`, `native_fn_decl_cpp`), zero lost — matching the Decision exactly (23 exported pre-slice → 28 post-slice). Fresh `mlcc -o ... compiler/main.mlc` translation from scratch: `decl_cpp_fn.cpp/.hpp` created; grepped `decl_cpp_fn::` across every generated `.cpp`/`.hpp` — found only in `decl_cpp.cpp` (the 1 direct caller), zero stray references elsewhere. Independent full `rake test_compiler_mlc` rerun from a clean shell (`TMPDIR` confirmed unset first): exit_code=0, arch lint `failures=0` (13 pre-existing allowlisted warnings unchanged). Line counts confirmed: `decl_cpp.mlc` 1171, `decl_cpp_fn.mlc` 204 — no drift. mlcc2 self-host g++ diff not re-run a third time (witnessed directly during Driver STEP=2 in the same continuous session, no source change since). No false-done found. **§104-13 slice 4 closed.** §104-13 itself stays **open** — 2 more groups surveyed (FFI/extern codegen, extend/impl codegen — largest, decl-segment orchestration), each needs its own Decision. Queue head → §104-13 slice 5 Decision.

### Slice 5 — `decl_cpp_ffi.mlc` (FFI/extern codegen — zero circular dependency, widely shared leaf)

#### Decision (STEP=0) — **frozen** 2026-07-28

| Item | Choice |
|------|--------|
| Problem | Re-derived the FFI/extern codegen group against the current 1171-line file (post slice-4 shift, all confirmed by grep, 2026-07-28): 15 items at lines 100-311 — `semantic_expression_is_extern` (100), `semantic_extern_ffi_c_name` (106), `semantic_extern_ffi_header` (112), `semantic_expression_is_ffi_extern` (118), `semantic_expression_is_bare_extern` (121), `ffi_header_include_line` (124), `collect_ffi_include_lines_from_declaration` (136), `gen_extern_type_decl_cpp` (153), `collect_ffi_include_lines` (210, export — already used externally by `codegen/module.mlc`), `ffi_parameter_type_items` (220), `ffi_parameter_name_items` (230), `ffi_extern_reuses_imported_binding` (243), `gen_ffi_fn_proto_cpp` (246), `gen_ffi_fn_decl_cpp` (258), `semantic_fn_body_is_extern` (307). Like slice 4, this is a widely shared leaf: 7 of the 15 are called from the remaining extend/decl-segment code still in `decl_cpp.mlc` — `semantic_expression_is_bare_extern` (44/70/758/882/1021/1072/1131), `semantic_expression_is_ffi_extern` (45/71/1133), `gen_ffi_fn_decl_cpp` (46/1134), `gen_ffi_fn_proto_cpp` (72), `gen_extern_type_decl_cpp` (62/929/961), `semantic_fn_body_is_extern` (85/322/352/792), `semantic_expression_is_extern` (573/665/891) — confirmed one-directional by grep: zero calls from any FFI-group function into anything defined in the extend/decl-segment groups, and zero usage of any of the 15 names in the 4 already-extracted modules (`decl_cpp_helpers.mlc`, `decl_cpp_type.mlc`, `decl_cpp_trait.mlc`, `decl_cpp_fn.mlc`, confirmed by grep). The other 8 (`semantic_extern_ffi_c_name`, `semantic_extern_ffi_header`, `ffi_header_include_line`, `collect_ffi_include_lines_from_declaration`, `ffi_parameter_type_items`, `ffi_parameter_name_items`, `ffi_extern_reuses_imported_binding`, plus `collect_ffi_include_lines`'s own internal helper role) have callers entirely within the group |
| Strategy (v1) | New `compiler/codegen/decl_cpp_ffi.mlc`. Move all 15 items wholesale. Export the 8 that need cross-module visibility: `collect_ffi_include_lines` (already exported, external caller `codegen/module.mlc` — unaffected per the transitive-import-forwarding precedent) plus 7 newly needed (`semantic_expression_is_bare_extern`, `semantic_expression_is_ffi_extern`, `gen_ffi_fn_decl_cpp`, `gen_ffi_fn_proto_cpp`, `gen_extern_type_decl_cpp`, `semantic_fn_body_is_extern`, `semantic_expression_is_extern` — zero external-to-`decl_cpp.mlc` callers repo-wide, confirmed by grep, so only cross-module-within-`decl_cpp.mlc` visibility is needed). The remaining 7 stay non-exported (`semantic_extern_ffi_c_name`, `semantic_extern_ffi_header`, `ffi_header_include_line`, `collect_ffi_include_lines_from_declaration`, `ffi_parameter_type_items`, `ffi_parameter_name_items`, `ffi_extern_reuses_imported_binding`). All dependencies (`Param` from `../frontend/ast`; `SemanticDeclaration`/`SemanticExpression`/`SemanticDeclarationFn`/`SemanticDeclarationExternType`/`SemanticDeclarationExported`/`SemanticExpressionExtern`/`sdecl_inner`/`FnEscapeInfo` from `../ir/semantic_ir`; `Type` from `../checker/registry`; `CppDeclaration`/`CppExpression`/`CppCall`/`CppCast`/`CppCastStatic`/`CppTypeName`/`CppTypePtr`/`CppUnary`/`CppUsing`/`CppClassDeclaration`/`CppClassDefinition`/`CppClassMemberFunctionDef`/`CppFunctionPrototype`/`CppParam`/`CppFnModifiers`/`CppIdent`/`CppFnDef` from `../cpp_ir/cpp_ast`; `emit_helpers` from `../cpp_emit/emit_helpers`; `CodegenContext` from `./context`; `compute_fn_body_context` from `./decl/decl`; `sem_type_to_cpp`/`type_to_cpp`/`template_prefix`/`requires_clause` from `./decl/type_gen`; `cpp_safe` from `./cpp_naming`; `empty_cpp_declaration`/`cpp_decl_from_native_declarations` from `./decl_cpp_helpers`; `function_parameter_def_items`/`context_with_fn_escape_cpp`/`merged_function_type_parameters_cpp` from `./decl_cpp_fn`) are already-imported external symbols, re-imported directly from their true source modules — zero dependency on `decl_cpp.mlc` itself. `decl_cpp.mlc` imports the 7 newly-exported items back (no injection needed — pure downstream calls, no back-reference); `collect_ffi_include_lines` stays importable transitively for `module.mlc` without `decl_cpp.mlc` re-exporting it (matches the precedent already proven for slices 2-4) |
| Primary gate | Red: `decl_cpp_ffi.mlc` absent, all 15 items still at the documented lines. Green: `decl_cpp_ffi.mlc` exists with 8 of 15 exported; `decl_cpp.mlc` shrinks by ~210 lines, gains 1 import line; bootstrap diff restricted to split modules + direct-caller namespace-prefix renames in `decl_cpp.cpp/.hpp` only (7 call sites across the extend/decl-segment groups get `decl_cpp_ffi::` prefixed — expected, is the split's entire point); `rake test_compiler_mlc` (1471+ passed, 0 failed); self-host mlcc2 diff before Critic close |
| Module touch | new `compiler/codegen/decl_cpp_ffi.mlc`; `compiler/codegen/decl_cpp.mlc` (shrinks, gains 1 import line) |
| REG | no (`compiler/**` only) |
| Out of scope | the 1 remaining group (extend/impl codegen + decl-segment orchestration — largest, likely needs its own multi-slice sub-sequencing given its size ~860 lines); any signature/algorithm change; MIR |

#### Steps (§104-13 — slice 5: decl_cpp_ffi)

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze | **done** |
| 1 | Red: confirm current boundaries (`decl_cpp_ffi.mlc` absent, 15 items at the documented lines, file at baseline 1171 lines) | **done** — `test -f` negative; all 15 items confirmed at lines 100/106/112/118/121/124/136/153/210/220/230/243/246/258/307, no drift; `decl_cpp.mlc` confirmed at baseline 1171 lines |
| 2 | Green: create `decl_cpp_ffi.mlc`, wire `decl_cpp.mlc` imports, bootstrap diff (split-scoped), `rake test_compiler_mlc`, mlcc2 self-host diff | **done** — `decl_cpp_ffi.mlc` created (229 lines, 15 items, 8 exported per Decision, plus 1 extra dependency on `gen_fn_proto_cpp_with_escape` from `./decl_cpp_fn` not spelled out in the Decision text but present in the moved code); `decl_cpp.mlc` reduced 1171→959 lines, imports wired via `./decl_cpp_ffi`, unused imports dropped (`SemanticExpressionExtern`, `CppCast`, `CppCastStatic`, `CppTypeName`, `CppTypePtr`, `CppUnary`, `CppUsing`, `CppClassDeclaration`, `CppClassDefinition`, `CppClassMemberFunctionDef`, `CppFunctionPrototype`, `CppParam`, `CppFnModifiers`, `CppIdent`, `type_to_cpp`, `compute_fn_body_context`); controlled bootstrap diff (same mlcc binary, only `decl_cpp.mlc` source toggled) scoped exactly to `decl_cpp.cpp/.hpp` (namespace/line-number only, statement content byte-identical) + new `decl_cpp_ffi.cpp/.hpp` + `module.cpp` (1 direct-caller rename, `decl_cpp::collect_ffi_include_lines` → `decl_cpp_ffi::collect_ffi_include_lines`), no other module touched; `rake test_compiler_mlc` exit_code=0, arch lint failures=0 (`decl_cpp.mlc` now 959 lines, still allowlisted); mlcc2 built via `build_bin.sh`, `mlcc2 -o out2 compiler/main.mlc` output IDENTICAL to mlcc's |
| 3 | Critic: full re-audit | **done — closed** |

**Green (2026-07-28):** `decl_cpp_ffi.mlc` created (229 lines): 15 items moved wholesale, 8 exported (`semantic_expression_is_extern`, `semantic_expression_is_ffi_extern`, `semantic_expression_is_bare_extern`, `gen_extern_type_decl_cpp`, `collect_ffi_include_lines`, `gen_ffi_fn_proto_cpp`, `gen_ffi_fn_decl_cpp`, `semantic_fn_body_is_extern`); one Decision-text omission caught during implementation — `gen_ffi_fn_proto_cpp` also needs `gen_fn_proto_cpp_with_escape` from `./decl_cpp_fn` (already imported into `decl_cpp.mlc`, not spelled out in the frozen dependency list), added to the new module's own imports, pure relocation, no algorithm change. `decl_cpp.mlc`: 1171 → 959 lines (gained 1 import line, dropped 16 now-unused imports). Controlled bootstrap diff (same `mlcc` binary held fixed, only `decl_cpp.mlc` source toggled): scoped to exactly `decl_cpp.cpp/.hpp` (statement bodies byte-identical when re-aligned, only `#include`/`using namespace`/`#line` annotations differ) + new `decl_cpp_ffi.cpp/.hpp` + `module.cpp` (1 direct-caller rename: `decl_cpp::collect_ffi_include_lines` → `decl_cpp_ffi::collect_ffi_include_lines`, expected — `module.mlc` is `collect_ffi_include_lines`'s only external-to-`decl_cpp.mlc` caller) — zero other module touched. `rake test_compiler_mlc`: exit_code=0, arch lint `failures=0`. mlcc2 self-host diff (`build_bin.sh`, in-repo `TMPDIR`): built `mlcc2` from the generated output, ran on `compiler/main.mlc` → `out2`, `diff -rq out out2 --exclude=obj` IDENTICAL. All `.tmp/s104-13-slice5/**` build artifacts cleaned up after.

#### Critic close (STEP=3, 2026-07-28)

Independent function/type-set diff: old `decl_cpp.mlc` (`git show 201254fd:...`, pre-slice-5 baseline, 74 names) vs new `decl_cpp.mlc` + `decl_cpp_ffi.mlc` combined (74) — `diff` empty, zero lost/duplicated. Export-status diff: exactly the 7 documented items gained `export` (`gen_extern_type_decl_cpp`, `gen_ffi_fn_decl_cpp`, `gen_ffi_fn_proto_cpp`, `semantic_expression_is_bare_extern`, `semantic_expression_is_extern`, `semantic_expression_is_ffi_extern`, `semantic_fn_body_is_extern`), zero lost — matching the Decision exactly (`collect_ffi_include_lines` was already exported pre-slice and remains so; 17 exported pre-slice → 24 post-slice). Fresh `mlcc -o ... compiler/main.mlc` translation from scratch: `decl_cpp_ffi.cpp/.hpp` created; grepped `decl_cpp_ffi::` across every generated `.cpp`/`.hpp` — found only in `decl_cpp.cpp`, `decl_cpp.hpp` (an inline extend-method body emitted in the header — legitimate, still the 1 direct-caller module) and `module.cpp` (the `collect_ffi_include_lines` call site), zero stray references elsewhere. Independent full `rake test_compiler_mlc` rerun from a clean shell (`TMPDIR` confirmed unset first): exit_code=0, arch lint `failures=0` (13 pre-existing allowlisted warnings unchanged). Line counts confirmed: `decl_cpp.mlc` 959, `decl_cpp_ffi.mlc` 229 — no drift. mlcc2 self-host g++ diff not re-run a third time (witnessed directly during Driver STEP=2 in the same continuous session, no source change since). No false-done found. **§104-13 slice 5 closed.** §104-13 itself stays **open** — 1 group remains (extend/impl codegen + decl-segment orchestration, largest, ~750 lines), likely needs its own multi-slice sub-sequencing given its size. Queue head → §104-13 slice 6 Decision.

### Slice 6 — `decl_cpp_extend.mlc` (extend/impl codegen — zero circular dependency; leaves decl-segment orchestration as the file's own glue layer, under threshold)

#### Decision (STEP=0) — **frozen** 2026-07-28

| Item | Choice |
|------|--------|
| Problem | Re-derived the remaining group against the current 959-line file (post slice-5 shift, confirmed by grep+read, 2026-07-28): the review's single "extend/impl codegen + decl-segment orchestration" group has actually always been 2 separable groups, confirmed one-directional by grep this turn — **extend/impl codegen**, 32 items at lines 101-634 (`extend_forward_protos_for_declaration_cpp` 101, `extend_method_forward_segments_cpp` 123 export, `extend_helper_protos_for_declaration_cpp` 129, `extend_helper_protos_for_exported_types_cpp` 156 export, `semantic_declaration_fn_return_type_cpp` 165, `extend_method_return_type_cpp` 175, `extend_forward_call_argument_list` 187 (dead — zero callers repo-wide, confirmed by grep), `extend_forward_call_argument_expressions` 197, `native_inline_noexcept_forwarding_fn_cpp` 207, `native_noexcept_one_line_returning_fn_cpp` 224, `extend_trait_wrapper_decl_cpp` 238, `extend_trait_concept_adapter_decl_cpp` 257, `trait_concept_cpp_name_for_static_assert` 278, `extend_trait_static_assert_decl_cpp` 284, `extend_trait_wrapper_for_method_cpp` 303, `gen_decl_extend_trait_wrappers_only_cpp` 316, `semantic_declaration_is_assoc_bind` 334, `extend_methods_have_assoc_bind` 340 (dead — zero callers repo-wide), `extend_trait_concept_method_suffix_cpp` 349, `gen_decl_extend_trait_concept_suffix_cpp` 375, `gen_decl_extend_trait_suffix_cpp` 398 export, `gen_extend_extern_method_cpp` 413 export, `extend_method_definition_cpp` 442, `gen_decl_extend_methods_cpp` 463 export, `gen_decl_extend_cpp` 483 export, `extend_wrapper_proto_for_method_cpp` 500, `gen_extend_wrapper_protos_cpp` 520 export, `extend_forward_proto_for_fn_method` 541, `extend_forward_proto_for_method` 550, `extend_trait_implementation_protos_cpp` 569 (dead — zero callers repo-wide), `extend_forward_segment_for_extend` 588, `extend_cpp_forward_segment_cpp` 606 export) — vs **decl-segment orchestration**, 23 items at lines 636-959 (`decl_segment_cpp`/`collect_all_decl_parts_cpp`/`collect_decl_parts_cpp`/`collect_fn_defs_cpp` and 19 helpers), plus the ~100-line hub at the top of the file (`gen_decl_cpp`/`gen_proto_cpp` `extend CodegenContext` block, the file's public dispatch entry point — stays here regardless of any split, it is what makes this the file's own module). Grep-confirmed one-directional: decl-segment orchestration and the hub call into extend/impl codegen at 7 sites (59/781/828/872/874/945/949, all through the 8 already-exported names), **zero** calls the other way — extend/impl codegen never references anything defined at lines 636+. Also confirmed zero use of any of the 32 names in the 5 already-extracted modules. 8 of the 32 already carry `export` (matches external test-importer usage in `compiler/tests/support/decl_gen_test_helpers.mlc`-style importers, confirmed by grep) — no new exports needed, unlike slices 4/5 |
| Strategy (v1) | New `compiler/codegen/decl_cpp_extend.mlc`. Move all 32 items wholesale (including the 3 confirmed-dead ones — pure relocation, dropping dead code is a behavioral-surface change outside a relocation slice's scope, same precedent as `function_declaration_template_prefix` in slice 4). Keep the existing 8 exports (`extend_method_forward_segments_cpp`, `extend_helper_protos_for_exported_types_cpp`, `gen_decl_extend_trait_suffix_cpp`, `gen_extend_extern_method_cpp`, `gen_decl_extend_methods_cpp`, `gen_decl_extend_cpp`, `gen_extend_wrapper_protos_cpp`, `extend_cpp_forward_segment_cpp`), no new ones needed. Dependencies (`Param` from `../frontend/ast`; `SemanticDeclaration`/`SemanticDeclarationFn`/`SemanticDeclarationType`/`SemanticDeclarationTypeAlias`/`SemanticDeclarationTrait`/`SemanticDeclarationExtend`/`SemanticDeclarationExported`/`SemanticDeclarationImport`/`SemanticDeclarationExternLib`/`SemanticDeclarationExternType`/`SemanticDeclarationAssocBind`/`sdecl_inner` from `../ir/semantic_ir`; `Type`/`trait_base_name` from `../checker/registry`; `CppDeclaration`/`CppFnProto`/`CppFnDef`/`CppExpression`/`CppCall`/`CppStaticAssert` from `../cpp_ir/cpp_ast`; `emit_helpers` from `../cpp_emit/emit_helpers`; `CodegenContext`/`trait_has_associated_types` from `./context`; `exported_type_names_from_decls` from `./decl/decl`; `gen_parameter_proto_items`/`gen_parameter_def_items`/`extract_method_name` from `./decl/decl_extend`; `list_contains` from `./decl/decl_index`; `sem_type_to_cpp`/`type_name_to_cpp` from `./decl/type_gen`; `cpp_safe` from `./cpp_naming`; `empty_cpp_declarations`/`append_cpp_declarations`/`is_semantic_declaration_fn` from `./decl_cpp_helpers`; `gen_fn_proto_cpp`/`native_fn_decl_cpp` from `./decl_cpp_fn`) are already-imported external symbols, re-imported directly from their true source modules. `decl_cpp.mlc` imports the 8 exports back — no injection needed, pure downstream calls. Also drop the pre-existing unused import `gen_extend_wrapper_protos` (distinct from the local `gen_extend_wrapper_protos_cpp`, from `./decl/decl_extend` — confirmed zero call sites anywhere in the file, a stale import predating this track, cleaned up incidentally while the import block is touched). Net effect: `decl_cpp.mlc` drops from 959 to ~426 lines (534 removed + 1 import line added), which puts it **under** the 800-line arch-lint threshold (`compiler/tests/run_architecture_lint.sh: MAX_FILE_LINES=800`) for the first time since before the 2026-06-29 review — so decl-segment orchestration + the hub do **not** need their own slice; the allowlist entry for `codegen/decl_cpp.mlc` in `compiler/tests/architecture_lint_allowlist.txt` gets removed |
| Primary gate | Red: `decl_cpp_extend.mlc` absent, all 32 items still at the documented lines, file at baseline 959 lines. Green: `decl_cpp_extend.mlc` exists with the 8 documented exports; `decl_cpp.mlc` shrinks to ~426 lines (under 800, allowlist entry removed), gains 1 import line; bootstrap diff restricted to split modules + direct-caller namespace-prefix renames in `decl_cpp.cpp/.hpp` only; `rake test_compiler_mlc` (1471+ passed, 0 failed, arch lint failures=0 with the allowlist entry now gone); self-host mlcc2 diff before Critic close |
| Module touch | new `compiler/codegen/decl_cpp_extend.mlc`; `compiler/codegen/decl_cpp.mlc` (shrinks, gains 1 import line, drops the stale `gen_extend_wrapper_protos` import); `compiler/tests/architecture_lint_allowlist.txt` (removes the now-unneeded `file_size:codegen/decl_cpp.mlc` line) |
| REG | no (`compiler/**` only) |
| Out of scope | decl-segment orchestration (stays in `decl_cpp.mlc` as its own glue layer, per the original slice-1 survey note — not extracted, expected to end up under threshold on its own); any signature/algorithm change; dropping the 3 dead functions (pure relocation only); MIR |

#### Steps (§104-13 — slice 6: decl_cpp_extend)

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze | **done** |
| 1 | Red: confirm current boundaries (`decl_cpp_extend.mlc` absent, 32 items at the documented lines, file at baseline 959 lines) | **done** — `test -f` negative; boundary lines 101/123/606/636 confirmed by spot-check, file at baseline 959 lines |
| 2 | Green: create `decl_cpp_extend.mlc`, wire `decl_cpp.mlc` imports, drop stale import + allowlist entry, bootstrap diff (split-scoped), `rake test_compiler_mlc`, mlcc2 self-host diff | **done** — see below, 1 Decision-text correction found during implementation |
| 3 | Critic: full re-audit, close slice + close §104-13 (last group of this file) | **done — closed** |

**Green (2026-07-29) — 1 correction to the Decision found during implementation:** the Decision's dependency analysis missed that the extend/impl group and the ~100-line dispatch hub (`gen_decl_cpp`/`gen_proto_cpp` free functions + `extend CodegenContext { gen_decl_cpp, gen_proto_cpp }`) are **mutually** dependent, not one-directional — `gen_decl_cpp`'s `SemanticDeclarationExtend` arm calls `gen_decl_extend_cpp` (extend group), and 4 extend-group functions (`extend_forward_protos_for_declaration_cpp`, `extend_helper_protos_for_declaration_cpp`, `extend_forward_proto_for_fn_method`, the dead `extend_trait_implementation_protos_cpp`) call back into `gen_proto_cpp`. Moving only the 32-item group as planned would have created a real circular import between `decl_cpp.mlc` and `decl_cpp_extend.mlc` (MLC has no forward-declare-across-modules escape hatch for this, unlike the `context_resolve`-closure-injection pattern used for genuine one-directional-but-deep cases elsewhere in this track). Fix: moved the hub (both free functions, already `export`ed, plus the `extend CodegenContext` block) into `decl_cpp_extend.mlc` together with the 32-item group — zero behavior change, the hub has zero dependency on decl-segment orchestration (confirmed by grep before moving). `decl_cpp.mlc`'s decl-segment orchestration in turn had exactly 1 direct-dot-call site on the moved extend impl (`context.gen_proto_cpp(declaration)` in `decl_segment_for_extend_declaration_cpp`) — rewritten to the equivalent free-function call `gen_proto_cpp(declaration, context)` (both already meant the same thing; avoids relying on unverified cross-module `extend`-block dot-dispatch semantics). Also found and dropped 2 stray unused imports surfaced by the same grep-audit discipline used in every prior slice: `semantic_fn_body_is_extern` (imported into `decl_cpp.mlc`, zero uses in decl-segment orchestration) and `TypeVariant`/`gen_fn_decl_cpp` (imported into `decl_cpp_extend.mlc`, zero uses — `TypeVariant` was only needed by the type-codegen group already extracted in slice 2, `gen_fn_decl_cpp` was superseded by `gen_fn_decl_cpp_with_escape` before this track started). Net result: `decl_cpp.mlc` 959→355 lines (removed the hub + 32-item group + `gen_extend_wrapper_protos` stale import, added 1 import line); new `decl_cpp_extend.mlc` created at 626 lines (32-item group + hub + 3 dead functions moved as-is, no algorithm change). **Both files now under the 800-line arch-lint threshold** — `file_size:codegen/decl_cpp.mlc` removed from `compiler/tests/architecture_lint_allowlist.txt`, no new entry needed for `decl_cpp_extend.mlc`. Fresh `mlcc -o ... compiler/main.mlc` translation from scratch (exit 0): `decl_cpp_extend.cpp/.hpp` created; grepped `decl_cpp_extend::` across every generated file — found only in `decl_cpp.cpp` (the 1 direct-caller module), zero stray references. `rake test_compiler_mlc`: exit_code=0, `1471 passed, 0 failed`, arch lint `failures=0 warnings=12` (down from 13 — `decl_cpp.mlc` no longer in the WARN list). mlcc2 self-host: built via `build_bin.sh` (`MLC_CXX=g++`, in-repo `TMPDIR`), `mlcc2 -o out2 compiler/main.mlc`, `diff -rq out out2 --exclude=obj` — IDENTICAL. Repeated the full translate+test+mlcc2-diff cycle a second time after the import cleanup (not just after the hub-move fix) — all green both times. `.tmp/s104-13-slice6/**` cleaned up after.

#### Critic close (STEP=3, 2026-07-29)

Independent function/type-set diff: old `decl_cpp.mlc` (`git show b193ec0e:...`, pre-slice-6 baseline, 60 top-level `fn`/`type` names) vs new `decl_cpp.mlc` + `decl_cpp_extend.mlc` combined (60) — `diff` empty, zero lost/duplicated (note: this count excludes the `extend CodegenContext { ... }` block's 2 nested methods, consistent with the counting method used in every prior slice of this track). Export-status diff: 17 exports before, 17 after, `diff` empty — zero gained, zero lost, matching the Decision's "no new exports needed" claim exactly (the hub's `gen_decl_cpp`/`gen_proto_cpp` were already `export`ed pre-slice, carried over as-is). Confirmed the 1 Decision correction (hub had to move with the group, 1 dot-call rewritten to free-function form) is a pure mechanical fix with no behavior change — re-read both new files in full, no other deviation from the moved-verbatim bodies found. Fresh `mlcc -o ... compiler/main.mlc` translation from scratch: `decl_cpp_extend.cpp/.hpp` created; grepped `decl_cpp_extend::` across every generated `.cpp`/`.hpp` — found only in `decl_cpp.cpp`, zero stray references elsewhere. Independent full `rake test_compiler_mlc` rerun: exit_code=0, `1471 passed, 0 failed`, arch lint `failures=0 warnings=12`. Independent `ReadLints` on both files: no linter errors. Line counts confirmed: `decl_cpp.mlc` 355, `decl_cpp_extend.mlc` 626 — both under the 800-line threshold, `compiler/tests/architecture_lint_allowlist.txt` no longer lists `codegen/decl_cpp.mlc`. mlcc2 self-host g++ diff not re-run a third time (witnessed directly during Driver STEP=2, twice, in the same continuous session, no source change since the last one). No false-done found. **§104-13 slice 6 closed.**

**§104-13 itself CLOSED** (`codegen/decl_cpp.mlc` split 1666→355 lines across 6 slices/modules over the track: `decl_cpp_helpers.mlc`, `decl_cpp_type.mlc`, `decl_cpp_trait.mlc`, `decl_cpp_fn.mlc`, `decl_cpp_ffi.mlc`, `decl_cpp_extend.mlc`; both the original file and every new module now under the 800-line arch-lint gate, allowlist entry removed — the exit criterion added 2026-07-29 after the §104-12 premature-close finding). **§104-12 slice 5 done 2026-07-30** (`transform_support.mlc` extracted, `transform.mlc` 881→753 lines, allowlist entry removed) — **§104-12 itself now CLOSED**, both split tracks meet the 2026-07-29 exit criterion. Critic re-audit 2026-07-30 confirmed clean (see §104-12 slice-5 section above). Queue head → §104-14 (`codegen/expr/match_gen.mlc` split, 1403 lines) Decision.

## §104-14 `codegen/expr/match_gen.mlc` split (1403 lines)

### Slice 1 — `match_result_type.mlc` (match-result C++ return-type resolution — zero injected function parameters, zero circular-import risk)

#### Decision (STEP=0) — **frozen** 2026-07-30

| Item | Choice |
|------|--------|
| Problem | `match_gen.mlc` (1403 lines) generates C++ for `match` expressions via 3 codegen strategies (std::visit string-lambdas, std::visit `CppExpression` IR, guarded if-chain) that all thread `gen_stmts`/`eval_expr_fn` function parameters (the injection pattern breaking the cycle with `eval.mlc`, per the file's own header comment) — most of the file cannot be split without either the injection pattern or accepting a circular import, same shape as `transform.mlc`/`decl_cpp_extend.mlc` hit in this track already. Surveyed for a zero-injected-parameter subset first (lowest risk): found one, lines 923-1085 (13 functions), the match-result-type-to-C++ resolution subsystem — `full_result_cpp_template`, `expression_result_cpp_type_for_codegen` (export), `result_ok_type_cpp_from_generic`, `result_ok_type_cpp_if_generic`, `result_err_type_cpp_from_generic`, `result_err_type_cpp_if_generic`, `match_ok_cpp_type`, `match_err_cpp_type`, `cpp_type_string_has_auto_placeholder`, `is_bare_result_cpp_name`, `match_return_cpp_type`, `result_template_from_match_type`, `match_expression_return_cpp_type` (export). Confirmed by grep: none of the 13 take a `gen_stmts`/`eval_expr_fn` parameter or call anything else in `match_gen.mlc` outside this group — every call is to already-imported externals (`sem_type_to_cpp`, `is_result_generic`, `result_ok_type`, `result_err_type`, `cpp_template_two_type_arguments`, `sexpr_type`) or to another function within the same 13. Grep-confirmed callers: `match_return_cpp_type` has 5 in-file callers outside the group (lines 320/917/1178/1203/1372, needs a new `export`); `expression_result_cpp_type_for_codegen` and `match_expression_return_cpp_type` are already `export`ed and have **zero** in-file callers — their only callers are external, `compiler/expr_visitor_cpp.mlc` (lines 388/518/567) via `import * as match_codegen from './codegen/expr/match_gen'` + dot-access. (`expr_visitor_cpp.mlc` also defines its own unrelated same-named local `match_return_cpp_type`, lines 561-568 — a false-positive naming collision confirmed by reading both definitions, not a caller of `match_gen.mlc`'s version.) |
| Strategy (v1) | New `compiler/codegen/expr/match_result_type.mlc`. Move all 13 items wholesale. Export exactly 3: `match_return_cpp_type` (new — needed by `match_gen.mlc`'s own remaining code), `expression_result_cpp_type_for_codegen`, `match_expression_return_cpp_type` (both already exported, carried over). The other 10 stay internal. Dependencies (`Type` from `../../checker/registry`; `SemanticExpression`/`sexpr_type` from `../../ir/semantic_ir`; `is_result_generic`/`result_ok_type`/`result_err_type` from `../../checker/check/method_types/result_option_method_types`; `sem_type_to_cpp`/`cpp_template_two_type_arguments` from `../decl/type_gen`; `CodegenContext` from `../context`) are already-imported external symbols in `match_gen.mlc`, all also still needed there by the remaining code — re-import directly into the new module, no removal from `match_gen.mlc`'s own import lines. `match_gen.mlc` adds 1 new line: `import { match_return_cpp_type } from './match_result_type'` (its own 5 call sites need it; it has zero internal use of the other 2 exports, so does not import them). External caller `expr_visitor_cpp.mlc`: rather than rely on unverified wildcard-import transitive-forwarding through the now-emptied names in `match_gen.mlc` (avoided per the `decl_cpp_extend.mlc` slice 6 precedent — "avoids relying on unverified cross-module dot-dispatch semantics"), add an explicit `import * as match_result_type from './codegen/expr/match_result_type'` and rewrite its 3 call sites from `match_codegen.expression_result_cpp_type_for_codegen`/`match_codegen.match_expression_return_cpp_type` to `match_result_type.expression_result_cpp_type_for_codegen`/`match_result_type.match_expression_return_cpp_type`. Net effect: `match_gen.mlc` drops from 1403 to ~1241 lines (163 lines removed + 1 import line added) — still **above** the 800-line threshold, confirming the track file's own "expect 2+ slices" estimate; stays on the allowlist this slice, no allowlist change yet |
| Primary gate | Red: `match_result_type.mlc` absent, all 13 items at the documented lines, file at baseline 1403 lines. Green: `match_result_type.mlc` exists with exactly the 3 documented exports; `match_gen.mlc` shrinks to ~1241 lines (still allowlisted), gains 1 import line; `expr_visitor_cpp.mlc` gains 1 import line, 3 call sites rewritten; bootstrap diff restricted to the split modules + `expr_visitor_cpp.cpp` (namespace-prefix rename only, zero logic change) + any other direct caller found by grep; `rake test_compiler_mlc` (1471+ passed, 0 failed, arch lint failures=0); self-host mlcc2 diff before Critic close |
| Module touch | new `compiler/codegen/expr/match_result_type.mlc`; `compiler/codegen/expr/match_gen.mlc` (shrinks, gains 1 import line); `compiler/expr_visitor_cpp.mlc` (gains 1 import line, 3 call sites rewritten) |
| REG | no (`compiler/**` only) |
| Out of scope | the remaining 3 codegen strategies (std::visit string-lambda, std::visit `CppExpression`, guarded if-chain) and their shared arm/binding helpers — all thread the injection-pattern parameters, need their own Decision(s) per strategy or shared-helper boundary; any signature/algorithm change; MIR |

#### Steps (§104-14 — slice 1: match_result_type)

| Step | Item | Gate |
|------|------|------|
| 0 | Decision freeze | **done** |
| 1 | Red: confirm current boundaries (`match_result_type.mlc` absent, 13 items at the documented lines, file at baseline 1403 lines) | **done** — `test -f` negative; all 13 boundary lines confirmed; file at baseline 1403 lines |
| 2 | Green: create `match_result_type.mlc`, wire `match_gen.mlc` + `expr_visitor_cpp.mlc` imports, bootstrap diff (split-scoped), `rake test_compiler_mlc`, mlcc2 self-host diff | **done** — see below, 1 Decision correction found during implementation (naming collision) |
| 3 | Critic: full re-audit | pending |

**Green (2026-07-30) — 1 correction to the Decision found during implementation:** the planned `import * as match_result_type from './codegen/expr/match_result_type'` wildcard import in `expr_visitor_cpp.mlc` triggered a real mlcc codegen bug — that file already defines its own unrelated local `fn match_return_cpp_type` (a small wrapper calling `match_expression_return_cpp_type`, false-positive name collision with the moved-and-now-exported `match_result_type.mlc` function of the same name). Once that name became reachable via import, mlcc's C++ emission qualified the **local** definition's own body with the imported module's namespace (`mlc::String match_result_type::match_return_cpp_type(...)` emitted inside `expr_visitor_cpp.cpp`) — a duplicate-definition link error against the real `match_result_type.cpp` definition. Switching the import from wildcard to named (`import { expression_result_cpp_type_for_codegen, match_expression_return_cpp_type } from ...`) did **not** fix it — confirmed the bug is triggered by any reachable same-name export, not the import style. Fix: renamed `expr_visitor_cpp.mlc`'s local function to `match_default_block_return_cpp_type` (its only caller, `gen_match_default_block_cpp`, updated too) — pure rename, zero algorithm change, no other repo-wide name collisions found for any of the 3 newly-exported names (grep-confirmed). `match_result_type.mlc` created (173 lines): 13 items moved wholesale, 3 exported (`match_return_cpp_type`, `expression_result_cpp_type_for_codegen`, `match_expression_return_cpp_type`) exactly per the Decision. `match_gen.mlc`: 1403 → 1240 lines (gained 1 import line for `match_return_cpp_type`). `expr_visitor_cpp.mlc`: gained 1 import line, 3 call sites rewritten to unqualified names (now resolved via the named import), 1 local function renamed. Controlled bootstrap diff (same `mlcc` binary held fixed, only the 3 touched `.mlc` files toggled between pre/post state): scoped to exactly `match_gen.cpp/.hpp` (shrink, `#line` renumber) + new `match_result_type.cpp/.hpp` + `expr_visitor_cpp.cpp/.hpp` (3 call-site qualifications + 1 rename) — zero other module touched, verified both raw and with `#line` directives stripped for a clean statement-level comparison. `rake test_compiler_mlc` (after clearing a stale precompiled-header, unrelated mtime drift from a concurrent session touching `runtime/include/mlc/io/file.hpp`): exit_code=0, `1471 passed, 0 failed`, arch lint `failures=0 warnings=11` (unchanged — `match_gen.mlc` still allowlisted at 1240 lines, still above 800). mlcc2 self-host diff (`build_bin.sh`, `MLC_CXX=g++`, in-repo `TMPDIR`): fresh `mlcc` → `mlc_p1`, `mlcc2` built from `mlc_p1`, `mlcc2` → `mlc_p2`, `diff -rq mlc_p1 mlc_p2 --exclude=obj` IDENTICAL. `compiler/out/mlcc` confirmed fresh (rebuilt as a side effect of the `rake test_compiler_mlc` run; smoke-tested by re-translating `compiler/main.mlc` with it and confirming `match_result_type.cpp/.hpp` present, `full_result_cpp_template` absent from `match_gen.cpp`). All `.tmp/s104-14-slice1/**` build artifacts cleaned up after.

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
