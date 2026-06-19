# Track: CppDeclFragment cleanup

Parent: [../PLAN.md](../PLAN.md) �Phase 2 ?????; previous: [TRACK_MODULE_TU_BRIDGE.md](TRACK_MODULE_TU_BRIDGE.md) (**closed**)

## Status: **closed** (step 5 audit 2026-06-04)

**STEP=5 audit (2026-06-04 Driver):**

| Check | Result |
|-------|--------|
| `make_fragment_cpp_declaration` production callers | **0** (export only in `emit_helpers.mlc`) |
| `Shared.new(CppDeclFragment(...))` outside emit_helpers | **0** |
| `module.mlc` TU assembly | **0** `CppDeclFragment` |
| Track goal (TU + decl_cpp string path) | **met** |
| Deferred (documented) | `CppStmtFragment` in `expr_visitor_cpp`, `let_pat_cpp`, `stmt_cpp`; `CppDeclFragment` retained in AST/printer for tests |
| Gate | build_tests **940**/0; build.sh OK |

**Uncommitted WIP:** `decl_cpp`, `module`, `cpp_*`, `registry`, `test_cpp_printer`, etc. — commit batch recommended (separate from track close).

**Meta recovery (2026-06-04):** Driver STEP=4 loop — work landed (`CppHostEntryMain`, `module.mlc` no `cli_wrapper` fragment); gate blocked by hung `build_tests|grep`. **Do not re-enqueue STEP=4.** Mark step 4 done; next STEP=5 audit.

**STEP=4 note (2026-06-04 Driver, Meta-closed):** `CppHostEntryMain` + `make_entry_host_main_cpp_declaration`; removed `cli_wrapper` string path. Gate: verify pending (run `build_tests.sh` without `| grep`).

**STEP=3 note (2026-06-04 Driver):** `CppUsingNamespace` + `append_using_namespace_declarations`; `using_namespace_names` in `cpp_naming`; `module.mlc` no fragment for using lines. Gate: build_tests **939**/0; build.sh OK.

**STEP=2 note (2026-06-04 Driver):** `CppBlankLine` in `cpp_ast`/`cpp_printer`; `make_blank_line_cpp_declaration` ? `CppBlankLine`; golden `test_cpp_printer`. Gate: build_tests **938**/0; build.sh OK.

**STEP=1 note (2026-06-04 Driver):** Removed `cpp_decl_from_string_output` + `cpp_decls_from_string_parts` from `decl_cpp.mlc` (0 callers). Gate: build_tests **937**/0; build.sh OK.

**Critic audit (2026-06-04, 6 driver turns):** Steps 1-3 OK worktree (939/0); not in HEAD. CppUsingNamespace (not CppUsing). Survivor cli_wrapper module.mlc:225. No reopen 1-3; commit WIP batch.

**Goal:** ??????? `CppDeclFragment` ?? production-???? ? ??????? string-helpers.

## Verify gate (every step)

```
compiler/tests/build_tests.sh
compiler/build.sh
compiler/out/mlcc -o .tmp_selfhost/p1 compiler/main.mlc
compiler/build_bin.sh .tmp_selfhost/p1 .tmp_selfhost/mlcc2
.tmp_selfhost/mlcc2 -o .tmp_selfhost/p2 compiler/main.mlc
diff -rq .tmp_selfhost/p1 .tmp_selfhost/p2   # empty
```

---

| Step | Item | Status |
|------|------|--------|
| 1 | ??????? `cpp_decl_from_string_output` + `cpp_decls_from_string_parts` ?? `decl_cpp.mlc` (0 production-callers) | done |
| 2 | `make_blank_line_cpp_declaration` � ???????? `CppDeclFragment("\n")` ?? `CppDeclEmpty` ??? ????? `CppBlankLine` ???? | done |
| 3 | `module.mlc:216` � `implementation_using_namespaces` ????? ? ???????? `CppUsing` ?????? | done |
| 4 | `module.mlc:224` � `cli_wrapper` ????? ? ???????? `CppFnDef` ??? `CppRawText` ???? | done |
| 5 | ??????? `CppDeclFragment` ?? `cpp_ast.mlc`, `emit_helpers.mlc`, `cpp_printer.mlc`; audit ?????? | done |

## Context

| ???? | ?????? | ??????? |
|------|--------|---------|
| `codegen/decl_cpp.mlc:27-34` | definition + `cpp_decls_from_string_parts` | ??????? ??? � 0 production-callers |
| `cpp/module_tu_helpers.mlc:67` | `make_blank_line_cpp_declaration` = `"\n"` | ?????????? ? `CppDeclEmpty` |
| `codegen/module.mlc:216` | `implementation_using_namespaces` | `using namespace X;` ?????? |
| `codegen/module.mlc:224` | `cli_wrapper` | `int main(...)` entry shim |

**Deferred ?? MODULE_TU_BRIDGE:**
- `expr_visitor_cpp.mlc:196` � `CppStmtFragment` (?? `CppDeclFragment`, ????????? ????)
- `let_pat_cpp.mlc:150,270` � `CppStmtFragment` (????????? ????)
