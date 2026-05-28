# Track: Security — fuzz depth and crash-safety (Phase 1 continuation)

Parent: [../PLAN.md](../PLAN.md) §Phase 1 §3–§4; previous: [TRACK_SAFETY.md](TRACK_SAFETY.md) (**closed**, `32f8335`), [TRACK_PERFORMANCE.md](TRACK_PERFORMANCE.md) (**closed**, `0671422`)

## Status: in progress (step 4 pending)

**Goal:** broaden invalid-input coverage; no core dump on garbage; structured negative tests beyond SAFETY baseline.

**Constraints:**
- One concern per sub-step; no `compiler/` + `lib/mlc/` in one commit.
- Self-host gate when `compiler/**` touched: `build_bin.sh` + diff empty.
- Shell fuzz gates must stay fast (<30s in `build_tests.sh`).

## Verify gate (every step)

```
bundle exec rake test_compiler_mlc   # 607 pass (baseline post-PERF)
compiler/build.sh                    # when compiler/** touched
compiler/out/mlcc -o .tmp_selfhost/p1 compiler/main.mlc
compiler/build_bin.sh .tmp_selfhost/p1 .tmp_selfhost/mlcc2
.tmp_selfhost/mlcc2 -o .tmp_selfhost/p2 compiler/main.mlc
diff -rq .tmp_selfhost/p1 .tmp_selfhost/p2   # empty
```

Fuzz gates (when `compiler/tests/fuzz/**` or `test_fuzz.mlc` touched):

```
compiler/tests/fuzz/run_fuzz_smoke.sh compiler/out/mlcc
compiler/tests/fuzz/run_negative_corpus.sh compiler/out/mlcc
```

---

| Step | Item | Status |
|------|------|--------|
| 1 | Negative corpus expansion — parser/checker edge `.mlc` files (8→16+) | done (`d9aa33b`) |
| 2 | Fuzz generator — record/type/trait variants; sync `run_fuzz_smoke.sh` | done (`e54e0c0`) |
| 3 | In-process fuzz sweep — widen `test_fuzz.mlc` seed range + garbage lex patterns | done (`c94797d`) |
| 4 | Frontend panic audit — replace silent failure paths in one module (lexer or parser) | pending |

## Step 1 detail (done — `d9aa33b`)

- Added 8 files in `negative_corpus/` (8→16): unclosed block, orphan `end`, bad generic arity, invalid trait syntax, empty type body, duplicate `fn`, unclosed record type, wrong call arity.
- Gate: 607 pass; `run_negative_corpus.sh` ok (16 files); no compiler source changes.

## Step 2 detail (done — `e54e0c0`)

- `random_program.mlc`: 9→12 kinds — type stub, record literal, nested `do`/`end`; exports `fuzz_variant_count`, `fuzz_program_kind`.
- `run_fuzz_smoke.sh` synced (% 12, cases 9–11).
- `test_fuzz.mlc`: variant count assert + kind-specific seeds; 614 pass.

## Step 3 detail (done — `c94797d`)

- Pipeline loop seeds 0..63 (was 0..31); `fuzz_deep_nesting_source` helper (depth 24).
- Garbage lex: unclosed string, deep `do`/`end` nesting; no new shell scripts.
- Gate: 680 pass (was 614).

## Step 4 detail

- Grep `unreachable` / empty match arms in `compiler/frontend/` (lexer, parser, predicates).
- One module per commit; `panic` with message on paths reachable from fuzz inputs.

## Deferred (not in this track)

- Differential fuzz mlcc vs Ruby — [RESEARCH.md](RESEARCH.md).
- treereduce ICE minimizer.
- `--emit=mir` artifact for structured fuzz.
- Property-based / QuickCheck-style generator.

## Next step (Driver)

**STEP=4** — Frontend panic audit (one module).
