# Track: Parser parity — negative corpus vs Ruby (Phase 1 §3 cont.)

Parent: [../PLAN.md](../PLAN.md) §Phase 1 §3; previous: [TRACK_SPAN_IR.md](TRACK_SPAN_IR.md) (**closed**, `e826f1a`)

## Status: **closed** (commit TBD)

**Closed:** step 5 audit — 14 corpus files at exit parity; 2 intentional mlcc-stricter divergences documented in `negative_corpus_known_divergences.txt`.

**Goal:** mlcc `--check-only` exit codes match Ruby checker on `negative_corpus/`; shrink `negative_corpus_known_divergences.txt` to zero (or document intentional mlcc-stricter cases).

**Constraints:**
- One concern per sub-step; no `compiler/` + `lib/mlc/` in one commit.
- Self-host gate when `compiler/**` touched: `build_bin.sh` + diff empty.
- Keep differential gate fast (<60s in `build_tests.sh`).

## Verify gate (every step)

```
bundle exec rake test_compiler_mlc   # 756 pass (baseline post step 4)
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
| 5 | Audit `negative_corpus_known_divergences.txt`; close track | done (TBD) |

## Known divergences (intentional mlcc-stricter)

Documented in `negative_corpus_known_divergences.txt` (2 files; exit parity not required):

| File | mlcc | Ruby | Decision |
|------|------|------|----------|
| `lex_unclosed_string.mlc` | 1 | 0 | **Keep mlcc-stricter** — lexer rejects `unterminated string`; Ruby parser accepts. Fail closed at lex. |
| `parse_orphan_end.mlc` | 1 | 0 | **Keep mlcc-stricter** — parse error on orphan `end`; Ruby accepts. |

Parse-side corpus items (steps 1–3) reached mlcc=1 ruby=1 via `--check-only` parse propagation.

## Step 5 audit

- Verified both known divergences stable: mlcc=1 ruby=0 (lex unterminated string; parse unexpected token at declaration).
- Differential gate: 14 parity + 2 known; no unexpected mismatches; checker-negative error-count parity ok.
- Replaced misleading `parser smoke: orphan end completes` with corpus-shaped parse error test.

## Deferred (not in this track)

- Parser `ref mut` — [TRACK_PLAN.md](TRACK_PLAN.md) step 15, separate branch.
- Full AST/codegen diff mlcc vs Ruby — out of scope.
- `lib/mlc/` parity — not in scope.
- Record default expression parity (E4) — separate track if needed.

## Next step

Track **closed**. Planner: plan-refresh.
