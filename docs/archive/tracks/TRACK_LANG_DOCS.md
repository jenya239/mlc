# Track: Language reference manual

Parent: [../../PLAN.md](../../PLAN.md) §27.
Trigger: 2026-07-11 backlog review for ~300-step planning horizon. No single
document describes the MLC language end-to-end — syntax cheatsheet exists
only as a compressed block in `CLAUDE.md` (for agent consumption); no
prose reference for a human reader (future external contributor, the gate
condition already named in [TRACK_DEBUG_SOURCE_MAP](../../agent/TRACK_DEBUG_SOURCE_MAP.md)).
Language features are individually documented across ~15 scattered docs
(`FFI_LAYER.md`, `CONCURRENCY_V2.md`, `LANGUAGE_AUDIT_2026_07.md`, ...) with
no single entry point.
Predecessor closed (Critic OK 2026-07-12):
[TRACK_CONCURRENCY_TEST_HARNESS.md](TRACK_CONCURRENCY_TEST_HARNESS.md).

## Status: **closed** (2026-07-12) — Critic OK

**Planner** (2026-07-12): activated after TEST_HARNESS Critic OK.
**Driver** (2026-07-12): STEP=1–11 done — `docs/LANGUAGE_REFERENCE.md` filled
from e2e/demo fixtures; cross-links; `scripts/lang_ref_lint.rb` (33 fences,
8 excerpts→Source). Docs-only; regression_gate N/A.
**Critic** (2026-07-12): STEP=1–11 vs `328cb686`…`022402ad`; PLAN/archive/
cross-links aligned; `lang_ref_lint` re-OK 33/0; archive relative links fixed;
**reopen: none**. Residuals: golden_harness fence stubs helper (support module
not an entry); `extern lib "pq"` form-only (FFI_LAYER); Mutex listing is
pretty-print of `test_mutex_syntax` program string.

## Next step

— (closed; Planner activates `TRACK_STDLIB_DOCS` §28)

## Goal

`docs/LANGUAGE_REFERENCE.md` — one document, organized by language area, each
section with: syntax, semantics in 2-4 sentences, one runnable example
(verified against an actual e2e test or demo file, not invented), link to
the implementing track if one exists. Not a tutorial (no "getting started"
narrative) — a reference, skimmable, accurate.

## Steps

| Step | Item | Status |
|------|------|--------|
| 1 | Skeleton + sections list: bindings (`let`/`const`/`let mut`/`let const`), functions, types (record/sum/generic), pattern matching (`match`, or-patterns, guards), traits (`extend`, orphan rule), error handling (`Result`/`Option`, `?`, `T!E`), closures + escape analysis, arrays/maps/strings + HOFs, concurrency (`spawn`/`Mutex`/`Channel`/`scope`), FFI (`extern fn`/`extern type`/`extern lib`) | **done** (2026-07-12) — `docs/LANGUAGE_REFERENCE.md` TOC + 10 section stubs |
| 2 | Section: bindings + functions — examples sourced from `compiler/tests/e2e/` (grep for the simplest matching fixture, do not write new prose-only examples) | **done** (2026-07-12) — `const`/`let` from e2e; `let mut` from `loops_demo`; `fn` one-line/multi-line from e2e; `export fn` from `golden_harness` |
| 3 | Section: types (record/sum/generic) + pattern matching | **done** (2026-07-12) — record/sum/Result e2e; match+guards e2e; or-pattern from `test_mir_vm_smoke` string fixture |
| 4 | Section: traits + orphan rule | **done** (2026-07-12) — inherent extend / trait impl / trait param from e2e; orphan E086 → TRACK_LANG_ORPHAN_RULE |
| 5 | Section: error handling (`Result`/`Option`/`?`/`T!E`) | **done** (2026-07-12) — Result+`?` e2e; `T!E` from error_union fixture; Option from `vm_option` |
| 6 | Section: closures + escape analysis (link `archive/tracks/TRACK_LANG_CLOSURE_ESCAPE.md` for the "why") | **done** (2026-07-12) — `vm_lambda` / `vm_lambda_immediate` / HOF lambdas; escape track link |
| 7 | Section: arrays/maps/strings + HOFs (`map`/`filter`/`fold`) | **done** (2026-07-12) — `empty_array` e2e; `array_hof_demo`; `vm_map`; `string_operations_demo` |
| 8 | Section: concurrency — link `CONCURRENCY_V2.md` for the full spec, this section is the condensed "how to use it" version with the accept-loop demo as the canonical example | **done** (2026-07-12) — spawn/scope e2e; accept-loop demo; Mutex from test string; CONCURRENCY_V2 link |
| 9 | Section: FFI — link `FFI_LAYER.md`, condensed version + the safety contract (`unsafe` framing from `TRACK_FFI_SAFETY`) | **done** (2026-07-12) — extern fn/type from fixtures; blocking from env.mlc; safety table + FFI_LAYER/SAFETY links |
| 10 | Cross-link from `README.md` + `AGENTS.md`/`docs/specs/index.md` so it is discoverable | **done** (2026-07-12) — README Roadmap; AGENTS On-demand; specs index row |
| 11 | Verify: every code example in the doc actually compiles (`mlcc -o /tmp/lang_ref_check <extracted example>`) — write a tiny extraction script (Ruby, per scripts-language rule) that pulls fenced ` ```mlc ` blocks and compiles each; wire as a doc-lint, not full regression_gate (docs-only, no self-host diff needed) | **done** (2026-07-12) — `scripts/lang_ref_lint.rb`; `mlc` wrap-compile; `mlc excerpt`→Source; 33/0 |

## Progress

- **Planner** (2026-07-12): activated after TEST_HARNESS Critic OK; STEP=1 next.
- **STEP=1–10** (2026-07-12): reference filled + cross-links (see prior SESSION turns).
- **STEP=11** (2026-07-12): `scripts/lang_ref_lint.rb`; excerpt fences for truncated listings; golden_harness stub helper in fence (support module not a standalone entry).

## Out of scope

- Tutorial/getting-started narrative (separate concern if ever requested).
- API docs for stdlib modules (`Tcp`/`HttpServer`/...) — see [TRACK_STDLIB_DOCS](TRACK_STDLIB_DOCS.md).
- Auto-generated docs from source comments (no doc-comment convention exists
  yet in MLC; not introducing one speculatively).
