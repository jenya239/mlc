# Track: C++ header import

Parent: [../PLAN.md](../PLAN.md) §Phase 3.5; design: [../CPP_PARSER_DESIGN.md](../CPP_PARSER_DESIGN.md); follows [TRACK_LSP.md](TRACK_LSP.md)

## Status: **planned** (blocked: depends on TRACK_LSP)

**Depends on:** TRACK_LSP closed (stable checker/registry); Cpp lexer/ast baseline in `compiler/cpp/`.

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
| 1 | Extend `cpp/cpp_lexer.mlc` + `cpp/cpp_tokens.mlc` — keywords, `#` token, block comments; roundtrip smoke | pending |
| 2 | Extend `cpp_ast.mlc` — `CppTypePtr`, `CppTypeConst`, `CppTypeRRef`, forward decls | pending |
| 3 | `cpp/parser/types.mlc` — parse qualifiers, pointers, templates | pending |
| 4 | `cpp/parser/decls.mlc` — struct/class, fn proto, enum, namespace; error recovery | pending |
| 5 | `import "header.h"` wiring + registry; tests; close track | pending |

### Out of scope

- Full C++20 parser
- Linking external objects (only type/fn registration)
