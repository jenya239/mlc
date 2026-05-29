# Track: Parser parity — negative corpus vs Ruby (Phase 1 §3 cont.)

Parent: [../PLAN.md](../PLAN.md) §Phase 1 §3; previous: [TRACK_SPAN_IR.md](TRACK_SPAN_IR.md) (**closed**, `e826f1a`)

## Status: **active** (step 5 pending)

**Goal:** mlcc `--check-only` exit codes match Ruby checker on `negative_corpus/`; shrink `negative_corpus_known_divergences.txt` to zero (or document intentional mlcc-stricter cases).

**Constraints:**
- One concern per sub-step; no `compiler/` + `lib/mlc/` in one commit.
- Self-host gate when `compiler/**` touched: `build_bin.sh` + diff empty.
- Keep differential gate fast (<60s in `build_tests.sh`).

## Verify gate (every step)

```
bundle exec rake test_compiler_mlc   # 755 pass (baseline post SPAN_IR)
compiler/build.sh                    # when compiler/** touched
compiler/out/mlcc -o .tmp_selfhost/p1 compiler/main.mlc
compiler/build_bin.sh .tmp_selfhost/p1 .tmp_selfhost/mlcc2
.tmp_selfhost/mlcc2 -o .tmp_selfhost/p2 compiler/main.mlc
diff -rq .tmp_selfhost/p1 .tmp_selfhost/p2   # empty
```

Differential (when parser/lexer or fuzz lists touched):

```
compiler/tests/fuzz/run_fuzz_differential.sh compiler/out/mlcc
compiler/tests/fuzz/run_negative_corpus.sh compiler/out/mlcc
```

---

| Step | Item | Status |
|------|------|--------|
| 1 | `parse_unclosed_block.mlc` + `parse_unclosed_record_type.mlc` — structured parse error, exit 1 | done |
| 2 | `parse_empty_type_body.mlc` + `parse_invalid_trait_syntax.mlc` | done (`221fae6`) |
| 3 | `parse_noise.mlc` — reject trailing garbage | done (`acadaa0`) |
| 4 | `lex_unclosed_string.mlc` — parity decision + differential list update | done (`17e4587`) |
| 5 | Audit `negative_corpus_known_divergences.txt`; close track | pending |

## Known divergences (intentional mlcc-stricter)

Documented in `negative_corpus_known_divergences.txt` (2 files; exit parity not required):

| File | mlcc | Ruby | Decision |
|------|------|------|----------|
| `lex_unclosed_string.mlc` | 1 | 0 | **Keep mlcc-stricter** — lexer rejects `unterminated string`; Ruby parser accepts. Security: fail closed at lex. |
| `parse_orphan_end.mlc` | 1 | 0 | **Keep mlcc-stricter** — parse error on orphan `end`; Ruby accepts. |

Parse-side corpus items (steps 1–3) reached mlcc=1 ruby=1 via `--check-only` parse propagation.

## Step 1 detail

- Files: `compiler/frontend/parser/decls.mlc` and/or `exprs.mlc` only.
- Targets: unclosed `do`/`end` block body; unclosed record type `{`.
- Tests: extend `compiler/tests/test_parser.mlc` + remove entries from known_divergences when fixed.
- Run `run_negative_corpus.sh` + `run_fuzz_differential.sh`.

## Deferred (not in this track)

- Parser `ref mut` — [TRACK_PLAN.md](TRACK_PLAN.md) step 15, separate branch.
- Full AST/codegen diff mlcc vs Ruby — out of scope.
- `lib/mlc/` parity — not in scope.
- Record default expression parity (E4) — separate track if needed.

## Step 2 detail

- Audit only: parity via step 1 `--check-only` parse propagation (mlcc=1 ruby=1 both files).
- Parser errors: `expected type body after =`; `expected } in trait body` (decls.mlc).
- Tests: corpus-shaped `test_parser.mlc` assertions; drop misleading `parser_garbage_smoke` for invalid trait.

## Step 3 detail

- Audit only: parity via step 1 `--check-only` (mlcc=1 ruby=1); corpus `fn @@@`.
- Parser errors: `expected identifier in function name`, `unexpected token in expression`.
- Test: corpus-shaped `test_parser.mlc`; drop misleading `parser_garbage_smoke` for parse_noise.

## Step 4 detail

- **Decision:** keep mlcc-stricter for unterminated strings (lex fail-closed); no Ruby/`lib/mlc/` change in this track.
- Corpus: `fn f() -> string = "unclosed` → mlcc exit 1 (`lex: unterminated string`), Ruby exit 0.
- Tests: corpus-shaped `test_lexer.mlc`; known_divergences comment updated.
- Differential list unchanged (still `1 0`); documented as intentional.

## Next step (Driver)

**STEP=5** — audit `negative_corpus_known_divergences.txt`; close track.
