# Track: C++ header import

Parent: [../PLAN.md](../PLAN.md) §Phase 3.5; design: [../CPP_PARSER_DESIGN.md](../CPP_PARSER_DESIGN.md); follows [TRACK_LSP.md](TRACK_LSP.md)

## Status: **open** STEP=5

**Baseline:** build_tests **1117/0**; build.sh ok; **diff_exit=0**.

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
| 3 | `cpp/parser/cpp_types.mlc` — parse qualifiers, pointers, templates | done |
| 4 | `cpp/parser/cpp_decls.mlc` — struct/class, fn proto, enum, namespace; error recovery | done |
| 5 | `import "header.h"` wiring + registry; tests; close track | pending |

**STEP=1 note (2026-05-19 Driver):** `cpp_tokens.mlc` — token kinds, header keywords (`class`…`friend`), `CHash`, `cpp_token_lexeme`/`cpp_tokens_to_source`. `lexer.mlc` — imports tokens, `#` → `CHash`, `//`/`/* */` via `skip_whitespace`. `predicates.mlc` — `cpp_is_hash` → `CHash`. `test_cpp_lexer.mlc` — keyword/`CHash`/roundtrip smoke. Gate **1030/0**; **diff_exit=0**.

**STEP=2 note (2026-05-19 Driver):** `cpp_ast.mlc` — `CppTypePtr`/`CppTypeConst`/`CppTypeRRef` + `print_type`; `CppForwardDecl(kind, name)` + printer (`class`/`enum`/`struct`). Tests: `test_cpp_ast.mlc`, `test_cpp_printer.mlc`. Gate **1033/0**; **diff_exit=0**.

**STEP=3 note (2026-06-21 Driver):** `cpp/parser/cpp_types.mlc` — qualifiers, qualified names, templates, `*`/`&`/`&&` (via `CAnd`). Renames for module stability: `cpp_lexer`, `cpp_predicates`, `cpp_parser`. `cpp_parser` uses type parser for using/struct fields. Tests: `test_cpp_types`, `test_cpp_parser`. Export `LexState` helpers from `frontend/lexer`. Gate **1111/0**; **diff_exit=0**.

**STEP=4 note (2026-06-21 Driver):** `cpp/parser/cpp_decls.mlc` — include/using/struct/class/enum/namespace/fn proto/variable fragment; forward decls; error recovery. Fix `cpp_type_join_qualified_name` (stop before declarator name). Tests extended in `test_cpp_parser`. Gate **1117/0**; **diff_exit=0**.

**Meta note (2026-05-19):** Guard `Driver:3:CPP_HEADER_IMPORT` drained — STEP=3 **not started** (no `compiler/cpp/parser/types.mlc`). STEP=1–2 changes **uncommitted** in worktree. Do not re-enqueue STEP=3; Driver resumes from SESSION payload.

### Out of scope

- Full C++20 parser
- Linking external objects (only type/fn registration)
- Trivia-preserving roundtrip (deferred; kind-roundtrip smoke only)
