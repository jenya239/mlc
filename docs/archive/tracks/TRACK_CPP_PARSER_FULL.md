# Track: Full C++ header parser (Ruby parity)

Parent: [../PLAN.md](../PLAN.md) �Phase 3.6; design: [../CPP_PARSER_DESIGN.md](../CPP_PARSER_DESIGN.md); follows [TRACK_CPP_HEADER_IMPORT.md](TRACK_CPP_HEADER_IMPORT.md)

## Status: **closed** (STEP=1-8 done)

**Depends on:** TRACK_CPP_HEADER_IMPORT closed (minimal subset); TRACK_LSP closed or STEP=5 done.

**Baseline:** build_tests **1234/0**; `build_bin` mlcc2 **ok**; `mlcc2 --check-only compiler/main.mlc` ok.

**Goal:** ??????????? ?????? C++ ?????????? ? trivia roundtrip, expressions, class bodies, template decls � ??????? ? Ruby ??? STL/real headers. ????????: [CPP_PARSER_DESIGN.md](../CPP_PARSER_DESIGN.md).

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
| 1 | Fix self-host codegen `cpp_decls.mlc` � `build_bin` green | done |
| 2 | Trivia model � `leading_trivia`/`trailing_trivia` on `CppToken`; lexer collects comments/`#pragma` | done |
| 3 | `parser/to_source.mlc` � AST ? source via trivia; lexer roundtrip tests | done |
| 4 | Extend `cpp_ast.mlc` � class members, access levels, fn modifiers, template decls, typedef | done |
| 5 | Expression parser (Pratt) � `parser/cpp_exprs.mlc`; literals char/float/hex | done |
| 6 | Full decl parser � class bodies, template decls, extern blocks, attributes `[[...]]` | done |
| 7 | Structural + roundtrip tests (?20 + ?10); differential vs Ruby on fixture corpus | done |
| 8 | Re-wire `header_import.mlc`; fix fixture paths; close track | done |

### STEP=8 notes (2026-06-24)

- `header_import.mlc`: typedef/template/extern/prototype ? MLC decls; `CppFunctionPrototype` params via `print_cpp_type`.
- Fixtures: `cpp_parser/` (differential); `header_import/header_import.h` (full import); removed duplicate `fixtures/sample_header.h`.
- Tests: `test_cpp_header_import.mlc` (+6). Gate: **1234/0**; differential 8/8; `mlcc2 --check-only` ok. Track **closed**.

### STEP=7 notes (2026-06-24)

- `test_cpp_parser_structural.mlc` (~20); `test_cpp_to_source.mlc` (+10 roundtrip); fixtures `compiler/tests/fixtures/cpp_parser/` (8).
- `run_cpp_parser_differential.sh`: Ruby `CppAst` stmt count vs mlcc `cpp_parse` decl count; `cpp_parse_count.mlc` tool.
- Fixes: `ModularCompiler#build` (host `main`); `args()[0]` for CLI; `extern_single.h` � extern decl (Ruby ?? ?????? `extern "C" {}`).
- Gate: **1228/0**; differential 8/8; `mlcc2 --check-only` ok.


- Root cause: one-line `if � then � else �` without trailing `end` swallows following decls in mlcc codegen.
- Fixes: `cpp_decl_skip_token` (`cpp_decls.mlc`); `resolve_definition_span` (`symbols.mlc`); `lsp_diagnostic_severity_number` (`lsp_protocol.mlc`); `cpp_parse` else-if chain; `header_import` wildcard `empty_decls`.
- Gate: **1145/0**; `build_bin` mlcc2 ok; `mlcc2 --check-only compiler/main.mlc` ok.

### STEP=2 notes (2026-06-24)

- `CppToken`: `leading_trivia`/`trailing_trivia`; `cpp_token_make`, `cpp_token_with_trivia`, `cpp_token_to_source`.
- Lexer: `cpp_collect_leading_trivia` (ws, `//`, `/* */`, `#pragma` lines); trailing horizontal ws; `#include` stays `CHash` token.
- `cpp_tokens_to_source` = concat per token (no spaces); `cpp_lex_roundtrip_preserves_source`.
- Gate: **1152/0**; `build_bin` mlcc2 ok; `mlcc2 --check-only` ok. Commits: `c7b61a22`, `90c250fc`.

### STEP=3 notes (2026-06-24)

- `parser/to_source.mlc`: `cpp_tokens_span_to_source`, `cpp_declaration_to_source`, `cpp_program_to_source`.
- Trivia path: `cpp_program_from_tokens_preserving_trivia` wraps token spans as `CppDeclarationFragment`.
- Canonical path: structured AST via `print_decl`. Tests: `test_cpp_to_source.mlc` (+10).
- Gate: **1169/0**; commit `374f3a29`.

### STEP=4 notes (2026-06-24)

- `CppFnModifiers`, `CppAccessLevel`, `CppClassMember`, `CppClassDefinition`, `CppFunctionPrototype`.
- Decl variants: `CppClassDeclaration`, `CppTypedefDeclaration`, `CppTemplateDeclaration`, `CppFunctionPrototypeDecl`.
- `print_decl` for class bodies (access, fields, virtual/const methods), typedef, template wrapper.
- Rename `CppMember` ? `CppClassMember` (avoid clash with `CppExpression::CppMember`).
- Gate: **1169/0**; `build_bin` + `mlcc2 --check-only` ok; commit `a2d7668d`.

### STEP=5 notes (2026-06-24)

- Lexer: `CLChar`/`CLFloat`/`CLHex` tokens; `cpp_scan_char`, `cpp_scan_number` (hex, float, exponent).
- `parser/cpp_exprs.mlc`: Pratt parser ? `CppExpression` (literals, unary/binary, call, member, index, ternary).
- `cpp_predicates.mlc`: literal/operator predicates; `CppCharLiteral`/`CppFloatLiteral` in `cpp_ast.mlc`.
- Tests: `test_cpp_exprs.mlc` (+16), lexer (+5). Gate: **1192/0**; `mlcc2 --check-only` ok.

### STEP=6 notes (2026-06-24)

- `cpp_decls.mlc`: attributes, class members, typedef/template/extern; struct/class -> `CppClassDeclaration`.
- `cpp_predicates.mlc`: decl keyword predicates; `typename` in `cpp_type_part_string`.
- `header_import.mlc`: `CppClassDeclaration` -> MLC `DeclType`.
- Tests: `test_cpp_parser.mlc` (+5). Gate: **1197/0**; `mlcc2 --check-only` ok.

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
| � | self-host codegen fix |

---

## Per-turn template (Scribe � every turn)

```
| turn | <ISO date> |
| role | Driver / Planner / Backlog |
| step | <e.g. 3 or plan-refresh> |
| done | <one line> |
| verify | <pass/fail/pending + command> |
| next | ROLE=� STEP=� |
```
