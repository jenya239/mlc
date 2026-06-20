# Track: C++ header import

Parent: [../PLAN.md](../PLAN.md) §Phase 3.5; design: [../CPP_PARSER_DESIGN.md](../CPP_PARSER_DESIGN.md); follows [TRACK_LSP.md](TRACK_LSP.md)

## Status: **planned** (blocked: depends on TRACK_LSP)

**Baseline:** build_tests **1033/0**; build.sh ok; **diff_exit=0**.

**Goal:** parse subset of C++ headers; register types/fn protos for `import "foo.h"`.

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
| 1 | Extend `cpp/lexer.mlc` + `cpp/cpp_tokens.mlc` — keywords, `CHash`, block comments; roundtrip smoke | done |
| 2 | Extend `cpp_ast.mlc` — `CppTypePtr`, `CppTypeConst`, `CppTypeRRef`, forward decls | done |
| 3 | `cpp/parser/types.mlc` — parse qualifiers, pointers, templates | pending |
| 4 | `cpp/parser/decls.mlc` — struct/class, fn proto, enum, namespace; error recovery | pending |
| 5 | `import "header.h"` wiring + registry; tests; close track | pending |

**STEP=1 note (2026-05-19 Driver):** `cpp_tokens.mlc` — token kinds, header keywords (`class`…`friend`), `CHash`, `cpp_token_lexeme`/`cpp_tokens_to_source`. `lexer.mlc` — imports tokens, `#` → `CHash`, `//`/`/* */` via `skip_whitespace`. `predicates.mlc` — `cpp_is_hash` → `CHash`. `test_cpp_lexer.mlc` — keyword/`CHash`/roundtrip smoke. Gate **1030/0**; **diff_exit=0**.

**STEP=2 note (2026-05-19 Driver):** `cpp_ast.mlc` — `CppTypePtr`/`CppTypeConst`/`CppTypeRRef` + `print_type`; `CppForwardDecl(kind, name)` + printer (`class`/`enum`/`struct`). Tests: `test_cpp_ast.mlc`, `test_cpp_printer.mlc`. Gate **1033/0**; **diff_exit=0**.

**Meta note (2026-05-19):** Guard `Driver:3:CPP_HEADER_IMPORT` drained — STEP=3 **not started** (no `compiler/cpp/parser/types.mlc`). STEP=1–2 changes **uncommitted** in worktree. Do not re-enqueue STEP=3; Driver resumes from SESSION payload.

### Out of scope

- Full C++20 parser
- Linking external objects (only type/fn registration)
- Trivia-preserving roundtrip (deferred; kind-roundtrip smoke only)
