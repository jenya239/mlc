# Track: Full C++ header parser (Ruby parity)

Parent: [../PLAN.md](../PLAN.md) §Phase 3.6; design: [../CPP_PARSER_DESIGN.md](../CPP_PARSER_DESIGN.md); follows [TRACK_CPP_HEADER_IMPORT.md](TRACK_CPP_HEADER_IMPORT.md)

## Status: **open** STEP=1

**Depends on:** TRACK_CPP_HEADER_IMPORT closed (minimal subset); TRACK_LSP closed or STEP=5 done.

**Baseline:** build_tests **1143/2**; minimal parser ~2.1k LOC vs Ruby `lib/cpp_ast/` ~11.6k LOC; self-host **fail** on `cpp_decls` codegen.

**Goal:** ??????????? ?????? C++ ?????????? ? trivia roundtrip, expressions, class bodies, template decls — ??????? ? Ruby ??? STL/real headers. ????????: [CPP_PARSER_DESIGN.md](../CPP_PARSER_DESIGN.md).

## Verify gate

```
compiler/tests/build_tests.sh
compiler/build.sh
compiler/out/mlcc -o .tmp_selfhost/p1 compiler/main.mlc
compiler/build_bin.sh .tmp_selfhost/p1 .tmp_selfhost/mlcc2
.tmp_selfhost/mlcc2 --check-only compiler/main.mlc
# roundtrip suite (?10 cases from CPP_PARSER_DESIGN)
# differential: mlcc cpp_parse vs Ruby lib/cpp_ast on fixtures/
```

---

| Step | Item | Status |
|------|------|--------|
| 1 | Fix self-host codegen `cpp_decls.mlc` — `build_bin` green | pending |
| 2 | Trivia model — `leading_trivia`/`trailing_trivia` on `CppToken`; lexer collects comments/`#pragma` | pending |
| 3 | `parser/to_source.mlc` — AST ? source via trivia; lexer roundtrip tests | pending |
| 4 | Extend `cpp_ast.mlc` — class members, access levels, fn modifiers, template decls, typedef | pending |
| 5 | Expression parser (Pratt) — `parser/cpp_exprs.mlc`; literals char/float/hex | pending |
| 6 | Full decl parser — class bodies, template decls, extern blocks, attributes `[[...]]` | pending |
| 7 | Structural + roundtrip tests (?20 + ?10); differential vs Ruby on fixture corpus | pending |
| 8 | Re-wire `header_import.mlc`; fix fixture paths; close track | pending |

### Out of scope

- Full C++20 / coroutines / modules
- `#define` / `#ifdef` evaluation (structural markers only)
- Linking external objects

### Gap vs minimal subset (TRACK_CPP_HEADER_IMPORT)

| ???? | ????? |
|------|-------|
| include, using, struct, fn proto, enum, namespace | class bodies (`public:`/`private:`, methods) |
| kind-roundtrip smoke | `parse ? to_source == src` |
| basic templates in types | `template<typename T> T max(...)` as decl |
| error recovery | Pratt expressions, char/float/hex literals |
| — | self-host codegen fix |

---

## Per-turn template (Scribe — every turn)

```
| turn | <ISO date> |
| role | Driver / Planner / Backlog |
| step | <e.g. 3 or plan-refresh> |
| done | <one line> |
| verify | <pass/fail/pending + command> |
| next | ROLE=… STEP=… |
```
