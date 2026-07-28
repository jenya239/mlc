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
threaded through 4 functions), **queue head is now §104-12 slice 3 STEP=1
(red)** (priority override 2026-07-28, user: "это должно быть приоритетом
сейчас" — Wave 1 moved ahead of §101/§102/§103; Wave 2 stays queued after
§103, Wave 3 stays gated)

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
| 1 | Red: confirm current boundaries | pending |
| 2 | Green: create `transform_call_args.mlc`, thread the 3 injected parameters, wire `transform.mlc` call site + import, bootstrap diff (split-scoped), `rake test_compiler_mlc`, mlcc2 self-host diff | pending |
| 3 | Critic: full re-audit | pending |

### Slice 4 — `transform_method.mlc` (needs `transform_exprs_fn` injection — depends on slice 2)

Renumbered from the original "Slice 3". `transform_result_option_hof_method_call` + `transform_array_hof_method_call` + `transform_regular_method_call` + `transform_extend_method_as_call` + `transform_region_alloc_method_call`. Needs `transform_exprs_fn` injected (3 call sites). `transform_method_call_after_object` stays in `transform.mlc` (calls `dispatch_transform_pass` directly, tightly coupled to the dispatcher).

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
