# Track: Language reference manual

Parent: [../PLAN.md](../PLAN.md) §27.
Trigger: 2026-07-11 backlog review for ~300-step planning horizon. No single
document describes the MLC language end-to-end — syntax cheatsheet exists
only as a compressed block in `CLAUDE.md` (for agent consumption); no
prose reference for a human reader (future external contributor, the gate
condition already named in [TRACK_DEBUG_SOURCE_MAP](TRACK_DEBUG_SOURCE_MAP.md)).
Language features are individually documented across ~15 scattered docs
(`FFI_LAYER.md`, `CONCURRENCY_V2.md`, `LANGUAGE_AUDIT_2026_07.md`, ...) with
no single entry point.
Predecessor closed (Critic OK 2026-07-12):
[../archive/tracks/TRACK_CONCURRENCY_TEST_HARNESS.md](../archive/tracks/TRACK_CONCURRENCY_TEST_HARNESS.md).

## Status: **active** (Planner 2026-07-12) — очередь §27

## Next step

**STEP=6** — Closures + escape analysis (link `TRACK_LANG_CLOSURE_ESCAPE`; cite e2e/demo fixture).

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
| 6 | Section: closures + escape analysis (link `archive/tracks/TRACK_LANG_CLOSURE_ESCAPE.md` for the "why") | pending |
| 7 | Section: arrays/maps/strings + HOFs (`map`/`filter`/`fold`) | pending |
| 8 | Section: concurrency — link `CONCURRENCY_V2.md` for the full spec, this section is the condensed "how to use it" version with the accept-loop demo as the canonical example | pending |
| 9 | Section: FFI — link `FFI_LAYER.md`, condensed version + the safety contract (`unsafe` framing from `TRACK_FFI_SAFETY`) | pending |
| 10 | Cross-link from `README.md` + `AGENTS.md`/`docs/specs/index.md` so it is discoverable | pending |
| 11 | Verify: every code example in the doc actually compiles (`mlcc -o /tmp/lang_ref_check <extracted example>`) — write a tiny extraction script (Ruby, per scripts-language rule) that pulls fenced ` ```mlc ` blocks and compiles each; wire as a doc-lint, not full regression_gate (docs-only, no self-host diff needed) | pending |

### STEP=1 sub-steps (Driver)

1. Create `docs/LANGUAGE_REFERENCE.md` with title, one-paragraph scope (reference not tutorial; examples must cite real e2e/demo paths), and a TOC linking to the section headings below.
2. Add stub `##` headings only (no filled prose yet) for: Bindings; Functions; Types; Pattern matching; Traits; Error handling; Closures; Arrays, maps, strings; Concurrency; FFI. Under each stub: `Status: pending — filled in STEP=N` and a bullet list of planned topics from the Steps table.
3. Do **not** invent example code in STEP=1; do **not** edit README/AGENTS (STEP=10). Point parent link to PLAN §27.
4. Verify: file exists; TOC anchors match headings; TRACK Status stays **active**, Next=STEP=2 after mark STEP=1 done.

## Progress

- **Planner** (2026-07-12): activated after TEST_HARNESS Critic OK; STEP=1 next.
- **STEP=1** (2026-07-12): `docs/LANGUAGE_REFERENCE.md` skeleton + TOC + stubs; no invented examples.
- **STEP=2** (2026-07-12): Bindings + Functions filled from e2e / cited fixtures.
- **STEP=3** (2026-07-12): Types + Pattern matching filled from e2e / mir smoke fixture.
- **STEP=4** (2026-07-12): Traits + orphan rule from e2e + archive track link.
- **STEP=5** (2026-07-12): Error handling Result/`?`/`T!E`/Option from fixtures.

## Out of scope

- Tutorial/getting-started narrative (separate concern if ever requested).
- API docs for stdlib modules (`Tcp`/`HttpServer`/...) — see [TRACK_STDLIB_DOCS](TRACK_STDLIB_DOCS.md).
- Auto-generated docs from source comments (no doc-comment convention exists
  yet in MLC; not introducing one speculatively).
