# Track: Code quality (compiler codebase)

Parent: [../PLAN.md](../PLAN.md) §Phase 2.6 + [CODE_REVIEW_2026_06.md](../CODE_REVIEW_2026_06.md); follows [TRACK_PARSE_PROGRAM_RESULT.md](TRACK_PARSE_PROGRAM_RESULT.md)

## Status: **open** (STEP=1 pending)

**Depends on:** TRACK_PARSE_PROGRAM_RESULT closed.

**Baseline:** build_tests **1005/0**; **diff_exit=0**; `bf6c46e8`.

**Goal:** adopt language features already supported by mlcc in `compiler/` — less boilerplate, fewer duplicate match arms.

## Verify gate

```
compiler/tests/build_tests.sh
compiler/build.sh
compiler/out/mlcc -o .tmp_selfhost/p1 compiler/main.mlc
compiler/build_bin.sh .tmp_selfhost/p1 .tmp_selfhost/mlcc2
.tmp_selfhost/mlcc2 -o .tmp_selfhost/p2 compiler/main.mlc
diff -rq .tmp_selfhost/p1 .tmp_selfhost/p2
```

---

| Step | Item | Status |
|------|------|--------|
| 1 | Record destructuring in `let` — lexer/parser hot paths (`frontend/lexer.mlc`, `cpp/lexer.mlc`) | pending |
| 2 | HOF in codegen — `.map`/`.fold` in `cpp_ast.mlc` printer paths (replace manual `while`+`push`) | pending |
| 3 | Or-patterns in checker — collapse duplicate variant arms (`check.mlc`, `derive_validation.mlc`) | pending |
| 4 | String-match adoption — `keyword_kind` and similar if-chains → `match` on `string` | pending |
| 5 | Audit + close — grep survivors; gate | pending |

### Out of scope

- `lib/mlc/` Ruby parity
- parser `ref mut`
