# Track: Parser panic audit (Phase 1 §4 cont.)

Parent: [../PLAN.md](../PLAN.md) §Phase 1 §4; previous: [TRACK_FUZZ_DIFF.md](TRACK_FUZZ_DIFF.md) (**closed**, `5463361`), [TRACK_SECURITY.md](TRACK_SECURITY.md) (**closed**, `a035c3d`, deferred parser audit)

## Status: **closed** (`5525d03`)

**Closed:** step 5 span_unknown audit — 26 sites in `decls.mlc` + `exprs.mlc`, all intentional synthetics (no silent-failure paths).

**Goal:** parser modules emit structured parse/lex errors instead of silent failure or internal panic paths; no crash on garbage input.

**Constraints:**
- One concern per sub-step; no `compiler/` + `lib/mlc/` in one commit.
- Self-host gate when `compiler/**` touched: `build_bin.sh` + diff empty.
- One parser module per step (mirror lexer audit in SECURITY step 4).

## Verify gate (every step)

```
bundle exec rake test_compiler_mlc   # 738 pass (baseline post-step 4)
compiler/build.sh                    # when compiler/** touched
compiler/out/mlcc -o .tmp_selfhost/p1 compiler/main.mlc
compiler/build_bin.sh .tmp_selfhost/p1 .tmp_selfhost/mlcc2
.tmp_selfhost/mlcc2 -o .tmp_selfhost/p2 compiler/main.mlc
diff -rq .tmp_selfhost/p1 .tmp_selfhost/p2   # empty
```

Fuzz gates (when `compiler/tests/fuzz/**` touched):

```
compiler/tests/fuzz/run_fuzz_smoke.sh compiler/out/mlcc
compiler/tests/fuzz/run_negative_corpus.sh compiler/out/mlcc
```

---

| Step | Item | Status |
|------|------|--------|
| 1 | `parser/decls.mlc` — audit silent/`span_unknown` paths; structured parse errors on decl edge cases | done (`b880aed`) |
| 2 | `parser/exprs.mlc` — same for expression parsing edge cases | done (`baf2c8f`) |
| 3 | `test_parser.mlc` — negative tests for new parse error paths | done (`e2954e1`) |
| 4 | In-process parser garbage smoke (unclosed/noise) in `test_parser.mlc` | done (`558069f`) |
| 5 | Parser `span_unknown` grep audit; close track | done (`5525d03`) |

## Step 5 audit (`span_unknown` in `compiler/frontend/parser/`)

26 occurrences — reviewed, **no silent-failure paths**; no code changes.

| Category | Count | Location | Rationale |
|----------|------:|----------|-----------|
| Synth Param/Pat/Expr defaults | 11 | decls, exprs | Placeholder AST nodes (self param, optional defaults, field defaults) |
| Pipe / template desugar | 5 | exprs | Synthetic `ExprCall`/`ExprBin`; real span propagation deferred |
| Block / if tail fallbacks | 5 | exprs | Empty block or missing else — structural `ExprUnit` |
| `__skip__` error-recovery decl | 1 | decls | Dummy body after `record_parse_error` (decl filtered from program) |
| Non-extern `extern_keyword_span` | 1 | decls | Dead branch when `is_extern` is false |
| Match guard placeholder | 1 | exprs | Default `true` before optional `if` guard |
| Param destructure marker | 1 | decls | Internal `plain_identifier_parameter_pattern_marker` |

Checker/codegen `span_unknown` — out of scope (see Deferred).

## Step 1 detail

- File: `compiler/frontend/parser/decls.mlc` only.
- Target: high-traffic decl parsers (`type`, `fn`, `trait`, `extend`) — replace recoverable silent paths with `parse_error` + span.
- Tests: extend `test_parser.mlc` only if needed for step 1 paths (else step 3).
- Do not touch `exprs.mlc` or lexer in this step.

## Deferred (not in this track)

- Parser `ref mut` — [TRACK_PLAN.md](TRACK_PLAN.md) step 15, separate branch.
- `--emit-compile-commands` / build determinism — future TRACK_BUILD.
- Full `span_unknown` sweep in checker/codegen — separate track if needed after parser.
- Differential fuzz mlcc vs Ruby — closed [TRACK_FUZZ_DIFF.md](TRACK_FUZZ_DIFF.md).

## Next step

Track **closed**. Planner: plan-refresh if new work from PLAN §Phase 1 §4.
