# Development (after CONTINUITY block in queued prompt)

Re-read [CONTINUITY.md](CONTINUITY.md) every turn — rules apply without restart.

## Principles

- **One sub-step per prompt** — one concern; bisect-friendly.
- **One layer per sub-step** — `compiler/` XOR `lib/mlc/`, not both (except tiny import fix).
- **Verify before next queue:** Tier A green every turn; Tier B before commit / TRACK close (see ladder).
- **Plans:** [PLAN.md](../PLAN.md) north star; [TRACK_*.md](.) active work; **Planner** extends TRACK, **Driver** executes.

## Roles (see CONTINUITY)

- **Driver** — code + verify (most turns).
- **Planner** — TRACK rows from PLAN; **if tests red → enqueue Driver `test-fix` first, no new features**; no `compiler/`.
- **Backlog** — TRACK vs git hygiene; no `compiler/`.

## Verification ladder (compiler changes)

**Tier A — every Driver turn (target &lt;5 min warm):**
```bash
bash scripts/dev_gate_fast.sh
```
`run_tests` (reuse `compiler/out/tests/run_tests` if tests unchanged) + `mlcc --check-only main.mlc` if `out/mlcc` exists + arch lint. No fuzz / LSP / differential.

**Tier B — before commit, TRACK close, or after `compiler/**` / fuzz / LSP edits:**
1. `compiler/tests/build_tests.sh` → full gate (~30 min cold). Abort if &gt; 10 min silent.
2. If **`lib/mlc/**` touched:** `bundle exec rake test_mlc`.
3. If **`compiler/**` touched:** `compiler/build.sh` → self-host diff:
   ```bash
   compiler/out/mlcc -o .tmp_selfhost/p1 compiler/main.mlc
   compiler/build_bin.sh .tmp_selfhost/p1 .tmp_selfhost/mlcc2
   .tmp_selfhost/mlcc2 -o .tmp_selfhost/p2 compiler/main.mlc
   diff -rq .tmp_selfhost/p1 .tmp_selfhost/p2
   ```
   Use `build_bin.sh`, not bare `g++` — parallel + ccache.
4. Step **14** in TRACK: mandatory self-host before closing visitor batch.

**Tier C — CI / release:** `.github/workflows/build-mlcc-once.yml` or manual full Tier B on clean tree.
Also: `bash scripts/regression_gate.sh` — frozen stdout suite, then `scripts/run_examples_compile_sweep.sh` (compile+link every `fn main` under `misc/examples`/`misc/gui`; soft-skips missing sysdeps).

## Current priority

**Queue head:** `TRACK_COMPILER_ARCHITECTURE_HYGIENE` §104-23 **CLOSED** 2026-07-31 (Critic-audited same day) — **this finishes the entire authorized Wave 1 list**; queue advanced to §101 (`EDITOR_CARET_VISUAL_ROW_CACHE`) Driver STEP=0 — investigating it found its own "confirmed root cause" claim was never actually measured (direct instrumentation: caret walk costs 74–330 µs/frame on the standing 100k-line perf smoke fixture, caret stays at line 0 there, not "essentially all of `draw_us`" as claimed). The real dominant cost (>95% of `draw_us`) is a different, previously-undiagnosed function — `editor_ux_max_line_columns_from_index` (horizontal-scrollbar content width) — fixed same day as new **§105 `TRACK_EDITOR_HORIZONTAL_SCROLLBAR_MAX_COLUMNS_CACHE`**: new `MaxLineColumnsCache` wired into `EditorFrameLayout` (§97b), 4 call sites in `demo_live.mlc` routed through it, new scenario `max_line_columns_cache_stable`. Perf smoke `frames=30 layout_us=59041 draw_us=8927341 total_us=9064023` → `frames=30 layout_us=375140 draw_us=84844 total_us=532723` (**draw_us 105× lower, total_us 17× lower**). §105 **CLOSED** 2026-07-31, Critic-audited same day: independent rebuild + re-measure in a separate output dir (`draw_us=78062 total_us=501532`, same order of magnitude as the Driver's numbers, confirms the fix); `scripts/run_ux_gate.sh` run twice (113/113 scenarios both times, 0 failures, includes the new `max_line_columns_cache_stable` scenario) — no false-done found. §101 (`EDITOR_CARET_VISUAL_ROW_CACHE`) red/green done same day: new `document_visual_row_prefix_pixel_budget` (`word_wrap.mlc`) computed once inside the existing pixel-budget wrap-cache pass (no added asymptotic cost) + `visual_row_index_for_caret_pixel_budget_cached` (O(1) prefix lookup + single-line scan, falls back to the uncached walk when the prefix table doesn't cover `caret_line` — e.g. `skip_full_pixel_wrap_now`); new `prefix_visual_rows: [i32]` field on `DocumentWrapCountCache`, pixel-budget path only; `demo_live.mlc`'s 1 live call site (inside the per-caret loop) routed through it. New scenario `caret_visual_row_cache_stable` (cached vs uncached match at caret line 0/mid-file/last-line + fallback path + idle/edit invalidation). `run_ux_gate.sh` ×2 clean (114/114, 0 fail). Perf smoke re-run unregressed vs §105's baseline (`draw_us=77818` vs `84844`/`78062` — this fixture keeps caret at line 0, so this fix's own win isn't visible here, expected). **Queue head is now §101 Critic (STEP=3)**, then §102/§103; Wave 2 (MIR-as-layer)/Wave 3 of §104 remain gated. §104-23 (determinism checks): new `compiler/scripts/check-determinism.sh` (`--dump-mir --check-only` twice + `--dump-sem --check-only` twice, diffed, prints `MIR DETERMINISTIC`/`SEM DETERMINISTIC`) + a new `rake test_determinism` task, no `.mlc` touched. `--check-only` added beyond the review's literal snippet after confirming (`pipeline.mlc:396-397`) it only skips codegen, not the dump. Ran against the real `compiler/main.mlc`: both dumps non-trivial (38441/3753 lines) and byte-identical across repeated runs on the first attempt — no hidden `Map`-ordering non-determinism found. Critic independently reproduced: fresh `mlcc` rebuild, exact dump-size match, `rake test_determinism` rerun, fail-fast-path test, Map-free-printer re-confirmation. §104-22 (`bootstrap-fast.sh`/`bootstrap-full.sh` tooling) **CLOSED** 2026-07-30: 2 new scripts, `compiler/scripts/bootstrap-fast.sh`/`bootstrap-full.sh`, no `.mlc` touched. The review's own literal Шаг 22 snippet fails to link as written (missing runtime `.cpp` sources), fixed by linking the same 3 runtime files `bisect_bootstrap_link.sh` already uses. Both scripts follow the existing `compiler/scripts/` convention (assume `compiler/out/mlcc` pre-built, fail fast otherwise, per `mir_bootstrap_report.sh`'s pattern). `bootstrap-fast.sh` (hybrid + fast-build + `-O0` link, then times the binary's own re-translation): exit 0, 37.054s real (Driver) / 36.648s (Critic, independent) for the re-translation (vs the project's documented ~4-5s `-O2 mlcc` benchmark — the intended fast-compile/slow-run trade-off, now measurable and reproducible). `bootstrap-full.sh` (`mlcc`→p1, `build_bin.sh`→`mlcc2`, `mlcc2`→p2, diff): exit 0, printed **`STAGE IDENTICAL`** twice independently from 2 genuine self-host round-trips. Critic-audited same day: confirmed via `git show --stat` that zero `.mlc` files were touched; read the review's own Шаг 22 text directly and confirmed both scripts correctly extend its illustrative-only snippets; independent fresh `mlcc` rebuild + independent reruns of both scripts reproduced the same results; confirmed via `file` that every `compiler/scripts/*.sh` sibling is Bash; additionally tested the missing-`mlcc` fail-fast path (exit 1, correct message) — no false-done found. §104-20 (`--cpp-mode=fast-build`) **CLOSED** 2026-07-30: opt-in flag (default `readable` unchanged) threaded through `CompileOptions`→`ModularCompileInput`→`PrecomputedCtx`→`CodegenContext`; above a 6-arm threshold, the non-guarded/non-wildcard/non-string match codegen path now reuses the already-production-proven if-chain generator instead of `std::visit(overloaded{...})`. Verified: default-path diff-empty vs a `git stash`-clean baseline (only the 9 scoped files differ, purely additive); `rake test_compiler_mlc` 1471/1471 unaffected; self-host round-trip byte-identical both modes; measured real C++ compile-time delta on the 44 affected files — **g++ −3.0%, clang++ −4.8%** aggregate, real but modest (per-file deltas noisy, dominated by per-arm lambda-body content rather than the `std::visit` mechanism itself). Critic-audited independently: default-path diff-empty re-verified from a fresh `git worktree` baseline (hit and fixed a `#line`-path-mismatch false-positive along the way, same lesson as §104-15/16), self-host round-trip on 2 independently-built `mlcc2` binaries, `rake test_compiler_mlc` 1471/0 rerun, compile-time payoff re-measured with a 2nd methodology (real `-c` compiles on a quiet machine, child-CPU-time, not the Driver's wall-clock; found `-fsyntax-only` understates the effect and discarded that first attempt) — g++ −4.22%/clang++ −5.66%, same direction/magnitude, confirms the payoff is real. Also fixed a stray duplicate-of-§104-16 paragraph misplaced under §104-20's Critic slot by an earlier turn. §104-19 (include planner/forward-decls) **REJECTED** 2026-07-30, evidence-based: this codebase's 2 most heavily cross-module-shared types (`ast::Expr`/`Stmt`, `registry_type::Type`) compile to C++ `std::variant` type aliases, which cannot be forward-declared at all, ruling out ~90% (4365/4811) of measured `Shared<T>` cross-module usages; separately, 66% (749/1127) of all direct `.hpp` `#include` lines are already transitively redundant today (reachable via a sibling include regardless), so even eligible forward-declarations would mostly have no compile-time effect. No `compiler/**` code changed this step, no self-host diff applicable (survey-only). §104-18 **CLOSED** 2026-07-30: new `compiler/cpp_emit/layout.mlc` (`layout_group_for_path`/`layout_group_names`, 5 groups by path prefix per the review's own branch order); `emit_layout: string` threaded through `CompileOptions`/`ModularCompileInput`/`compile_modular` (default `'split'`, unchanged) and 5 `ModularCompileInput`-literal call sites (`compile_driver.mlc` + 4 test files); `run_codegen_pass` gains a hybrid branch that concatenates each module's printed `.cpp` body into 1 of 5 group files instead of 1-per-module (`.hpp` writes unchanged). Review's own literal acceptance test passed: `mlcc --emit-layout=hybrid -o <dir> compiler/main.mlc` → exactly 5 `.cpp` files (frontend 6607/sema 17161/mir 4099/cpp_backend 16883/driver 6352 lines) + the same 185 `.hpp` files; a binary built from those 5 files re-translates `compiler/main.mlc` byte-identically to one built from the 185 split-mode files — empirically confirms the review's own namespace-concatenation-safety argument (every module's `.cpp` already self-contained in its own `namespace {}` with an identically-redefined `#define main mlc_user_main`). New test coverage added (`compiler/tests/test_layout.mlc`, 2 assertions in `test_dump_flags.mlc`, registered in `suite_registry.mlc`) but a pre-existing, unrelated bug blocked running it through the standard harness this turn: `build_tests_self.sh` fails with a self-hosted-mlcc import-path-resolution error for subdirectory entry points (`compiler/tests/tests_main.mlc`), and a direct Ruby `ModularCompiler` invocation on the same entry hits an unrelated parser `Unexpected EOF in block` error partway through the 254-module graph — both reproduced identically on the pre-change baseline via `git stash` (confirming neither is caused by this step, both pre-existing and out of scope). Substituted a standalone probe placed directly under `compiler/` (avoiding both broken subdirectory-entry paths), running the identical assertions through the full working `mlcc`→C++→binary pipeline — all passed, including the 5-group-file pipeline-integration check and the 2 new flag-parsing checks; probe deleted after verification, not committed. Full from-scratch Ruby-bootstrap `mlcc` rebuild clean (0 errors); `rake test_compiler_mlc` phases 3-9 (which invoke the fresh `mlcc` binary directly in default split mode) all pass; mlcc2 self-host diff (default split mode, `build_bin.sh MLC_CXX=g++`) IDENTICAL. **Critic-audited same day: independent diff-review of all 13 touched files exact-matching the Decision's Module-touch list, independent from-scratch Ruby-bootstrap rebuild, independent split-mode (185/185) and hybrid-mode (exactly 5 non-trivial `.cpp` files) translations, 2 independently-built binaries from split/hybrid source whose own re-translations are byte-identical, total `.cpp` byte count preserved exactly across split/hybrid (3328011 bytes both ways), independent `rake test_compiler_mlc` rerun (1471/0), an independent probe (not reusing the Driver's) built from the actual committed test source running all 25 assertions end-to-end — 25/0, and an independent mlcc1→mlcc2 self-hosting round-trip diff — IDENTICAL; disclosed `tests_main.mlc` limitation confirmed genuine/pre-existing. No false-done found. §104-18 CLOSED.** §104-16 **CLOSED** 2026-07-30 (1 slice): split `checker/infer/infer.mlc` (962 lines) via a new `infer_record.mlc` (record-literal / record-update field-value inference, 9 items, 2 exported, using the `infer_expr_fn` injection convention already used by every sibling `infer_*.mlc` module — zero external callers outside `infer.mlc` confirmed by grep). `infer.mlc` 962→747 lines — already ≤800, gate met by 1 slice alone. Bootstrap diff scoped to exactly `infer.cpp/.hpp` + new `infer_record.cpp/.hpp`; caught and fixed a false-positive whole-tree diff mid-verification (worktree baseline translated with an absolute `main.mlc` path vs the working tree with a relative one — produces spurious `#line`-only differences in every file; corrected by using matching relative paths for both, same lesson as §104-15). `rake test_compiler_mlc` 1471/0 arch lint failures=0 warnings=8 (down from 9); mlcc2 self-host diff IDENTICAL. Also removed 4 stale `file_size:` allowlist entries found inert while editing that file: `infer.mlc` itself, `checker/registry.mlc` (missed during §104-15's own close last turn), `cpp_ir/cpp_ast.mlc`/`cpp_emit/print.mlc` (pre-existing, predate this track, both already well under 800). Independently re-confirmed by Critic same day: function/type-name-set diff 41/41 empty at top level, 71/71 empty at full nesting; byte-level body diff all 8 moved items verbatim modulo the documented injection edits; full-file reconstruction diff confirming zero unrelated changes; from-scratch Ruby-bootstrap rebuild + fresh translation + duplicate-symbol check clean; independent `rake test_compiler_mlc` rerun 1471/0; independent mlcc2 self-host diff rebuilt from scratch — IDENTICAL. No false-done found. §104-15 **CLOSED** 2026-07-30 (1 slice): split `checker/registry.mlc` (1060 lines) via a new `registry_type.mlc` (the `Type`/`TypeRegistry` core). Confirmed reading the self-hosted parser that MLC has no `export ... from` re-export syntax (the review's Часть 3 assumption was wrong) — an empirical 3-file scratch probe (real `g++` compile+link+run) proved the plain `import { X } from './m'` forwarding already used in §104-14 extends to `type` declarations, so no language extension was needed. `registry.mlc` 1060→728 lines — **already ≤800, gate met by 1 slice alone**; `registry_annotation.mlc`/`registry_build.mlc` from the review's plan were not needed. 1 real defect found: `infer_literals.mlc` used `Type`'s variant constructors without importing anything registry-related (the only file in the compiler doing this) — worked before only because the transitive-resolution chain was exactly 2 hops; adding `registry_type` as a 3rd hop exceeded `mlcc`'s codegen qualifier-lookup depth and emitted invalid unqualified C++; fixed with an explicit import (matches every other file's convention). `rake test_compiler_mlc` 1471/0 arch lint failures=0 warnings=9 (down from 10); mlcc2 self-host diff IDENTICAL. Mid-slice incident, resolved, logged in the track file: an accidental stash-pop-of-the-wrong-stash + a `git checkout --theirs -- .` mistake reverted 3 non-track WIP files, recovered from Cursor local editor history; separately the root disk hit 100% full mid-test-run (unrelated), freed via `ccache -C` + stale `.tmp/` cleanup. Independently re-confirmed by Critic same day: function/type-name-set diff 68/68 empty at every nesting depth, byte-level body diff all 9 moved items verbatim, full-file reconstruction diff confirming zero unrelated changes to the remaining 724 lines, from-scratch Ruby-bootstrap `mlcc` rebuild + fresh translation + duplicate-symbol check clean, independent `rake test_compiler_mlc` rerun 1471/0, independent mlcc2 self-host diff rebuilt from scratch and IDENTICAL — no false-done found. §104-14 (`codegen/expr/match_gen.mlc` split) **CLOSED** 2026-07-30, re-confirmed by an independent Critic re-audit same day: 1403→414 lines across 5 slices/modules (`match_result_type.mlc` 173 lines, `match_arm_lambda.mlc` 71 lines, `match_field_binding.mlc` 65 lines, `match_generic_ctor_type.mlc` 125 lines, `match_guarded_gen.mlc` 630 lines), allowlist entry removed. Slice 5 (the guarded-if-chain + string-literal-if-chain statement-builder core, a single flavor-agnostic implementation shared by both the string- and `_cpp`-flavored remaining entry points via a `print_expr` adapter closure, 33 items + 1 type, 7 exports) needed 5 call-site fixes in `compiler/expr_visitor_cpp.mlc` (Decision estimated 3; the missed 2 — `gen_match_guarded_expression_cpp`'s 2nd call site and `match_visit_uses_void_lambdas` — were caught only by the mandatory mlcc2 `g++` bootstrap-binary build failing, not by the mlcc-translate-only bootstrap diff or `rake test_compiler_mlc`, confirming that self-host-build step is load-bearing). Slice 4 dropped 4 now-fully-unused import names from `match_gen.mlc`; Critic confirmed the pre-existing `type_parameter_name_index` name duplicate is namespace-isolated and not a regression. 1 earlier correction during slice 1: a name collision between the newly-exported `match_return_cpp_type` and an unrelated same-named local helper in `expr_visitor_cpp.mlc` triggered a real mlcc codegen bug (mis-qualified the local definition's namespace, duplicate-symbol risk), fixed by renaming the local helper to `match_default_block_return_cpp_type`. Remaining in `match_gen.mlc` (now 414 lines, under the threshold): the string-return-type strategy and the `CppExpression`-IR strategy, both left as-is — file already compliant, a further split is optional, not required by the gate. §104-12 (`transform.mlc` split) **CLOSED** 2026-07-30, re-confirmed by an independent Critic re-audit same day: 1765→753 lines across 5 modules (`transform_coerce/context/call_args/method/support.mlc`), allowlist entry removed. §104-13 (`codegen/decl_cpp.mlc` split) **CLOSED** 2026-07-29: 6 slices, 1666→355 lines across `decl_cpp.mlc` + 5 new modules (`decl_cpp_helpers/type/trait/fn/ffi/extend.mlc`), all ≤800, allowlist entry removed, all Critic-audited. §104-1/2/3 found already pre-existing, see track file.
**`TRACK_EDITOR_COMMENT_TOGGLE_NO_FULL_STRINGIFY` (§100)** **closed** 2026-07-28 (Critic OK). Archived.
**`TRACK_EDITOR_WORD_DELETE_NO_FULL_STRINGIFY` (§99)** **closed** 2026-07-25 (Critic OK). Archived.
**`TRACK_EDITOR_NEWLINE_INDENT_NO_FULL_STRINGIFY` (§98)** **closed** 2026-07-25 (Critic OK). Archived.
**`TRACK_EDITOR_EDIT_AUTOCLOSE_NO_FULL_STRINGIFY` (§93)** **closed** 2026-07-25 (Critic OK). Archived.
**`TRACK_EDITOR_RENDER_ARCHITECTURE` (§97)** **closed** 2026-07-25 (Critic OK). P0 §97a/b/c+§96 done.
**`TRACK_EDITOR_HOVER_SCROLLBAR_PAINT_GAP` (§94) / `DRAG_SELECTION` (§95) / `WHEEL_HOVER` (§96)** **superseded by §97**.
**`TRACK_EDITOR_CLIPBOARD_SLICE_NO_FULL_STRINGIFY` (§92)** **closed** 2026-07-25 (Critic OK). Archived.
**`TRACK_EDITOR_TREE_CLICK_NO_STRINGIFY` (§91)** **closed** 2026-07-25 (Critic OK). Archived.
**`TRACK_EDITOR_DEMO_OPEN_PATH_NO_STRINGIFY` (§90)** **closed** 2026-07-25 (Critic OK). Archived.
**`TRACK_EDITOR_DEMO_SAVE_NO_DOUBLE_STRINGIFY` (§89)** **closed** 2026-07-25 (Critic OK). Archived.
**`TRACK_EDITOR_SESSION_RESTORE_NO_STRINGIFY` (§88)** **closed** 2026-07-25 (Critic OK). Archived.
**`TRACK_EDITOR_DROP_FILE_NO_STRINGIFY` (§87)** **closed** 2026-07-25 (Critic OK). Archived.
**`TRACK_EDITOR_PROBE_REUSE_LINE_INDEX_TEXT` (§86)** **closed** 2026-07-25 (Critic OK). Archived.
**`TRACK_EDITOR_PROBE_REUSE_FRAME_LINE_INDEX` (§85)** **closed** 2026-07-25 (Critic OK). Archived.
**`TRACK_EDITOR_OVERFLOW_REUSE_FRAME_LINE_INDEX` (§84)** **closed** 2026-07-25 (Critic OK). Archived.
**`TRACK_EDITOR_GOTO_LINE_REUSE_FRAME_LINE_INDEX` (§83)** **closed** 2026-07-25 (Critic OK). Archived.
**`TRACK_EDITOR_SELECTION_REUSE_FRAME_LINE_INDEX` (§82)** **closed** 2026-07-25 (Critic OK). Archived.
**`TRACK_EDITOR_SCROLL_REUSE_FRAME_LINE_INDEX` (§81)** **closed** 2026-07-25 (Critic OK). Archived.
**`TRACK_EDITOR_PAINT_REUSE_FRAME_LINE_INDEX` (§80)** **closed** 2026-07-25 (Critic OK). Archived.
**`TRACK_EDITOR_SHARED_SYNTAX_SPAN_CACHE` (§79)** **closed** 2026-07-25 (Critic OK). Archived.
**`TRACK_EDITOR_MINIMAP_HIGHLIGHT_CACHE` (§78)** **closed** 2026-07-25 (Critic OK). Archived.
**`TRACK_EDITOR_MINIMAP_INDICATOR_THEME_TINT` (§65)** **closed** 2026-07-24 (Critic OK). Archived.
**`TRACK_EDITOR_OVERLAY_THEME_TINT` (§64)** **closed** 2026-07-24 (Critic OK). Archived.
**`TRACK_EDITOR_FOLDER_SCROLLBAR_THUMB_DRAG` (§63)** **closed** 2026-07-24 (Critic OK). Archived.
**`TRACK_EDITOR_SCROLLBAR_THEME_TINT` (§62)** **closed** 2026-07-24 (Critic OK). Archived.
**`TRACK_EDITOR_SCROLLBAR_THUMB_DRAG` (§61)** **closed** 2026-07-24 (Critic OK). Archived.
**`TRACK_EDITOR_HORIZONTAL_SCROLLBAR` (§60)** **closed** 2026-07-24 (Critic OK). Archived.
**`TRACK_EDITOR_TEXT_DRAW_SCROLL_X` (§59)** **closed** 2026-07-24 (Critic OK). Archived.
**`TRACK_EDITOR_WRAP_HIT_SCROLL_X` (§58)** **closed** 2026-07-24 (Critic OK). Archived.
**`TRACK_EDITOR_IDLE_MULTI_CARET_OVERLAY` (§57)** **closed** 2026-07-24 (Critic OK). Archived.
**`TRACK_EDITOR_MULTI_CARET_DRAW` (§56)** **closed** 2026-07-24 (Critic OK). Archived.
**`TRACK_EDITOR_MULTI_SELECTION_VISUAL` (§55)** **closed** 2026-07-24 (Critic OK). Archived.
**`TRACK_EDITOR_WRAP_SELECTION_VISUAL` (§54)** **closed** 2026-07-24 (Critic OK). Archived.
**`TRACK_EDITOR_PROPORTIONAL_HIT_TEST` (§53)** **closed** 2026-07-24 (Critic OK). Archived.
**`TRACK_EDITOR_WRAP_BUDGET_ADVANCE` (§52)** **closed** 2026-07-24 (Critic OK). Archived.
**`TRACK_EDITOR_IDLE_CPU_BUDGET` (§51)** **closed** 2026-07-22 (Critic OK). Archived.
**`TRACK_EDITOR_IDLE_CARET_BLINK` (§50)** **closed** 2026-07-22 (Critic OK). Archived.
**`TRACK_EDITOR_SOLID_PASS_CONTENT_CLIP` (§49)** **closed** 2026-07-22 (Critic OK). Archived.
**`TRACK_UX_GATE_AUTO_DISCOVER` (§48)** closed 2026-07-22 (Critic OK); archived.
**`TRACK_EDITOR_WRAPPED_TEXT_BLEEDS_INTO_MINIMAP` (§46 #37)** closed 2026-07-22 (Critic OK); archived.
**`TRACK_EDITOR_IDLE_BUSY_LOOP_92PCT_CPU` (§46 #38)** closed 2026-07-22 (Critic OK); archived.
**`TRACK_EDITOR_UX_BACKLOG` (§46)** **done** 2026-07-22 — #1…#38 closed (#22 superseded).
**`TRACK_CONCURRENCY_TESTRUNTIME_MLC_SURFACE` (§47 #11)** closed 2026-07-22; archived.
**`TRACK_CONCURRENCY_SUPERVISOR_MLC_SURFACE` (§47 #10)** closed 2026-07-21; archived.
**`TRACK_CONCURRENCY_ISOLATE_MLC_SURFACE` (§47 #9)** closed 2026-07-21; archived.
**`TRACK_CONCURRENCY_FFI_METADATA` (§47 #8)** closed 2026-07-21; archived.
**`TRACK_CONCURRENCY_ATOMICS` (§47 #7)** closed 2026-07-21; archived.
**`TRACK_MLC_CONCURRENCY_REFINEMENT` (§47)** **done** 2026-07-22 — `#1`–`#11` closed.
**`TRACK_CONCURRENCY_CHANNEL_RENDEZVOUS_UNBOUNDED` (§47 #6)** closed 2026-07-20; archived.
**`TRACK_CONCURRENCY_CANCELLATION_WAKES_BLOCKING` (§47 #5)** closed 2026-07-20; archived.
**`TRACK_CONCURRENCY_SYNC_TRAIT` (§47 #4)** closed 2026-07-20; archived.
**`TRACK_CONCURRENCY_MOVE_TRACKING` (§47 #3)** closed 2026-07-20; archived.
**`TRACK_CONCURRENCY_SEND_BOUND` (§47 #2)** closed 2026-07-20; archived.
**`TRACK_CONCURRENCY_MUTABLE_CAPTURE_CHECK` (§47 #1)** closed 2026-07-20; archived.
**`TRACK_EDITOR_WRAP_PER_FRAME_ON_LARGE_FILE` (§46 #36)** closed 2026-07-20; archived.
**`TRACK_EDITOR_MINIMAP` (§46 #35)** closed 2026-07-20; archived.
**`TRACK_EDITOR_CONTENT_SCROLLBAR` (§46 #33e)** closed 2026-07-19; archived.
**`TRACK_EDITOR_TREE_PARENT_DOUBLE_CLICK` (§46 #33d)** closed 2026-07-19; archived.
**`TRACK_EDITOR_CHROME_THEME_DRIFT` (§46 #33c)** closed 2026-07-19; archived.
**`TRACK_EDITOR_ROW_BYTE_RANGE_BLEED` (§46 #33b)** closed 2026-07-19; archived.
**`TRACK_EDITOR_TREE_EXPAND_COLLAPSE` (§46 #33)** closed 2026-07-19; archived.
**`TRACK_EDITOR_CHROME_HOVER_STATE` (§46 #32)** closed 2026-07-19; archived.
**`TRACK_EDITOR_TAB_STRIP_CLASSIC_LAYOUT` (§46 #31)** closed 2026-07-19; archived.
**`TRACK_EDITOR_COLUMN_PIXEL_MODEL_FIX` (§46 #31a)** closed 2026-07-19; archived.
**`TRACK_EDITOR_UX_L1_BACKFILL` (§46 #30)** closed 2026-07-19; archived.
**`TRACK_LANG_RECORD_UPDATE` (§46 #29)** closed 2026-07-19; archived.
**`TRACK_EDITOR_WIDE_CHAR_COLUMNS` (§46 #28)** closed 2026-07-19; archived.
**`TRACK_EDITOR_DROP_FILE` (§46 #27)** closed 2026-07-19; archived.
**`TRACK_EDITOR_INDENT_MODEL` (§46 #26)** closed 2026-07-19; archived.
**`TRACK_EDITOR_CONTEXT_MENU` (§46 #25)** closed 2026-07-19; archived.
**`TRACK_EDITOR_TRAILING_WS_VIZ` (§46 #24)** closed 2026-07-19; archived.
**`TRACK_EDITOR_MULTI_CURSOR` (§46 #23)** closed 2026-07-19; archived.
**`TRACK_EDITOR_DIRTY_CLOSE_L1` (§46 #21)** closed 2026-07-19; archived.
**`TRACK_EDITOR_SESSION_CARET_RESTORE` (§46 #20)** closed 2026-07-19; archived.
**`TRACK_EDITOR_COMMENT_TOGGLE` (§46 #19)** closed 2026-07-19; archived.
**`TRACK_EDITOR_BRACKET_AUTOCLOSE` (§46 #18)** closed 2026-07-19; archived.
**`TRACK_EDITOR_TAB_REORDER` (§46 #15)** closed 2026-07-18; archived.
**`TRACK_EDITOR_REPLACE` (§46 #14)** closed 2026-07-18; archived.
**`TRACK_EDITOR_ENCODING_GUARD` (§46 #13)** closed 2026-07-18; archived.
**`TRACK_EDITOR_CRLF_PRESERVE` (§46 #12)** closed 2026-07-18; archived.
**`TRACK_EDITOR_CURRENT_LINE_HL` (§46 #11)** closed 2026-07-18; archived.
**`TRACK_EDITOR_AUTO_INDENT` (§46 #10)** closed 2026-07-18; archived.
**`TRACK_EDITOR_DRAG_AUTOSCROLL` (§46 #9)** closed 2026-07-18; archived.
**`TRACK_EDITOR_GOTO_LINE` (§46 #8)** closed 2026-07-18; archived.
**`TRACK_EDITOR_FIND` (§46 #7)** closed 2026-07-18; archived.
**`TRACK_EDITOR_UNDO_COALESCE` (§46 #6)** closed 2026-07-18; archived.
**`TRACK_EDITOR_SHIFT_CLICK_EXTEND` (§46 #5)** closed 2026-07-18; archived.
**`TRACK_EDITOR_MOUSE_WORD_LINE_SELECT` (§46 #4)** closed 2026-07-18; archived.
**`TRACK_EDITOR_WORD_BOUNDARIES` (§46 #3)** closed 2026-07-18; archived.
**`TRACK_EDITOR_KEYBOARD_NAV_WIRE` (§46 #2)** closed 2026-07-18; archived.
**`TRACK_EDITOR_DEMO_ORCHESTRATOR` (§46 #1)** closed 2026-07-18; archived.
**`TRACK_EDITOR_CLEAN_ARCHITECTURE` (§45)** / §43 / §44 archived.
SCRIPT_VM / AUTO_CYCLE / MIR Epic 5 — gated; do not open.
## Step sizing (good vs bad)

| Good | Bad |
|------|------|
| Step 9: one arm + tests | «Migrate all expr_eval» |
| Planner adds steps 10–12 | Driver invents scope without TRACK |
| Backlog flags uncommitted > 15 | Silent TRACK drift |

## Code rules

- No abbreviations (`.cursor/rules/no-abbreviations.mdc`).
- Minimal diff; match surrounding style.

## Orchestration

Queued identical driver prompts; state in [SESSION.md](SESSION.md) `next`.
No MCP enqueue. Commits without asking the user.
