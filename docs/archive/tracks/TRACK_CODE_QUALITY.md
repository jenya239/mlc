# Track: Code quality (compiler codebase)

Parent: [../PLAN.md](../PLAN.md) §Phase 2.6 + [CODE_REVIEW_2026_06.md](../CODE_REVIEW_2026_06.md); follows [TRACK_PARSE_PROGRAM_RESULT.md](TRACK_PARSE_PROGRAM_RESULT.md)

## Status: **closed** (STEP=5 done)

**Closed:** 2026-06-19. Gate: build_tests **1005/0**; **diff_exit=0**.

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
| 1 | Record destructuring in `let` — lexer/parser hot paths (`frontend/lexer.mlc`, `cpp/lexer.mlc`) | done |
| 2 | HOF in codegen — `.map`/`.fold` in `cpp_ast.mlc` printer paths (replace manual `while`+`push`) | done |
| 3 | Or-patterns in checker — collapse duplicate variant arms (`check.mlc`, `derive_validation.mlc`) | done |
| 4 | String-match adoption — `keyword_kind` and similar if-chains → `match` on `string` | done |
| 5 | Audit + close — grep survivors; gate | done |

### Audit survivors (deferred, out of track scope)

| Pattern | Location | Note |
|---------|----------|------|
| `keyword_kind` split (`is_reserved` + `reserved_kind`) | `frontend/lexer.mlc`, `cpp/lexer.mlc` | bootstrap: no `Ident(word)` in string-match wildcard |
| `while`+`push` string escape | `codegen/cpp_naming.mlc` `escape_str` | STEP=2 scoped `cpp_ast.mlc` only |
| `else if name ==` chains | `codegen/decl/type_gen.mlc` | contextual, not keyword dispatch |
| Large `while`+`push` in codegen/checker | `module.mlc`, `decl_index.mlc`, … | future COW/fold audits |

### Out of scope

- `lib/mlc/` Ruby parity
- parser `ref mut`
